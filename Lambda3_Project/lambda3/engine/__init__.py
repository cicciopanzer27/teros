"""
Lambda Calculus Reduction Engine

Performs β-reduction on lambda terms.
"""

try:
    from lambda3.engine.reducer import reduce
except ImportError:
    from .reducer import reduce

__all__ = ["reduce"]

