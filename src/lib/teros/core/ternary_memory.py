"""
TernaryMemory - Memory layout and management for ternary computing.

This module defines the memory layout and basic operations for the
Ternary Virtual Machine, including segmented memory architecture.
"""

from typing import List, Optional, Union, Dict, Any
import numpy as np
from .trit import Trit
from .tritarray import TritArray


class TernaryMemory:
    """
    Ternary memory implementation with segmented architecture.
    
    Memory is divided into segments:
    - Code: Program instructions
    - Data: Global variables and constants
    - Stack: Function calls and local variables
    - Heap: Dynamic allocation
    - Kernel: System data structures
    """
    
    # Memory segment types
    SEGMENT_CODE = "code"
    SEGMENT_DATA = "data"
    SEGMENT_STACK = "stack"
    SEGMENT_HEAP = "heap"
    SEGMENT_KERNEL = "kernel"
    
    # Memory layout constants
    DEFAULT_MEMORY_SIZE = 3**6  # 729 trits (3^6)
    PAGE_SIZE = 3**3  # 27 trits per page
    MAX_PAGES = 3**3  # 27 pages maximum
    
    def __init__(self, size: int = DEFAULT_MEMORY_SIZE):
        """
        Initialize ternary memory.
        
        Args:
            size: Total memory size in trits
        """
        self.size = size
        self.pages = (size + self.PAGE_SIZE - 1) // self.PAGE_SIZE
        self.memory = np.zeros(size, dtype=np.int8)
        self.segments = {}
        self.allocated_pages = set()
        self.free_pages = set(range(self.pages))
        
        # Initialize segments
        self._initialize_segments()
    
    def _initialize_segments(self) -> None:
        """Initialize memory segments."""
        # Calculate segment sizes (proportional to total memory)
        total_pages = self.pages
        code_pages = total_pages // 4
        data_pages = total_pages // 4
        stack_pages = total_pages // 4
        heap_pages = total_pages // 4
        kernel_pages = total_pages - (code_pages + data_pages + stack_pages + heap_pages)
        
        # Allocate segments
        self.segments = {
            self.SEGMENT_CODE: {
                'start': 0,
                'size': code_pages * self.PAGE_SIZE,
                'pages': code_pages,
                'type': self.SEGMENT_CODE
            },
            self.SEGMENT_DATA: {
                'start': code_pages * self.PAGE_SIZE,
                'size': data_pages * self.PAGE_SIZE,
                'pages': data_pages,
                'type': self.SEGMENT_DATA
            },
            self.SEGMENT_STACK: {
                'start': (code_pages + data_pages) * self.PAGE_SIZE,
                'size': stack_pages * self.PAGE_SIZE,
                'pages': stack_pages,
                'type': self.SEGMENT_STACK
            },
            self.SEGMENT_HEAP: {
                'start': (code_pages + data_pages + stack_pages) * self.PAGE_SIZE,
                'size': heap_pages * self.PAGE_SIZE,
                'pages': heap_pages,
                'type': self.SEGMENT_HEAP
            },
            self.SEGMENT_KERNEL: {
                'start': (code_pages + data_pages + stack_pages + heap_pages) * self.PAGE_SIZE,
                'size': kernel_pages * self.PAGE_SIZE,
                'pages': kernel_pages,
                'type': self.SEGMENT_KERNEL
            }
        }
    
    def get_segment(self, segment_type: str) -> Dict[str, Any]:
        """Get segment information."""
        if segment_type not in self.segments:
            raise ValueError(f"Invalid segment type: {segment_type}")
        return self.segments[segment_type]
    
    def get_segment_address(self, segment_type: str, offset: int = 0) -> int:
        """Get absolute address within a segment."""
        segment = self.get_segment(segment_type)
        return segment['start'] + offset
    
    def is_valid_address(self, address: int) -> bool:
        """Check if address is valid."""
        return 0 <= address < self.size
    
    def is_in_segment(self, address: int, segment_type: str) -> bool:
        """Check if address is within a specific segment."""
        segment = self.get_segment(segment_type)
        return segment['start'] <= address < segment['start'] + segment['size']
    
    def load_trit(self, address: int) -> Trit:
        """Load a single trit from memory."""
        if not self.is_valid_address(address):
            raise IndexError(f"Invalid memory address: {address}")
        return Trit(int(self.memory[address]))
    
    def store_trit(self, address: int, trit: Union[Trit, int]) -> None:
        """Store a single trit to memory."""
        if not self.is_valid_address(address):
            raise IndexError(f"Invalid memory address: {address}")
        
        if isinstance(trit, Trit):
            self.memory[address] = trit.value
        else:
            self.memory[address] = int(trit)
    
    def load_tritarray(self, address: int, size: int) -> TritArray:
        """Load a TritArray from memory."""
        if not self.is_valid_address(address) or not self.is_valid_address(address + size - 1):
            raise IndexError(f"Invalid memory range: {address} to {address + size - 1}")
        
        trits = []
        for i in range(size):
            trits.append(int(self.memory[address + i]))
        
        return TritArray(trits)
    
    def store_tritarray(self, address: int, tritarray: TritArray) -> None:
        """Store a TritArray to memory."""
        if not self.is_valid_address(address) or not self.is_valid_address(address + len(tritarray) - 1):
            raise IndexError(f"Invalid memory range: {address} to {address + len(tritarray) - 1}")
        
        for i, trit in enumerate(tritarray):
            self.memory[address + i] = trit.value
    
    def load_bytes(self, address: int, size: int) -> bytes:
        """Load bytes from memory (for binary compatibility)."""
        if not self.is_valid_address(address) or not self.is_valid_address(address + size - 1):
            raise IndexError(f"Invalid memory range: {address} to {address + size - 1}")
        
        return bytes(self.memory[address:address + size])
    
    def store_bytes(self, address: int, data: bytes) -> None:
        """Store bytes to memory (for binary compatibility)."""
        if not self.is_valid_address(address) or not self.is_valid_address(address + len(data) - 1):
            raise IndexError(f"Invalid memory range: {address} to {address + len(data) - 1}")
        
        self.memory[address:address + len(data)] = list(data)
    
    def clear_segment(self, segment_type: str) -> None:
        """Clear a memory segment."""
        segment = self.get_segment(segment_type)
        start = segment['start']
        size = segment['size']
        self.memory[start:start + size] = 0
    
    def clear_all(self) -> None:
        """Clear all memory."""
        self.memory.fill(0)
    
    def copy_segment(self, src_segment: str, dst_segment: str) -> None:
        """Copy one segment to another."""
        src = self.get_segment(src_segment)
        dst = self.get_segment(dst_segment)
        
        if src['size'] != dst['size']:
            raise ValueError(f"Segment size mismatch: {src['size']} != {dst['size']}")
        
        src_start = src['start']
        dst_start = dst['start']
        size = src['size']
        
        self.memory[dst_start:dst_start + size] = self.memory[src_start:src_start + size]
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory usage statistics."""
        total_pages = self.pages
        allocated_pages = len(self.allocated_pages)
        free_pages = len(self.free_pages)
        
        segment_stats = {}
        for segment_type, segment in self.segments.items():
            segment_stats[segment_type] = {
                'start': segment['start'],
                'size': segment['size'],
                'pages': segment['pages'],
                'usage': self._calculate_segment_usage(segment)
            }
        
        return {
            'total_size': self.size,
            'total_pages': total_pages,
            'allocated_pages': allocated_pages,
            'free_pages': free_pages,
            'segments': segment_stats
        }
    
    def _calculate_segment_usage(self, segment: Dict[str, Any]) -> float:
        """Calculate segment usage percentage."""
        start = segment['start']
        size = segment['size']
        
        # Count non-zero trits
        non_zero_count = np.count_nonzero(self.memory[start:start + size])
        return non_zero_count / size if size > 0 else 0.0
    
    def allocate_page(self) -> Optional[int]:
        """Allocate a free page."""
        if not self.free_pages:
            return None
        
        page = self.free_pages.pop()
        self.allocated_pages.add(page)
        return page
    
    def deallocate_page(self, page: int) -> None:
        """Deallocate a page."""
        if page in self.allocated_pages:
            self.allocated_pages.remove(page)
            self.free_pages.add(page)
            
            # Clear the page
            start = page * self.PAGE_SIZE
            end = start + self.PAGE_SIZE
            self.memory[start:end] = 0
    
    def get_page_address(self, page: int) -> int:
        """Get the starting address of a page."""
        return page * self.PAGE_SIZE
    
    def get_page_from_address(self, address: int) -> int:
        """Get the page number containing an address."""
        return address // self.PAGE_SIZE
    
    def is_page_allocated(self, page: int) -> bool:
        """Check if a page is allocated."""
        return page in self.allocated_pages
    
    def get_page_contents(self, page: int) -> np.ndarray:
        """Get the contents of a page."""
        if not self.is_page_allocated(page):
            raise ValueError(f"Page {page} is not allocated")
        
        start = self.get_page_address(page)
        end = start + self.PAGE_SIZE
        return self.memory[start:end].copy()
    
    def set_page_contents(self, page: int, contents: np.ndarray) -> None:
        """Set the contents of a page."""
        if not self.is_page_allocated(page):
            raise ValueError(f"Page {page} is not allocated")
        
        if len(contents) != self.PAGE_SIZE:
            raise ValueError(f"Invalid page contents size: {len(contents)} != {self.PAGE_SIZE}")
        
        start = self.get_page_address(page)
        end = start + self.PAGE_SIZE
        self.memory[start:end] = contents
    
    def dump_memory(self, start: int = 0, end: Optional[int] = None) -> str:
        """Dump memory contents as a string."""
        if end is None:
            end = self.size
        
        if not self.is_valid_address(start) or not self.is_valid_address(end - 1):
            raise IndexError(f"Invalid memory range: {start} to {end - 1}")
        
        lines = []
        for i in range(start, end, 16):  # 16 trits per line
            line = f"{i:04x}: "
            for j in range(16):
                if i + j < end:
                    trit = self.memory[i + j]
                    line += f"{trit:2d} "
                else:
                    line += "   "
            
            # Add ASCII representation
            line += " |"
            for j in range(16):
                if i + j < end:
                    trit = self.memory[i + j]
                    if trit == -1:
                        line += "-"
                    elif trit == 0:
                        line += "0"
                    elif trit == 1:
                        line += "+"
                    else:
                        line += "?"
                else:
                    line += " "
            line += "|"
            
            lines.append(line)
        
        return "\n".join(lines)
    
    def __len__(self) -> int:
        """Get memory size."""
        return self.size
    
    def __getitem__(self, address: int) -> Trit:
        """Get a trit at the specified address."""
        return self.load_trit(address)
    
    def __setitem__(self, address: int, value: Union[Trit, int]) -> None:
        """Set a trit at the specified address."""
        self.store_trit(address, value)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryMemory(size={self.size}, pages={self.pages})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"TernaryMemory(size={self.size}, pages={self.pages}, allocated={len(self.allocated_pages)})"
