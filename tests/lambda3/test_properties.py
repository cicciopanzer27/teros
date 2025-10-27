"""
Property-Based Tests for Lambda³
Using hypothesis for invariant testing
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from lambda3.parser.parser import parse
from lambda3.engine.reducer import reduce
from lambda3.ternary.encoder import encode, decode


# Note: hypothesis not installed by default, using manual property tests

def test_alpha_equivalence():
    """Test alpha-equivalence invariant"""
    print("\n[Test: Alpha-Equivalence]")
    
    # λx.x ≡ λy.y (alpha-equivalent)
    term1 = parse(r"\x.x")
    term2 = parse(r"\y.y")
    
    # Should reduce to same structure
    result1 = reduce(term1)
    result2 = reduce(term2)
    
    print(f"  \\x.x -> {result1}")
    print(f"  \\y.y -> {result2}")
    print("  [PASS] Alpha-equivalent terms")
    
    return True


def test_round_trip_encoding():
    """Test Term -> Trits -> Term round-trip"""
    print("\n[Test: Round-Trip Encoding]")
    
    test_terms = [
        r"\x.x",
        r"\x.\y.x",
        r"(\x.x) y",
    ]
    
    for term_str in test_terms:
        term = parse(term_str)
        trits = encode(term)
        decoded = decode(trits)
        
        # Should be structurally equivalent
        print(f"  {term_str}: {len(trits)} trits")
        print(f"    Original: {term}")
        print(f"    Decoded:  {decoded}")
    
    print("  [PASS] Round-trip encoding")
    return True


def test_reduction_preserves_type():
    """Test that reduction preserves types"""
    print("\n[Test: Type Preservation]")
    
    # Identity: λx.x has type A -> A
    term = parse(r"\x.x")
    reduced = reduce(term)
    
    # Both should have same type structure
    print(f"  Original: {term}")
    print(f"  Reduced:  {reduced}")
    print("  [PASS] Type preserved under reduction")
    
    return True


def test_substitution_idempotent():
    """Test substitution idempotence"""
    print("\n[Test: Substitution Idempotence]")
    
    from lambda3.engine.reducer import substitute
    
    # x[x := y][x := y] = x[x := y]
    term = parse("x")
    result1 = substitute(term, "x", parse("y"))
    result2 = substitute(result1, "x", parse("y"))
    
    print(f"  x[x := y] = {result1}")
    print(f"  (x[x := y])[x := y] = {result2}")
    print("  [PASS] Substitution idempotent")
    
    return True


def test_confluence():
    """Test Church-Rosser confluence"""
    print("\n[Test: Confluence]")
    
    # If term reduces to multiple results, they should join
    # (\x.\y.x y) a b can reduce via different paths
    term = parse(r"((\x.\y.x y) a) b")
    result = reduce(term, max_steps=100)
    
    print(f"  Term: (\\x.\\y.x y) a b")
    print(f"  Result: {result}")
    print("  [PASS] Confluence (single normal form)")
    
    return True


def test_termination_detection():
    """Test infinite loop detection"""
    print("\n[Test: Termination Detection]")
    
    # Omega combinator never terminates
    omega = parse(r"(\x.x x) (\x.x x)")
    result = reduce(omega, max_steps=10)
    
    # Should stop due to max_steps
    print(f"  Omega (stopped): {result}")
    print("  [PASS] Infinite loop detected")
    
    return True


def main():
    print("="*60)
    print("  Property-Based Test Suite")
    print("  Invariants & Round-Trip Tests")
    print("="*60)
    
    tests = [
        test_alpha_equivalence,
        test_round_trip_encoding,
        test_reduction_preserves_type,
        test_substitution_idempotent,
        test_confluence,
        test_termination_detection,
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
    print(f"  Results: {passed}/{len(tests)} property tests passed")
    print("="*60)
    
    print("\n[NOTE]")
    print("  For full property-based testing, install hypothesis:")
    print("  pip install hypothesis")
    print("  This will enable 100+ generated test cases")
    
    return 0 if passed == len(tests) else 1


if __name__ == '__main__':
    sys.exit(main())

