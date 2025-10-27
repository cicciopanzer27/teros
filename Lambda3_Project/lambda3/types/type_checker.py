"""
Simple Type System for Lambda Calculus (STLC)
Simply Typed Lambda Calculus with type checking
"""

from typing import Dict, Optional, Union
from dataclasses import dataclass
from enum import Enum, auto

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


# ============================================================================
# TYPE DEFINITIONS
# ============================================================================

@dataclass(frozen=True)
class BaseType:
    """Base types: Bool, Int, String, etc."""
    name: str
    
    def __str__(self):
        return self.name
    
    def __repr__(self):
        return f"BaseType({self.name})"


@dataclass(frozen=True)
class ArrowType:
    """Arrow type: A → B (function type)"""
    from_type: 'Type'
    to_type: 'Type'
    
    def __str__(self):
        # Add parens for nested arrows
        from_str = str(self.from_type)
        if isinstance(self.from_type, ArrowType):
            from_str = f"({from_str})"
        return f"{from_str} -> {self.to_type}"
    
    def __repr__(self):
        return f"ArrowType({self.from_type!r}, {self.to_type!r})"


@dataclass(frozen=True)
class TypeVar:
    """Type variable: α, β, γ (for polymorphism)"""
    name: str
    
    def __str__(self):
        return self.name
    
    def __repr__(self):
        return f"TypeVar({self.name})"


# Union type for all types
Type = Union[BaseType, ArrowType, TypeVar]


# Predefined types
BOOL = BaseType("Bool")
INT = BaseType("Int")
STRING = BaseType("String")
UNIT = BaseType("Unit")


# ============================================================================
# TYPE CONTEXT
# ============================================================================

class TypeContext:
    """
    Type context (Γ)
    Maps variables to their types
    """
    
    def __init__(self, parent: Optional['TypeContext'] = None):
        self.bindings: Dict[Union[int, str], Type] = {}
        self.parent = parent
    
    def lookup(self, var: Union[int, str]) -> Optional[Type]:
        """Look up variable type"""
        if var in self.bindings:
            return self.bindings[var]
        elif self.parent:
            return self.parent.lookup(var)
        return None
    
    def extend(self, var: Union[int, str], type_: Type) -> 'TypeContext':
        """Extend context with new binding"""
        new_ctx = TypeContext(parent=self)
        new_ctx.bindings[var] = type_
        return new_ctx
    
    def __str__(self):
        items = [f"{var}: {type_}" for var, type_ in self.bindings.items()]
        return "{" + ", ".join(items) + "}"


# ============================================================================
# TYPE ERRORS
# ============================================================================

class TypeError(Exception):
    """Type error during type checking"""
    pass


# ============================================================================
# TYPE CHECKER
# ============================================================================

class TypeChecker:
    """
    Type checker for Simply Typed Lambda Calculus
    """
    
    def __init__(self):
        self.type_var_counter = 0
    
    def fresh_type_var(self) -> TypeVar:
        """Generate fresh type variable"""
        name = f"t{self.type_var_counter}"
        self.type_var_counter += 1
        return TypeVar(name)
    
    def check(self, term: LambdaTerm, context: Optional[TypeContext] = None) -> Type:
        """
        Type check a lambda term
        Returns the type if well-typed, raises TypeError otherwise
        
        Type rules:
        - Var: Γ ⊢ x : Γ(x)
        - Abs: Γ, x:A ⊢ M:B  →  Γ ⊢ λx.M : A→B
        - App: Γ ⊢ M:A→B  Γ ⊢ N:A  →  Γ ⊢ M N : B
        """
        if context is None:
            context = TypeContext()
        
        if isinstance(term, Var):
            # Variable rule
            var_type = context.lookup(term.name)
            if var_type is None:
                raise TypeError(f"Unbound variable: {term.name}")
            return var_type
        
        elif isinstance(term, Abs):
            # Abstraction rule
            # Assume input type (or use fresh type var)
            input_type = self.fresh_type_var()
            
            # Extend context
            extended = context.extend(term.var, input_type)
            
            # Check body
            body_type = self.check(term.body, extended)
            
            # Return arrow type
            return ArrowType(input_type, body_type)
        
        elif isinstance(term, App):
            # Application rule
            func_type = self.check(term.func, context)
            arg_type = self.check(term.arg, context)
            
            # func_type must be arrow type
            if not isinstance(func_type, ArrowType):
                raise TypeError(f"Expected function type, got: {func_type}")
            
            # Check argument type matches
            if not self.types_equal(func_type.from_type, arg_type):
                raise TypeError(
                    f"Type mismatch: expected {func_type.from_type}, got {arg_type}"
                )
            
            # Return result type
            return func_type.to_type
        
        raise TypeError(f"Unknown term type: {type(term)}")
    
    def types_equal(self, t1: Type, t2: Type) -> bool:
        """Check if two types are equal"""
        if isinstance(t1, BaseType) and isinstance(t2, BaseType):
            return t1.name == t2.name
        elif isinstance(t1, ArrowType) and isinstance(t2, ArrowType):
            return (self.types_equal(t1.from_type, t2.from_type) and
                    self.types_equal(t1.to_type, t2.to_type))
        elif isinstance(t1, TypeVar) and isinstance(t2, TypeVar):
            return t1.name == t2.name
        return False


def type_check(term: LambdaTerm, context: Optional[TypeContext] = None) -> Type:
    """
    Convenience function for type checking
    
    Args:
        term: Lambda term to type check
        context: Type context (optional)
        
    Returns:
        Type of the term
        
    Raises:
        TypeError: If term is not well-typed
        
    Example:
        >>> from lambda3.parser.parser import parse
        >>> term = parse("\\x.x")
        >>> type_ = type_check(term)
        >>> print(type_)  # t0 -> t0 (identity)
    """
    checker = TypeChecker()
    return checker.check(term, context)


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    from parser.parser import parse
    import sys
    
    print("Type Checker Tests:")
    print("=" * 60)
    
    test_cases = [
        (r"\x.x", "Identity"),
        (r"\x.\y.x", "Const"),
        (r"\f.\x.f x", "Application"),
    ]
    
    all_passed = True
    
    for source, description in test_cases:
        try:
            print(f"\nTest: {description}")
            print(f"Term: {source}")
            
            term = parse(source)
            type_ = type_check(term)
            
            print(f"Type: {type_}")
            print("PASS")
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        print("Type checker is working!")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)

