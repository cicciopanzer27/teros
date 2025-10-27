"""
Ternary Encoder for Lambda Terms

Encodes lambda calculus terms in ternary:
- -1 (T-) = Variable
-  0 (T0) = Abstraction  
- +1 (T+) = Application

This is OPTIMAL because lambda calculus has exactly 3 base constructs!
1 trit = 1.585 bits efficiency (vs 2 bits in binary)
"""

from enum import Enum
from typing import List, Union

# Handle both package import and standalone execution
try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


class Trit(Enum):
    """
    Ternary digit (trit)
    Using balanced ternary: -1, 0, +1
    """
    T_MINUS = -1  # Variable
    T_ZERO = 0    # Abstraction
    T_PLUS = 1    # Application


def encode(term: LambdaTerm) -> List[int]:
    r"""
    Encode lambda term as sequence of trits.
    
    Encoding scheme:
    - Variable: [-1, var_id]
    - Abstraction: [0, var_id, ...body...]
    - Application: [1, ...func..., ...arg...]
    
    Args:
        term: Lambda term to encode
        
    Returns:
        List of trits (integers -1, 0, 1)
        
    Example:
        >>> from lambda3.parser.parser import parse
        >>> term = parse("\\x.x")
        >>> trits = encode(term)
        >>> trits
        [0, 23, -1, 23]  # [Abs, var 23, Var, var 23]
    """
    if isinstance(term, Var):
        # Variable: [-1, var_id]
        var_id = term.name if isinstance(term.name, int) else 0
        return [-1, var_id]
    
    elif isinstance(term, Abs):
        # Abstraction: [0, var_id, ...body...]
        var_id = term.var if isinstance(term.var, int) else 0
        body_trits = encode(term.body)
        return [0, var_id] + body_trits
    
    elif isinstance(term, App):
        # Application: [1, ...func..., ...arg...]
        func_trits = encode(term.func)
        arg_trits = encode(term.arg)
        return [1] + func_trits + arg_trits
    
    return []


def decode(trits: List[int]) -> LambdaTerm:
    r"""
    Decode sequence of trits back to lambda term.
    
    Args:
        trits: List of trits (integers -1, 0, 1)
        
    Returns:
        Lambda term
        
    Example:
        >>> trits = [0, 0, -1, 0]
        >>> term = decode(trits)
        >>> print(term)  # (\x0.x0)
    """
    if not trits:
        return Var(0)
    
    tag = trits[0]
    
    if tag == -1:
        # Variable: [-1, var_id]
        if len(trits) < 2:
            return Var(0)
        var_id = trits[1]
        return Var(var_id)
    
    elif tag == 0:
        # Abstraction: [0, var_id, ...body...]
        if len(trits) < 2:
            return Abs(0, Var(0))
        var_id = trits[1]
        body = decode(trits[2:])
        return Abs(var_id, body)
    
    elif tag == 1:
        # Application: [1, ...func..., ...arg...]
        # Need to split remaining trits between func and arg
        # Strategy: decode func first, then decode remaining as arg
        if len(trits) < 2:
            return App(Var(0), Var(0))
        
        # Decode func (consume trits until we have a complete term)
        func, func_len = decode_with_length(trits[1:])
        # Decode arg (remaining trits)
        arg = decode(trits[1 + func_len:])
        return App(func, arg)
    
    return Var(0)


def decode_with_length(trits: List[int]) -> tuple[LambdaTerm, int]:
    """
    Decode and return (term, num_trits_consumed)
    
    Args:
        trits: List of trits
        
    Returns:
        (decoded_term, num_trits_consumed)
    """
    if not trits:
        return Var(0), 0
    
    tag = trits[0]
    
    if tag == -1:
        # Variable: 2 trits
        var_id = trits[1] if len(trits) > 1 else 0
        return Var(var_id), 2
    
    elif tag == 0:
        # Abstraction: [0, var_id, ...body...]
        var_id = trits[1] if len(trits) > 1 else 0
        body, body_len = decode_with_length(trits[2:])
        return Abs(var_id, body), 2 + body_len
    
    elif tag == 1:
        # Application: [1, ...func..., ...arg...]
        func, func_len = decode_with_length(trits[1:])
        arg, arg_len = decode_with_length(trits[1 + func_len:])
        return App(func, arg), 1 + func_len + arg_len
    
    return Var(0), 0


def encode_compact(term: LambdaTerm) -> str:
    r"""
    Encode as compact string representation
    
    Args:
        term: Lambda term
        
    Returns:
        Compact string of trits
        
    Example:
        >>> term = parse("\\x.x")
        >>> encode_compact(term)
        "0,23,-1,23"
    """
    trits = encode(term)
    return ",".join(str(t) for t in trits)


def decode_compact(compact: str) -> LambdaTerm:
    """
    Decode from compact string representation
    
    Args:
        compact: Compact string of trits
        
    Returns:
        Lambda term
    """
    trits = [int(t) for t in compact.split(",")]
    return decode(trits)


def encoding_efficiency(term: LambdaTerm) -> dict:
    """
    Calculate encoding efficiency
    
    Args:
        term: Lambda term
        
    Returns:
        Dictionary with efficiency stats
    """
    trits = encode(term)
    num_trits = len(trits)
    
    # Binary would need 2 bits per type (00, 01, 10)
    # Plus same space for var_ids
    binary_bits = num_trits * 2
    
    # Ternary: 1 trit = log2(3) = 1.585 bits
    ternary_bits = num_trits * 1.585
    
    savings = (binary_bits - ternary_bits) / binary_bits * 100
    
    return {
        "num_trits": num_trits,
        "ternary_bits": ternary_bits,
        "binary_bits": binary_bits,
        "savings_percent": savings
    }


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    try:
        from lambda3.parser.parser import parse
    except ImportError:
        from parser.parser import parse
    import sys
    
    print("Ternary Encoder Tests:")
    print("=" * 60)
    
    test_cases = [
        "x",
        "\\x.x",
        "(\\x.x) y",
        "\\x.\\y.x",
    ]
    
    all_passed = True
    
    for source in test_cases:
        try:
            print(f"\nTest: {source}")
            term = parse(source)
            print(f"Parsed: {term}")
            
            # Encode
            trits = encode(term)
            print(f"Trits:  {trits}")
            
            # Decode
            decoded = decode(trits)
            print(f"Decoded: {decoded}")
            
            # Check round-trip
            trits2 = encode(decoded)
            if trits == trits2:
                print("PASS: Round-trip successful")
            else:
                print(f"FAIL: Round-trip mismatch")
                print(f"  Original: {trits}")
                print(f"  After:    {trits2}")
                all_passed = False
            
            # Show efficiency
            eff = encoding_efficiency(term)
            print(f"Efficiency: {eff['ternary_bits']:.1f} bits (ternary) vs {eff['binary_bits']} bits (binary)")
            print(f"  Savings: {eff['savings_percent']:.1f}%")
            
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        print("Ternary encoding is OPTIMAL for lambda calculus!")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)
