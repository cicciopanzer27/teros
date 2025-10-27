"""
Natural Language Generation for Lambda³
Convert lambda terms and proofs to natural language
"""

from .generator import NLGGenerator
from .explainer import ProofExplainer
from .formatter import LambdaFormatter

__all__ = ['NLGGenerator', 'ProofExplainer', 'LambdaFormatter']
