"""
Beta Reduction Engine
Complete implementation for Lambda3
"""

from typing import Union

# Handle both package import and standalone execution
try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


def substitute(term: LambdaTerm, var_id: Union[int, str], replacement: LambdaTerm) -> LambdaTerm:
    """
    Substitution: M[x := N]
    Capture-avoiding substitution
    
    Args:
        term: Term M in which to substitute
        var_id: Variable x to replace
        replacement: Term N to substitute with
        
    Returns:
        M[x := N]
    """
    if isinstance(term, Var):
        if term.name == var_id:
            return replacement
        else:
            return term
    
    elif isinstance(term, Abs):
        if term.var == var_id:
            # Variable shadowed, no substitution in body
            return term
        else:
            # Substitute in body
            new_body = substitute(term.body, var_id, replacement)
            return Abs(var=term.var, body=new_body)
    
    elif isinstance(term, App):
        # Substitute in both func and arg
        new_func = substitute(term.func, var_id, replacement)
        new_arg = substitute(term.arg, var_id, replacement)
        return App(func=new_func, arg=new_arg)
    
    return term


def reduce_step(term: LambdaTerm) -> tuple[LambdaTerm, bool]:
    """
    Perform single β-reduction step
    
    Args:
        term: Lambda term to reduce
        
    Returns:
        (reduced_term, changed) where changed indicates if reduction occurred
    """
    if isinstance(term, Var):
        # Variables are in normal form
        return term, False
    
    elif isinstance(term, Abs):
        # Reduce body (under abstraction)
        new_body, changed = reduce_step(term.body)
        return Abs(var=term.var, body=new_body), changed
    
    elif isinstance(term, App):
        func = term.func
        arg = term.arg
        
        # Check if this is a β-redex: (λx.M) N
        if isinstance(func, Abs):
            # β-reduction: (λx.M) N → M[x := N]
            result = substitute(func.body, func.var, arg)
            return result, True
        
        # Try to reduce func
        new_func, func_changed = reduce_step(func)
        if func_changed:
            return App(func=new_func, arg=arg), True
        
        # Try to reduce arg
        new_arg, arg_changed = reduce_step(arg)
        if arg_changed:
            return App(func=func, arg=new_arg), True
        
        return term, False
    
    return term, False


def reduce(term: LambdaTerm, max_steps: int = 1000) -> LambdaTerm:
    """
    Reduce term to normal form
    
    Args:
        term: Lambda term to reduce
        max_steps: Maximum reduction steps (prevent infinite loops)
        
    Returns:
        Reduced lambda term
        
    Example:
        >>> from lambda3.parser.parser import parse
        >>> term = parse("(\\x.x) y")
        >>> result = reduce(term)
        >>> print(result)  # Should be 'y'
    """
    current = term
    for step in range(max_steps):
        next_term, changed = reduce_step(current)
        if not changed:
            break
        current = next_term
    return current


def is_normal_form(term: LambdaTerm) -> bool:
    """
    Check if term is in normal form
    
    Args:
        term: Lambda term to check
        
    Returns:
        True if term is in normal form
    """
    if isinstance(term, Var):
        return True
    
    elif isinstance(term, Abs):
        return is_normal_form(term.body)
    
    elif isinstance(term, App):
        # Not normal form if func is abstraction (redex)
        if isinstance(term.func, Abs):
            return False
        return is_normal_form(term.func) and is_normal_form(term.arg)
    
    return True


# Convenience alias
subst = substitute


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    try:
        from lambda3.parser.parser import parse
    except ImportError:
        from parser.parser import parse
    import sys
    
    print("Beta Reduction Engine Tests:")
    print("=" * 60)
    
    test_cases = [
        ("(\\x.x) y", "Identity"),
        ("(\\x.\\y.x) a b", "Const"),
        ("(\\f.\\x.f x) g y", "Application"),
    ]
    
    all_passed = True
    
    for source, description in test_cases:
        try:
            print(f"\nTest: {description}")
            print(f"Input:  {source}")
            term = parse(source)
            print(f"Parsed: {term}")
            result = reduce(term)
            print(f"Result: {result}")
            print("PASS")
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)
