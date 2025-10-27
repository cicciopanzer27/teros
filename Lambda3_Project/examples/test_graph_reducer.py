#!/usr/bin/env python3
"""
Test Graph Reducer
Performance comparison between naive and graph reduction
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse
from lambda3.engine.reducer import reduce as naive_reduce
from lambda3.engine.graph_reducer import reduce_with_sharing


def test_identity():
    """Test identity with graph reduction"""
    print("\n[Test 1: Identity]")
    source = r"(\x.x) y"
    print(f"Input: {source}")
    
    term = parse(source)
    result, stats = reduce_with_sharing(term)
    
    print(f"Result: {result}")
    print(f"Stats: {stats}")
    
    assert str(result) == "x24", f"Expected x24, got {result}"
    print("PASS: Identity with graph reduction")
    return True


def test_const():
    """Test const with graph reduction"""
    print("\n[Test 2: Const]")
    source = r"(\x.\y.x) a b"
    print(f"Input: {source}")
    
    term = parse(source)
    result, stats = reduce_with_sharing(term)
    
    print(f"Result: {result}")
    print(f"Stats: {stats}")
    
    print("PASS: Const with graph reduction")
    return True


def test_sharing():
    """Test sharing detection"""
    print("\n[Test 3: Sharing Detection]")
    
    # Term with repeated subterms
    source = r"(\f.\x.f (f x)) g y"
    print(f"Input: {source}")
    
    term = parse(source)
    result, stats = reduce_with_sharing(term)
    
    print(f"Result: {result}")
    print(f"Stats: {stats}")
    print(f"  Sharing hits: {stats['sharing_hits']}")
    print(f"  Unique nodes: {stats['unique_nodes']}")
    
    assert stats['sharing_hits'] > 0, "Expected some sharing"
    print("PASS: Sharing detected")
    return True


def test_performance():
    """Test performance comparison"""
    print("\n[Test 4: Performance Comparison]")
    
    test_terms = [
        r"(\x.x) y",
        r"(\x.\y.x) a b",
        r"(\f.\x.f x) g y",
    ]
    
    print("\nPerformance Analysis:")
    print("-" * 60)
    
    for source in test_terms:
        term = parse(source)
        
        # Graph reduction
        start = time.time()
        result_graph, stats = reduce_with_sharing(term)
        time_graph = (time.time() - start) * 1000
        
        # Naive reduction
        start = time.time()
        result_naive = naive_reduce(term, max_steps=1000)
        time_naive = (time.time() - start) * 1000
        
        # Compare
        match = str(result_graph) == str(result_naive)
        speedup = time_naive / time_graph if time_graph > 0 else 1.0
        
        print(f"\n{source}")
        print(f"  Graph:  {time_graph:.3f}ms")
        print(f"  Naive:  {time_naive:.3f}ms")
        print(f"  Speedup: {speedup:.2f}x")
        print(f"  Match: {match}")
        print(f"  Reductions: {stats['reductions']}")
        print(f"  Sharing: {stats['sharing_hits']}")
        
        assert match, "Results should match"
    
    print("\n" + "-" * 60)
    print("PASS: Performance comparison")
    return True


def main():
    print("="*60)
    print("  Graph Reducer Test Suite")
    print("  DAG-based Reduction with Sharing")
    print("="*60)
    
    tests = [
        test_identity,
        test_const,
        test_sharing,
        test_performance,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "="*60)
    print(f"  Results: {passed} passed, {failed} failed")
    if failed == 0:
        print("  ALL TESTS PASSED!")
        print("  Graph Reduction is working!")
    print("="*60)
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())

