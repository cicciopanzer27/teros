"""
Intent Classifier for Lambda³ NLU
Classify user intents for natural language understanding
"""

from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from enum import Enum
import re
import json

class Intent(Enum):
    """User intent types"""
    PARSE = "parse"
    REDUCE = "reduce"
    TYPE = "type"
    ENCODE = "encode"
    HELP = "help"
    EXPLAIN = "explain"
    TUTORIAL = "tutorial"
    UNKNOWN = "unknown"

@dataclass
class IntentResult:
    """Intent classification result"""
    intent: Intent
    confidence: float
    entities: Dict[str, str]
    parameters: Dict[str, str]

class IntentClassifier:
    """
    Intent classifier for Lambda³ NLU
    
    Classifies user queries into actionable intents:
    - parse: "What is the identity function?"
    - reduce: "Simplify (λx.x) y"
    - type: "What type is λx.x?"
    - encode: "Show ternary encoding of λx.x"
    """
    
    def __init__(self):
        self.intent_patterns = {
            Intent.PARSE: [
                r'what is (?:the )?(\w+)',
                r'define (\w+)',
                r'explain (\w+)',
                r'show me (\w+)',
                r'what does (\w+) mean'
            ],
            Intent.REDUCE: [
                r'reduce (.+)',
                r'simplify (.+)',
                r'evaluate (.+)',
                r'compute (.+)',
                r'calculate (.+)'
            ],
            Intent.TYPE: [
                r'what type is (.+)',
                r'type of (.+)',
                r'type (.+)',
                r'what is the type of (.+)'
            ],
            Intent.ENCODE: [
                r'ternary encoding of (.+)',
                r'encode (.+)',
                r'show trits for (.+)',
                r'ternary representation of (.+)'
            ],
            Intent.HELP: [
                r'help',
                r'commands',
                r'what can you do',
                r'how to use'
            ],
            Intent.EXPLAIN: [
                r'explain (.+)',
                r'how does (.+) work',
                r'why (.+)',
                r'tell me about (.+)'
            ],
            Intent.TUTORIAL: [
                r'tutorial',
                r'learn',
                r'teach me',
                r'how to'
            ]
        }
        
        self.entity_patterns = {
            'function': [
                r'identity function',
                r'constant function',
                r'composition',
                r'application'
            ],
            'term': [
                r'\\x\.x',
                r'\\x\.\\y\.x',
                r'\\f\.\\g\.\\x\.f \(g x\)'
            ],
            'variable': [
                r'[a-zA-Z]\w*'
            ]
        }
    
    def classify(self, query: str) -> IntentResult:
        """Classify user intent from query"""
        query = query.lower().strip()
        
        # Find best matching intent
        best_intent = Intent.UNKNOWN
        best_confidence = 0.0
        best_entities = {}
        best_parameters = {}
        
        for intent, patterns in self.intent_patterns.items():
            for pattern in patterns:
                match = re.search(pattern, query)
                if match:
                    confidence = self._calculate_confidence(pattern, match)
                    if confidence > best_confidence:
                        best_intent = intent
                        best_confidence = confidence
                        best_entities = self._extract_entities(query)
                        best_parameters = self._extract_parameters(match)
        
        return IntentResult(
            intent=best_intent,
            confidence=best_confidence,
            entities=best_entities,
            parameters=best_parameters
        )
    
    def _calculate_confidence(self, pattern: str, match: re.Match) -> float:
        """Calculate confidence score for intent match"""
        # Base confidence from pattern complexity
        base_confidence = 0.5
        
        # Boost for exact matches
        if match.group(0) == match.string.strip():
            base_confidence += 0.3
        
        # Boost for longer matches
        match_length = len(match.group(0))
        query_length = len(match.string)
        length_boost = (match_length / query_length) * 0.2
        
        return min(base_confidence + length_boost, 1.0)
    
    def _extract_entities(self, query: str) -> Dict[str, str]:
        """Extract entities from query"""
        entities = {}
        
        for entity_type, patterns in self.entity_patterns.items():
            for pattern in patterns:
                if re.search(pattern, query, re.IGNORECASE):
                    entities[entity_type] = pattern
                    break
        
        return entities
    
    def _extract_parameters(self, match: re.Match) -> Dict[str, str]:
        """Extract parameters from match"""
        parameters = {}
        
        if match.groups():
            for i, group in enumerate(match.groups()):
                parameters[f'param_{i+1}'] = group
        
        return parameters
    
    def get_suggestions(self, intent: Intent) -> List[str]:
        """Get suggestions for specific intent"""
        suggestions = {
            Intent.PARSE: [
                "What is the identity function?",
                "Define the constant function",
                "Show me function composition"
            ],
            Intent.REDUCE: [
                "Reduce (\\x.x) y",
                "Simplify (\\x.\\y.x) a b",
                "Evaluate function application"
            ],
            Intent.TYPE: [
                "What type is \\x.x?",
                "Type of (\\x.x) y",
                "What is the type of \\f.\\g.\\x.f (g x)?"
            ],
            Intent.ENCODE: [
                "Show ternary encoding of \\x.x",
                "Encode (\\x.x) y",
                "Ternary representation of \\f.\\g.\\x.f (g x)"
            ],
            Intent.HELP: [
                "help",
                "commands",
                "what can you do"
            ],
            Intent.EXPLAIN: [
                "Explain lambda calculus",
                "How does beta reduction work?",
                "Tell me about Church numerals"
            ],
            Intent.TUTORIAL: [
                "tutorial",
                "learn lambda calculus",
                "how to use this system"
            ]
        }
        
        return suggestions.get(intent, [])
    
    def get_intent_description(self, intent: Intent) -> str:
        """Get description of intent"""
        descriptions = {
            Intent.PARSE: "Parse and understand lambda terms",
            Intent.REDUCE: "Reduce lambda terms to normal form",
            Intent.TYPE: "Infer types of lambda terms",
            Intent.ENCODE: "Show ternary encoding of lambda terms",
            Intent.HELP: "Show help and available commands",
            Intent.EXPLAIN: "Explain lambda calculus concepts",
            Intent.TUTORIAL: "Interactive tutorial and learning"
        }
        
        return descriptions.get(intent, "Unknown intent")

# ============================================================================
# DEMO
# ============================================================================

def demo_intent_classifier():
    """Demonstrate intent classification"""
    print("="*60)
    print("  Lambda³ Intent Classifier Demo")
    print("="*60)
    
    classifier = IntentClassifier()
    
    test_queries = [
        "What is the identity function?",
        "Reduce (\\x.x) y",
        "What type is \\x.x?",
        "Show ternary encoding of \\x.x",
        "help",
        "Explain lambda calculus",
        "tutorial"
    ]
    
    for query in test_queries:
        result = classifier.classify(query)
        print(f"\nQuery: {query}")
        print(f"Intent: {result.intent.value}")
        print(f"Confidence: {result.confidence:.2f}")
        print(f"Entities: {result.entities}")
        print(f"Parameters: {result.parameters}")
        
        # Get suggestions
        suggestions = classifier.get_suggestions(result.intent)
        if suggestions:
            print(f"Suggestions: {suggestions[:2]}")

def main():
    print("="*60)
    print("  Lambda³ Intent Classifier")
    print("  Natural Language Intent Understanding")
    print("="*60)
    
    demo_intent_classifier()
    
    print("\n" + "="*60)
    print("Intent Classifier Features:")
    print("  Parse lambda terms")
    print("  Reduce expressions")
    print("  Type inference")
    print("  Ternary encoding")
    print("  Help and tutorials")
    print("  Entity extraction")
    print("  Confidence scoring")
    print("="*60)
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
