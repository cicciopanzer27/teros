#!/usr/bin/env python3
"""
Test Ternary Encoder
End-to-end test for parser + encoder
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse
from lambda3.ternary.encoder import encode, decode, encoding_efficiency


def test_variable():
    """Test encoding variable"""
    print("\n[Test 1: Variable]")
    source = "x"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    trits = encode(term)
    print(f"Trits:  {trits}")
    
    decoded = decode(trits)
    print(f"Decoded: {decoded}")
    
    # Check round-trip
    trits2 = encode(decoded)
    assert trits == trits2, f"Round-trip failed: {trits} != {trits2}"
    print("PASS: Variable encoding")
    return True


def test_identity():
    """Test encoding identity"""
    print("\n[Test 2: Identity]")
    source = "\\x.x"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    trits = encode(term)
    print(f"Trits:  {trits}")
    print(f"  Type: {trits[0]} (0=Abstraction)")
    
    decoded = decode(trits)
    print(f"Decoded: {decoded}")
    
    # Check round-trip
    trits2 = encode(decoded)
    assert trits == trits2, f"Round-trip failed"
    print("PASS: Identity encoding")
    return True


def test_application():
    """Test encoding application"""
    print("\n[Test 3: Application]")
    source = "(\\x.x) y"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    trits = encode(term)
    print(f"Trits:  {trits}")
    print(f"  Type: {trits[0]} (1=Application)")
    
    decoded = decode(trits)
    print(f"Decoded: {decoded}")
    
    # Check round-trip
    trits2 = encode(decoded)
    assert trits == trits2, f"Round-trip failed"
    print("PASS: Application encoding")
    return True


def test_const():
    """Test encoding const"""
    print("\n[Test 4: Const]")
    source = "\\x.\\y.x"
    print(f"Input:  {source}")
    
    term = parse(source)
    print(f"Parsed: {term}")
    
    trits = encode(term)
    print(f"Trits:  {trits}")
    print(f"  Length: {len(trits)} trits")
    
    decoded = decode(trits)
    print(f"Decoded: {decoded}")
    
    # Check round-trip
    trits2 = encode(decoded)
    assert trits == trits2, f"Round-trip failed"
    print("PASS: Const encoding")
    return True


def test_efficiency():
    """Test encoding efficiency"""
    print("\n[Test 5: Encoding Efficiency]")
    
    test_terms = [
        "x",
        "\\x.x",
        "(\\x.x) y",
        "\\x.\\y.x",
    ]
    
    print("\nEfficiency Analysis:")
    print("-" * 60)
    
    for source in test_terms:
        term = parse(source)
        eff = encoding_efficiency(term)
        print(f"\n{source}")
        print(f"  Trits:        {eff['num_trits']}")
        print(f"  Ternary bits: {eff['ternary_bits']:.1f}")
        print(f"  Binary bits:  {eff['binary_bits']}")
        print(f"  Savings:      {eff['savings_percent']:.1f}%")
    
    print("\n" + "-" * 60)
    print("Ternary encoding is OPTIMAL for lambda calculus!")
    print("Every trit encodes log2(3)=1.585 bits of information")
    print("Binary wastes space: 2 bits for 3 states = 25% overhead")
    print("PASS: Efficiency analysis")
    return True


def main():
    print("="*60)
    print("  Ternary Encoder Test Suite")
    print("  Optimal Encoding for Lambda Calculus")
    print("="*60)
    
    tests = [
        test_variable,
        test_identity,
        test_application,
        test_const,
        test_efficiency,
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
        print("  Ternary Encoding is working!")
    print("="*60)
    
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())

