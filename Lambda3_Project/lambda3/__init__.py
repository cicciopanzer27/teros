"""
Lambda³ - Hybrid Neural-Symbolic AI on Ternary Substrate

The first AI system that REASONS instead of just pattern-matching.
"""

__version__ = "0.1.0"
__author__ = "Lambda³ Project Team"
__email__ = "team@lambda3.ai"

from lambda3.parser import parse
from lambda3.engine import reduce
from lambda3.ternary import encode, decode

__all__ = [
    "parse",
    "reduce",
    "encode",
    "decode",
]

