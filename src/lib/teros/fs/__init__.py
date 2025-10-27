"""
Ternary File System (TFS) implementation.

This module provides the Ternary File System for TEROS,
including superblock, inodes, directory entries, and file operations.
"""

from .tfs import TernaryFileSystem
from .superblock import TFSSuperblock
from .inode import TFSInode
from .directory import TFSDirectory
from .file_operations import TFSFileOperations

__all__ = [
    "TernaryFileSystem",
    "TFSSuperblock", 
    "TFSInode",
    "TFSDirectory",
    "TFSFileOperations",
]
