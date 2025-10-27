"""
Tests for Garbage Collector
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.runtime.gc import LambdaGC, ReferenceCounter, CycleDetector, MemoryPool


def test_reference_counting():
    """Test basic reference counting"""
    print("\n[Test 1: Reference Counting]")
    
    rc = ReferenceCounter()
    
    # Allocate objects
    obj1 = rc.allocate(16)
    obj2 = rc.allocate(32)
    
    # Increment refs
    rc.incref(obj1)
    rc.incref(obj1)
    
    assert rc.objects[obj1].ref_count == 2, "Ref count should be 2"
    
    # Decrement
    rc.decref(obj1)
    assert rc.objects[obj1].ref_count == 1, "Ref count should be 1"
    
    # Decrement to zero - should free
    rc.decref(obj1)
    assert obj1 not in rc.objects, "Object should be freed"
    
    print("[PASS] Reference counting works")
    return True


def test_cycle_detection():
    """Test cycle detection"""
    print("\n[Test 2: Cycle Detection]")
    
    rc = ReferenceCounter()
    
    # Create cycle: obj1 -> obj2 -> obj1
    obj1 = rc.allocate(16, [])
    obj2 = rc.allocate(16, [obj1])
    rc.objects[obj1].children = [obj2]  # Complete the cycle
    
    cd = CycleDetector(rc)
    cycles = cd.detect_cycles()
    
    assert obj1 in cycles or obj2 in cycles, "Should detect cycle"
    
    print(f"[PASS] Detected {len(cycles)} objects in cycles")
    return True


def test_memory_pools():
    """Test memory pools"""
    print("\n[Test 3: Memory Pools]")
    
    pool = MemoryPool(16, 10)
    
    # Allocate all
    allocated = []
    for _ in range(10):
        idx = pool.allocate()
        assert idx is not None, "Should allocate"
        allocated.append(idx)
    
    # Pool exhausted
    idx = pool.allocate()
    assert idx is None, "Pool should be exhausted"
    
    # Free one
    pool.free(allocated[0])
    
    # Can allocate again
    idx = pool.allocate()
    assert idx is not None, "Should allocate after free"
    
    print("[PASS] Memory pools work")
    return True


def test_integrated_gc():
    """Test integrated GC"""
    print("\n[Test 4: Integrated GC]")
    
    gc = LambdaGC()
    
    # Allocate and add root
    obj = gc.allocate(16)
    gc.add_root(obj)
    
    stats = gc.get_stats()
    assert stats['roots'] == 1, "Should have 1 root"
    
    # Remove root
    gc.remove_root(obj)
    
    stats = gc.get_stats()
    assert stats['roots'] == 0, "Should have 0 roots"
    
    print("[PASS] Integrated GC works")
    return True


def main():
    print("="*60)
    print("  GC Test Suite")
    print("="*60)
    
    tests = [
        test_reference_counting,
        test_cycle_detection,
        test_memory_pools,
        test_integrated_gc,
    ]
    
    passed = 0
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"[FAIL]: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "="*60)
    print(f"  Results: {passed}/{len(tests)} tests passed")
    print("="*60)
    
    return 0 if passed == len(tests) else 1


if __name__ == '__main__':
    sys.exit(main())

