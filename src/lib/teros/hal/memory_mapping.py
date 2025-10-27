"""
Ternary Memory Mapping for Hardware Abstraction Layer.

This module provides memory mapping between binary RAM and ternary address space.
"""

from typing import Dict, List, Optional, Tuple, Union
import mmap
import os
import sys
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from .trit_encoder import TritCodec, Endianness


class MemoryProtection(Enum):
    """Memory protection levels."""
    READ_ONLY = "read_only"
    WRITE_ONLY = "write_only"
    READ_WRITE = "read_write"
    EXECUTE = "execute"
    NO_ACCESS = "no_access"


class MemorySegment(Enum):
    """Memory segment types."""
    CODE = "code"
    DATA = "data"
    STACK = "stack"
    HEAP = "heap"
    SHARED = "shared"


class TernaryMemoryMapper:
    """
    Ternary Memory Mapper - Maps ternary address space to binary RAM.
    
    Provides efficient mapping between ternary virtual addresses
    and binary physical memory with caching and optimization.
    """
    
    def __init__(self, memory_size: int = 1024 * 1024,  # 1MB default
                 page_size: int = 4096,
                 endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize ternary memory mapper.
        
        Args:
            memory_size: Total memory size in bytes
            page_size: Page size for memory management
            endianness: Byte order for memory operations
        """
        self.memory_size = memory_size
        self.page_size = page_size
        self.endianness = endianness
        self.codec = TritCodec(endianness)
        
        # Memory mapping
        self.binary_memory = None
        self.ternary_address_space = {}
        self.page_table = {}
        self.memory_protection = {}
        
        # Statistics
        self.stats = {
            'pages_allocated': 0,
            'pages_freed': 0,
            'cache_hits': 0,
            'cache_misses': 0,
            'memory_reads': 0,
            'memory_writes': 0
        }
        
        # Initialize memory
        self._initialize_memory()
    
    def _initialize_memory(self) -> None:
        """Initialize binary memory and mapping structures."""
        try:
            # Create memory-mapped file for binary memory
            self.binary_memory = mmap.mmap(-1, self.memory_size)
            
            # Initialize page table
            num_pages = self.memory_size // self.page_size
            for i in range(num_pages):
                self.page_table[i] = {
                    'physical_page': i,
                    'ternary_address': None,
                    'protection': MemoryProtection.READ_WRITE,
                    'segment': MemorySegment.HEAP,
                    'allocated': False
                }
            
            print(f"Initialized ternary memory mapper: {self.memory_size} bytes, "
                  f"{num_pages} pages of {self.page_size} bytes each")
                  
        except Exception as e:
            raise RuntimeError(f"Failed to initialize memory mapper: {e}")
    
    def map_ternary_address(self, ternary_addr: int, size: int,
                           protection: MemoryProtection = MemoryProtection.READ_WRITE,
                           segment: MemorySegment = MemorySegment.HEAP) -> bool:
        """
        Map ternary address to binary memory.
        
        Args:
            ternary_addr: Ternary virtual address
            size: Size in trits
            protection: Memory protection level
            segment: Memory segment type
            
        Returns:
            True if mapping successful, False otherwise
        """
        try:
            # Calculate required pages
            bytes_needed = (size + 3) // 4  # 4 trits per byte
            pages_needed = (bytes_needed + self.page_size - 1) // self.page_size
            
            # Find available pages
            available_pages = self._find_available_pages(pages_needed)
            if not available_pages:
                return False
            
            # Map ternary address to pages
            for i, page_num in enumerate(available_pages):
                self.page_table[page_num]['ternary_address'] = ternary_addr + (i * self.page_size)
                self.page_table[page_num]['protection'] = protection
                self.page_table[page_num]['segment'] = segment
                self.page_table[page_num]['allocated'] = True
                
                self.stats['pages_allocated'] += 1
            
            # Store mapping in ternary address space
            self.ternary_address_space[ternary_addr] = {
                'size': size,
                'pages': available_pages,
                'protection': protection,
                'segment': segment
            }
            
            return True
            
        except Exception as e:
            print(f"Failed to map ternary address {ternary_addr}: {e}")
            return False
    
    def unmap_ternary_address(self, ternary_addr: int) -> bool:
        """
        Unmap ternary address from binary memory.
        
        Args:
            ternary_addr: Ternary virtual address to unmap
            
        Returns:
            True if unmapping successful, False otherwise
        """
        if ternary_addr not in self.ternary_address_space:
            return False
        
        try:
            mapping = self.ternary_address_space[ternary_addr]
            
            # Free pages
            for page_num in mapping['pages']:
                self.page_table[page_num]['allocated'] = False
                self.page_table[page_num]['ternary_address'] = None
                self.stats['pages_freed'] += 1
            
            # Remove from ternary address space
            del self.ternary_address_space[ternary_addr]
            
            return True
            
        except Exception as e:
            print(f"Failed to unmap ternary address {ternary_addr}: {e}")
            return False
    
    def read_ternary(self, ternary_addr: int, size: int) -> List[Trit]:
        """
        Read trits from ternary address space.
        
        Args:
            ternary_addr: Ternary virtual address
            size: Number of trits to read
            
        Returns:
            List of Trit objects
            
        Raises:
            ValueError: If address is not mapped or protection violation
        """
        if ternary_addr not in self.ternary_address_space:
            raise ValueError(f"Ternary address {ternary_addr} not mapped")
        
        mapping = self.ternary_address_space[ternary_addr]
        if mapping['protection'] == MemoryProtection.WRITE_ONLY:
            raise ValueError(f"Read access denied for address {ternary_addr}")
        
        try:
            # Calculate binary address
            binary_addr = self._ternary_to_binary_address(ternary_addr)
            if binary_addr is None:
                raise ValueError(f"Cannot resolve ternary address {ternary_addr}")
            
            # Read binary data
            binary_data = self.binary_memory[binary_addr:binary_addr + (size + 3) // 4]
            
            # Decode to trits
            trits = self.codec.decode(binary_data)
            
            # Trim to requested size
            trits = trits[:size]
            
            self.stats['memory_reads'] += 1
            return trits
            
        except Exception as e:
            raise RuntimeError(f"Failed to read ternary address {ternary_addr}: {e}")
    
    def write_ternary(self, ternary_addr: int, trits: List[Trit]) -> bool:
        """
        Write trits to ternary address space.
        
        Args:
            ternary_addr: Ternary virtual address
            trits: List of Trit objects to write
            
        Returns:
            True if write successful, False otherwise
            
        Raises:
            ValueError: If address is not mapped or protection violation
        """
        if ternary_addr not in self.ternary_address_space:
            raise ValueError(f"Ternary address {ternary_addr} not mapped")
        
        mapping = self.ternary_address_space[ternary_addr]
        if mapping['protection'] == MemoryProtection.READ_ONLY:
            raise ValueError(f"Write access denied for address {ternary_addr}")
        
        try:
            # Calculate binary address
            binary_addr = self._ternary_to_binary_address(ternary_addr)
            if binary_addr is None:
                raise ValueError(f"Cannot resolve ternary address {ternary_addr}")
            
            # Encode trits to binary
            binary_data = self.codec.encode(trits)
            
            # Write binary data
            self.binary_memory[binary_addr:binary_addr + len(binary_data)] = binary_data
            
            self.stats['memory_writes'] += 1
            return True
            
        except Exception as e:
            print(f"Failed to write ternary address {ternary_addr}: {e}")
            return False
    
    def _ternary_to_binary_address(self, ternary_addr: int) -> Optional[int]:
        """Convert ternary address to binary address."""
        if ternary_addr not in self.ternary_address_space:
            return None
        
        mapping = self.ternary_address_space[ternary_addr]
        first_page = mapping['pages'][0]
        
        # Calculate offset within page
        page_offset = ternary_addr % self.page_size
        
        # Calculate binary address
        binary_addr = (first_page * self.page_size) + page_offset
        
        return binary_addr
    
    def _find_available_pages(self, count: int) -> List[int]:
        """Find available pages for allocation."""
        available_pages = []
        
        for page_num, page_info in self.page_table.items():
            if not page_info['allocated']:
                available_pages.append(page_num)
                if len(available_pages) >= count:
                    break
        
        return available_pages if len(available_pages) >= count else []
    
    def get_memory_stats(self) -> dict:
        """Get memory mapping statistics."""
        return {
            'total_memory': self.memory_size,
            'page_size': self.page_size,
            'total_pages': len(self.page_table),
            'allocated_pages': sum(1 for p in self.page_table.values() if p['allocated']),
            'ternary_mappings': len(self.ternary_address_space),
            **self.stats
        }
    
    def cleanup(self) -> None:
        """Cleanup memory resources."""
        if self.binary_memory:
            self.binary_memory.close()
            self.binary_memory = None
        
        # Clear mappings
        self.ternary_address_space.clear()
        self.page_table.clear()
        
        print("Ternary memory mapper cleaned up")
    
    def __del__(self):
        """Destructor - ensure cleanup."""
        self.cleanup()
