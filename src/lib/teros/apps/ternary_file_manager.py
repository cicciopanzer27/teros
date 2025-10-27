"""
Ternary File Manager - File system browser for ternary data.

This module provides a complete file manager application for ternary files.
"""

from typing import List, Union, Optional, Dict, Any, Tuple
import os
import sys
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..libs.libio import TernaryFileIO, TernaryConsoleIO
from ..libs.libstring import TernaryString


class FileType(Enum):
    """File types."""
    DIRECTORY = "directory"
    FILE = "file"
    SYMLINK = "symlink"
    UNKNOWN = "unknown"


class FileInfo:
    """
    File Information - Represents file metadata.
    
    Provides file information and statistics.
    """
    
    def __init__(self, path: str):
        """
        Initialize file info.
        
        Args:
            path: File path
        """
        self.path = path
        self.name = os.path.basename(path)
        self.size = 0
        self.file_type = FileType.UNKNOWN
        self.permissions = ""
        self.owner = ""
        self.group = ""
        self.modified_time = 0
        self.accessed_time = 0
        self.created_time = 0
        
        # Load file information
        self._load_info()
    
    def _load_info(self) -> None:
        """Load file information."""
        try:
            if os.path.exists(self.path):
                stat = os.stat(self.path)
                
                self.size = stat.st_size
                self.modified_time = stat.st_mtime
                self.accessed_time = stat.st_atime
                self.created_time = stat.st_ctime
                
                # Determine file type
                if os.path.isdir(self.path):
                    self.file_type = FileType.DIRECTORY
                elif os.path.isfile(self.path):
                    self.file_type = FileType.FILE
                elif os.path.islink(self.path):
                    self.file_type = FileType.SYMLINK
                
                # Get permissions
                self.permissions = oct(stat.st_mode)[-3:]
                
                # Get owner and group (if available)
                try:
                    import pwd
                    import grp
                    self.owner = pwd.getpwuid(stat.st_uid).pw_name
                    self.group = grp.getgrgid(stat.st_gid).gr_name
                except ImportError:
                    self.owner = str(stat.st_uid)
                    self.group = str(stat.st_gid)
                
        except Exception as e:
            print(f"Failed to load file info for {self.path}: {e}")
    
    def get_size_string(self) -> str:
        """Get human-readable size string."""
        if self.size < 1024:
            return f"{self.size} B"
        elif self.size < 1024 * 1024:
            return f"{self.size / 1024:.1f} KB"
        elif self.size < 1024 * 1024 * 1024:
            return f"{self.size / (1024 * 1024):.1f} MB"
        else:
            return f"{self.size / (1024 * 1024 * 1024):.1f} GB"
    
    def get_modified_string(self) -> str:
        """Get human-readable modified time string."""
        return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(self.modified_time))
    
    def is_ternary_file(self) -> bool:
        """Check if file is a ternary file."""
        # Check file extension
        if self.name.endswith('.t3') or self.name.endswith('.ternary'):
            return True
        
        # Check file content (first few bytes)
        try:
            with open(self.path, 'rb') as f:
                data = f.read(16)
                # Check if data looks like ternary encoding
                return len(data) > 0 and all(byte in [0, 1, 2, 3] for byte in data)
        except:
            return False
    
    def __str__(self) -> str:
        """Get string representation."""
        type_char = {
            FileType.DIRECTORY: 'd',
            FileType.FILE: 'f',
            FileType.SYMLINK: 'l',
            FileType.UNKNOWN: '?'
        }.get(self.file_type, '?')
        
        return f"{type_char} {self.permissions} {self.owner:8s} {self.group:8s} {self.get_size_string():8s} {self.get_modified_string()} {self.name}"


