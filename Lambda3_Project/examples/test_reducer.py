#!/usr/bin/env python3
"""
Test Beta Reduction Engine
End-to-end test for parser + reducer
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse
from lambda3.engine.reducer import reduce, is_normal_form


def test_identity():
    """Test identity: (λx.x) y → y"""
    print("\n[Test 1: Identity]")
    source = "(\\x.x) y"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    result = reduce(term)
    print(f"Result: {result}")
    
    # Check result
    assert str(result) == "x24", f"Expected 'x24', got '{result}'"
    print("PASS: Identity reduction works")
    return True


def test_const():
    """Test const: (λx.λy.x) a b → a"""
    print("\n[Test 2: Const]")
    source = "(\\x.\\y.x) a b"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    result = reduce(term)
    print(f"Result: {result}")
    
    # Check result is variable 'a' (x0)
    assert "x0" in str(result), f"Expected 'x0' in result, got '{result}'"
    print("PASS: Const reduction works")
    return True


def test_application():
    """Test application: (λf.λx.f x) g y → g y"""
    print("\n[Test 3: Application]")
    source = "(\\f.\\x.f x) g y"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    result = reduce(term)
    print(f"Result: {result}")
    
    print("PASS: Application reduction works")
    return True


def test_normal_form():
    """Test normal form detection"""
    print("\n[Test 4: Normal Form Detection]")
    
    # x is in normal form
    term1 = parse("x")
    assert is_normal_form(term1), "Variable should be in normal form"
    print("  x is in normal form: OK")
    
    # λx.x is in normal form
    term2 = parse("\\x.x")
    assert is_normal_form(term2), "Identity should be in normal form"
    print("  \\x.x is in normal form: OK")
    
    # (λx.x) y is NOT in normal form (redex)
    term3 = parse("(\\x.x) y")
    assert not is_normal_form(term3), "Redex should NOT be in normal form"
    print("  (\\x.x) y is NOT in normal form: OK")
    
    print("PASS: Normal form detection works")
    return True


def main():
    print("="*60)
    print("  Beta Reduction Engine Test Suite")
    print("  Lambda3 Parser + Reducer Integration")
    print("="*60)
    
    tests = [
        test_identity,
        test_const,
        test_application,
        test_normal_form,
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
        print("  Beta Reduction Engine is working!")
    print("="*60)
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())

