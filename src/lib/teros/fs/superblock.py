"""
TFS Superblock implementation.

This module provides the superblock structure for the Ternary File System,
including filesystem metadata and management information.
"""

from typing import Dict, List, Optional, Any
import time
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TFSSuperblock:
    """
    TFS Superblock - Filesystem metadata and management.
    
    Contains essential filesystem information including block counts,
    inode information, and filesystem state.
    """
    
    def __init__(self, total_blocks: int = 10000, block_size: int = 4096):
        """
        Initialize TFS superblock.
        
        Args:
            total_blocks: Total number of blocks in filesystem
            block_size: Size of each block in bytes
        """
        # Filesystem identification
        self.magic_number = 0x5445524F  # "TERO" in hex
        self.version = 1
        self.filesystem_name = "TFS"
        
        # Block information
        self.total_blocks = total_blocks
        self.block_size = block_size
        self.blocks_per_group = 8192  # Blocks per block group
        self.total_block_groups = (total_blocks + self.blocks_per_group - 1) // self.blocks_per_group
        
        # Inode information
        self.inodes_per_group = 1024
        self.total_inodes = self.total_block_groups * self.inodes_per_group
        self.free_inodes = self.total_inodes
        
        # Block allocation
        self.free_blocks = total_blocks - 100  # Reserve some blocks for metadata
        self.reserved_blocks = 100
        
        # Timestamps
        self.creation_time = time.time()
        self.last_mount_time = 0
        self.last_write_time = 0
        
        # Filesystem state
        self.mount_count = 0
        self.max_mount_count = 1000
        self.state = "clean"  # clean, dirty, error
        self.errors = "continue"  # continue, remount-ro, panic
        
        # Block group descriptors
        self.block_groups = []
        self._initialize_block_groups()
        
        # Filesystem statistics
        self.stats = {
            'total_files': 0,
            'total_directories': 0,
            'total_links': 0,
            'total_symlinks': 0,
            'total_sockets': 0,
            'total_pipes': 0,
            'total_devices': 0,
            'bytes_used': 0,
            'bytes_free': 0
        }
    
    def _initialize_block_groups(self) -> None:
        """Initialize block group descriptors."""
        for group_id in range(self.total_block_groups):
            group = {
                'id': group_id,
                'block_bitmap': group_id * self.blocks_per_group + 1,
                'inode_bitmap': group_id * self.blocks_per_group + 2,
                'inode_table': group_id * self.blocks_per_group + 3,
                'free_blocks': self.blocks_per_group - 3,  # Reserve for metadata
                'free_inodes': self.inodes_per_group,
                'used_dirs': 0
            }
            self.block_groups.append(group)
    
    def mount(self) -> bool:
        """
        Mount the filesystem.
        
        Returns:
            True if mount successful, False otherwise
        """
        if self.state == "error":
            return False
        
        self.mount_count += 1
        self.last_mount_time = time.time()
        self.state = "dirty"  # Mark as dirty when mounted
        
        return True
    
    def unmount(self) -> bool:
        """
        Unmount the filesystem.
        
        Returns:
            True if unmount successful, False otherwise
        """
        if self.state == "error":
            return False
        
        self.last_write_time = time.time()
        self.state = "clean"
        
        return True
    
    def allocate_block(self) -> Optional[int]:
        """
        Allocate a free block.
        
        Returns:
            Block number if allocated, None if no free blocks
        """
        if self.free_blocks <= 0:
            return None
        
        # Find a free block in block groups
        for group in self.block_groups:
            if group['free_blocks'] > 0:
                # Find specific free block in this group
                block_num = self._find_free_block_in_group(group['id'])
                if block_num is not None:
                    group['free_blocks'] -= 1
                    self.free_blocks -= 1
                    return block_num
        
        return None
    
    def deallocate_block(self, block_num: int) -> bool:
        """
        Deallocate a block.
        
        Args:
            block_num: Block number to deallocate
            
        Returns:
            True if deallocated successfully, False otherwise
        """
        group_id = block_num // self.blocks_per_group
        if 0 <= group_id < len(self.block_groups):
            group = self.block_groups[group_id]
            group['free_blocks'] += 1
            self.free_blocks += 1
            return True
        return False
    
    def allocate_inode(self) -> Optional[int]:
        """
        Allocate a free inode.
        
        Returns:
            Inode number if allocated, None if no free inodes
        """
        if self.free_inodes <= 0:
            return None
        
        # Find a free inode in block groups
        for group in self.block_groups:
            if group['free_inodes'] > 0:
                # Find specific free inode in this group
                inode_num = self._find_free_inode_in_group(group['id'])
                if inode_num is not None:
                    group['free_inodes'] -= 1
                    self.free_inodes -= 1
                    return inode_num
        
        return None
    
    def deallocate_inode(self, inode_num: int) -> bool:
        """
        Deallocate an inode.
        
        Args:
            inode_num: Inode number to deallocate
            
        Returns:
            True if deallocated successfully, False otherwise
        """
        group_id = inode_num // self.inodes_per_group
        if 0 <= group_id < len(self.block_groups):
            group = self.block_groups[group_id]
            group['free_inodes'] += 1
            self.free_inodes += 1
            return True
        return False
    
    def _find_free_block_in_group(self, group_id: int) -> Optional[int]:
        """Find a free block in a specific group."""
        # This would typically involve checking a bitmap
        # For now, return a simple calculation
        if group_id < len(self.block_groups):
            group = self.block_groups[group_id]
            if group['free_blocks'] > 0:
                # Simple allocation - in real implementation would check bitmap
                return group_id * self.blocks_per_group + 10 + (self.blocks_per_group - group['free_blocks'])
        return None
    
    def _find_free_inode_in_group(self, group_id: int) -> Optional[int]:
        """Find a free inode in a specific group."""
        # This would typically involve checking a bitmap
        # For now, return a simple calculation
        if group_id < len(self.block_groups):
            group = self.block_groups[group_id]
            if group['free_inodes'] > 0:
                # Simple allocation - in real implementation would check bitmap
                return group_id * self.inodes_per_group + (self.inodes_per_group - group['free_inodes'])
        return None
    
    def get_filesystem_info(self) -> Dict[str, Any]:
        """Get filesystem information."""
        return {
            'magic_number': self.magic_number,
            'version': self.version,
            'filesystem_name': self.filesystem_name,
            'total_blocks': self.total_blocks,
            'block_size': self.block_size,
            'free_blocks': self.free_blocks,
            'total_inodes': self.total_inodes,
            'free_inodes': self.free_inodes,
            'mount_count': self.mount_count,
            'max_mount_count': self.max_mount_count,
            'state': self.state,
            'creation_time': self.creation_time,
            'last_mount_time': self.last_mount_time,
            'last_write_time': self.last_write_time
        }
    
    def get_block_group_info(self, group_id: int) -> Optional[Dict[str, Any]]:
        """Get block group information."""
        if 0 <= group_id < len(self.block_groups):
            return self.block_groups[group_id].copy()
        return None
    
    def update_stats(self, file_type: str, size_change: int = 0) -> None:
        """
        Update filesystem statistics.
        
        Args:
            file_type: Type of file (file, directory, link, etc.)
            size_change: Change in size (positive for creation, negative for deletion)
        """
        if file_type == 'file':
            if size_change > 0:
                self.stats['total_files'] += 1
            else:
                self.stats['total_files'] -= 1
        elif file_type == 'directory':
            if size_change > 0:
                self.stats['total_directories'] += 1
            else:
                self.stats['total_directories'] -= 1
        elif file_type == 'link':
            if size_change > 0:
                self.stats['total_links'] += 1
            else:
                self.stats['total_links'] -= 1
        elif file_type == 'symlink':
            if size_change > 0:
                self.stats['total_symlinks'] += 1
            else:
                self.stats['total_symlinks'] -= 1
        
        self.stats['bytes_used'] += size_change
        self.stats['bytes_free'] = self.free_blocks * self.block_size
    
    def check_filesystem(self) -> Dict[str, Any]:
        """
        Check filesystem consistency.
        
        Returns:
            Dictionary with check results
        """
        issues = []
        
        # Check if filesystem is in error state
        if self.state == "error":
            issues.append("Filesystem is in error state")
        
        # Check mount count
        if self.mount_count >= self.max_mount_count:
            issues.append("Maximum mount count reached")
        
        # Check free space
        if self.free_blocks < self.reserved_blocks:
            issues.append("Insufficient free blocks")
        
        if self.free_inodes < 10:
            issues.append("Insufficient free inodes")
        
        return {
            'consistent': len(issues) == 0,
            'issues': issues,
            'free_blocks_percent': (self.free_blocks / self.total_blocks) * 100,
            'free_inodes_percent': (self.free_inodes / self.total_inodes) * 100
        }
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert superblock to dictionary."""
        return {
            'magic_number': self.magic_number,
            'version': self.version,
            'filesystem_name': self.filesystem_name,
            'total_blocks': self.total_blocks,
            'block_size': self.block_size,
            'blocks_per_group': self.blocks_per_group,
            'total_block_groups': self.total_block_groups,
            'inodes_per_group': self.inodes_per_group,
            'total_inodes': self.total_inodes,
            'free_inodes': self.free_inodes,
            'free_blocks': self.free_blocks,
            'reserved_blocks': self.reserved_blocks,
            'creation_time': self.creation_time,
            'last_mount_time': self.last_mount_time,
            'last_write_time': self.last_write_time,
            'mount_count': self.mount_count,
            'max_mount_count': self.max_mount_count,
            'state': self.state,
            'errors': self.errors,
            'stats': self.stats.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TFSSuperblock(name={self.filesystem_name}, blocks={self.total_blocks}, free={self.free_blocks})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TFSSuperblock(name={self.filesystem_name}, version={self.version}, "
                f"blocks={self.total_blocks}/{self.free_blocks}, "
                f"inodes={self.total_inodes}/{self.free_inodes})")
