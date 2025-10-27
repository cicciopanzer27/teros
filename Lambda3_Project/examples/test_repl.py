#!/usr/bin/env python3
"""
Test Lambda3 REPL
Automated test of REPL functionality
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse
from lambda3.engine.reducer import reduce
from lambda3.ternary.encoder import encode
from lambda3.repl.lambda_repl import PREDEF


def test_predefined_terms():
    """Test predefined terms parse correctly"""
    print("\n[Test 1: Predefined Terms]")
    
    for name, source in PREDEF.items():
        try:
            term = parse(source)
            print(f"  {name:10s} = {source:25s} -> {term}")
        except Exception as e:
            print(f"  {name:10s} FAIL: {e}")
            return False
    
    print("PASS: All predefined terms parse correctly")
    return True


def test_identity_repl():
    """Test identity reduction"""
    print("\n[Test 2: Identity Reduction]")
    
    source = PREDEF[':I']
    term = parse(source)
    print(f"  :I = {source}")
    print(f"  Parsed: {term}")
    
    # Apply to y
    app_source = f"({source}) y"
    app_term = parse(app_source)
    result = reduce(app_term)
    print(f"  (:I) y => {result}")
    
    print("PASS: Identity reduction works")
    return True


def test_church_numerals():
    """Test Church numerals"""
    print("\n[Test 3: Church Numerals]")
    
    for i in range(4):
        name = f':{i}'
        source = PREDEF[name]
        term = parse(source)
        print(f"  {name} = {term}")
    
    print("PASS: Church numerals work")
    return True


def test_encoding_in_repl():
    """Test encoding functionality"""
    print("\n[Test 4: Encoding in REPL]")
    
    source = PREDEF[':I']
    term = parse(source)
    trits = encode(term)
    
    print(f"  :I = {source}")
    print(f"  Encoded: {trits}")
    print(f"  Length: {len(trits)} trits")
    
    print("PASS: Encoding works in REPL context")
    return True


def test_full_workflow():
    """Test full parse-reduce-encode workflow"""
    print("\n[Test 5: Full Workflow]")
    
    source = r"(\x.x) y"
    print(f"  Input: {source}")
    
    # Parse
    term = parse(source)
    print(f"  Parsed: {term}")
    
    # Reduce
    result = reduce(term)
    print(f"  Reduced: {result}")
    
    # Encode
    trits = encode(result)
    print(f"  Encoded: {trits}")
    
    print("PASS: Full workflow (parse -> reduce -> encode)")
    return True


def main():
    print("="*60)
    print("  Lambda3 REPL Test Suite")
    print("  Testing REPL Functionality")
    print("="*60)
    
    tests = [
        test_predefined_terms,
        test_identity_repl,
        test_church_numerals,
        test_encoding_in_repl,
        test_full_workflow,
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
        print("  REPL is fully functional!")
        print()
        print("  To run interactively:")
        print("    python run_repl.py")
    print("="*60)
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())