class TernaryFileManager:
    """
    Ternary File Manager - File system browser for ternary data.
    
    Provides comprehensive file management capabilities.
    """
    
    def __init__(self, start_path: str = None):
        """
        Initialize file manager.
        
        Args:
            start_path: Starting directory path
        """
        self.current_path = start_path or os.getcwd()
        self.history = []  # Navigation history
        self.bookmarks = {}  # Bookmarked paths
        
        # File manager state
        self.show_hidden = False
        self.sort_by = 'name'  # name, size, modified, type
        self.sort_reverse = False
        self.view_mode = 'list'  # list, grid, tree
        
        # Statistics
        self.stats = {
            'files_viewed': 0,
            'directories_navigated': 0,
            'files_created': 0,
            'files_deleted': 0,
            'files_copied': 0,
            'files_moved': 0
        }
    
    def navigate_to(self, path: str) -> bool:
        """
        Navigate to directory.
        
        Args:
            path: Directory path
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Resolve path
            if os.path.isabs(path):
                target_path = path
            else:
                target_path = os.path.join(self.current_path, path)
            
            target_path = os.path.abspath(target_path)
            
            # Check if path exists and is directory
            if not os.path.exists(target_path):
                print(f"Path does not exist: {target_path}")
                return False
            
            if not os.path.isdir(target_path):
                print(f"Path is not a directory: {target_path}")
                return False
            
            # Add current path to history
            self.history.append(self.current_path)
            
            # Navigate to new path
            self.current_path = target_path
            self.stats['directories_navigated'] += 1
            
            return True
            
        except Exception as e:
            print(f"Failed to navigate to {path}: {e}")
            return False
    
    def navigate_back(self) -> bool:
        """Navigate back in history."""
        if self.history:
            previous_path = self.history.pop()
            self.current_path = previous_path
            self.stats['directories_navigated'] += 1
            return True
        return False
    
    def list_directory(self, path: str = None) -> List[FileInfo]:
        """
        List directory contents.
        
        Args:
            path: Directory path (if None, use current path)
            
        Returns:
            List of FileInfo objects
        """
        try:
            if path is None:
                path = self.current_path
            
            if not os.path.exists(path):
                print(f"Path does not exist: {path}")
                return []
            
            if not os.path.isdir(path):
                print(f"Path is not a directory: {path}")
                return []
            
            # Get directory contents
            entries = os.listdir(path)
            
            # Filter hidden files if needed
            if not self.show_hidden:
                entries = [entry for entry in entries if not entry.startswith('.')]
            
            # Create FileInfo objects
            file_infos = []
            for entry in entries:
                entry_path = os.path.join(path, entry)
                file_info = FileInfo(entry_path)
                file_infos.append(file_info)
            
            # Sort files
            file_infos = self._sort_files(file_infos)
            
            self.stats['files_viewed'] += len(file_infos)
            return file_infos
            
        except Exception as e:
            print(f"Failed to list directory {path}: {e}")
            return []
    
    def _sort_files(self, files: List[FileInfo]) -> List[FileInfo]:
        """Sort files according to current sort settings."""
        if self.sort_by == 'name':
            files.sort(key=lambda f: f.name.lower(), reverse=self.sort_reverse)
        elif self.sort_by == 'size':
            files.sort(key=lambda f: f.size, reverse=self.sort_reverse)
        elif self.sort_by == 'modified':
            files.sort(key=lambda f: f.modified_time, reverse=self.sort_reverse)
        elif self.sort_by == 'type':
            files.sort(key=lambda f: f.file_type.value, reverse=self.sort_reverse)
        
        return files
    
    def create_file(self, filename: str, content: List[Trit] = None) -> bool:
        """
        Create new file.
        
        Args:
            filename: File name
            content: File content (if None, create empty file)
            
        Returns:
            True if successful, False otherwise
        """
        try:
            file_path = os.path.join(self.current_path, filename)
            
            if content is None:
                content = []
            
            # Write file
            TernaryFileIO.write_file(file_path, content)
            
            self.stats['files_created'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to create file {filename}: {e}")
            return False
    
    def create_directory(self, dirname: str) -> bool:
        """
        Create new directory.
        
        Args:
            dirname: Directory name
            
        Returns:
            True if successful, False otherwise
        """
        try:
            dir_path = os.path.join(self.current_path, dirname)
            os.makedirs(dir_path, exist_ok=True)
            return True
            
        except Exception as e:
            print(f"Failed to create directory {dirname}: {e}")
            return False
    
    def delete_file(self, filename: str) -> bool:
        """
        Delete file.
        
        Args:
            filename: File name
            
        Returns:
            True if successful, False otherwise
        """
        try:
            file_path = os.path.join(self.current_path, filename)
            
            if os.path.isfile(file_path):
                os.remove(file_path)
                self.stats['files_deleted'] += 1
                return True
            else:
                print(f"File does not exist: {filename}")
                return False
                
        except Exception as e:
            print(f"Failed to delete file {filename}: {e}")
            return False
    
    def delete_directory(self, dirname: str) -> bool:
        """
        Delete directory.
        
        Args:
            dirname: Directory name
            
        Returns:
            True if successful, False otherwise
        """
        try:
            dir_path = os.path.join(self.current_path, dirname)
            
            if os.path.isdir(dir_path):
                os.rmdir(dir_path)
                return True
            else:
                print(f"Directory does not exist: {dirname}")
                return False
                
        except Exception as e:
            print(f"Failed to delete directory {dirname}: {e}")
            return False
    
    def copy_file(self, source: str, destination: str) -> bool:
        """
        Copy file.
        
        Args:
            source: Source file path
            destination: Destination file path
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Resolve paths
            if not os.path.isabs(source):
                source = os.path.join(self.current_path, source)
            
            if not os.path.isabs(destination):
                destination = os.path.join(self.current_path, destination)
            
            # Copy file
            TernaryFileIO.copy_file(source, destination)
            
            self.stats['files_copied'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to copy file {source} to {destination}: {e}")
            return False
    
    def move_file(self, source: str, destination: str) -> bool:
        """
        Move file.
        
        Args:
            source: Source file path
            destination: Destination file path
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Resolve paths
            if not os.path.isabs(source):
                source = os.path.join(self.current_path, source)
            
            if not os.path.isabs(destination):
                destination = os.path.join(self.current_path, destination)
            
            # Move file
            os.rename(source, destination)
            
            self.stats['files_moved'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to move file {source} to {destination}: {e}")
            return False
    
    def rename_file(self, old_name: str, new_name: str) -> bool:
        """
        Rename file.
        
        Args:
            old_name: Old file name
            new_name: New file name
            
        Returns:
            True if successful, False otherwise
        """
        try:
            old_path = os.path.join(self.current_path, old_name)
            new_path = os.path.join(self.current_path, new_name)
            
            os.rename(old_path, new_path)
            return True
            
        except Exception as e:
            print(f"Failed to rename file {old_name} to {new_name}: {e}")
            return False
    
    def get_file_info(self, filename: str) -> Optional[FileInfo]:
        """
        Get file information.
        
        Args:
            filename: File name
            
        Returns:
            FileInfo object or None if not found
        """
        try:
            file_path = os.path.join(self.current_path, filename)
            return FileInfo(file_path)
        except Exception as e:
            print(f"Failed to get file info for {filename}: {e}")
            return None
    
    def search_files(self, pattern: str, search_path: str = None) -> List[FileInfo]:
        """
        Search for files matching pattern.
        
        Args:
            pattern: Search pattern
            search_path: Path to search in (if None, use current path)
            
        Returns:
            List of matching FileInfo objects
        """
        try:
            if search_path is None:
                search_path = self.current_path
            
            matches = []
            
            # Walk directory tree
            for root, dirs, files in os.walk(search_path):
                for file in files:
                    if pattern in file:
                        file_path = os.path.join(root, file)
                        file_info = FileInfo(file_path)
                        matches.append(file_info)
            
            return matches
            
        except Exception as e:
            print(f"Failed to search files: {e}")
            return []
    
    def add_bookmark(self, name: str, path: str) -> None:
        """Add bookmark."""
        self.bookmarks[name] = os.path.abspath(path)
    
    def remove_bookmark(self, name: str) -> None:
        """Remove bookmark."""
        if name in self.bookmarks:
            del self.bookmarks[name]
    
    def get_bookmarks(self) -> Dict[str, str]:
        """Get all bookmarks."""
        return self.bookmarks.copy()
    
    def navigate_to_bookmark(self, name: str) -> bool:
        """Navigate to bookmark."""
        if name in self.bookmarks:
            return self.navigate_to(self.bookmarks[name])
        else:
            print(f"Bookmark not found: {name}")
            return False
    
    def get_stats(self) -> Dict[str, Any]:
        """Get file manager statistics."""
        return {
            'current_path': self.current_path,
            'history_size': len(self.history),
            'bookmarks': len(self.bookmarks),
            'show_hidden': self.show_hidden,
            'sort_by': self.sort_by,
            'sort_reverse': self.sort_reverse,
            'view_mode': self.view_mode,
            **self.stats
        }
    
    def display_directory(self, path: str = None) -> None:
        """
        Display directory contents.
        
        Args:
            path: Directory path (if None, use current path)
        """
        files = self.list_directory(path)
        
        if not files:
            print("Directory is empty")
            return
        
        print(f"Contents of {self.current_path}:")
        print()
        
        # Display header
        print(f"{'Type':<4} {'Perm':<4} {'Owner':<8} {'Group':<8} {'Size':<8} {'Modified':<19} {'Name'}")
        print("-" * 80)
        
        # Display files
        for file_info in files:
            print(file_info)
        
        print()
        print(f"Total: {len(files)} items")
    
    def display_file_info(self, filename: str) -> None:
        """
        Display detailed file information.
        
        Args:
            filename: File name
        """
        file_info = self.get_file_info(filename)
        
        if file_info:
            print(f"File: {file_info.name}")
            print(f"Path: {file_info.path}")
            print(f"Type: {file_info.file_type.value}")
            print(f"Size: {file_info.get_size_string()}")
            print(f"Permissions: {file_info.permissions}")
            print(f"Owner: {file_info.owner}")
            print(f"Group: {file_info.group}")
            print(f"Modified: {file_info.get_modified_string()}")
            print(f"Ternary file: {file_info.is_ternary_file()}")
        else:
            print(f"File not found: {filename}")
