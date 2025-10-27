"""
Ternary File System (TFS) implementation.

This module provides the main TFS implementation,
integrating all file system components.
"""

from typing import Dict, List, Optional, Any, Union
import time
from .superblock import TFSSuperblock
from .inode import TFSInode, FileType
from .directory import TFSDirectory, TFSDirectoryEntry
from .file_operations import TFSFileOperations, TFSFileHandle, SeekWhence


class TernaryFileSystem:
    """
    Ternary File System (TFS) - Main filesystem implementation.
    
    Integrates all TFS components including superblock, inodes,
    directories, and file operations.
    """
    
    def __init__(self, total_blocks: int = 10000, block_size: int = 4096):
        """
        Initialize TFS.
        
        Args:
            total_blocks: Total number of blocks
            block_size: Block size in bytes
        """
        # Initialize filesystem components
        self.superblock = TFSSuperblock(total_blocks, block_size)
        self.inodes = {}  # inode_num -> TFSInode
        self.directories = {}  # inode_num -> TFSDirectory
        self.file_operations = TFSFileOperations()
        
        # Filesystem state
        self.mounted = False
        self.root_inode = None
        
        # Filesystem statistics
        self.stats = {
            'total_files': 0,
            'total_directories': 0,
            'total_links': 0,
            'bytes_used': 0,
            'bytes_free': 0,
            'operations_performed': 0
        }
        
        # Initialize root directory
        self._initialize_root_directory()
    
    def _initialize_root_directory(self) -> None:
        """Initialize root directory."""
        # Create root inode
        root_inode = TFSInode(1, FileType.DIRECTORY)
        root_inode.set_permissions(0o755)
        root_inode.set_owner(0, 0)  # root user
        
        self.inodes[1] = root_inode
        self.root_inode = root_inode
        
        # Create root directory
        root_dir = TFSDirectory(root_inode)
        root_dir.set_parent_inode(1)  # Root is its own parent
        
        self.directories[1] = root_dir
        
        # Update statistics
        self.stats['total_directories'] += 1
    
    def mount(self) -> bool:
        """
        Mount the filesystem.
        
        Returns:
            True if mount successful, False otherwise
        """
        if self.mounted:
            return False
        
        success = self.superblock.mount()
        if success:
            self.mounted = True
            self.stats['operations_performed'] += 1
        
        return success
    
    def unmount(self) -> bool:
        """
        Unmount the filesystem.
        
        Returns:
            True if unmount successful, False otherwise
        """
        if not self.mounted:
            return False
        
        # Close all open files
        self.file_operations.close_all_files()
        
        success = self.superblock.unmount()
        if success:
            self.mounted = False
            self.stats['operations_performed'] += 1
        
        return success
    
    def create_file(self, path: str, file_type: FileType = FileType.REGULAR, 
                   permissions: int = 0o644) -> Optional[int]:
        """
        Create a new file.
        
        Args:
            path: File path
            file_type: Type of file to create
            permissions: File permissions
            
        Returns:
            Inode number if created successfully, None otherwise
        """
        if not self.mounted:
            return None
        
        # Parse path
        path_parts = self._parse_path(path)
        if not path_parts:
            return None
        
        filename = path_parts[-1]
        parent_path = '/'.join(path_parts[:-1]) if len(path_parts) > 1 else '/'
        
        # Get parent directory
        parent_inode = self._get_inode_by_path(parent_path)
        if not parent_inode or not self._is_directory(parent_inode):
            return None
        
        # Check if file already exists
        if self._file_exists_in_directory(parent_inode, filename):
            return None
        
        # Allocate new inode
        inode_num = self.superblock.allocate_inode()
        if inode_num is None:
            return None
        
        # Create inode
        inode = TFSInode(inode_num, file_type)
        inode.set_permissions(permissions)
        inode.set_owner(0, 0)  # Default owner
        
        self.inodes[inode_num] = inode
        
        # Add to parent directory
        parent_dir = self.directories.get(parent_inode.inode_num)
        if parent_dir:
            parent_dir.add_entry(filename, inode_num, file_type)
        
        # Update statistics
        self._update_stats_for_file_type(file_type, 1)
        self.stats['operations_performed'] += 1
        
        return inode_num
    
    def delete_file(self, path: str) -> bool:
        """
        Delete a file.
        
        Args:
            path: File path
            
        Returns:
            True if deleted successfully, False otherwise
        """
        if not self.mounted:
            return False
        
        # Get file inode
        inode = self._get_inode_by_path(path)
        if not inode:
            return False
        
        # Get parent directory
        path_parts = self._parse_path(path)
        if not path_parts:
            return False
        
        filename = path_parts[-1]
        parent_path = '/'.join(path_parts[:-1]) if len(path_parts) > 1 else '/'
        parent_inode = self._get_inode_by_path(parent_path)
        
        if not parent_inode:
            return False
        
        # Remove from parent directory
        parent_dir = self.directories.get(parent_inode.inode_num)
        if parent_dir:
            parent_dir.remove_entry(filename)
        
        # Deallocate inode
        self.superblock.deallocate_inode(inode.inode_num)
        
        # Remove from inodes
        if inode.inode_num in self.inodes:
            del self.inodes[inode.inode_num]
        
        # Remove from directories if it's a directory
        if inode.inode_num in self.directories:
            del self.directories[inode.inode_num]
        
        # Update statistics
        self._update_stats_for_file_type(inode.file_type, -1)
        self.stats['operations_performed'] += 1
        
        return True
    
    def open_file(self, path: str, mode: str = 'r') -> Optional[int]:
        """
        Open a file.
        
        Args:
            path: File path
            mode: Open mode
            
        Returns:
            File handle ID if opened successfully, None otherwise
        """
        if not self.mounted:
            return None
        
        inode = self._get_inode_by_path(path)
        if not inode:
            return None
        
        file_id = self.file_operations.open_file(inode, mode)
        self.stats['operations_performed'] += 1
        
        return file_id
    
    def close_file(self, file_id: int) -> bool:
        """
        Close a file.
        
        Args:
            file_id: File handle ID
            
        Returns:
            True if closed successfully, False otherwise
        """
        success = self.file_operations.close_file(file_id)
        if success:
            self.stats['operations_performed'] += 1
        return success
    
    def read_file(self, file_id: int, size: int = -1) -> bytes:
        """
        Read from a file.
        
        Args:
            file_id: File handle ID
            size: Number of bytes to read
            
        Returns:
            Data read from file
        """
        data = self.file_operations.read_file(file_id, size)
        self.stats['operations_performed'] += 1
        return data
    
    def write_file(self, file_id: int, data: bytes) -> int:
        """
        Write to a file.
        
        Args:
            file_id: File handle ID
            data: Data to write
            
        Returns:
            Number of bytes written
        """
        bytes_written = self.file_operations.write_file(file_id, data)
        self.stats['operations_performed'] += 1
        return bytes_written
    
    def seek_file(self, file_id: int, offset: int, whence: SeekWhence = SeekWhence.SEEK_SET) -> int:
        """
        Seek in a file.
        
        Args:
            file_id: File handle ID
            offset: Offset to seek to
            whence: Seek type
            
        Returns:
            New position in file
        """
        position = self.file_operations.seek_file(file_id, offset, whence)
        self.stats['operations_performed'] += 1
        return position
    
    def list_directory(self, path: str = '/') -> List[str]:
        """
        List directory contents.
        
        Args:
            path: Directory path
            
        Returns:
            List of entry names
        """
        if not self.mounted:
            return []
        
        inode = self._get_inode_by_path(path)
        if not inode or not self._is_directory(inode):
            return []
        
        directory = self.directories.get(inode.inode_num)
        if not directory:
            return []
        
        entries = directory.list_entries()
        self.stats['operations_performed'] += 1
        
        return entries
    
    def get_file_info(self, path: str) -> Optional[Dict[str, Any]]:
        """
        Get file information.
        
        Args:
            path: File path
            
        Returns:
            File information or None if not found
        """
        if not self.mounted:
            return None
        
        inode = self._get_inode_by_path(path)
        if not inode:
            return None
        
        info = inode.to_dict()
        self.stats['operations_performed'] += 1
        
        return info
    
    def _parse_path(self, path: str) -> List[str]:
        """Parse file path into components."""
        if not path or path == '/':
            return ['/']
        
        # Remove leading slash and split
        parts = path.lstrip('/').split('/')
        if not parts or parts == ['']:
            return ['/']
        
        return parts
    
    def _get_inode_by_path(self, path: str) -> Optional[TFSInode]:
        """Get inode by path."""
        if path == '/':
            return self.root_inode
        
        path_parts = self._parse_path(path)
        if not path_parts:
            return None
        
        current_inode = self.root_inode
        
        for part in path_parts:
            if not self._is_directory(current_inode):
                return None
            
            directory = self.directories.get(current_inode.inode_num)
            if not directory:
                return None
            
            entry = directory.get_entry(part)
            if not entry:
                return None
            
            current_inode = self.inodes.get(entry.inode_num)
            if not current_inode:
                return None
        
        return current_inode
    
    def _is_directory(self, inode: TFSInode) -> bool:
        """Check if inode represents a directory."""
        return inode.file_type == FileType.DIRECTORY
    
    def _file_exists_in_directory(self, parent_inode: TFSInode, filename: str) -> bool:
        """Check if file exists in directory."""
        directory = self.directories.get(parent_inode.inode_num)
        if not directory:
            return False
        
        return directory.has_entry(filename)
    
    def _update_stats_for_file_type(self, file_type: FileType, change: int) -> None:
        """Update statistics for file type."""
        if file_type == FileType.REGULAR:
            self.stats['total_files'] += change
        elif file_type == FileType.DIRECTORY:
            self.stats['total_directories'] += change
        elif file_type == FileType.SYMLINK:
            self.stats['total_links'] += change
    
    def get_filesystem_info(self) -> Dict[str, Any]:
        """Get filesystem information."""
        return {
            'mounted': self.mounted,
            'superblock': self.superblock.get_filesystem_info(),
            'stats': self.stats.copy(),
            'open_files': len(self.file_operations.get_open_files())
        }
    
    def get_filesystem_stats(self) -> Dict[str, Any]:
        """Get filesystem statistics."""
        return self.stats.copy()
    
    def check_filesystem(self) -> Dict[str, Any]:
        """Check filesystem consistency."""
        return self.superblock.check_filesystem()
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryFileSystem(mounted={self.mounted}, files={len(self.inodes)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryFileSystem(mounted={self.mounted}, "
                f"files={len(self.inodes)}, directories={len(self.directories)})")
