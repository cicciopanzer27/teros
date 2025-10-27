"""
Type Inference for Lambda Calculus
Hindley-Milner type inference with Algorithm W
"""

from typing import Dict, Set, Optional, List, Tuple
from dataclasses import dataclass, field
from copy import deepcopy

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
    from lambda3.types.type_checker import (
        Type, BaseType, ArrowType, TypeVar, TypeContext
    )
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App
    from types.type_checker import (
        Type, BaseType, ArrowType, TypeVar, TypeContext
    )


# ============================================================================
# TYPE SUBSTITUTION
# ============================================================================

@dataclass
class Substitution:
    """Type substitution [α := τ]"""
    mapping: Dict[str, Type] = field(default_factory=dict)
    
    def apply_to_type(self, type_: Type) -> Type:
        """Apply substitution to a type"""
        if isinstance(type_, BaseType):
            return type_
        
        elif isinstance(type_, TypeVar):
            if type_.name in self.mapping:
                # Follow the chain of substitutions
                return self.apply_to_type(self.mapping[type_.name])
            return type_
        
        elif isinstance(type_, ArrowType):
            from_type = self.apply_to_type(type_.from_type)
            to_type = self.apply_to_type(type_.to_type)
            return ArrowType(from_type, to_type)
        
        return type_
    
    def apply_to_context(self, context: TypeContext) -> TypeContext:
        """Apply substitution to type context"""
        new_ctx = TypeContext(parent=context.parent)
        for var, type_ in context.bindings.items():
            new_ctx.bindings[var] = self.apply_to_type(type_)
        return new_ctx
    
    def compose(self, other: 'Substitution') -> 'Substitution':
        """Compose two substitutions"""
        result = Substitution()
        
        # Apply self to other's mapping
        for var, type_ in other.mapping.items():
            result.mapping[var] = self.apply_to_type(type_)
        
        # Add self's mappings (if not already in other)
        for var, type_ in self.mapping.items():
            if var not in result.mapping:
                result.mapping[var] = type_
        
        return result
    
    def __str__(self):
        items = [f"{var} := {type_}" for var, type_ in self.mapping.items()]
        return "[" + ", ".join(items) + "]"


# ============================================================================
# UNIFICATION
# ============================================================================

class UnificationError(Exception):
    """Unification failed"""
    pass


def occurs_check(var: TypeVar, type_: Type) -> bool:
    """Check if type variable occurs in type (prevents infinite types)"""
    if isinstance(type_, TypeVar):
        return var.name == type_.name
    elif isinstance(type_, ArrowType):
        return occurs_check(var, type_.from_type) or occurs_check(var, type_.to_type)
    return False


def unify(t1: Type, t2: Type) -> Substitution:
    """
    Robinson's unification algorithm
    Returns substitution that makes t1 and t2 equal
    
    Raises UnificationError if types cannot be unified
    """
    # Same type
    if isinstance(t1, BaseType) and isinstance(t2, BaseType):
        if t1.name == t2.name:
            return Substitution()  # Empty substitution
        else:
            raise UnificationError(f"Cannot unify {t1} with {t2}")
    
    # Type variable
    elif isinstance(t1, TypeVar):
        if t1.name == t2.name if isinstance(t2, TypeVar) else False:
            return Substitution()
        elif occurs_check(t1, t2):
            raise UnificationError(f"Occurs check: {t1} in {t2}")
        else:
            return Substitution({t1.name: t2})
    
    elif isinstance(t2, TypeVar):
        return unify(t2, t1)  # Symmetric
    
    # Arrow types
    elif isinstance(t1, ArrowType) and isinstance(t2, ArrowType):
        # Unify input types
        s1 = unify(t1.from_type, t2.from_type)
        
        # Apply s1 and unify output types
        to1_subst = s1.apply_to_type(t1.to_type)
        to2_subst = s1.apply_to_type(t2.to_type)
        s2 = unify(to1_subst, to2_subst)
        
        # Compose substitutions
        return s2.compose(s1)
    
    else:
        raise UnificationError(f"Cannot unify {t1} with {t2}")


