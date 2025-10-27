"""
Ternary Editor - Text editor for ternary data.

This module provides a complete text editor application for ternary files.
"""

from typing import List, Union, Optional, Dict, Any
import os
import sys
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..libs.libstring import TernaryString, TernaryStringBuilder
from ..libs.libio import TernaryFileIO, TernaryConsoleIO


class TernaryEditor:
    """
    Ternary Editor - Text editor for ternary data.
    
    Provides comprehensive text editing capabilities for ternary files.
    """
    
    def __init__(self, filename: str = None):
        """
        Initialize ternary editor.
        
        Args:
            filename: File to edit
        """
        self.filename = filename
        self.content = TernaryString()
        self.cursor_position = 0
        self.selection_start = None
        self.selection_end = None
        
        # Editor state
        self.modified = False
        self.line_numbers = True
        self.word_wrap = True
        self.tab_size = 4
        
        # History for undo/redo
        self.history = []
        self.history_index = -1
        self.max_history = 100
        
        # Statistics
        self.stats = {
            'operations': 0,
            'saves': 0,
            'loads': 0,
            'undos': 0,
            'redos': 0
        }
        
        # Load file if specified
        if filename and os.path.exists(filename):
            self.load_file(filename)
    
    def load_file(self, filename: str) -> bool:
        """
        Load file into editor.
        
        Args:
            filename: File to load
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Read file as ternary data
            trits = TernaryFileIO.read_file(filename)
            
            # Convert to TernaryString
            self.content = TernaryString(trits)
            self.filename = filename
            self.cursor_position = 0
            self.modified = False
            
            # Add to history
            self._add_to_history()
            
            self.stats['loads'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to load file {filename}: {e}")
            return False
    
    def save_file(self, filename: str = None) -> bool:
        """
        Save file from editor.
        
        Args:
            filename: File to save (if None, use current filename)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if filename:
                self.filename = filename
            
            if not self.filename:
                raise ValueError("No filename specified")
            
            # Convert content to trits
            trits = self.content.trits
            
            # Write file
            TernaryFileIO.write_file(self.filename, trits)
            self.modified = False
            
            # Add to history
            self._add_to_history()
            
            self.stats['saves'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to save file {self.filename}: {e}")
            return False
    
    def insert_text(self, text: str, position: int = None) -> None:
        """
        Insert text at position.
        
        Args:
            text: Text to insert
            position: Position to insert at (if None, use cursor position)
        """
        if position is None:
            position = self.cursor_position
        
        # Convert text to trits
        trits = []
        for char in text:
            if char == '1':
                trits.append(Trit(1))
            elif char == '-':
                trits.append(Trit(-1))
            else:
                trits.append(Trit(0))
        
        # Insert trits
        for i, trit in enumerate(trits):
            self.content.insert(position + i, trit)
        
        # Update cursor position
        self.cursor_position = position + len(trits)
        self.modified = True
        
        # Add to history
        self._add_to_history()
        
        self.stats['operations'] += 1
    
    def delete_text(self, start: int, end: int) -> None:
        """
        Delete text between positions.
        
        Args:
            start: Start position
            end: End position
        """
        if start < 0 or end > len(self.content) or start > end:
            raise ValueError("Invalid delete range")
        
        # Delete trits
        for i in range(end - start):
            self.content.remove(start)
        
        # Update cursor position
        if self.cursor_position > start:
            self.cursor_position = max(start, self.cursor_position - (end - start))
        
        self.modified = True
        
        # Add to history
        self._add_to_history()
        
        self.stats['operations'] += 1
    
    def replace_text(self, old_text: str, new_text: str) -> int:
        """
        Replace text in content.
        
        Args:
            old_text: Text to replace
            new_text: Replacement text
            
        Returns:
            Number of replacements made
        """
        # Convert to TernaryString
        old_string = TernaryString.from_string(old_text)
        new_string = TernaryString.from_string(new_text)
        
        # Replace in content
        result = self.content.replace(old_string, new_string)
        
        # Update content
        self.content = result
        self.modified = True
        
        # Add to history
        self._add_to_history()
        
        self.stats['operations'] += 1
        
        # Count replacements
        return self.content.count(old_string)
    
    def find_text(self, text: str, start: int = 0) -> int:
        """
        Find text in content.
        
        Args:
            text: Text to find
            start: Start position
            
        Returns:
            Position of text or -1 if not found
        """
        search_string = TernaryString.from_string(text)
        return self.content.find(search_string, start)
    
    def get_line(self, line_number: int) -> TernaryString:
        """
        Get line by number.
        
        Args:
            line_number: Line number (0-based)
            
        Returns:
            Line content as TernaryString
        """
        lines = self.get_lines()
        if 0 <= line_number < len(lines):
            return lines[line_number]
        else:
            return TernaryString()
    
    def get_lines(self) -> List[TernaryString]:
        """
        Get all lines.
        
        Returns:
            List of lines as TernaryString objects
        """
        # Split by newline (ternary representation)
        newline = TernaryString([Trit(0)])  # Assuming 0 represents newline
        return self.content.split(newline)
    
    def get_line_count(self) -> int:
        """Get total number of lines."""
        return len(self.get_lines())
    
    def get_cursor_line(self) -> int:
        """Get current line number."""
        lines = self.get_lines()
        current_pos = 0
        
        for i, line in enumerate(lines):
            if self.cursor_position <= current_pos + len(line):
                return i
            current_pos += len(line) + 1  # +1 for newline
        
        return len(lines) - 1
    
    def get_cursor_column(self) -> int:
        """Get current column number."""
        lines = self.get_lines()
        current_pos = 0
        
        for line in lines:
            if self.cursor_position <= current_pos + len(line):
                return self.cursor_position - current_pos
            current_pos += len(line) + 1  # +1 for newline
        
        return 0
    
    def move_cursor(self, position: int) -> None:
        """
        Move cursor to position.
        
        Args:
            position: New cursor position
        """
        if 0 <= position <= len(self.content):
            self.cursor_position = position
    
    def move_cursor_line(self, line: int, column: int = 0) -> None:
        """
        Move cursor to line and column.
        
        Args:
            line: Line number
            column: Column number
        """
        lines = self.get_lines()
        
        if 0 <= line < len(lines):
            line_length = len(lines[line])
            column = min(column, line_length)
            
            # Calculate position
            position = 0
            for i in range(line):
                position += len(lines[i]) + 1  # +1 for newline
            
            position += column
            self.move_cursor(position)
    
    def select_text(self, start: int, end: int) -> None:
        """
        Select text between positions.
        
        Args:
            start: Start position
            end: End position
        """
        if start < 0 or end > len(self.content) or start > end:
            raise ValueError("Invalid selection range")
        
        self.selection_start = start
        self.selection_end = end
    
    def get_selection(self) -> TernaryString:
        """Get selected text."""
        if self.selection_start is not None and self.selection_end is not None:
            return self.content.substring(self.selection_start, self.selection_end)
        else:
            return TernaryString()
    
    def clear_selection(self) -> None:
        """Clear text selection."""
        self.selection_start = None
        self.selection_end = None
    
    def copy_text(self) -> TernaryString:
        """Copy selected text."""
        return self.get_selection()
    
    def cut_text(self) -> TernaryString:
        """Cut selected text."""
        selection = self.get_selection()
        if len(selection) > 0:
            self.delete_text(self.selection_start, self.selection_end)
            self.clear_selection()
        return selection
    
    def paste_text(self, text: TernaryString) -> None:
        """Paste text at cursor position."""
        if len(text) > 0:
            self.insert_text(str(text), self.cursor_position)
    
    def undo(self) -> bool:
        """Undo last operation."""
        if self.history_index > 0:
            self.history_index -= 1
            self.content = self.history[self.history_index].copy()
            self.modified = True
            self.stats['undos'] += 1
            return True
        return False
    
    def redo(self) -> bool:
        """Redo last undone operation."""
        if self.history_index < len(self.history) - 1:
            self.history_index += 1
            self.content = self.history[self.history_index].copy()
            self.modified = True
            self.stats['redos'] += 1
            return True
        return False
    
    def _add_to_history(self) -> None:
        """Add current state to history."""
        # Remove future history if we're not at the end
        if self.history_index < len(self.history) - 1:
            self.history = self.history[:self.history_index + 1]
        
        # Add current state
        self.history.append(self.content.copy())
        self.history_index += 1
        
        # Limit history size
        if len(self.history) > self.max_history:
            self.history.pop(0)
            self.history_index -= 1
    
    def get_stats(self) -> Dict[str, Any]:
        """Get editor statistics."""
        return {
            'filename': self.filename,
            'content_length': len(self.content),
            'line_count': self.get_line_count(),
            'cursor_position': self.cursor_position,
            'modified': self.modified,
            'history_size': len(self.history),
            **self.stats
        }
    
    def display(self, start_line: int = 0, end_line: int = None) -> None:
        """
        Display editor content.
        
        Args:
            start_line: Start line to display
            end_line: End line to display
        """
        lines = self.get_lines()
        
        if end_line is None:
            end_line = len(lines)
        
        for i in range(start_line, min(end_line, len(lines))):
            line = lines[i]
            line_num = f"{i+1:4d}: " if self.line_numbers else ""
            print(f"{line_num}{line}")
    
    def search_and_replace(self, search_text: str, replace_text: str, 
                          replace_all: bool = False) -> int:
        """
        Search and replace text.
        
        Args:
            search_text: Text to search for
            replace_text: Replacement text
            replace_all: Whether to replace all occurrences
            
        Returns:
            Number of replacements made
        """
        replacements = 0
        start = 0
        
        while True:
            position = self.find_text(search_text, start)
            if position == -1:
                break
            
            # Replace at position
            self.delete_text(position, position + len(search_text))
            self.insert_text(replace_text, position)
            
            replacements += 1
            start = position + len(replace_text)
            
            if not replace_all:
                break
        
        return replacements
