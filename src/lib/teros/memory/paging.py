"""
Ternary Paging System - Virtual memory management for TEROS.

This module provides paging functionality for the TEROS memory system,
including page tables and virtual-to-physical address translation.
"""

from typing import Dict, List, Optional, Any, Union
import time
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryPage:
    """
    Ternary memory page implementation.
    
    A page represents a fixed-size block of ternary memory (729 trits = 3^6).
    """
    
    def __init__(self, page_id: int, size: int = 729):
        """
        Initialize a ternary page.
        
        Args:
            page_id: Unique page identifier
            size: Page size in trits (default: 729 = 3^6)
        """
        self.page_id = page_id
        self.size = size
        self.data = [Trit(0) for _ in range(size)]
        self.accessed = False
        self.modified = False
        self.timestamp = time.time()
        self.reference_count = 0
    
    def load_trit(self, offset: int) -> Trit:
        """
        Load a trit from the page.
        
        Args:
            offset: Offset within the page
            
        Returns:
            Trit at the specified offset
        """
        if 0 <= offset < self.size:
            self.accessed = True
            self.timestamp = time.time()
            return self.data[offset]
        else:
            raise IndexError(f"Page offset {offset} out of range [0, {self.size})")
    
    def store_trit(self, offset: int, trit: Trit) -> None:
        """
        Store a trit in the page.
        
        Args:
            offset: Offset within the page
            trit: Trit to store
        """
        if 0 <= offset < self.size:
            self.data[offset] = trit
            self.accessed = True
            self.modified = True
            self.timestamp = time.time()
        else:
            raise IndexError(f"Page offset {offset} out of range [0, {self.size})")
    
    def load_tritarray(self, offset: int, size: int) -> TritArray:
        """
        Load a TritArray from the page.
        
        Args:
            offset: Starting offset within the page
            size: Number of trits to load
            
        Returns:
            TritArray containing the data
        """
        if 0 <= offset < self.size and offset + size <= self.size:
            self.accessed = True
            self.timestamp = time.time()
            trits = [self.data[offset + i] for i in range(size)]
            return TritArray(trits)
        else:
            raise IndexError(f"Page range [{offset}, {offset + size}) out of range [0, {self.size})")
    
    def store_tritarray(self, offset: int, tritarray: TritArray) -> None:
        """
        Store a TritArray in the page.
        
        Args:
            offset: Starting offset within the page
            tritarray: TritArray to store
        """
        if 0 <= offset < self.size and offset + len(tritarray) <= self.size:
            for i, trit in enumerate(tritarray):
                self.data[offset + i] = trit
            self.accessed = True
            self.modified = True
            self.timestamp = time.time()
        else:
            raise IndexError(f"Page range [{offset}, {offset + len(tritarray)}) out of range [0, {self.size})")
    
    def clear(self) -> None:
        """Clear the page data."""
        self.data = [Trit(0) for _ in range(self.size)]
        self.accessed = False
        self.modified = False
        self.timestamp = time.time()
    
    def is_empty(self) -> bool:
        """Check if the page is empty (all zeros)."""
        return all(trit.value == 0 for trit in self.data)
    
    def get_usage(self) -> float:
        """Get page usage percentage."""
        non_zero_count = sum(1 for trit in self.data if trit.value != 0)
        return non_zero_count / self.size
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryPage(id={self.page_id}, size={self.size}, accessed={self.accessed})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryPage(id={self.page_id}, size={self.size}, accessed={self.accessed}, "
                f"modified={self.modified}, usage={self.get_usage():.2f})")


