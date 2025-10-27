"""
Ternary Memory Manager - Advanced memory management for TEROS.

This module provides the main memory manager that coordinates
paging, allocation, garbage collection, and memory protection.
"""

from typing import Dict, List, Optional, Any, Union, Set
import time
from ..core.ternary_memory import TernaryMemory
from ..core.trit import Trit
from ..core.tritarray import TritArray
from .paging import TernaryPage, TernaryPageTable
from .buddy_allocator import TernaryBuddyAllocator
from .garbage_collector import TernaryGarbageCollector
from .memory_protection import TernaryMemoryProtection


class TernaryMemoryManager:
    """
    Advanced memory manager for TEROS.
    
    Coordinates paging, allocation, garbage collection, and memory protection
    to provide efficient and secure memory management for ternary programs.
    """
    
    def __init__(self, total_memory: int = 3**6, page_size: int = 3**3):
        """
        Initialize the memory manager.
        
        Args:
            total_memory: Total memory size in trits
            page_size: Page size in trits
        """
        self.total_memory = total_memory
        self.page_size = page_size
        self.num_pages = total_memory // page_size
        
        # Initialize components
        self.memory = TernaryMemory(total_memory)
        self.page_table = TernaryPageTable(self.num_pages)
        self.buddy_allocator = TernaryBuddyAllocator(self.num_pages)
        self.garbage_collector = TernaryGarbageCollector(self.memory)
        self.memory_protection = TernaryMemoryProtection()
        
        # Memory statistics
        self.stats = {
            'total_memory': total_memory,
            'used_memory': 0,
            'free_memory': total_memory,
            'allocated_pages': 0,
            'free_pages': self.num_pages,
            'allocation_count': 0,
            'deallocation_count': 0,
            'gc_runs': 0,
            'gc_collected': 0,
            'page_faults': 0,
            'protection_violations': 0
        }
        
        # Memory regions
        self.regions = {
            'kernel': {'start': 0, 'size': self.num_pages // 4, 'type': 'kernel'},
            'user': {'start': self.num_pages // 4, 'size': self.num_pages // 2, 'type': 'user'},
            'heap': {'start': 3 * self.num_pages // 4, 'size': self.num_pages // 4, 'type': 'heap'}
        }
        
        # Allocation tracking
        self.allocations = {}
        self.allocation_id = 0
    
    def allocate(self, size: int, region: str = 'heap', 
                protection: str = 'read_write') -> Optional[int]:
        """
        Allocate memory.
        
        Args:
            size: Size to allocate in trits
            region: Memory region to allocate from
            protection: Memory protection level
            
        Returns:
            Virtual address of allocated memory, or None if allocation failed
        """
        if size <= 0:
            return None
        
        # Calculate number of pages needed
        pages_needed = (size + self.page_size - 1) // self.page_size
        
        # Check if region has enough space
        if not self._check_region_capacity(region, pages_needed):
            return None
        
        # Allocate pages using buddy allocator
        page_indices = self.buddy_allocator.allocate(pages_needed)
        if not page_indices:
            return None
        
        # Set up page table entries
        virtual_address = self._allocate_virtual_address(region, pages_needed)
        if virtual_address is None:
            self.buddy_allocator.deallocate(page_indices)
            return None
        
        # Map virtual to physical pages
        for i, page_idx in enumerate(page_indices):
            self.page_table.map_page(virtual_address + i * self.page_size, page_idx)
        
        # Set memory protection
        self.memory_protection.set_protection(virtual_address, size, protection)
        
        # Track allocation
        allocation_id = self._track_allocation(virtual_address, size, region, protection)
        
        # Update statistics
        self.stats['used_memory'] += size
        self.stats['free_memory'] -= size
        self.stats['allocated_pages'] += pages_needed
        self.stats['free_pages'] -= pages_needed
        self.stats['allocation_count'] += 1
        
        return virtual_address
    
    def deallocate(self, address: int) -> bool:
        """
        Deallocate memory.
        
        Args:
            address: Virtual address to deallocate
            
        Returns:
            True if deallocation successful, False otherwise
        """
        if address not in self.allocations:
            return False
        
        allocation = self.allocations[address]
        size = allocation['size']
        pages_needed = (size + self.page_size - 1) // self.page_size
        
        # Get physical page indices
        page_indices = []
        for i in range(pages_needed):
            page_idx = self.page_table.get_physical_page(address + i * self.page_size)
            if page_idx is not None:
                page_indices.append(page_idx)
        
        # Unmap pages
        for i in range(pages_needed):
            self.page_table.unmap_page(address + i * self.page_size)
        
        # Deallocate physical pages
        self.buddy_allocator.deallocate(page_indices)
        
        # Remove protection
        self.memory_protection.remove_protection(address)
        
        # Remove from tracking
        del self.allocations[address]
        
        # Update statistics
        self.stats['used_memory'] -= size
        self.stats['free_memory'] += size
        self.stats['allocated_pages'] -= pages_needed
        self.stats['free_pages'] += pages_needed
        self.stats['deallocation_count'] += 1
        
        return True
    
    def read(self, address: int, size: int) -> Optional[TritArray]:
        """
        Read memory.
        
        Args:
            address: Virtual address to read from
            size: Number of trits to read
            
        Returns:
            TritArray containing the data, or None if read failed
        """
        # Check memory protection
        if not self.memory_protection.check_access(address, size, 'read'):
            self.stats['protection_violations'] += 1
            return None
        
        # Translate virtual address to physical
        physical_address = self._translate_address(address)
        if physical_address is None:
            self.stats['page_faults'] += 1
            return None
        
        # Read from physical memory
        return self.memory.load_tritarray(physical_address, size)
    
    def write(self, address: int, data: TritArray) -> bool:
        """
        Write memory.
        
        Args:
            address: Virtual address to write to
            data: Data to write
            
        Returns:
            True if write successful, False otherwise
        """
        # Check memory protection
        if not self.memory_protection.check_access(address, len(data), 'write'):
            self.stats['protection_violations'] += 1
            return False
        
        # Translate virtual address to physical
        physical_address = self._translate_address(address)
        if physical_address is None:
            self.stats['page_faults'] += 1
            return False
        
        # Write to physical memory
        self.memory.store_tritarray(physical_address, data)
        return True
    
    def garbage_collect(self) -> Dict[str, Any]:
        """
        Run garbage collection.
        
        Returns:
            Garbage collection statistics
        """
        start_time = time.time()
        
        # Run garbage collection
        collected = self.garbage_collector.collect()
        
        # Update statistics
        self.stats['gc_runs'] += 1
        self.stats['gc_collected'] += collected
        
        # Update memory statistics
        self._update_memory_stats()
        
        gc_time = time.time() - start_time
        
        return {
            'collected': collected,
            'time': gc_time,
            'total_runs': self.stats['gc_runs'],
            'total_collected': self.stats['gc_collected']
        }
    
    def defragment(self) -> Dict[str, Any]:
        """
        Defragment memory.
        
        Returns:
            Defragmentation statistics
        """
        start_time = time.time()
        
        # Get all allocations
        allocations = list(self.allocations.items())
        allocations.sort(key=lambda x: x[0])  # Sort by address
        
        # Move allocations to compact memory
        moved_count = 0
        new_address = 0
        
        for old_address, allocation in allocations:
            if old_address != new_address:
                # Move allocation
                size = allocation['size']
                data = self.read(old_address, size)
                if data:
                    # Deallocate old location
                    self.deallocate(old_address)
                    
                    # Allocate new location
                    new_addr = self.allocate(size, allocation['region'], allocation['protection'])
                    if new_addr:
                        self.write(new_addr, data)
                        moved_count += 1
                        new_address = new_addr + size
        
        defrag_time = time.time() - start_time
        
        return {
            'moved_allocations': moved_count,
            'time': defrag_time
        }
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics."""
        self._update_memory_stats()
        return self.stats.copy()
    
    def get_allocation_info(self, address: int) -> Optional[Dict[str, Any]]:
        """Get information about an allocation."""
        return self.allocations.get(address)
    
    def get_all_allocations(self) -> Dict[int, Dict[str, Any]]:
        """Get all current allocations."""
        return self.allocations.copy()
    
    def _check_region_capacity(self, region: str, pages_needed: int) -> bool:
        """Check if region has enough capacity."""
        if region not in self.regions:
            return False
        
        region_info = self.regions[region]
        available_pages = region_info['size']
        
        # Count currently allocated pages in region
        allocated_pages = 0
        for allocation in self.allocations.values():
            if allocation['region'] == region:
                size = allocation['size']
                allocated_pages += (size + self.page_size - 1) // self.page_size
        
        return (allocated_pages + pages_needed) <= available_pages
    
    def _allocate_virtual_address(self, region: str, pages_needed: int) -> Optional[int]:
        """Allocate a virtual address in the specified region."""
        if region not in self.regions:
            return None
        
        region_info = self.regions[region]
        start_page = region_info['start']
        end_page = start_page + region_info['size']
        
        # Find contiguous free pages
        for page in range(start_page, end_page - pages_needed + 1):
            if self._is_page_range_free(page, pages_needed):
                return page * self.page_size
        
        return None
    
    def _is_page_range_free(self, start_page: int, num_pages: int) -> bool:
        """Check if a range of pages is free."""
        for i in range(num_pages):
            if self.page_table.is_page_mapped(start_page + i):
                return False
        return True
    
    def _translate_address(self, virtual_address: int) -> Optional[int]:
        """Translate virtual address to physical address."""
        page_idx = virtual_address // self.page_size
        offset = virtual_address % self.page_size
        
        physical_page = self.page_table.get_physical_page(virtual_address)
        if physical_page is None:
            return None
        
        return physical_page * self.page_size + offset
    
    def _track_allocation(self, address: int, size: int, region: str, protection: str) -> int:
        """Track an allocation."""
        self.allocation_id += 1
        self.allocations[address] = {
            'id': self.allocation_id,
            'size': size,
            'region': region,
            'protection': protection,
            'timestamp': time.time()
        }
        return self.allocation_id
    
    def _update_memory_stats(self) -> None:
        """Update memory statistics."""
        total_allocated = sum(allocation['size'] for allocation in self.allocations.values())
        self.stats['used_memory'] = total_allocated
        self.stats['free_memory'] = self.total_memory - total_allocated
        
        total_pages = sum((allocation['size'] + self.page_size - 1) // self.page_size 
                         for allocation in self.allocations.values())
        self.stats['allocated_pages'] = total_pages
        self.stats['free_pages'] = self.num_pages - total_pages
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryMemoryManager(total={self.total_memory}, used={self.stats['used_memory']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryMemoryManager(total={self.total_memory}, used={self.stats['used_memory']}, "
                f"allocations={len(self.allocations)})")
