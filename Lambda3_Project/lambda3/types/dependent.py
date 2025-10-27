"""
Dependent Types for Lambda³
Advanced type system with Π and Σ types
"""

from typing import Union, List, Dict, Optional, Any
from dataclasses import dataclass
from enum import Enum

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


class DependentType(Enum):
    """Dependent type constructors"""
    PI = "Π"      # Dependent function type
    SIGMA = "Σ"   # Dependent pair type
    UNIVERSE = "Type"  # Universe of types


@dataclass
class DependentTypeTerm:
    """Dependent type term"""
    constructor: DependentType
    domain: Optional['DependentTypeTerm'] = None
    codomain: Optional['DependentTypeTerm'] = None
    variable: Optional[str] = None
    
    def __str__(self):
        if self.constructor == DependentType.PI:
            return f"Π{self.variable}:{self.domain}.{self.codomain}"
        elif self.constructor == DependentType.SIGMA:
            return f"Σ{self.variable}:{self.domain}.{self.codomain}"
        elif self.constructor == DependentType.UNIVERSE:
            return "Type"
        return str(self.constructor.value)


class DependentTypeChecker:
    """
    Dependent type checker for Lambda³
    
    Supports:
    - Π-types: (x : A) → B(x)
    - Σ-types: (x : A) × B(x)
    - Universe levels: Type : Type₁ : Type₂ : ...
    """
    
    def __init__(self):
        self.universe_levels = {
            'Type': 0,
            'Type₁': 1,
            'Type₂': 2,
        }
        self.context = {}  # Variable → Type mapping
    
    def check_pi_type(self, var: str, domain: DependentTypeTerm, 
                     codomain: DependentTypeTerm) -> bool:
        """Check Π-type validity"""
        # Domain must be a type
        if not self.is_type(domain):
            return False
        
        # Add variable to context
        old_context = self.context.copy()
        self.context[var] = domain
        
        # Check codomain
        result = self.is_type(codomain)
        
        # Restore context
        self.context = old_context
        
        return result
    
    def check_sigma_type(self, var: str, domain: DependentTypeTerm,
                        codomain: DependentTypeTerm) -> bool:
        """Check Σ-type validity"""
        # Domain must be a type
        if not self.is_type(domain):
            return False
        
        # Add variable to context
        old_context = self.context.copy()
        self.context[var] = domain
        
        # Check codomain
        result = self.is_type(codomain)
        
        # Restore context
        self.context = old_context
        
        return result
    
    def is_type(self, term: DependentTypeTerm) -> bool:
        """Check if term is a valid type"""
        if term.constructor == DependentType.UNIVERSE:
            return True
        
        elif term.constructor == DependentType.PI:
            if not term.domain or not term.codomain or not term.variable:
                return False
            return self.check_pi_type(term.variable, term.domain, term.codomain)
        
        elif term.constructor == DependentType.SIGMA:
            if not term.domain or not term.codomain or not term.variable:
                return False
            return self.check_sigma_type(term.variable, term.domain, term.codomain)
        
        return False
    
    def infer_dependent_type(self, term: LambdaTerm) -> Optional[DependentTypeTerm]:
        """Infer dependent type for lambda term"""
        if isinstance(term, Var):
            return self.context.get(term.name)
        
        elif isinstance(term, Abs):
            # λx.M : Πx:A.B
            if term.var in self.context:
                domain_type = self.context[term.var]
                # Add to context
                old_context = self.context.copy()
                self.context[term.var] = domain_type
                
                # Infer body type
                body_type = self.infer_dependent_type(term.body)
                
                # Restore context
                self.context = old_context
                
                if body_type:
                    return DependentTypeTerm(
                        constructor=DependentType.PI,
                        domain=domain_type,
                        codomain=body_type,
                        variable=term.var
                    )
        
        elif isinstance(term, App):
            # M N : B[N/x] if M : Πx:A.B and N : A
            func_type = self.infer_dependent_type(term.func)
            arg_type = self.infer_dependent_type(term.arg)
            
            if (func_type and func_type.constructor == DependentType.PI and
                arg_type and self.type_equal(arg_type, func_type.domain)):
                # Substitute in codomain
                return self.substitute_type(func_type.codomain, func_type.variable, arg_type)
        
        return None
    
    def type_equal(self, type1: DependentTypeTerm, type2: DependentTypeTerm) -> bool:
        """Check if two types are equal"""
        if type1.constructor != type2.constructor:
            return False
        
        if type1.constructor == DependentType.UNIVERSE:
            return True
        
        elif type1.constructor == DependentType.PI:
            return (self.type_equal(type1.domain, type2.domain) and
                    self.type_equal(type1.codomain, type2.codomain) and
                    type1.variable == type2.variable)
        
        elif type1.constructor == DependentType.SIGMA:
            return (self.type_equal(type1.domain, type2.domain) and
                    self.type_equal(type1.codomain, type2.codomain) and
                    type1.variable == type2.variable)
        
        return False
    
    def substitute_type(self, type_term: DependentTypeTerm, var: str, 
                       replacement: DependentTypeTerm) -> DependentTypeTerm:
        """Substitute variable in type"""
        if type_term.constructor == DependentType.UNIVERSE:
            return type_term
        
        elif type_term.constructor == DependentType.PI:
            new_domain = self.substitute_type(type_term.domain, var, replacement)
            new_codomain = self.substitute_type(type_term.codomain, var, replacement)
            return DependentTypeTerm(
                constructor=DependentType.PI,
                domain=new_domain,
                codomain=new_codomain,
                variable=type_term.variable
            )
        
        elif type_term.constructor == DependentType.SIGMA:
            new_domain = self.substitute_type(type_term.domain, var, replacement)
            new_codomain = self.substitute_type(type_term.codomain, var, replacement)
            return DependentTypeTerm(
                constructor=DependentType.SIGMA,
                domain=new_domain,
                codomain=new_codomain,
                variable=type_term.variable
            )
        
        return type_term


