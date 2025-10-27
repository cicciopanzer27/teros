"""
Ternary Memory Pool Manager for Hardware Abstraction Layer.

This module provides memory pooling and garbage collection for ternary objects.
"""

from typing import Dict, List, Optional, Set, Any, Tuple
import threading
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class PoolType(Enum):
    """Memory pool types."""
    SMALL = "small"      # 1-8 trits
    MEDIUM = "medium"    # 9-64 trits
    LARGE = "large"      # 65-512 trits
    HUGE = "huge"        # 513+ trits


class AllocationStrategy(Enum):
    """Memory allocation strategies."""
    FIRST_FIT = "first_fit"
    BEST_FIT = "best_fit"
    WORST_FIT = "worst_fit"
    BUDDY = "buddy"


class TernaryMemoryPool:
    """
    Ternary Memory Pool - Manages memory allocation for ternary objects.
    
    Provides efficient memory pooling with garbage collection for ternary data.
    """
    
    def __init__(self, pool_size: int = 1024 * 1024,  # 1MB default
                 strategy: AllocationStrategy = AllocationStrategy.BUDDY):
        """
        Initialize ternary memory pool.
        
        Args:
            pool_size: Total pool size in bytes
            strategy: Memory allocation strategy
        """
        self.pool_size = pool_size
        self.strategy = strategy
        
        # Memory pools by type
        self.pools = {
            PoolType.SMALL: self._create_pool(PoolType.SMALL, 8, 1000),
            PoolType.MEDIUM: self._create_pool(PoolType.MEDIUM, 64, 500),
            PoolType.LARGE: self._create_pool(PoolType.LARGE, 512, 100),
            PoolType.HUGE: self._create_pool(PoolType.HUGE, 2048, 50)
        }
        
        # Allocation tracking
        self.allocations = {}  # address -> allocation_info
        self.free_blocks = {}  # pool_type -> List[free_blocks]
        self.used_blocks = {}  # pool_type -> Set[used_addresses]
        
        # Garbage collection
        self.gc_threshold = 0.8  # Trigger GC when 80% full
        self.gc_enabled = True
        self.gc_stats = {
            'collections': 0,
            'objects_freed': 0,
            'memory_freed': 0
        }
        
        # Threading
        self.lock = threading.Lock()
        
        # Statistics
        self.stats = {
            'total_allocations': 0,
            'total_deallocations': 0,
            'current_allocations': 0,
            'memory_used': 0,
            'memory_available': pool_size
        }
    
    def _create_pool(self, pool_type: PoolType, block_size: int, 
                    num_blocks: int) -> Dict[str, Any]:
        """Create memory pool of specified type."""
        return {
            'type': pool_type,
            'block_size': block_size,
            'num_blocks': num_blocks,
            'blocks': [None] * num_blocks,
            'free_list': list(range(num_blocks)),
            'used_set': set()
        }
    
    def allocate(self, size: int, pool_type: PoolType = None) -> Optional[int]:
        """
        Allocate memory for ternary object.
        
        Args:
            size: Size in trits
            pool_type: Specific pool type (if None, auto-select)
            
        Returns:
            Memory address if successful, None otherwise
        """
        with self.lock:
            try:
                # Auto-select pool type if not specified
                if pool_type is None:
                    pool_type = self._select_pool_type(size)
                
                # Check if pool type is appropriate
                if not self._is_pool_appropriate(pool_type, size):
                    return None
                
                # Allocate from pool
                address = self._allocate_from_pool(pool_type, size)
                if address is None:
                    return None
                
                # Track allocation
                self._track_allocation(address, size, pool_type)
                
                # Update statistics
                self.stats['total_allocations'] += 1
                self.stats['current_allocations'] += 1
                self.stats['memory_used'] += size
                self.stats['memory_available'] -= size
                
                return address
                
            except Exception as e:
                print(f"Failed to allocate memory: {e}")
                return None
    
    def deallocate(self, address: int) -> bool:
        """
        Deallocate memory for ternary object.
        
        Args:
            address: Memory address to deallocate
            
        Returns:
            True if deallocation successful, False otherwise
        """
        with self.lock:
            try:
                if address not in self.allocations:
                    return False
                
                allocation = self.allocations[address]
                pool_type = allocation['pool_type']
                size = allocation['size']
                
                # Deallocate from pool
                success = self._deallocate_from_pool(pool_type, address)
                if not success:
                    return False
                
                # Remove from tracking
                del self.allocations[address]
                
                # Update statistics
                self.stats['total_deallocations'] += 1
                self.stats['current_allocations'] -= 1
                self.stats['memory_used'] -= size
                self.stats['memory_available'] += size
                
                return True
                
            except Exception as e:
                print(f"Failed to deallocate memory: {e}")
                return False
    
    def _select_pool_type(self, size: int) -> PoolType:
        """Select appropriate pool type for size."""
        if size <= 8:
            return PoolType.SMALL
        elif size <= 64:
            return PoolType.MEDIUM
        elif size <= 512:
            return PoolType.LARGE
        else:
            return PoolType.HUGE
    
    def _is_pool_appropriate(self, pool_type: PoolType, size: int) -> bool:
        """Check if pool type is appropriate for size."""
        pool = self.pools[pool_type]
        return size <= pool['block_size']
    
    def _allocate_from_pool(self, pool_type: PoolType, size: int) -> Optional[int]:
        """Allocate memory from specific pool."""
        pool = self.pools[pool_type]
        
        # Check if pool has free blocks
        if not pool['free_list']:
            return None
        
        # Allocate block
        block_index = pool['free_list'].pop(0)
        pool['used_set'].add(block_index)
        
        # Calculate address
        address = self._calculate_address(pool_type, block_index)
        
        return address
    
    def _deallocate_from_pool(self, pool_type: PoolType, address: int) -> bool:
        """Deallocate memory from specific pool."""
        pool = self.pools[pool_type]
        
        # Calculate block index from address
        block_index = self._calculate_block_index(pool_type, address)
        
        if block_index not in pool['used_set']:
            return False
        
        # Free block
        pool['used_set'].remove(block_index)
        pool['free_list'].append(block_index)
        
        return True
    
    def _calculate_address(self, pool_type: PoolType, block_index: int) -> int:
        """Calculate memory address from pool type and block index."""
        # Simple address calculation - in real implementation,
        # this would be more sophisticated
        base_address = hash(pool_type.value) * 1000000
        return base_address + (block_index * 1000)
    
    def _calculate_block_index(self, pool_type: PoolType, address: int) -> int:
        """Calculate block index from memory address."""
        # Simple block index calculation - in real implementation,
        # this would be more sophisticated
        base_address = hash(pool_type.value) * 1000000
        return (address - base_address) // 1000
    
    def _track_allocation(self, address: int, size: int, pool_type: PoolType) -> None:
        """Track memory allocation."""
        self.allocations[address] = {
            'size': size,
            'pool_type': pool_type,
            'allocated_at': time.time()
        }
    
    def garbage_collect(self) -> Dict[str, int]:
        """
        Perform garbage collection.
        
        Returns:
            Garbage collection statistics
        """
        if not self.gc_enabled:
            return {'collections': 0, 'objects_freed': 0, 'memory_freed': 0}
        
        with self.lock:
            try:
                # Find unreferenced objects
                unreferenced = self._find_unreferenced_objects()
                
                # Free unreferenced objects
                objects_freed = 0
                memory_freed = 0
                
                for address in unreferenced:
                    if address in self.allocations:
                        allocation = self.allocations[address]
                        size = allocation['size']
                        
                        if self.deallocate(address):
                            objects_freed += 1
                            memory_freed += size
                
                # Update GC statistics
                self.gc_stats['collections'] += 1
                self.gc_stats['objects_freed'] += objects_freed
                self.gc_stats['memory_freed'] += memory_freed
                
                return {
                    'collections': self.gc_stats['collections'],
                    'objects_freed': objects_freed,
                    'memory_freed': memory_freed
                }
                
            except Exception as e:
                print(f"Garbage collection failed: {e}")
                return {'collections': 0, 'objects_freed': 0, 'memory_freed': 0}
    
    def _find_unreferenced_objects(self) -> List[int]:
        """Find unreferenced objects (simplified implementation)."""
        # In real implementation, this would use mark-and-sweep
        # or reference counting to find unreferenced objects
        
        unreferenced = []
        for address in self.allocations:
            # Simple heuristic: objects older than 1 second are unreferenced
            allocation = self.allocations[address]
            if time.time() - allocation['allocated_at'] > 1.0:
                unreferenced.append(address)
        
        return unreferenced
    
    def should_garbage_collect(self) -> bool:
        """Check if garbage collection should be triggered."""
        if not self.gc_enabled:
            return False
        
        # Trigger GC if memory usage exceeds threshold
        usage_ratio = self.stats['memory_used'] / self.pool_size
        return usage_ratio >= self.gc_threshold
    
    def get_pool_stats(self) -> Dict[str, Any]:
        """Get memory pool statistics."""
        with self.lock:
            return {
                'pool_size': self.pool_size,
                'strategy': self.strategy.value,
                'gc_enabled': self.gc_enabled,
                'gc_threshold': self.gc_threshold,
                'pools': {
                    pool_type.value: {
                        'block_size': pool['block_size'],
                        'num_blocks': pool['num_blocks'],
                        'free_blocks': len(pool['free_list']),
                        'used_blocks': len(pool['used_set'])
                    }
                    for pool_type, pool in self.pools.items()
                },
                **self.stats,
                **self.gc_stats
            }
    
    def cleanup(self) -> None:
        """Cleanup memory pool."""
        with self.lock:
            # Clear all allocations
            self.allocations.clear()
            
            # Reset pools
            for pool in self.pools.values():
                pool['free_list'] = list(range(pool['num_blocks']))
                pool['used_set'].clear()
            
            # Reset statistics
            self.stats['current_allocations'] = 0
            self.stats['memory_used'] = 0
            self.stats['memory_available'] = self.pool_size
            
            print("Memory pool cleaned up")
    
    def __del__(self):
        """Destructor."""
        self.cleanup()
