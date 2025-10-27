"""
TFS Directory implementation.

This module provides directory operations for the Ternary File System,
including directory entries and directory management.
"""

from typing import Dict, List, Optional, Any, Tuple
import time
from ..core.trit import Trit
from ..core.tritarray import TritArray
from .inode import TFSInode, FileType


class TFSDirectoryEntry:
    """
    TFS Directory Entry.
    
    Represents a single entry in a directory, containing
    name, inode number, and file type information.
    """
    
    def __init__(self, name: str, inode_num: int, file_type: FileType):
        """
        Initialize directory entry.
        
        Args:
            name: Entry name
            inode_num: Inode number
            file_type: File type
        """
        self.name = name
        self.inode_num = inode_num
        self.file_type = file_type
        self.creation_time = time.time()
        self.last_access_time = time.time()
        
        # Ternary-specific metadata
        self.ternary_name_hash = self._calculate_name_hash(name)
        self.ternary_encoding = 'utf-8'
    
    def _calculate_name_hash(self, name: str) -> str:
        """Calculate ternary hash for the name."""
        # Simple hash calculation
        return str(hash(name))
    
    def update_access_time(self) -> None:
        """Update last access time."""
        self.last_access_time = time.time()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert entry to dictionary."""
        return {
            'name': self.name,
            'inode_num': self.inode_num,
            'file_type': self.file_type.value,
            'creation_time': self.creation_time,
            'last_access_time': self.last_access_time,
            'ternary_name_hash': self.ternary_name_hash,
            'ternary_encoding': self.ternary_encoding
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TFSDirectoryEntry(name={self.name}, inode={self.inode_num}, type={self.file_type.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TFSDirectoryEntry(name={self.name}, inode={self.inode_num}, "
                f"type={self.file_type.value}, hash={self.ternary_name_hash})")


class TFSDirectory:
    """
    TFS Directory - Directory management for TFS.
    
    Provides directory operations including entry management,
    navigation, and directory metadata.
    """
    
    def __init__(self, inode: TFSInode):
        """
        Initialize TFS directory.
        
        Args:
            inode: Directory inode
        """
        self.inode = inode
        self.entries = {}  # name -> TFSDirectoryEntry
        self.entry_count = 0
        
        # Directory metadata
        self.parent_inode = None
        self.creation_time = time.time()
        self.last_modification_time = time.time()
        
        # Ternary-specific directory metadata
        self.ternary_metadata = {
            'directory_encoding': 'utf-8',
            'entry_ordering': 'alphabetical',
            'ternary_sort_key': None
        }
    
    def add_entry(self, name: str, inode_num: int, file_type: FileType) -> bool:
        """
        Add an entry to the directory.
        
        Args:
            name: Entry name
            inode_num: Inode number
            file_type: File type
            
        Returns:
            True if added successfully, False otherwise
        """
        if name in self.entries:
            return False  # Entry already exists
        
        entry = TFSDirectoryEntry(name, inode_num, file_type)
        self.entries[name] = entry
        self.entry_count += 1
        
        # Update directory metadata
        self.last_modification_time = time.time()
        self.inode.update_modification_time()
        
        return True
    
    def remove_entry(self, name: str) -> bool:
        """
        Remove an entry from the directory.
        
        Args:
            name: Entry name
            
        Returns:
            True if removed successfully, False otherwise
        """
        if name not in self.entries:
            return False
        
        del self.entries[name]
        self.entry_count -= 1
        
        # Update directory metadata
        self.last_modification_time = time.time()
        self.inode.update_modification_time()
        
        return True
    
    def get_entry(self, name: str) -> Optional[TFSDirectoryEntry]:
        """
        Get directory entry by name.
        
        Args:
            name: Entry name
            
        Returns:
            Directory entry or None if not found
        """
        if name in self.entries:
            entry = self.entries[name]
            entry.update_access_time()
            return entry
        return None
    
    def has_entry(self, name: str) -> bool:
        """
        Check if directory has an entry.
        
        Args:
            name: Entry name
            
        Returns:
            True if entry exists, False otherwise
        """
        return name in self.entries
    
    def list_entries(self, include_hidden: bool = False) -> List[str]:
        """
        List all entries in the directory.
        
        Args:
            include_hidden: Whether to include hidden entries
            
        Returns:
            List of entry names
        """
        entries = list(self.entries.keys())
        
        if not include_hidden:
            # Filter out hidden entries (starting with '.')
            entries = [name for name in entries if not name.startswith('.')]
        
        # Sort entries based on ternary ordering
        if self.ternary_metadata['entry_ordering'] == 'alphabetical':
            entries.sort()
        
        return entries
    
    def get_entry_info(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get detailed information about an entry.
        
        Args:
            name: Entry name
            
        Returns:
            Entry information or None if not found
        """
        entry = self.get_entry(name)
        if entry:
            return entry.to_dict()
        return None
    
    def get_all_entries(self) -> List[Dict[str, Any]]:
        """Get all entries with their information."""
        entries = []
        for name, entry in self.entries.items():
            entry_info = entry.to_dict()
            entries.append(entry_info)
        return entries
    
    def find_entries_by_type(self, file_type: FileType) -> List[str]:
        """
        Find entries by file type.
        
        Args:
            file_type: File type to search for
            
        Returns:
            List of entry names matching the type
        """
        matching_entries = []
        for name, entry in self.entries.items():
            if entry.file_type == file_type:
                matching_entries.append(name)
        return matching_entries
    
    def find_entries_by_pattern(self, pattern: str) -> List[str]:
        """
        Find entries matching a pattern.
        
        Args:
            pattern: Pattern to match (supports wildcards)
            
        Returns:
            List of entry names matching the pattern
        """
        matching_entries = []
        for name in self.entries.keys():
            if self._match_pattern(name, pattern):
                matching_entries.append(name)
        return matching_entries
    
    def _match_pattern(self, name: str, pattern: str) -> bool:
        """Match name against pattern (simple wildcard matching)."""
        if '*' in pattern:
            # Simple wildcard matching
            import fnmatch
            return fnmatch.fnmatch(name, pattern)
        else:
            return name == pattern
    
    def get_directory_size(self) -> int:
        """Get directory size in bytes."""
        # Calculate size based on entries
        size = 0
        for entry in self.entries.values():
            # Each entry takes some space
            size += len(entry.name) + 32  # Name + metadata overhead
        return size
    
    def get_entry_count(self) -> int:
        """Get number of entries in directory."""
        return self.entry_count
    
    def is_empty(self) -> bool:
        """Check if directory is empty."""
        return self.entry_count == 0
    
    def set_parent_inode(self, parent_inode: int) -> None:
        """
        Set parent directory inode.
        
        Args:
            parent_inode: Parent directory inode number
        """
        self.parent_inode = parent_inode
        self.last_modification_time = time.time()
    
    def get_parent_inode(self) -> Optional[int]:
        """Get parent directory inode."""
        return self.parent_inode
    
    def set_ternary_metadata(self, key: str, value: Any) -> None:
        """
        Set ternary-specific metadata.
        
        Args:
            key: Metadata key
            value: Metadata value
        """
        self.ternary_metadata[key] = value
        self.last_modification_time = time.time()
    
    def get_ternary_metadata(self, key: str) -> Optional[Any]:
        """
        Get ternary-specific metadata.
        
        Args:
            key: Metadata key
            
        Returns:
            Metadata value or None if not found
        """
        return self.ternary_metadata.get(key)
    
    def sort_entries(self, key: str = 'name', reverse: bool = False) -> None:
        """
        Sort directory entries.
        
        Args:
            key: Sort key ('name', 'type', 'size', 'time')
            reverse: Whether to sort in reverse order
        """
        if key == 'name':
            sorted_entries = sorted(self.entries.items(), key=lambda x: x[0], reverse=reverse)
        elif key == 'type':
            sorted_entries = sorted(self.entries.items(), key=lambda x: x[1].file_type.value, reverse=reverse)
        elif key == 'time':
            sorted_entries = sorted(self.entries.items(), key=lambda x: x[1].creation_time, reverse=reverse)
        else:
            return  # Unknown sort key
        
        # Rebuild entries dictionary in sorted order
        self.entries = dict(sorted_entries)
    
    def calculate_ternary_hash(self) -> str:
        """Calculate ternary hash for the directory."""
        # Combine all entry information
        hash_data = ""
        for name, entry in sorted(self.entries.items()):
            hash_data += f"{name}{entry.inode_num}{entry.file_type.value}"
        return str(hash(hash_data))
    
    def verify_integrity(self) -> bool:
        """Verify directory integrity."""
        # Check if entry count matches actual entries
        if len(self.entries) != self.entry_count:
            return False
        
        # Check for duplicate inode numbers
        inode_numbers = [entry.inode_num for entry in self.entries.values()]
        if len(inode_numbers) != len(set(inode_numbers)):
            return False
        
        return True
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert directory to dictionary."""
        return {
            'inode_num': self.inode.inode_num,
            'entry_count': self.entry_count,
            'entries': {name: entry.to_dict() for name, entry in self.entries.items()},
            'parent_inode': self.parent_inode,
            'creation_time': self.creation_time,
            'last_modification_time': self.last_modification_time,
            'ternary_metadata': self.ternary_metadata.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TFSDirectory(inode={self.inode.inode_num}, entries={self.entry_count})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TFSDirectory(inode={self.inode.inode_num}, "
                f"entries={self.entry_count}, parent={self.parent_inode})")
