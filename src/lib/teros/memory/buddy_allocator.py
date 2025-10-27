"""
Ternary Buddy Allocator - Memory allocation for TEROS.

This module provides a buddy system allocator optimized for ternary memory,
using powers of 3 for efficient allocation and deallocation.
"""

from typing import Dict, List, Optional, Any, Union, Set
import math
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryBuddyAllocator:
    """
    Buddy system allocator for ternary memory.
    
    Uses powers of 3 for efficient allocation and deallocation,
    optimized for ternary memory layout.
    """
    
    def __init__(self, total_pages: int):
        """
        Initialize the buddy allocator.
        
        Args:
            total_pages: Total number of pages available
        """
        self.total_pages = total_pages
        
        # Calculate maximum power of 3 that fits in total_pages
        self.max_power = int(math.log(total_pages, 3))
        self.max_size = 3 ** self.max_power
        
        # Free lists for each power of 3
        self.free_lists = {}
        for power in range(self.max_power + 1):
            self.free_lists[power] = set()
        
        # Allocated blocks tracking
        self.allocated_blocks = {}
        self.block_id = 0
        
        # Statistics
        self.stats = {
            'total_pages': total_pages,
            'allocated_pages': 0,
            'free_pages': total_pages,
            'allocations': 0,
            'deallocations': 0,
            'splits': 0,
            'merges': 0,
            'fragmentation': 0.0
        }
        
        # Initialize with all pages free
        self._initialize_free_lists()
    
    def allocate(self, size: int) -> Optional[List[int]]:
        """
        Allocate memory of the specified size.
        
        Args:
            size: Number of pages to allocate
            
        Returns:
            List of page indices, or None if allocation failed
        """
        if size <= 0:
            return None
        
        # Find the smallest power of 3 that can accommodate the request
        required_power = self._get_required_power(size)
        if required_power > self.max_power:
            return None
        
        # Try to find a free block of the required size
        if required_power in self.free_lists and self.free_lists[required_power]:
            # Found exact size block
            block = self.free_lists[required_power].pop()
            return self._allocate_block(block, required_power, size)
        
        # Try to find a larger block and split it
        for power in range(required_power + 1, self.max_power + 1):
            if power in self.free_lists and self.free_lists[power]:
                block = self.free_lists[power].pop()
                return self._split_and_allocate(block, power, required_power, size)
        
        # No suitable block found
        return None
    
    def deallocate(self, page_indices: List[int]) -> bool:
        """
        Deallocate memory.
        
        Args:
            page_indices: List of page indices to deallocate
            
        Returns:
            True if deallocation successful, False otherwise
        """
        if not page_indices:
            return False
        
        # Find the block containing these pages
        block_id = None
        for bid, block_info in self.allocated_blocks.items():
            if set(page_indices).issubset(set(block_info['pages'])):
                block_id = bid
                break
        
        if block_id is None:
            return False
        
        block_info = self.allocated_blocks[block_id]
        start_page = block_info['start_page']
        power = block_info['power']
        
        # Remove from allocated blocks
        del self.allocated_blocks[block_id]
        
        # Add to free list
        self.free_lists[power].add(start_page)
        
        # Try to merge with buddies
        self._merge_buddies(start_page, power)
        
        # Update statistics
        self.stats['allocated_pages'] -= len(page_indices)
        self.stats['free_pages'] += len(page_indices)
        self.stats['deallocations'] += 1
        
        return True
    
    def get_free_pages(self) -> List[int]:
        """Get list of free pages."""
        free_pages = []
        for power, pages in self.free_lists.items():
            for start_page in pages:
                size = 3 ** power
                free_pages.extend(range(start_page, start_page + size))
        return sorted(free_pages)
    
    def get_allocated_pages(self) -> List[int]:
        """Get list of allocated pages."""
        allocated_pages = []
        for block_info in self.allocated_blocks.values():
            allocated_pages.extend(block_info['pages'])
        return sorted(allocated_pages)
    
    def get_fragmentation(self) -> float:
        """Calculate memory fragmentation."""
        total_free = sum(len(pages) * (3 ** power) for power, pages in self.free_lists.items())
        if total_free == 0:
            return 0.0
        
        # Calculate fragmentation as ratio of largest free block to total free
        max_free_block = max((3 ** power for power, pages in self.free_lists.items() if pages), default=0)
        return 1.0 - (max_free_block / total_free) if total_free > 0 else 0.0
    
    def defragment(self) -> Dict[str, Any]:
        """
        Defragment memory by compacting allocated blocks.
        
        Returns:
            Defragmentation statistics
        """
        # Get all allocated blocks
        blocks = list(self.allocated_blocks.items())
        blocks.sort(key=lambda x: x[1]['start_page'])
        
        # Move blocks to compact memory
        moved_count = 0
        new_start = 0
        
        for block_id, block_info in blocks:
            old_start = block_info['start_page']
            size = len(block_info['pages'])
            
            if old_start != new_start:
                # Move block
                new_pages = list(range(new_start, new_start + size))
                
                # Update block info
                self.allocated_blocks[block_id]['start_page'] = new_start
                self.allocated_blocks[block_id]['pages'] = new_pages
                
                moved_count += 1
                new_start += size
        
        return {
            'moved_blocks': moved_count,
            'fragmentation_before': self.get_fragmentation(),
            'fragmentation_after': self.get_fragmentation()
        }
    
    def _initialize_free_lists(self) -> None:
        """Initialize free lists with all pages."""
        # Start with all pages as one large block
        self.free_lists[self.max_power].add(0)
    
    def _get_required_power(self, size: int) -> int:
        """Get the required power of 3 for the given size."""
        power = 0
        while 3 ** power < size:
            power += 1
        return power
    
    def _allocate_block(self, start_page: int, power: int, size: int) -> List[int]:
        """Allocate a block of the specified size."""
        block_size = 3 ** power
        pages = list(range(start_page, start_page + size))
        
        # Track allocation
        self.block_id += 1
        self.allocated_blocks[self.block_id] = {
            'start_page': start_page,
            'power': power,
            'size': size,
            'pages': pages
        }
        
        # Update statistics
        self.stats['allocated_pages'] += size
        self.stats['free_pages'] -= size
        self.stats['allocations'] += 1
        
        return pages
    
    def _split_and_allocate(self, start_page: int, power: int, required_power: int, size: int) -> List[int]:
        """Split a larger block and allocate the required size."""
        # Split the block down to the required size
        current_power = power
        current_start = start_page
        
        while current_power > required_power:
            # Split into 3 parts
            part_size = 3 ** (current_power - 1)
            
            # Add the other two parts to free lists
            self.free_lists[current_power - 1].add(current_start + part_size)
            self.free_lists[current_power - 1].add(current_start + 2 * part_size)
            
            # Continue with the first part
            current_power -= 1
            self.stats['splits'] += 1
        
        # Allocate the final block
        return self._allocate_block(current_start, current_power, size)
    
    def _merge_buddies(self, start_page: int, power: int) -> None:
        """Try to merge a block with its buddies."""
        if power >= self.max_power:
            return
        
        # Calculate buddy addresses
        block_size = 3 ** power
        buddy1 = start_page + block_size
        buddy2 = start_page + 2 * block_size
        
        # Check if buddies are free
        if (buddy1 in self.free_lists[power] and 
            buddy2 in self.free_lists[power]):
            
            # Remove buddies from free lists
            self.free_lists[power].remove(buddy1)
            self.free_lists[power].remove(buddy2)
            
            # Add merged block to higher power free list
            self.free_lists[power + 1].add(start_page)
            
            # Update statistics
            self.stats['merges'] += 1
            
            # Try to merge with higher level buddies
            self._merge_buddies(start_page, power + 1)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get allocation statistics."""
        self.stats['fragmentation'] = self.get_fragmentation()
        return self.stats.copy()
    
    def get_memory_layout(self) -> Dict[str, Any]:
        """Get current memory layout."""
        layout = {
            'total_pages': self.total_pages,
            'allocated_pages': self.get_allocated_pages(),
            'free_pages': self.get_free_pages(),
            'free_lists': {
                power: list(pages) for power, pages in self.free_lists.items() if pages
            },
            'allocated_blocks': {
                bid: {
                    'start_page': info['start_page'],
                    'size': info['size'],
                    'power': info['power']
                }
                for bid, info in self.allocated_blocks.items()
            }
        }
        return layout
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryBuddyAllocator(pages={self.total_pages}, allocated={self.stats['allocated_pages']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryBuddyAllocator(pages={self.total_pages}, allocated={self.stats['allocated_pages']}, "
                f"fragmentation={self.get_fragmentation():.2f})")
