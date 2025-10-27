"""
TVM Backend - Python wrapper for native lambda engine
Bridges Lambda³ with TEROS TVM
"""

from typing import Optional, Union
from dataclasses import dataclass
from enum import Enum


class LambdaTermType(Enum):
    """Lambda term types"""
    VAR = -1  # Variable
    ABS = 0   # Abstraction
    APP = 1   # Application


@dataclass
class LambdaTerm:
    """
    Python representation of lambda term
    Mirrors the C struct for easy integration
    """
    term_type: LambdaTermType
    data: Union[int, tuple]
    
    def __str__(self) -> str:
        """Pretty print lambda term"""
        if self.term_type == LambdaTermType.VAR:
            return f"x{self.data}"
        elif self.term_type == LambdaTermType.ABS:
            var_id, body = self.data
            return f"(\\x{var_id}.{body})"
        elif self.term_type == LambdaTermType.APP:
            func, arg = self.data
            return f"({func} {arg})"
        return "?"
    
    @staticmethod
    def var(var_id: int) -> 'LambdaTerm':
        """Create variable"""
        return LambdaTerm(LambdaTermType.VAR, var_id)
    
    @staticmethod
    def abs(var_id: int, body: 'LambdaTerm') -> 'LambdaTerm':
        """Create abstraction"""
        return LambdaTerm(LambdaTermType.ABS, (var_id, body))
    
    @staticmethod
    def app(func: 'LambdaTerm', arg: 'LambdaTerm') -> 'LambdaTerm':
        """Create application"""
        return LambdaTerm(LambdaTermType.APP, (func, arg))


def lambda_substitute(term: LambdaTerm, var_id: int, replacement: LambdaTerm) -> LambdaTerm:
    """
    Substitution: M[x := N]
    """
    if term.term_type == LambdaTermType.VAR:
        if term.data == var_id:
            return replacement
        else:
            return term
    
    elif term.term_type == LambdaTermType.ABS:
        abs_var_id, body = term.data
        if abs_var_id == var_id:
            # Variable shadowed
            return term
        else:
            new_body = lambda_substitute(body, var_id, replacement)
            return LambdaTerm.abs(abs_var_id, new_body)
    
    elif term.term_type == LambdaTermType.APP:
        func, arg = term.data
        new_func = lambda_substitute(func, var_id, replacement)
        new_arg = lambda_substitute(arg, var_id, replacement)
        return LambdaTerm.app(new_func, new_arg)
    
    return term


def lambda_reduce_step(term: LambdaTerm) -> tuple[LambdaTerm, bool]:
    """
    Perform single β-reduction step
    Returns (reduced_term, changed)
    """
    if term.term_type == LambdaTermType.VAR:
        return term, False
    
    elif term.term_type == LambdaTermType.ABS:
        var_id, body = term.data
        new_body, changed = lambda_reduce_step(body)
        return LambdaTerm.abs(var_id, new_body), changed
    
    elif term.term_type == LambdaTermType.APP:
        func, arg = term.data
        
        # Check if this is a β-redex: (λx.M) N
        if func.term_type == LambdaTermType.ABS:
            var_id, body = func.data
            # β-reduction: (λx.M) N → M[x := N]
            result = lambda_substitute(body, var_id, arg)
            return result, True
        
        # Try to reduce func
        new_func, func_changed = lambda_reduce_step(func)
        if func_changed:
            return LambdaTerm.app(new_func, arg), True
        
        # Try to reduce arg
        new_arg, arg_changed = lambda_reduce_step(arg)
        if arg_changed:
            return LambdaTerm.app(func, new_arg), True
        
        return term, False
    
    return term, False


def lambda_reduce(term: LambdaTerm, max_steps: int = 1000) -> LambdaTerm:
    """
    Reduce term to normal form
    """
    current = term
    for step in range(max_steps):
        next_term, changed = lambda_reduce_step(current)
        if not changed:
            break
        current = next_term
    return current