class TernaryPageTable:
    """
    Ternary page table implementation.
    
    Manages virtual-to-physical page mapping and provides
    address translation for the ternary memory system.
    """
    
    def __init__(self, num_pages: int, page_size: int = 729):
        """
        Initialize the page table.
        
        Args:
            num_pages: Number of physical pages
            page_size: Size of each page in trits
        """
        self.num_pages = num_pages
        self.page_size = page_size
        
        # Page table entries: virtual_page -> physical_page
        self.page_table = {}
        
        # Reverse mapping: physical_page -> virtual_page
        self.reverse_table = {}
        
        # Page metadata
        self.page_metadata = {}
        
        # Free pages
        self.free_pages = set(range(num_pages))
        
        # Statistics
        self.stats = {
            'total_pages': num_pages,
            'allocated_pages': 0,
            'free_pages': num_pages,
            'page_faults': 0,
            'translations': 0
        }
    
    def map_page(self, virtual_address: int, physical_page: int) -> bool:
        """
        Map a virtual page to a physical page.
        
        Args:
            virtual_address: Virtual address
            physical_page: Physical page number
            
        Returns:
            True if mapping successful, False otherwise
        """
        virtual_page = virtual_address // self.page_size
        
        # Check if physical page is available
        if physical_page not in self.free_pages:
            return False
        
        # Check if virtual page is already mapped
        if virtual_page in self.page_table:
            return False
        
        # Create mapping
        self.page_table[virtual_page] = physical_page
        self.reverse_table[physical_page] = virtual_page
        self.free_pages.remove(physical_page)
        
        # Initialize page metadata
        self.page_metadata[physical_page] = {
            'virtual_page': virtual_page,
            'accessed': False,
            'modified': False,
            'timestamp': time.time(),
            'reference_count': 0
        }
        
        # Update statistics
        self.stats['allocated_pages'] += 1
        self.stats['free_pages'] -= 1
        
        return True
    
    def unmap_page(self, virtual_address: int) -> bool:
        """
        Unmap a virtual page.
        
        Args:
            virtual_address: Virtual address
            
        Returns:
            True if unmapping successful, False otherwise
        """
        virtual_page = virtual_address // self.page_size
        
        if virtual_page not in self.page_table:
            return False
        
        physical_page = self.page_table[virtual_page]
        
        # Remove mappings
        del self.page_table[virtual_page]
        del self.reverse_table[physical_page]
        del self.page_metadata[physical_page]
        
        # Add physical page back to free list
        self.free_pages.add(physical_page)
        
        # Update statistics
        self.stats['allocated_pages'] -= 1
        self.stats['free_pages'] += 1
        
        return True
    
    def get_physical_page(self, virtual_address: int) -> Optional[int]:
        """
        Get the physical page for a virtual address.
        
        Args:
            virtual_address: Virtual address
            
        Returns:
            Physical page number, or None if not mapped
        """
        virtual_page = virtual_address // self.page_size
        
        if virtual_page in self.page_table:
            self.stats['translations'] += 1
            return self.page_table[virtual_page]
        else:
            self.stats['page_faults'] += 1
            return None
    
    def is_page_mapped(self, virtual_address: int) -> bool:
        """
        Check if a virtual page is mapped.
        
        Args:
            virtual_address: Virtual address
            
        Returns:
            True if page is mapped, False otherwise
        """
        virtual_page = virtual_address // self.page_size
        return virtual_page in self.page_table
    
    def get_page_metadata(self, virtual_address: int) -> Optional[Dict[str, Any]]:
        """
        Get metadata for a virtual page.
        
        Args:
            virtual_address: Virtual address
            
        Returns:
            Page metadata, or None if not mapped
        """
        virtual_page = virtual_address // self.page_size
        
        if virtual_page in self.page_table:
            physical_page = self.page_table[virtual_page]
            return self.page_metadata.get(physical_page)
        
        return None
    
    def update_page_metadata(self, virtual_address: int, **kwargs) -> bool:
        """
        Update metadata for a virtual page.
        
        Args:
            virtual_address: Virtual address
            **kwargs: Metadata fields to update
            
        Returns:
            True if update successful, False otherwise
        """
        virtual_page = virtual_address // self.page_size
        
        if virtual_page in self.page_table:
            physical_page = self.page_table[virtual_page]
            if physical_page in self.page_metadata:
                self.page_metadata[physical_page].update(kwargs)
                return True
        
        return False
    
    def get_free_page(self) -> Optional[int]:
        """
        Get a free physical page.
        
        Returns:
            Free page number, or None if no free pages
        """
        if self.free_pages:
            return self.free_pages.pop()
        return None
    
    def return_free_page(self, physical_page: int) -> None:
        """
        Return a physical page to the free list.
        
        Args:
            physical_page: Physical page number
        """
        if 0 <= physical_page < self.num_pages:
            self.free_pages.add(physical_page)
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics."""
        return self.stats.copy()
    
    def get_page_usage(self) -> Dict[str, Any]:
        """Get page usage information."""
        total_pages = self.num_pages
        allocated_pages = len(self.page_table)
        free_pages = len(self.free_pages)
        
        return {
            'total_pages': total_pages,
            'allocated_pages': allocated_pages,
            'free_pages': free_pages,
            'usage_percentage': (allocated_pages / total_pages) * 100 if total_pages > 0 else 0
        }
    
    def defragment(self) -> Dict[str, Any]:
        """
        Defragment the page table.
        
        Returns:
            Defragmentation statistics
        """
        start_time = time.time()
        
        # Get all virtual pages
        virtual_pages = list(self.page_table.keys())
        virtual_pages.sort()
        
        # Get all free pages
        free_pages = list(self.free_pages)
        free_pages.sort()
        
        # Move pages to compact memory
        moved_count = 0
        new_physical = 0
        
        for virtual_page in virtual_pages:
            old_physical = self.page_table[virtual_page]
            
            if old_physical != new_physical:
                # Move page
                if new_physical in free_pages:
                    # Update mappings
                    del self.reverse_table[old_physical]
                    self.page_table[virtual_page] = new_physical
                    self.reverse_table[new_physical] = virtual_page
                    
                    # Update metadata
                    if old_physical in self.page_metadata:
                        metadata = self.page_metadata[old_physical]
                        del self.page_metadata[old_physical]
                        self.page_metadata[new_physical] = metadata
                    
                    # Update free pages
                    self.free_pages.remove(new_physical)
                    self.free_pages.add(old_physical)
                    
                    moved_count += 1
                    new_physical += 1
        
        defrag_time = time.time() - start_time
        
        return {
            'moved_pages': moved_count,
            'time': defrag_time
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryPageTable(pages={self.num_pages}, allocated={len(self.page_table)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryPageTable(pages={self.num_pages}, allocated={len(self.page_table)}, "
                f"free={len(self.free_pages)})")