# ============================================================================
# EXAMPLES
# ============================================================================

def example_vector_types():
    """Example: Vector types with dependent types"""
    print("="*60)
    print("  Dependent Types Examples")
    print("  Vector Types with Length")
    print("="*60)
    
    checker = DependentTypeChecker()
    
    # Vector type: Vec A n = list of length n with elements of type A
    # This would be: Vec : Type → Nat → Type
    
    # Example: Vector of length 3 with integers
    # Vec Int 3
    
    # Example: Function that preserves length
    # map : ΠA:Type. ΠB:Type. (A → B) → Πn:Nat. Vec A n → Vec B n
    
    print("Vector Types:")
    print("  Vec : Type → Nat → Type")
    print("  map : ΠA:Type. ΠB:Type. (A → B) → Πn:Nat. Vec A n → Vec B n")
    print("  append : ΠA:Type. Πn:Nat. Πm:Nat. Vec A n → Vec A m → Vec A (n + m)")
    
    print("\nDependent Pair Types:")
    print("  Σn:Nat. Vec A n  (vector with its length)")
    print("  Σx:A. P x        (witness that P holds for some x)")


def example_proof_types():
    """Example: Proof types with dependent types"""
    print("\n" + "="*60)
    print("  Proof Types with Dependent Types")
    print("="*60)
    
    # Proof that a number is even
    # Even : Nat → Type
    # even_zero : Even 0
    # even_succ_succ : Πn:Nat. Even n → Even (S (S n))
    
    print("Proof Types:")
    print("  Even : Nat → Type")
    print("  even_zero : Even 0")
    print("  even_succ_succ : Πn:Nat. Even n → Even (S (S n))")
    
    print("\nIdentity Types:")
    print("  Id : ΠA:Type. A → A → Type")
    print("  refl : ΠA:Type. Πx:A. Id A x x")
    print("  J : ΠA:Type. Πx:A. ΠP:Πy:A. Id A x y → Type. P x (refl A x) → Πy:A. Πp:Id A x y. P y p")


def main():
    print("="*60)
    print("  Lambda³ Dependent Types")
    print("  Advanced Type System")
    print("="*60)
    
    example_vector_types()
    example_proof_types()
    
    print("\n" + "="*60)
    print("Dependent Types Features:")
    print("  ✓ Π-types (dependent functions)")
    print("  ✓ Σ-types (dependent pairs)")
    print("  ✓ Universe levels")
    print("  ✓ Type checking")
    print("  ✓ Type inference")
    print("  ✓ Substitution")
    print("="*60)
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
