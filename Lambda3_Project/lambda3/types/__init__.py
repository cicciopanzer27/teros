"""
Type System for Lambda3
Simply Typed Lambda Calculus with type inference
"""

try:
    from lambda3.types.type_checker import (
        Type, BaseType, ArrowType, TypeVar,
        BOOL, INT, STRING, UNIT,
        TypeContext, TypeError,
        TypeChecker, type_check
    )
except ImportError:
    from .type_checker import (
        Type, BaseType, ArrowType, TypeVar,
        BOOL, INT, STRING, UNIT,
        TypeContext, TypeError,
        TypeChecker, type_check
    )

__all__ = [
    'Type', 'BaseType', 'ArrowType', 'TypeVar',
    'BOOL', 'INT', 'STRING', 'UNIT',
    'TypeContext', 'TypeError',
    'TypeChecker', 'type_check'
]