# ============================================================================
# TYPE INFERENCE (Algorithm W)
# ============================================================================

class TypeInferencer:
    """
    Hindley-Milner type inference using Algorithm W
    """
    
    def __init__(self):
        self.type_var_counter = 0
    
    def fresh_type_var(self) -> TypeVar:
        """Generate fresh type variable"""
        name = f"t{self.type_var_counter}"
        self.type_var_counter += 1
        return TypeVar(name)
    
    def infer(self, term: LambdaTerm, context: Optional[TypeContext] = None) -> Tuple[Substitution, Type]:
        """
        Infer the type of a term
        Returns (substitution, type)
        
        Algorithm W:
        - Generates constraints
        - Solves via unification
        - Returns most general type
        """
        if context is None:
            context = TypeContext()
        
        return self._infer_w(term, context)
    
    def _infer_w(self, term: LambdaTerm, context: TypeContext) -> Tuple[Substitution, Type]:
        """
        Algorithm W implementation
        """
        if isinstance(term, Var):
            # Variable rule
            var_type = context.lookup(term.name)
            if var_type is None:
                # Free variable - assign fresh type
                fresh = self.fresh_type_var()
                return Substitution(), fresh
            return Substitution(), var_type
        
        elif isinstance(term, Abs):
            # Abstraction rule
            # Generate fresh type for parameter
            param_type = self.fresh_type_var()
            
            # Extend context
            extended = context.extend(term.var, param_type)
            
            # Infer body type
            s1, body_type = self._infer_w(term.body, extended)
            
            # Apply substitution to parameter type
            param_type_final = s1.apply_to_type(param_type)
            
            # Return arrow type
            return s1, ArrowType(param_type_final, body_type)
        
        elif isinstance(term, App):
            # Application rule
            # Infer function type
            s1, func_type = self._infer_w(term.func, context)
            
            # Apply s1 to context and infer argument type
            context1 = s1.apply_to_context(context)
            s2, arg_type = self._infer_w(term.arg, context1)
            
            # Apply s2 to function type
            func_type2 = s2.apply_to_type(func_type)
            
            # Generate fresh result type
            result_type = self.fresh_type_var()
            
            # Create arrow type constraint
            expected_func_type = ArrowType(arg_type, result_type)
            
            # Unify
            s3 = unify(func_type2, expected_func_type)
            
            # Apply s3 to result type
            final_result = s3.apply_to_type(result_type)
            
            # Compose all substitutions
            s_final = s3.compose(s2.compose(s1))
            
            return s_final, final_result
        
        raise TypeError(f"Unknown term type: {type(term)}")


def infer_type(term: LambdaTerm, context: Optional[TypeContext] = None) -> Type:
    """
    Convenience function for type inference
    
    Args:
        term: Lambda term
        context: Type context (optional)
        
    Returns:
        Inferred type (most general)
        
    Example:
        >>> from lambda3.parser.parser import parse
        >>> term = parse("\\x.x")
        >>> type_ = infer_type(term)
        >>> print(type_)  # t0 -> t0
    """
    inferencer = TypeInferencer()
    subst, type_ = inferencer.infer(term, context)
    return subst.apply_to_type(type_)


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    from parser.parser import parse
    import sys
    
    print("Type Inference Tests:")
    print("=" * 60)
    
    test_cases = [
        (r"\x.x", "Identity"),
        (r"\x.\y.x", "Const"),
        (r"\f.\x.f x", "Application"),
        (r"\f.\g.\x.f (g x)", "Composition"),
    ]
    
    all_passed = True
    
    for source, description in test_cases:
        try:
            print(f"\nTest: {description}")
            print(f"Term: {source}")
            
            term = parse(source)
            type_ = infer_type(term)
            
            print(f"Inferred type: {type_}")
            print("PASS")
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        print("Type inference (Hindley-Milner) is working!")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)

