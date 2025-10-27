"""
Ternary Garbage Collector - Automatic memory management for TEROS.

This module provides garbage collection for the ternary memory system,
using mark-and-sweep algorithm optimized for ternary data structures.
"""

from typing import Dict, List, Optional, Any, Union, Set
import time
from ..core.ternary_memory import TernaryMemory
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryGarbageCollector:
    """
    Garbage collector for ternary memory.
    
    Implements mark-and-sweep garbage collection optimized for
    ternary data structures and memory layout.
    """
    
    def __init__(self, memory: TernaryMemory):
        """
        Initialize the garbage collector.
        
        Args:
            memory: Ternary memory to collect
        """
        self.memory = memory
        self.marked_objects = set()
        self.root_objects = set()
        self.object_map = {}
        self.object_id = 0
        
        # Collection statistics
        self.stats = {
            'total_collections': 0,
            'total_collected': 0,
            'total_time': 0.0,
            'last_collection_time': 0.0,
            'objects_allocated': 0,
            'objects_collected': 0,
            'memory_freed': 0
        }
        
        # Collection thresholds
        self.collection_threshold = 0.8  # Collect when 80% of memory is used
        self.min_collection_interval = 1.0  # Minimum time between collections
    
    def collect(self) -> int:
        """
        Run garbage collection.
        
        Returns:
            Number of objects collected
        """
        start_time = time.time()
        
        # Check if collection is needed
        if not self._should_collect():
            return 0
        
        # Mark phase
        self._mark_phase()
        
        # Sweep phase
        collected = self._sweep_phase()
        
        # Update statistics
        collection_time = time.time() - start_time
        self.stats['total_collections'] += 1
        self.stats['total_collected'] += collected
        self.stats['total_time'] += collection_time
        self.stats['last_collection_time'] = collection_time
        self.stats['objects_collected'] += collected
        
        return collected
    
    def register_object(self, address: int, size: int, obj_type: str) -> int:
        """
        Register an object for garbage collection.
        
        Args:
            address: Memory address of the object
            size: Size of the object in trits
            obj_type: Type of the object
            
        Returns:
            Object ID for tracking
        """
        self.object_id += 1
        self.object_map[self.object_id] = {
            'address': address,
            'size': size,
            'type': obj_type,
            'marked': False,
            'timestamp': time.time()
        }
        
        self.stats['objects_allocated'] += 1
        return self.object_id
    
    def unregister_object(self, obj_id: int) -> bool:
        """
        Unregister an object from garbage collection.
        
        Args:
            obj_id: Object ID to unregister
            
        Returns:
            True if unregistration successful, False otherwise
        """
        if obj_id in self.object_map:
            del self.object_map[obj_id]
            return True
        return False
    
    def add_root_object(self, obj_id: int) -> None:
        """
        Add an object as a root object.
        
        Args:
            obj_id: Object ID to add as root
        """
        self.root_objects.add(obj_id)
    
    def remove_root_object(self, obj_id: int) -> None:
        """
        Remove an object from root objects.
        
        Args:
            obj_id: Object ID to remove from roots
        """
        self.root_objects.discard(obj_id)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get garbage collection statistics."""
        return self.stats.copy()
    
    def get_memory_usage(self) -> Dict[str, Any]:
        """Get current memory usage."""
        total_objects = len(self.object_map)
        marked_objects = sum(1 for obj in self.object_map.values() if obj['marked'])
        
        return {
            'total_objects': total_objects,
            'marked_objects': marked_objects,
            'unmarked_objects': total_objects - marked_objects,
            'root_objects': len(self.root_objects),
            'memory_usage': self._calculate_memory_usage()
        }
    
    def _should_collect(self) -> bool:
        """Check if garbage collection should run."""
        # Check memory usage threshold
        memory_usage = self._calculate_memory_usage()
        if memory_usage > self.collection_threshold:
            return True
        
        # Check minimum collection interval
        if hasattr(self, '_last_collection_time'):
            time_since_last = time.time() - self._last_collection_time
            if time_since_last < self.min_collection_interval:
                return False
        
        return False
    
    def _calculate_memory_usage(self) -> float:
        """Calculate current memory usage."""
        if not self.object_map:
            return 0.0
        
        total_size = sum(obj['size'] for obj in self.object_map.values())
        max_memory = self.memory.size
        return total_size / max_memory if max_memory > 0 else 0.0
    
    def _mark_phase(self) -> None:
        """Mark phase of garbage collection."""
        # Clear previous marks
        self.marked_objects.clear()
        
        # Mark all objects as unmarked
        for obj_id, obj in self.object_map.items():
            obj['marked'] = False
        
        # Mark root objects
        for obj_id in self.root_objects:
            if obj_id in self.object_map:
                self._mark_object(obj_id)
    
    def _mark_object(self, obj_id: int) -> None:
        """
        Mark an object and all objects it references.
        
        Args:
            obj_id: Object ID to mark
        """
        if obj_id in self.marked_objects:
            return
        
        if obj_id not in self.object_map:
            return
        
        # Mark this object
        self.marked_objects.add(obj_id)
        self.object_map[obj_id]['marked'] = True
        
        # Find and mark referenced objects
        obj = self.object_map[obj_id]
        referenced_objects = self._find_referenced_objects(obj)
        
        for ref_obj_id in referenced_objects:
            self._mark_object(ref_obj_id)
    
    def _find_referenced_objects(self, obj: Dict[str, Any]) -> List[int]:
        """
        Find objects referenced by the given object.
        
        Args:
            obj: Object to analyze
            
        Returns:
            List of referenced object IDs
        """
        referenced = []
        address = obj['address']
        size = obj['size']
        
        # Scan the object's memory for references
        for offset in range(0, size, 3):  # Check every 3 trits (address size)
            try:
                # Read potential reference
                ref_data = self.memory.load_tritarray(address + offset, 3)
                ref_value = ref_data.to_decimal()
                
                # Check if this could be a valid object reference
                if self._is_valid_reference(ref_value):
                    # Find object at this address
                    ref_obj_id = self._find_object_at_address(ref_value)
                    if ref_obj_id is not None:
                        referenced.append(ref_obj_id)
            except:
                # Skip invalid memory access
                continue
        
        return referenced
    
    def _is_valid_reference(self, value: int) -> bool:
        """Check if a value could be a valid object reference."""
        # Basic validation: check if value is within memory bounds
        return 0 <= value < self.memory.size
    
    def _find_object_at_address(self, address: int) -> Optional[int]:
        """Find object ID at the given address."""
        for obj_id, obj in self.object_map.items():
            if obj['address'] == address:
                return obj_id
        return None
    
    def _sweep_phase(self) -> int:
        """Sweep phase of garbage collection."""
        collected = 0
        
        # Find unmarked objects
        unmarked_objects = []
        for obj_id, obj in self.object_map.items():
            if not obj['marked']:
                unmarked_objects.append(obj_id)
        
        # Collect unmarked objects
        for obj_id in unmarked_objects:
            obj = self.object_map[obj_id]
            
            # Clear the object's memory
            self._clear_object_memory(obj)
            
            # Remove from object map
            del self.object_map[obj_id]
            
            # Remove from root objects if present
            self.root_objects.discard(obj_id)
            
            collected += 1
            self.stats['memory_freed'] += obj['size']
        
        return collected
    
    def _clear_object_memory(self, obj: Dict[str, Any]) -> None:
        """Clear memory occupied by an object."""
        address = obj['address']
        size = obj['size']
        
        # Clear memory by setting all trits to 0
        for i in range(size):
            self.memory.store_trit(address + i, Trit(0))
    
    def force_collection(self) -> int:
        """Force garbage collection regardless of thresholds."""
        start_time = time.time()
        
        # Mark phase
        self._mark_phase()
        
        # Sweep phase
        collected = self._sweep_phase()
        
        # Update statistics
        collection_time = time.time() - start_time
        self.stats['total_collections'] += 1
        self.stats['total_collected'] += collected
        self.stats['total_time'] += collection_time
        self.stats['last_collection_time'] = collection_time
        self.stats['objects_collected'] += collected
        
        return collected
    
    def get_collection_efficiency(self) -> float:
        """Get garbage collection efficiency."""
        if self.stats['total_collections'] == 0:
            return 0.0
        
        return self.stats['total_collected'] / self.stats['total_collections']
    
    def get_average_collection_time(self) -> float:
        """Get average garbage collection time."""
        if self.stats['total_collections'] == 0:
            return 0.0
        
        return self.stats['total_time'] / self.stats['total_collections']
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryGarbageCollector(objects={len(self.object_map)}, collections={self.stats['total_collections']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryGarbageCollector(objects={len(self.object_map)}, "
                f"collections={self.stats['total_collections']}, "
                f"collected={self.stats['total_collected']})")
