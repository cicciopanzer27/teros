"""
Proof Explainer for Lambda³ NLG
Generate explanations for lambda calculus proofs and reductions
"""

from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import re

class ExplanationType(Enum):
    """Types of explanations"""
    STEP_BY_STEP = "step_by_step"
    CONCEPTUAL = "conceptual"
    TECHNICAL = "technical"
    TUTORIAL = "tutorial"

@dataclass
class ExplanationStep:
    """Single step in explanation"""
    step_number: int
    description: str
    lambda_term: str
    rule_applied: str
    reasoning: str

@dataclass
class ProofExplanation:
    """Complete proof explanation"""
    title: str
    steps: List[ExplanationStep]
    conclusion: str
    difficulty: str
    concepts: List[str]

class ProofExplainer:
    """
    Proof explainer for Lambda³ NLG
    
    Generates explanations for:
    - Beta reduction steps
    - Type inference proofs
    - Lambda calculus concepts
    - Tutorial explanations
    """
    
    def __init__(self):
        self.reduction_rules = {
            'beta': 'β-reduction (function application)',
            'alpha': 'α-conversion (variable renaming)',
            'eta': 'η-reduction (function equivalence)',
            'substitution': 'Variable substitution'
        }
        
        self.concept_explanations = {
            'identity': 'The identity function returns its input unchanged',
            'constant': 'The constant function ignores its second argument',
            'composition': 'Function composition applies one function to the result of another',
            'application': 'Function application applies a function to an argument'
        }
    
    def explain_reduction(self, original: str, reduced: str, steps: List[Tuple[str, str]]) -> ProofExplanation:
        """Explain a lambda reduction step by step"""
        explanation_steps = []
        
        for i, (term, rule) in enumerate(steps):
            step = ExplanationStep(
                step_number=i + 1,
                description=f"Apply {rule}",
                lambda_term=term,
                rule_applied=rule,
                reasoning=self._get_reasoning(rule, term)
            )
            explanation_steps.append(step)
        
        return ProofExplanation(
            title=f"Reduction of {original}",
            steps=explanation_steps,
            conclusion=f"Final result: {reduced}",
            difficulty="intermediate",
            concepts=["beta reduction", "lambda calculus"]
        )
    
    def explain_concept(self, concept: str) -> str:
        """Explain a lambda calculus concept"""
        if concept in self.concept_explanations:
            return self.concept_explanations[concept]
        
        # Generate explanation for unknown concepts
        return f"The {concept} is a fundamental concept in lambda calculus."
    
    def explain_type_inference(self, term: str, type_result: str) -> ProofExplanation:
        """Explain type inference process"""
        steps = [
            ExplanationStep(
                step_number=1,
                description="Analyze the lambda term structure",
                lambda_term=term,
                rule_applied="Structural analysis",
                reasoning="Identify variables, abstractions, and applications"
            ),
            ExplanationStep(
                step_number=2,
                description="Apply type inference rules",
                lambda_term=term,
                rule_applied="Hindley-Milner inference",
                reasoning="Use unification to determine types"
            ),
            ExplanationStep(
                step_number=3,
                description="Generate type signature",
                lambda_term=term,
                rule_applied="Type generation",
                reasoning="Create final type based on inference"
            )
        ]
        
        return ProofExplanation(
            title=f"Type inference for {term}",
            steps=steps,
            conclusion=f"Inferred type: {type_result}",
            difficulty="advanced",
            concepts=["type theory", "Hindley-Milner", "unification"]
        )
    
    def explain_tutorial(self, topic: str) -> ProofExplanation:
        """Generate tutorial explanation"""
        if topic == "lambda_calculus":
            return self._lambda_calculus_tutorial()
        elif topic == "beta_reduction":
            return self._beta_reduction_tutorial()
        elif topic == "type_theory":
            return self._type_theory_tutorial()
        else:
            return self._general_tutorial(topic)
    
    def _lambda_calculus_tutorial(self) -> ProofExplanation:
        """Lambda calculus tutorial"""
        steps = [
            ExplanationStep(
                step_number=1,
                description="Introduction to lambda calculus",
                lambda_term="",
                rule_applied="Conceptual",
                reasoning="Lambda calculus is a formal system for expressing computation"
            ),
            ExplanationStep(
                step_number=2,
                description="Variables and abstractions",
                lambda_term="\\x.x",
                rule_applied="Definition",
                reasoning="Variables are placeholders, abstractions create functions"
            ),
            ExplanationStep(
                step_number=3,
                description="Function application",
                lambda_term="(\\x.x) y",
                rule_applied="Application",
                reasoning="Apply function to argument using parentheses"
            )
        ]
        
        return ProofExplanation(
            title="Lambda Calculus Tutorial",
            steps=steps,
            conclusion="You now understand the basics of lambda calculus!",
            difficulty="beginner",
            concepts=["lambda calculus", "functions", "computation"]
        )
    
    def _beta_reduction_tutorial(self) -> ProofExplanation:
        """Beta reduction tutorial"""
        steps = [
            ExplanationStep(
                step_number=1,
                description="What is beta reduction?",
                lambda_term="(\\x.M) N",
                rule_applied="β-reduction",
                reasoning="Replace all occurrences of x in M with N"
            ),
            ExplanationStep(
                step_number=2,
                description="Example: Identity function",
                lambda_term="(\\x.x) y",
                rule_applied="β-reduction",
                reasoning="Replace x with y: x becomes y"
            ),
            ExplanationStep(
                step_number=3,
                description="Result",
                lambda_term="y",
                rule_applied="Simplification",
                reasoning="The result is y"
            )
        ]
        
        return ProofExplanation(
            title="Beta Reduction Tutorial",
            steps=steps,
            conclusion="Beta reduction is the core computation rule of lambda calculus",
            difficulty="intermediate",
            concepts=["beta reduction", "computation", "substitution"]
        )
    
    def _type_theory_tutorial(self) -> ProofExplanation:
        """Type theory tutorial"""
        steps = [
            ExplanationStep(
                step_number=1,
                description="What are types?",
                lambda_term="x : A",
                rule_applied="Type annotation",
                reasoning="Types describe what kind of value a term represents"
            ),
            ExplanationStep(
                step_number=2,
                description="Function types",
                lambda_term="f : A → B",
                rule_applied="Arrow type",
                reasoning="Functions have arrow types: input type → output type"
            ),
            ExplanationStep(
                step_number=3,
                description="Type inference",
                lambda_term="\\x.x",
                rule_applied="Inference",
                reasoning="The system can automatically determine types"
            )
        ]
        
        return ProofExplanation(
            title="Type Theory Tutorial",
            steps=steps,
            conclusion="Types help ensure correctness and catch errors",
            difficulty="advanced",
            concepts=["type theory", "types", "correctness"]
        )
    
    def _general_tutorial(self, topic: str) -> ProofExplanation:
        """General tutorial for any topic"""
        steps = [
            ExplanationStep(
                step_number=1,
                description=f"Introduction to {topic}",
                lambda_term="",
                rule_applied="Conceptual",
                reasoning=f"Let's explore {topic} step by step"
            )
        ]
        
        return ProofExplanation(
            title=f"{topic.title()} Tutorial",
            steps=steps,
            conclusion=f"You've learned about {topic}!",
            difficulty="beginner",
            concepts=[topic]
        )
    
    def _get_reasoning(self, rule: str, term: str) -> str:
        """Get reasoning for a reduction rule"""
        if rule == 'beta':
            return "Apply function to argument by substitution"
        elif rule == 'alpha':
            return "Rename variable to avoid conflicts"
        elif rule == 'eta':
            return "Simplify function equivalence"
        else:
            return f"Apply {rule} rule"
    
    def generate_summary(self, explanation: ProofExplanation) -> str:
        """Generate summary of explanation"""
        summary = f"**{explanation.title}**\n\n"
        summary += f"**Difficulty:** {explanation.difficulty}\n"
        summary += f"**Concepts:** {', '.join(explanation.concepts)}\n\n"
        
        for step in explanation.steps:
            summary += f"**Step {step.step_number}:** {step.description}\n"
            if step.lambda_term:
                summary += f"  Term: `{step.lambda_term}`\n"
            summary += f"  Reasoning: {step.reasoning}\n\n"
        
        summary += f"**Conclusion:** {explanation.conclusion}\n"
        
        return summary

