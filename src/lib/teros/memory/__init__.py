"""
Ternary Memory Management implementation.

This module provides advanced memory management for the TEROS system,
including paging, buddy allocation, and garbage collection.
"""

from .memory_manager import TernaryMemoryManager
from .paging import TernaryPage, TernaryPageTable
from .buddy_allocator import TernaryBuddyAllocator
from .garbage_collector import TernaryGarbageCollector
from .memory_protection import TernaryMemoryProtection

__all__ = [
    "TernaryMemoryManager",
    "TernaryPage",
    "TernaryPageTable", 
    "TernaryBuddyAllocator",
    "TernaryGarbageCollector",
    "TernaryMemoryProtection",
]
