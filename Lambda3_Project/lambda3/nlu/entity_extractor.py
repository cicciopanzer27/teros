"""
Entity Extractor for Lambda³ NLU
Extract entities from natural language queries
"""

from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import re

class EntityType(Enum):
    """Entity types"""
    FUNCTION = "function"
    VARIABLE = "variable"
    TERM = "term"
    TYPE = "type"
    NUMBER = "number"
    OPERATOR = "operator"

@dataclass
class Entity:
    """Extracted entity"""
    type: EntityType
    value: str
    start: int
    end: int
    confidence: float

class EntityExtractor:
    """
    Entity extractor for Lambda³ NLU
    
    Extracts entities from natural language:
    - Functions: identity, constant, composition
    - Variables: x, y, z, f, g
    - Terms: λx.x, (λx.x) y
    - Types: A → B, Int, Bool
    """
    
    def __init__(self):
        self.entity_patterns = {
            EntityType.FUNCTION: [
                r'identity function',
                r'constant function',
                r'composition',
                r'application',
                r'successor',
                r'predecessor'
            ],
            EntityType.VARIABLE: [
                r'\b[a-zA-Z]\w*\b'
            ],
            EntityType.TERM: [
                r'\\[a-zA-Z]\w*\.[^\\s]+',
                r'\(\\[a-zA-Z]\w*\.[^\\s]+\)',
                r'\\[a-zA-Z]\w*\.\\[a-zA-Z]\w*\.[^\\s]+'
            ],
            EntityType.TYPE: [
                r'[A-Z]\w*',
                r'[A-Z]\w*\s*→\s*[A-Z]\w*',
                r'Int',
                r'Bool',
                r'String'
            ],
            EntityType.NUMBER: [
                r'\b\d+\b',
                r'\b\d+\.\d+\b'
            ],
            EntityType.OPERATOR: [
                r'→',
                r'×',
                r'\+',
                r'\-',
                r'\*',
                r'/'
            ]
        }
        
        self.function_aliases = {
            'id': 'identity function',
            'const': 'constant function',
            'comp': 'composition',
            'app': 'application',
            'succ': 'successor',
            'pred': 'predecessor'
        }
    
    def extract_entities(self, text: str) -> List[Entity]:
        """Extract all entities from text"""
        entities = []
        
        for entity_type, patterns in self.entity_patterns.items():
            for pattern in patterns:
                matches = re.finditer(pattern, text, re.IGNORECASE)
                for match in matches:
                    entity = Entity(
                        type=entity_type,
                        value=match.group(),
                        start=match.start(),
                        end=match.end(),
                        confidence=self._calculate_confidence(entity_type, match.group())
                    )
                    entities.append(entity)
        
        # Remove overlapping entities (keep highest confidence)
        entities = self._remove_overlaps(entities)
        
        return entities
    
    def _calculate_confidence(self, entity_type: EntityType, value: str) -> float:
        """Calculate confidence for entity"""
        base_confidence = 0.5
        
        # Boost for exact matches
        if entity_type == EntityType.FUNCTION:
            if value.lower() in self.function_aliases.values():
                base_confidence += 0.3
            elif value.lower() in self.function_aliases:
                base_confidence += 0.2
        
        # Boost for longer matches
        if len(value) > 3:
            base_confidence += 0.1
        
        return min(base_confidence, 1.0)
    
    def _remove_overlaps(self, entities: List[Entity]) -> List[Entity]:
        """Remove overlapping entities, keeping highest confidence"""
        if not entities:
            return entities
        
        # Sort by confidence (highest first)
        entities.sort(key=lambda e: e.confidence, reverse=True)
        
        result = []
        for entity in entities:
            # Check for overlaps
            overlaps = False
            for existing in result:
                if (entity.start < existing.end and entity.end > existing.start):
                    overlaps = True
                    break
            
            if not overlaps:
                result.append(entity)
        
        # Sort by position
        result.sort(key=lambda e: e.start)
        return result
    
    def extract_lambda_terms(self, text: str) -> List[str]:
        """Extract lambda terms from text"""
        lambda_patterns = [
            r'\\[a-zA-Z]\w*\.[^\\s]+',
            r'\(\\[a-zA-Z]\w*\.[^\\s]+\)',
            r'\\[a-zA-Z]\w*\.\\[a-zA-Z]\w*\.[^\\s]+'
        ]
        
        terms = []
        for pattern in lambda_patterns:
            matches = re.findall(pattern, text)
            terms.extend(matches)
        
        return list(set(terms))  # Remove duplicates
    
    def extract_variables(self, text: str) -> List[str]:
        """Extract variables from text"""
        variable_pattern = r'\b[a-zA-Z]\w*\b'
        matches = re.findall(variable_pattern, text)
        
        # Filter out common words
        common_words = {'the', 'is', 'of', 'and', 'or', 'in', 'to', 'for', 'with', 'by'}
        variables = [v for v in matches if v.lower() not in common_words]
        
        return list(set(variables))
    
    def extract_functions(self, text: str) -> List[str]:
        """Extract function names from text"""
        function_patterns = [
            r'identity function',
            r'constant function',
            r'composition',
            r'application',
            r'successor',
            r'predecessor'
        ]
        
        functions = []
        for pattern in function_patterns:
            if re.search(pattern, text, re.IGNORECASE):
                functions.append(pattern)
        
        return functions
    
    def get_entity_suggestions(self, entity_type: EntityType) -> List[str]:
        """Get suggestions for entity type"""
        suggestions = {
            EntityType.FUNCTION: [
                "identity function",
                "constant function",
                "composition",
                "application"
            ],
            EntityType.VARIABLE: [
                "x", "y", "z", "f", "g", "h"
            ],
            EntityType.TERM: [
                "\\x.x",
                "\\x.\\y.x",
                "\\f.\\g.\\x.f (g x)"
            ],
            EntityType.TYPE: [
                "A → B",
                "Int",
                "Bool",
                "String"
            ],
            EntityType.NUMBER: [
                "0", "1", "2", "3"
            ],
            EntityType.OPERATOR: [
                "→", "×", "+", "-"
            ]
        }
        
        return suggestions.get(entity_type, [])

