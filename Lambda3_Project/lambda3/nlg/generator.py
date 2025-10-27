"""
Natural Language Generation for Lambda³
Convert lambda terms and proofs to human-readable explanations
"""

from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
import re

try:
    from lambda3.parser.parser import parse
    from lambda3.engine.reducer import reduce
    from lambda3.types.inference import infer_type
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.parser import parse
    from engine.reducer import reduce
    from lambda3.types.inference import infer_type


@dataclass
class Explanation:
    """Natural language explanation"""
    text: str
    confidence: float
    examples: List[str]
    related_concepts: List[str]


class NLGGenerator:
    """
    Natural Language Generation for Lambda Calculus
    
    Converts lambda terms to explanations like:
    - λx.x → "The identity function that returns its input unchanged"
    - (λx.λy.x) a b → "Apply the constant function to get 'a'"
    """
    
    def __init__(self):
        self.function_patterns = {
            r'\\x\.x': {
                'name': 'identity function',
                'description': 'returns its input unchanged',
                'example': 'f(x) = x'
            },
            r'\\x\.\\y\.x': {
                'name': 'constant function',
                'description': 'ignores second argument, returns first',
                'example': 'f(x, y) = x'
            },
            r'\\f\.\\g\.\\x\.f \(g x\)': {
                'name': 'function composition',
                'description': 'applies g first, then f to the result',
                'example': '(f ∘ g)(x) = f(g(x))'
            },
            r'\\x\.\\y\.x y': {
                'name': 'application function',
                'description': 'applies first argument to second',
                'example': 'f(x, y) = x(y)'
            }
        }
        
        self.logical_patterns = {
            'A -> A': 'implication (A implies A)',
            'A -> B': 'implication (A implies B)',
            'A & B': 'conjunction (A and B)',
            'A | B': 'disjunction (A or B)',
            '~A': 'negation (not A)'
        }
    
    def explain_lambda_term(self, term_str: str) -> Explanation:
        """Generate natural language explanation for lambda term"""
        try:
            term = parse(term_str)
            term_str_clean = str(term)
            
            # Check for known patterns
            for pattern, info in self.function_patterns.items():
                if re.search(pattern, term_str_clean):
                    return self._explain_known_pattern(term_str_clean, info)
            
            # General explanation
            return self._explain_general_term(term)
            
        except Exception as e:
            return Explanation(
                text=f"Could not parse lambda term: {e}",
                confidence=0.0,
                examples=[],
                related_concepts=[]
            )
    
    def _explain_known_pattern(self, term: str, info: Dict) -> Explanation:
        """Explain known function patterns"""
        text = f"This is the {info['name']}. It {info['description']}."
        examples = [info['example']]
        related = ['lambda calculus', 'functional programming', 'mathematical functions']
        
        return Explanation(
            text=text,
            confidence=0.9,
            examples=examples,
            related_concepts=related
        )
    
    def _explain_general_term(self, term) -> Explanation:
        """Explain general lambda term"""
        term_str = str(term)
        
        # Count constructs
        abstractions = term_str.count('\\')
        applications = term_str.count('(') - term_str.count('\\')
        variables = len(set(re.findall(r'[a-zA-Z]\w*', term_str)))
        
        # Generate explanation
        parts = []
        
        if abstractions > 0:
            parts.append(f"a function with {abstractions} parameter{'s' if abstractions > 1 else ''}")
        
        if applications > 0:
            parts.append(f"applied {applications} time{'s' if applications > 1 else ''}")
        
        if variables > 0:
            parts.append(f"using {variables} variable{'s' if variables > 1 else ''}")
        
        if parts:
            text = f"This lambda term is {' and '.join(parts)}."
        else:
            text = "This is a simple lambda term."
        
        # Add complexity assessment
        complexity = abstractions + applications
        if complexity > 5:
            text += " It's a complex expression."
        elif complexity > 2:
            text += " It's moderately complex."
        else:
            text += " It's a simple expression."
        
        return Explanation(
            text=text,
            confidence=0.7,
            examples=[f"Example: {term_str}"],
            related_concepts=['lambda calculus', 'functional programming']
        )
    
    def explain_reduction(self, original: str, reduced: str) -> Explanation:
        """Explain beta reduction step"""
        if original == reduced:
            text = "No reduction possible - term is in normal form."
            confidence = 0.9
        else:
            text = f"Beta reduction: {original} → {reduced}"
            confidence = 0.8
        
        return Explanation(
            text=text,
            confidence=confidence,
            examples=[f"Step: {original} → {reduced}"],
            related_concepts=['beta reduction', 'lambda calculus', 'computation']
        )
    
    def explain_type(self, term_str: str, type_str: str) -> Explanation:
        """Explain type of lambda term"""
        text = f"This lambda term has type: {type_str}"
        
        # Add type explanation
        if '->' in type_str:
            parts = type_str.split(' -> ')
            if len(parts) == 2:
                text += f" It's a function from {parts[0]} to {parts[1]}."
            else:
                text += " It's a higher-order function."
        elif type_str in ['A', 'B', 'C']:
            text += f" It's a variable of type {type_str}."
        else:
            text += " It's a well-typed lambda term."
        
        return Explanation(
            text=text,
            confidence=0.8,
            examples=[f"Type: {type_str}"],
            related_concepts=['type theory', 'Hindley-Milner', 'type inference']
        )
    
    def explain_proof_step(self, goal: str, tactic: str, result: str) -> Explanation:
        """Explain a proof step"""
        tactic_explanations = {
            'intro': 'introduce assumption',
            'exact': 'use exact match',
            'assumption': 'use available assumption',
            'apply': 'apply theorem or lemma',
            'split': 'split conjunction',
            'left': 'use left disjunct',
            'right': 'use right disjunct',
            'contradiction': 'use proof by contradiction'
        }
        
        tactic_desc = tactic_explanations.get(tactic, f'use {tactic} tactic')
        
        text = f"To prove {goal}, we {tactic_desc}. This gives us {result}."
        
        return Explanation(
            text=text,
            confidence=0.8,
            examples=[f"Goal: {goal}", f"Tactic: {tactic}", f"Result: {result}"],
            related_concepts=['proof theory', 'formal verification', 'logic']
        )
    
    def generate_tutorial(self, topic: str) -> Explanation:
        """Generate tutorial content"""
        tutorials = {
            'lambda_calculus': {
                'text': 'Lambda calculus is a formal system for expressing computation using functions.',
                'examples': [
                    'λx.x (identity function)',
                    'λx.λy.x (constant function)',
                    '(λx.x) y → y (beta reduction)'
                ],
                'concepts': ['functions', 'variables', 'abstraction', 'application']
            },
            'type_theory': {
                'text': 'Type theory provides a foundation for mathematical logic and programming languages.',
                'examples': [
                    'x : A (variable x has type A)',
                    'λx.M : A → B (function from A to B)',
                    'M N : B (application result)'
                ],
                'concepts': ['types', 'inference', 'Hindley-Milner', 'polymorphism']
            },
            'proof_assistant': {
                'text': 'Proof assistants help verify mathematical proofs using formal methods.',
                'examples': [
                    'intro - introduce assumption',
                    'exact - use exact match',
                    'apply - apply theorem'
                ],
                'concepts': ['formal verification', 'theorem proving', 'tactics']
            }
        }
        
        if topic in tutorials:
            info = tutorials[topic]
            return Explanation(
                text=info['text'],
                confidence=0.9,
                examples=info['examples'],
                related_concepts=info['concepts']
            )
        else:
            return Explanation(
                text=f"Tutorial for {topic} not available.",
                confidence=0.0,
                examples=[],
                related_concepts=[]
            )


# ============================================================================
# DEMO
# ============================================================================

def demo_nlg():
    """Demonstrate NLG capabilities"""
    print("="*60)
    print("  Natural Language Generation Demo")
    print("="*60)
    
    generator = NLGGenerator()
    
    test_terms = [
        r"\x.x",
        r"\x.\y.x",
        r"\f.\g.\x.f (g x)",
        r"(\x.x) y",
        r"(\x.\y.x) a b"
    ]
    
    for term in test_terms:
        print(f"\nLambda Term: {term}")
        explanation = generator.explain_lambda_term(term)
        print(f"Explanation: {explanation.text}")
        print(f"Confidence: {explanation.confidence:.2f}")
        if explanation.examples:
            print(f"Example: {explanation.examples[0]}")


def main():
    print("="*60)
    print("  Lambda³ NLG Generator")
    print("  Natural Language Explanations")
    print("="*60)
    
    demo_nlg()
    
    print("\n" + "="*60)
    print("NLG Features:")
    print("  ✓ Lambda term explanations")
    print("  ✓ Reduction step explanations")
    print("  ✓ Type explanations")
    print("  ✓ Proof step explanations")
    print("  ✓ Tutorial generation")
    print("="*60)
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
