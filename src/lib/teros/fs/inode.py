"""
TFS Inode implementation.

This module provides the inode structure for the Ternary File System,
including file metadata and data block management.
"""

from typing import Dict, List, Optional, Any, Union
import time
import stat
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class FileType(Enum):
    """File types in TFS."""
    REGULAR = "regular"
    DIRECTORY = "directory"
    SYMLINK = "symlink"
    SOCKET = "socket"
    PIPE = "pipe"
    BLOCK_DEVICE = "block_device"
    CHAR_DEVICE = "char_device"


class TFSInode:
    """
    TFS Inode - File metadata and data block management.
    
    Represents a file or directory in the Ternary File System,
    containing metadata and pointers to data blocks.
    """
    
    def __init__(self, inode_num: int, file_type: FileType = FileType.REGULAR):
        """
        Initialize TFS inode.
        
        Args:
            inode_num: Inode number
            file_type: Type of file
        """
        self.inode_num = inode_num
        self.file_type = file_type
        
        # File permissions (ternary-based)
        self.permissions = 0o755  # Default permissions
        self.owner_uid = 0
        self.group_gid = 0
        
        # File size and blocks
        self.size = 0
        self.blocks_used = 0
        self.block_size = 4096  # Default block size
        
        # Timestamps
        self.creation_time = time.time()
        self.last_access_time = time.time()
        self.last_modification_time = time.time()
        self.last_change_time = time.time()
        
        # Link count
        self.link_count = 1
        
        # Data block pointers (ternary-based addressing)
        self.direct_blocks = [None] * 12  # Direct block pointers
        self.single_indirect = None  # Single indirect block
        self.double_indirect = None  # Double indirect block
        self.triple_indirect = None  # Triple indirect block
        
        # Extended attributes
        self.extended_attributes = {}
        
        # File flags
        self.flags = 0
        
        # Device information (for device files)
        self.device_major = 0
        self.device_minor = 0
        
        # Ternary-specific metadata
        self.ternary_metadata = {
            'trit_count': 0,
            'ternary_encoding': 'balanced',
            'ternary_compression': False,
            'ternary_checksum': None
        }
    
    def set_permissions(self, permissions: int) -> None:
        """
        Set file permissions.
        
        Args:
            permissions: Permission bits
        """
        self.permissions = permissions
        self.last_change_time = time.time()
    
    def get_permissions(self) -> int:
        """Get file permissions."""
        return self.permissions
    
    def set_owner(self, uid: int, gid: int) -> None:
        """
        Set file owner and group.
        
        Args:
            uid: User ID
            gid: Group ID
        """
        self.owner_uid = uid
        self.group_gid = gid
        self.last_change_time = time.time()
    
    def get_owner(self) -> tuple:
        """Get file owner and group."""
        return (self.owner_uid, self.group_gid)
    
    def set_size(self, size: int) -> None:
        """
        Set file size.
        
        Args:
            size: New file size
        """
        self.size = size
        self.last_modification_time = time.time()
        self.last_change_time = time.time()
    
    def get_size(self) -> int:
        """Get file size."""
        return self.size
    
    def add_block(self, block_num: int) -> bool:
        """
        Add a data block to the inode.
        
        Args:
            block_num: Block number to add
            
        Returns:
            True if added successfully, False otherwise
        """
        # Try direct blocks first
        for i in range(len(self.direct_blocks)):
            if self.direct_blocks[i] is None:
                self.direct_blocks[i] = block_num
                self.blocks_used += 1
                return True
        
        # Try single indirect block
        if self.single_indirect is None:
            # Would allocate single indirect block here
            self.single_indirect = block_num
            self.blocks_used += 1
            return True
        
        # For simplicity, we'll limit to direct blocks and single indirect
        # In a full implementation, would handle double and triple indirect
        return False
    
    def remove_block(self, block_num: int) -> bool:
        """
        Remove a data block from the inode.
        
        Args:
            block_num: Block number to remove
            
        Returns:
            True if removed successfully, False otherwise
        """
        # Check direct blocks
        for i in range(len(self.direct_blocks)):
            if self.direct_blocks[i] == block_num:
                self.direct_blocks[i] = None
                self.blocks_used -= 1
                return True
        
        # Check single indirect block
        if self.single_indirect == block_num:
            self.single_indirect = None
            self.blocks_used -= 1
            return True
        
        return False
    
    def get_blocks(self) -> List[int]:
        """Get all data block numbers."""
        blocks = []
        
        # Add direct blocks
        for block in self.direct_blocks:
            if block is not None:
                blocks.append(block)
        
        # Add single indirect block
        if self.single_indirect is not None:
            blocks.append(self.single_indirect)
        
        return blocks
    
    def update_access_time(self) -> None:
        """Update last access time."""
        self.last_access_time = time.time()
    
    def update_modification_time(self) -> None:
        """Update last modification time."""
        self.last_modification_time = time.time()
        self.last_change_time = time.time()
    
    def increment_link_count(self) -> None:
        """Increment link count."""
        self.link_count += 1
        self.last_change_time = time.time()
    
    def decrement_link_count(self) -> None:
        """Decrement link count."""
        if self.link_count > 0:
            self.link_count -= 1
            self.last_change_time = time.time()
    
    def get_link_count(self) -> int:
        """Get link count."""
        return self.link_count
    
    def is_directory(self) -> bool:
        """Check if inode represents a directory."""
        return self.file_type == FileType.DIRECTORY
    
    def is_regular_file(self) -> bool:
        """Check if inode represents a regular file."""
        return self.file_type == FileType.REGULAR
    
    def is_symlink(self) -> bool:
        """Check if inode represents a symbolic link."""
        return self.file_type == FileType.SYMLINK
    
    def is_device(self) -> bool:
        """Check if inode represents a device file."""
        return self.file_type in [FileType.BLOCK_DEVICE, FileType.CHAR_DEVICE]
    
    def set_device_info(self, major: int, minor: int) -> None:
        """
        Set device information.
        
        Args:
            major: Major device number
            minor: Minor device number
        """
        self.device_major = major
        self.device_minor = minor
        self.last_change_time = time.time()
    
    def get_device_info(self) -> tuple:
        """Get device information."""
        return (self.device_major, self.device_minor)
    
    def set_extended_attribute(self, name: str, value: Any) -> None:
        """
        Set extended attribute.
        
        Args:
            name: Attribute name
            value: Attribute value
        """
        self.extended_attributes[name] = value
        self.last_change_time = time.time()
    
    def get_extended_attribute(self, name: str) -> Optional[Any]:
        """
        Get extended attribute.
        
        Args:
            name: Attribute name
            
        Returns:
            Attribute value or None if not found
        """
        return self.extended_attributes.get(name)
    
    def remove_extended_attribute(self, name: str) -> bool:
        """
        Remove extended attribute.
        
        Args:
            name: Attribute name
            
        Returns:
            True if removed, False if not found
        """
        if name in self.extended_attributes:
            del self.extended_attributes[name]
            self.last_change_time = time.time()
            return True
        return False
    
    def set_ternary_metadata(self, key: str, value: Any) -> None:
        """
        Set ternary-specific metadata.
        
        Args:
            key: Metadata key
            value: Metadata value
        """
        self.ternary_metadata[key] = value
        self.last_change_time = time.time()
    
    def get_ternary_metadata(self, key: str) -> Optional[Any]:
        """
        Get ternary-specific metadata.
        
        Args:
            key: Metadata key
            
        Returns:
            Metadata value or None if not found
        """
        return self.ternary_metadata.get(key)
    
    def calculate_checksum(self) -> str:
        """Calculate ternary checksum for the inode."""
        # Simple checksum calculation
        checksum_data = f"{self.inode_num}{self.size}{self.blocks_used}{self.creation_time}"
        return str(hash(checksum_data))
    
    def verify_checksum(self) -> bool:
        """Verify inode checksum."""
        current_checksum = self.calculate_checksum()
        stored_checksum = self.ternary_metadata.get('ternary_checksum')
        return current_checksum == stored_checksum
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert inode to dictionary."""
        return {
            'inode_num': self.inode_num,
            'file_type': self.file_type.value,
            'permissions': self.permissions,
            'owner_uid': self.owner_uid,
            'group_gid': self.group_gid,
            'size': self.size,
            'blocks_used': self.blocks_used,
            'creation_time': self.creation_time,
            'last_access_time': self.last_access_time,
            'last_modification_time': self.last_modification_time,
            'last_change_time': self.last_change_time,
            'link_count': self.link_count,
            'direct_blocks': self.direct_blocks.copy(),
            'single_indirect': self.single_indirect,
            'double_indirect': self.double_indirect,
            'triple_indirect': self.triple_indirect,
            'extended_attributes': self.extended_attributes.copy(),
            'flags': self.flags,
            'device_major': self.device_major,
            'device_minor': self.device_minor,
            'ternary_metadata': self.ternary_metadata.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TFSInode(num={self.inode_num}, type={self.file_type.value}, size={self.size})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TFSInode(num={self.inode_num}, type={self.file_type.value}, "
                f"size={self.size}, blocks={self.blocks_used}, "
                f"links={self.link_count})")