# ============================================================================
# DEMO
# ============================================================================

def demo_entity_extractor():
    """Demonstrate entity extraction"""
    print("="*60)
    print("  Lambda³ Entity Extractor Demo")
    print("="*60)
    
    extractor = EntityExtractor()
    
    test_texts = [
        "What is the identity function?",
        "Reduce (\\x.x) y to normal form",
        "What type is \\x.x?",
        "Show ternary encoding of \\f.\\g.\\x.f (g x)",
        "The composition of f and g is \\x.f (g x)"
    ]
    
    for text in test_texts:
        print(f"\nText: {text}")
        
        # Extract entities
        entities = extractor.extract_entities(text)
        print(f"Entities: {len(entities)}")
        for entity in entities:
            print(f"  {entity.type.value}: '{entity.value}' (confidence: {entity.confidence:.2f})")
        
        # Extract specific types
        lambda_terms = extractor.extract_lambda_terms(text)
        if lambda_terms:
            print(f"Lambda terms: {lambda_terms}")
        
        variables = extractor.extract_variables(text)
        if variables:
            print(f"Variables: {variables}")
        
        functions = extractor.extract_functions(text)
        if functions:
            print(f"Functions: {functions}")

def main():
    print("="*60)
    print("  Lambda³ Entity Extractor")
    print("  Natural Language Entity Extraction")
    print("="*60)
    
    demo_entity_extractor()
    
    print("\n" + "="*60)
    print("Entity Extractor Features:")
    print("  Function extraction")
    print("  Variable extraction")
    print("  Lambda term extraction")
    print("  Type extraction")
    print("  Number extraction")
    print("  Operator extraction")
    print("  Overlap resolution")
    print("  Confidence scoring")
    print("="*60)
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
