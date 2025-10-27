"""
Natural Language Parser for Lambda³
Convert natural language queries to lambda terms
"""

import re
from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass

try:
    from lambda3.parser.parser import parse
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.parser import parse


@dataclass
class NLUResult:
    """Result of natural language understanding"""
    intent: str
    entities: Dict[str, str]
    lambda_term: Optional[str]
    confidence: float
    explanation: str


class NLUParser:
    """
    Natural Language Understanding Parser
    
    Converts queries like:
    - "What is the identity function?" → λx.x
    - "Show me function composition" → λf.λg.λx.f(g x)
    - "Prove A implies A" → A → A
    """
    
    def __init__(self):
        self.intent_patterns = {
            'define': [
                r'what is (?:the )?(\w+)',
                r'define (\w+)',
                r'explain (\w+)',
                r'show me (\w+)'
            ],
            'prove': [
                r'prove (.+)',
                r'show that (.+)',
                r'demonstrate (.+)'
            ],
            'reduce': [
                r'reduce (.+)',
                r'simplify (.+)',
                r'evaluate (.+)'
            ],
            'compose': [
                r'compose (.+)',
                r'combine (.+)',
                r'apply (.+)'
            ]
        }
        
        self.function_definitions = {
            'identity': r'\x.x',
            'constant': r'\x.\y.x',
            'composition': r'\f.\g.\x.f (g x)',
            'application': r'\f.\x.f x',
            'curry': r'\f.\x.\y.f x y',
            'uncurry': r'\f.\x y.f x y',
            'flip': r'\f.\x.\y.f y x',
            'twice': r'\f.\x.f (f x)',
            'thrice': r'\f.\x.f (f (f x))',
        }
        
        self.logical_patterns = {
            'implication': r'(.+) implies (.+)',
            'conjunction': r'(.+) and (.+)',
            'disjunction': r'(.+) or (.+)',
            'negation': r'not (.+)',
        }
    
    def parse(self, query: str) -> NLUResult:
        """
        Parse natural language query
        
        Args:
            query: Natural language input
            
        Returns:
            NLUResult with intent, entities, and lambda term
        """
        query = query.lower().strip()
        
        # Classify intent
        intent = self._classify_intent(query)
        
        # Extract entities
        entities = self._extract_entities(query)
        
        # Generate lambda term
        lambda_term, explanation = self._generate_lambda_term(query, intent, entities)
        
        # Calculate confidence
        confidence = self._calculate_confidence(intent, entities, lambda_term)
        
        return NLUResult(
            intent=intent,
            entities=entities,
            lambda_term=lambda_term,
            confidence=confidence,
            explanation=explanation
        )
    
    def _classify_intent(self, query: str) -> str:
        """Classify the intent of the query"""
        for intent, patterns in self.intent_patterns.items():
            for pattern in patterns:
                if re.search(pattern, query):
                    return intent
        
        return 'unknown'
    
    def _extract_entities(self, query: str) -> Dict[str, str]:
        """Extract entities from the query"""
        entities = {}
        
        # Extract function names
        for func_name in self.function_definitions.keys():
            if func_name in query:
                entities['function'] = func_name
        
        # Extract logical propositions
        for pattern_name, pattern in self.logical_patterns.items():
            match = re.search(pattern, query)
            if match:
                entities['logical_pattern'] = pattern_name
                entities['propositions'] = match.groups()
        
        # Extract variables
        var_pattern = r'\b([A-Z])\b'
        variables = re.findall(var_pattern, query)
        if variables:
            entities['variables'] = variables
        
        return entities
    
    def _generate_lambda_term(self, query: str, intent: str, entities: Dict[str, str]) -> Tuple[Optional[str], str]:
        """Generate lambda term from query"""
        
        if intent == 'define':
            if 'function' in entities:
                func_name = entities['function']
                if func_name in self.function_definitions:
                    term = self.function_definitions[func_name]
                    explanation = f"Definition of {func_name} function"
                    return term, explanation
        
        elif intent == 'prove':
            if 'logical_pattern' in entities:
                pattern = entities['logical_pattern']
                if pattern == 'implication' and 'propositions' in entities:
                    p, q = entities['propositions']
                    term = f"({p} -> {q})"
                    explanation = f"Implication: {p} implies {q}"
                    return term, explanation
        
        elif intent == 'reduce':
            # Try to extract lambda expression
            lambda_match = re.search(r'(\\([a-z]+\\)\\.[^\\s]+)', query)
            if lambda_match:
                term = lambda_match.group(1)
                explanation = f"Reduction of {term}"
                return term, explanation
        
        elif intent == 'compose':
            # Function composition
            term = r'\f.\g.\x.f (g x)'
            explanation = "Function composition"
            return term, explanation
        
        return None, "Could not generate lambda term"
    
    def _calculate_confidence(self, intent: str, entities: Dict[str, str], lambda_term: Optional[str]) -> float:
        """Calculate confidence score"""
        confidence = 0.0
        
        # Intent confidence
        if intent != 'unknown':
            confidence += 0.3
        
        # Entity confidence
        if entities:
            confidence += 0.3
        
        # Lambda term confidence
        if lambda_term:
            confidence += 0.4
        
        return min(confidence, 1.0)
    
    def get_suggestions(self, query: str) -> List[str]:
        """Get suggestions for incomplete queries"""
        suggestions = []
        
        if 'function' in query.lower():
            suggestions.extend([
                "What is the identity function?",
                "Define the constant function",
                "Show me function composition"
            ])
        
        if 'prove' in query.lower():
            suggestions.extend([
                "Prove A implies A",
                "Show that A and B implies A",
                "Demonstrate modus ponens"
            ])
        
        if 'reduce' in query.lower():
            suggestions.extend([
                "Reduce (\\x.x) y",
                "Simplify (\\x.\\y.x) a b",
                "Evaluate function application"
            ])
        
        return suggestions


# ============================================================================
# DEMO
# ============================================================================

def demo_nlu():
    """Demonstrate NLU capabilities"""
    print("="*60)
    print("  Natural Language Understanding Demo")
    print("="*60)
    
    parser = NLUParser()
    
    test_queries = [
        "What is the identity function?",
        "Define the constant function",
        "Show me function composition",
        "Prove A implies A",
        "Reduce (\\x.x) y",
        "Compose two functions"
    ]
    
    for query in test_queries:
        print(f"\nQuery: {query}")
        result = parser.parse(query)
        
        print(f"  Intent: {result.intent}")
        print(f"  Entities: {result.entities}")
        print(f"  Lambda Term: {result.lambda_term}")
        print(f"  Confidence: {result.confidence:.2f}")
        print(f"  Explanation: {result.explanation}")
        
        if result.lambda_term:
            try:
                parsed = parse(result.lambda_term)
                print(f"  Parsed: {parsed}")
            except Exception as e:
                print(f"  Parse Error: {e}")


def main():
    print("="*60)
    print("  Lambda³ NLU Parser")
    print("  Natural Language to Lambda Terms")
    print("="*60)
    
    demo_nlu()
    
    print("\n" + "="*60)
    print("NLU Features:")
    print("  ✓ Intent classification")
    print("  ✓ Entity extraction")
    print("  ✓ Lambda term generation")
    print("  ✓ Confidence scoring")
    print("  ✓ Query suggestions")
    print("="*60)
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
