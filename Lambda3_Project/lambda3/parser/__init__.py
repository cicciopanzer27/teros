"""
Lambda Calculus Parser

Parses lambda terms from strings into AST.

Example:
    >>> from lambda3.parser import parse
    >>> term = parse("(λx.x) y")
    >>> print(term)
    App(Abs('x', Var('x')), Var('y'))
"""

try:
    from lambda3.parser.lambda_parser import parse, LambdaTerm
except ImportError:
    from .lambda_parser import parse, LambdaTerm

__all__ = ["parse", "LambdaTerm"]