# ============================================================================
# DEMO
# ============================================================================

def demo_proof_explainer():
    """Demonstrate proof explanation"""
    print("="*60)
    print("  Lambda³ Proof Explainer Demo")
    print("="*60)
    
    explainer = ProofExplainer()
    
    # Test reduction explanation
    print("1. Reduction Explanation:")
    steps = [("(\\x.x) y", "beta"), ("y", "simplification")]
    explanation = explainer.explain_reduction("(\\x.x) y", "y", steps)
    print(f"Title: {explanation.title}")
    print(f"Steps: {len(explanation.steps)}")
    print(f"Conclusion: {explanation.conclusion}")
    
    # Test concept explanation
    print("\n2. Concept Explanation:")
    concept_explanation = explainer.explain_concept("identity")
    print(f"Identity function: {concept_explanation}")
    
    # Test tutorial
    print("\n3. Tutorial Explanation:")
    tutorial = explainer.explain_tutorial("lambda_calculus")
    print(f"Tutorial: {tutorial.title}")
    print(f"Steps: {len(tutorial.steps)}")
    print(f"Difficulty: {tutorial.difficulty}")
    
    # Test summary generation
    print("\n4. Summary Generation:")
    summary = explainer.generate_summary(explanation)
    print("Summary generated successfully")

def main():
    print("="*60)
    print("  Lambda³ Proof Explainer")
    print("  Natural Language Proof Explanations")
    print("="*60)
    
    demo_proof_explainer()
    
    print("\n" + "="*60)
    print("Proof Explainer Features:")
    print("  Step-by-step explanations")
    print("  Concept explanations")
    print("  Tutorial generation")
    print("  Type inference explanations")
    print("  Summary generation")
    print("  Multiple difficulty levels")
    print("="*60)
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
