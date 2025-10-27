"""
Hello Lambda - First Lambda³ Example

Demonstrates basic lambda calculus operations.
"""

from lambda3 import parse, reduce, encode


def main():
    print("="*60)
    print("Lambda³ - Hello Lambda Example")
    print("="*60)
    print()
    
    # Example 1: Identity function
    print("1. Identity Function")
    print("   Input:  λx.x")
    term1 = parse("λx.x")
    print(f"   Parsed: {term1}")
    trits1 = encode(term1)
    print(f"   Trits:  {[t.value for t in trits1]}")
    print()
    
    # Example 2: Application
    print("2. Application")
    print("   Input:  (λx.x) y")
    term2 = parse("(λx.x) y")
    print(f"   Parsed: {term2}")
    result2 = reduce(term2)
    print(f"   Result: {result2}")
    print()
    
    # Example 3: Ternary encoding
    print("3. Why Ternary is Perfect for Lambda")
    print("   Lambda has 3 constructs:")
    print("   - Variable    → 0 (trit)")
    print("   - Abstraction → 1 (trit)")
    print("   - Application → 2 (trit)")
    print()
    print("   Binary would need:")
    print("   - 00 = Variable")
    print("   - 01 = Abstraction")
    print("   - 10 = Application")
    print("   - 11 = WASTED!")
    print()
    print("   Ternary is optimal: 1 trit = 1.585 bits")
    print()
    
    print("="*60)
    print("Next steps: See list_todo5.md for full roadmap")
    print("="*60)


if __name__ == "__main__":
    main()

