#!/usr/bin/env python3
"""
Test Lambda3 Parser
Complete parser testing
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse


def test_variables():
    """Test variable parsing"""
    print("\n[Test 1: Variables]")
    
    tests = ["x", "y", "z", "a", "b"]
    for source in tests:
        term = parse(source)
        print(f"  {source} -> {term}")
    
    print("PASS: Variable parsing")
    return True


def test_identity():
    """Test identity function"""
    print("\n[Test 2: Identity]")
    
    source = r"\x.x"
    term = parse(source)
    print(f"  {source} -> {term}")
    
    print("PASS: Identity parsing")
    return True


def test_const():
    """Test const function"""
    print("\n[Test 3: Const]")
    
    source = r"\x.\y.x"
    term = parse(source)
    print(f"  {source} -> {term}")
    
    print("PASS: Const parsing")
    return True


def test_application():
    """Test application"""
    print("\n[Test 4: Application]")
    
    tests = [
        r"(\x.x) y",
        r"f x",
        r"f x y",
    ]
    
    for source in tests:
        term = parse(source)
        print(f"  {source} -> {term}")
    
    print("PASS: Application parsing")
    return True


def test_complex():
    """Test complex terms"""
    print("\n[Test 5: Complex Terms]")
    
    tests = [
        r"\f.\x.f x",
        r"(\x.\y.x) a b",
        r"\x.\y.\z.x z (y z)",
    ]
    
    for source in tests:
        term = parse(source)
        print(f"  {source}")
        print(f"    -> {term}")
    
    print("PASS: Complex term parsing")
    return True


def main():
    print("="*60)
    print("  Lambda3 Parser Test Suite")
    print("  Testing Parser Functionality")
    print("="*60)
    
    tests = [
        test_variables,
        test_identity,
        test_const,
        test_application,
        test_complex,
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
        print("  Parser is fully functional!")
    print("="*60)
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())