def lambda_parse(source: str) -> Optional[LambdaTerm]:
    r"""
    Parse lambda term from string
    Supports: λx.M  or  \x.M  for abstraction
              M N   for application
              x     for variable
    """
    # Simple recursive descent parser
    source = source.strip()
    
    # Lambda abstraction
    if source.startswith('λ') or source.startswith('\\'):
        # Format: λx.body or \x.body
        rest = source[1:].strip()
        if '.' not in rest:
            return None
        var_part, body_part = rest.split('.', 1)
        var_part = var_part.strip()
        body_part = body_part.strip()
        
        # Extract variable ID
        if var_part.startswith('x'):
            var_id = int(var_part[1:])
        else:
            var_id = ord(var_part) - ord('a')
        
        # Parse body
        body = lambda_parse(body_part)
        if body is None:
            return None
        
        return LambdaTerm.abs(var_id, body)
    
    # Application (space-separated)
    if ' ' in source:
        parts = source.split(' ', 1)
        func = lambda_parse(parts[0])
        arg = lambda_parse(parts[1])
        if func and arg:
            return LambdaTerm.app(func, arg)
        return None
    
    # Variable
    if source.startswith('x'):
        var_id = int(source[1:])
        return LambdaTerm.var(var_id)
    elif len(source) == 1 and source.isalpha():
        var_id = ord(source) - ord('a')
        return LambdaTerm.var(var_id)
    
    return None


# ============================================================================
# CHURCH ENCODINGS
# ============================================================================

def church_numeral(n: int) -> LambdaTerm:
    """
    Create Church numeral N
    N = λf.λx.f^N(x)
    """
    # Start with x (var 1)
    result = LambdaTerm.var(1)
    
    # Apply f (var 0) n times
    for i in range(n):
        f = LambdaTerm.var(0)
        result = LambdaTerm.app(f, result)
    
    # Wrap in λx. and λf.
    result = LambdaTerm.abs(1, result)
    result = LambdaTerm.abs(0, result)
    
    return result


def church_boolean(value: bool) -> LambdaTerm:
    """
    Create Church boolean
    TRUE  = λx.λy.x
    FALSE = λx.λy.y
    """
    x = LambdaTerm.var(0)
    y = LambdaTerm.var(1)
    
    if value:
        # TRUE: return first argument
        inner = LambdaTerm.abs(1, x)
        return LambdaTerm.abs(0, inner)
    else:
        # FALSE: return second argument
        inner = LambdaTerm.abs(1, y)
        return LambdaTerm.abs(0, inner)


# Predefined terms
CHURCH_ZERO = church_numeral(0)
CHURCH_ONE = church_numeral(1)
CHURCH_TWO = church_numeral(2)
CHURCH_TRUE = church_boolean(True)
CHURCH_FALSE = church_boolean(False)

# Combinators
COMBINATOR_I = LambdaTerm.abs(0, LambdaTerm.var(0))  # λx.x (identity)
COMBINATOR_K = LambdaTerm.abs(0, LambdaTerm.abs(1, LambdaTerm.var(0)))  # λx.λy.x (const)
# S combinator: λx.λy.λz.x z (y z)
S_body = LambdaTerm.app(
    LambdaTerm.app(LambdaTerm.var(0), LambdaTerm.var(2)),
    LambdaTerm.app(LambdaTerm.var(1), LambdaTerm.var(2))
)
COMBINATOR_S = LambdaTerm.abs(0, LambdaTerm.abs(1, LambdaTerm.abs(2, S_body)))


# ============================================================================
# TVM INTEGRATION (TODO)
# ============================================================================

def execute_on_tvm(term: LambdaTerm) -> LambdaTerm:
    """
    Execute lambda term on Ternary Virtual Machine
    TODO: Actual TVM integration
    For now, use Python reducer
    """
    return lambda_reduce(term)


def compile_to_t3(term: LambdaTerm) -> bytes:
    """
    Compile lambda term to T3 bytecode
    TODO: Actual T3 code generation
    """
    # Placeholder
    return b''

