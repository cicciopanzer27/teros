"""
Trit - Basic ternary value implementation.

A Trit represents a single ternary digit with values:
- -1 (Negative/False)
- 0 (Neutral/Unknown) 
- 1 (Positive/True)

This is the fundamental building block for all ternary operations in TEROS.
"""

from typing import Union, Optional
import numpy as np


class Trit:
    """
    Basic ternary value implementation.
    
    A Trit represents a single ternary digit with three possible values:
    - -1: Negative/False
    - 0: Neutral/Unknown
    - 1: Positive/True
    """
    
    # Valid ternary values
    NEGATIVE = -1
    NEUTRAL = 0
    POSITIVE = 1
    
    # Binary encoding for efficient storage
    BINARY_ENCODING = {
        -1: 0b00,  # 00
        0: 0b01,   # 01  
        1: 0b10    # 10
    }
    
    # Reverse encoding for decoding
    BINARY_DECODING = {v: k for k, v in BINARY_ENCODING.items()}
    
    def __init__(self, value: Union[int, 'Trit', str, None] = 0):
        """
        Initialize a Trit with a ternary value.
        
        Args:
            value: The ternary value (-1, 0, 1), another Trit, string representation, or None
        """
        if isinstance(value, Trit):
            self._value = value._value
        elif isinstance(value, str):
            self._value = self._from_string(value)
        elif value is None:
            self._value = 0
        else:
            self._value = self._validate_value(int(value))
    
    def _validate_value(self, value: int) -> int:
        """Validate that the value is a valid ternary value."""
        if value not in [-1, 0, 1]:
            raise ValueError(f"Invalid ternary value: {value}. Must be -1, 0, or 1")
        return value
    
    def _from_string(self, value: str) -> int:
        """Convert string representation to ternary value."""
        value = value.strip().lower()
        if value in ['-1', 'negative', 'neg', 'false', 'f']:
            return -1
        elif value in ['0', 'neutral', 'neu', 'unknown', 'u']:
            return 0
        elif value in ['1', 'positive', 'pos', 'true', 't']:
            return 1
        else:
            raise ValueError(f"Invalid string representation: {value}")
    
    @property
    def value(self) -> int:
        """Get the ternary value."""
        return self._value
    
    @value.setter
    def value(self, new_value: int) -> None:
        """Set the ternary value."""
        self._value = self._validate_value(new_value)
    
    def __int__(self) -> int:
        """Convert to integer."""
        return self._value
    
    def __str__(self) -> str:
        """String representation."""
        return str(self._value)
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Trit({self._value})"
    
    def __bool__(self) -> bool:
        """Boolean conversion (True for positive, False for negative/neutral)."""
        return self._value == 1
    
    def __eq__(self, other) -> bool:
        """Equality comparison."""
        if isinstance(other, Trit):
            return self._value == other._value
        return self._value == other
    
    def __ne__(self, other) -> bool:
        """Inequality comparison."""
        return not self.__eq__(other)
    
    def __lt__(self, other) -> bool:
        """Less than comparison."""
        if isinstance(other, Trit):
            return self._value < other._value
        return self._value < other
    
    def __le__(self, other) -> bool:
        """Less than or equal comparison."""
        if isinstance(other, Trit):
            return self._value <= other._value
        return self._value <= other
    
    def __gt__(self, other) -> bool:
        """Greater than comparison."""
        if isinstance(other, Trit):
            return self._value > other._value
        return self._value > other
    
    def __ge__(self, other) -> bool:
        """Greater than or equal comparison."""
        if isinstance(other, Trit):
            return self._value >= other._value
        return self._value >= other
    
    def __hash__(self) -> int:
        """Hash for use in sets and dictionaries."""
        return hash(self._value)
    
    # Arithmetic operations
    def __add__(self, other) -> 'Trit':
        """Ternary addition."""
        if isinstance(other, Trit):
            return Trit(self._ternary_add(self._value, other._value))
        return Trit(self._ternary_add(self._value, int(other)))
    
    def __sub__(self, other) -> 'Trit':
        """Ternary subtraction."""
        if isinstance(other, Trit):
            return Trit(self._ternary_sub(self._value, other._value))
        return Trit(self._ternary_sub(self._value, int(other)))
    
    def __mul__(self, other) -> 'Trit':
        """Ternary multiplication."""
        if isinstance(other, Trit):
            return Trit(self._ternary_mul(self._value, other._value))
        return Trit(self._ternary_mul(self._value, int(other)))
    
    def __neg__(self) -> 'Trit':
        """Unary negation."""
        return Trit(-self._value)
    
    def __abs__(self) -> 'Trit':
        """Absolute value."""
        return Trit(abs(self._value))
    
    # Logic operations
    def __and__(self, other) -> 'Trit':
        """Ternary AND (conjunction)."""
        if isinstance(other, Trit):
            return Trit(self._ternary_and(self._value, other._value))
        return Trit(self._ternary_and(self._value, int(other)))
    
    def __or__(self, other) -> 'Trit':
        """Ternary OR (disjunction)."""
        if isinstance(other, Trit):
            return Trit(self._ternary_or(self._value, other._value))
        return Trit(self._ternary_or(self._value, int(other)))
    
    def __invert__(self) -> 'Trit':
        """Ternary NOT (negation)."""
        return Trit(self._ternary_not(self._value))
    
    def __xor__(self, other) -> 'Trit':
        """Ternary XOR (exclusive or)."""
        if isinstance(other, Trit):
            return Trit(self._ternary_xor(self._value, other._value))
        return Trit(self._ternary_xor(self._value, int(other)))
    
    # Ternary arithmetic lookup tables
    @staticmethod
    def _ternary_add(a: int, b: int) -> int:
        """Ternary addition lookup table."""
        table = {
            (-1, -1): -1, (-1, 0): -1, (-1, 1): 0,
            (0, -1): -1,  (0, 0): 0,   (0, 1): 1,
            (1, -1): 0,   (1, 0): 1,   (1, 1): 1
        }
        return table.get((a, b), 0)
    
    @staticmethod
    def _ternary_sub(a: int, b: int) -> int:
        """Ternary subtraction lookup table."""
        table = {
            (-1, -1): 0,  (-1, 0): -1, (-1, 1): -1,
            (0, -1): 1,   (0, 0): 0,   (0, 1): -1,
            (1, -1): 1,   (1, 0): 1,   (1, 1): 0
        }
        return table.get((a, b), 0)
    
    @staticmethod
    def _ternary_mul(a: int, b: int) -> int:
        """Ternary multiplication lookup table."""
        table = {
            (-1, -1): 1,  (-1, 0): 0,  (-1, 1): -1,
            (0, -1): 0,   (0, 0): 0,   (0, 1): 0,
            (1, -1): -1,  (1, 0): 0,   (1, 1): 1
        }
        return table.get((a, b), 0)
    
    # Ternary logic lookup tables
    @staticmethod
    def _ternary_and(a: int, b: int) -> int:
        """Ternary AND lookup table."""
        table = {
            (-1, -1): -1, (-1, 0): -1, (-1, 1): -1,
            (0, -1): -1,  (0, 0): 0,    (0, 1): 0,
            (1, -1): -1,  (1, 0): 0,    (1, 1): 1
        }
        return table.get((a, b), 0)
    
    @staticmethod
    def _ternary_or(a: int, b: int) -> int:
        """Ternary OR lookup table."""
        table = {
            (-1, -1): -1, (-1, 0): 0,  (-1, 1): 1,
            (0, -1): 0,   (0, 0): 0,    (0, 1): 1,
            (1, -1): 1,   (1, 0): 1,    (1, 1): 1
        }
        return table.get((a, b), 0)
    
    @staticmethod
    def _ternary_not(a: int) -> int:
        """Ternary NOT lookup table."""
        return -a
    
    @staticmethod
    def _ternary_xor(a: int, b: int) -> int:
        """Ternary XOR lookup table."""
        table = {
            (-1, -1): 0,  (-1, 0): -1, (-1, 1): 1,
            (0, -1): -1,  (0, 0): 0,   (0, 1): 1,
            (1, -1): 1,   (1, 0): 1,   (1, 1): 0
        }
        return table.get((a, b), 0)
    
    # Binary encoding/decoding for efficient storage
    def to_binary(self) -> int:
        """Convert to binary representation for storage."""
        return self.BINARY_ENCODING[self._value]
    
    @classmethod
    def from_binary(cls, binary_value: int) -> 'Trit':
        """Create Trit from binary representation."""
        if binary_value not in cls.BINARY_DECODING:
            raise ValueError(f"Invalid binary encoding: {binary_value}")
        return cls(cls.BINARY_DECODING[binary_value])
    
    # Utility methods
    def is_positive(self) -> bool:
        """Check if trit is positive."""
        return self._value == 1
    
    def is_negative(self) -> bool:
        """Check if trit is negative."""
        return self._value == -1
    
    def is_neutral(self) -> bool:
        """Check if trit is neutral."""
        return self._value == 0
    
    def is_truthy(self) -> bool:
        """Check if trit is truthy (positive)."""
        return self._value == 1
    
    def is_falsy(self) -> bool:
        """Check if trit is falsy (negative)."""
        return self._value == -1
    
    def is_unknown(self) -> bool:
        """Check if trit is unknown (neutral)."""
        return self._value == 0
    
    def to_string(self, format_type: str = "numeric") -> str:
        """Convert to string representation."""
        if format_type == "numeric":
            return str(self._value)
        elif format_type == "symbolic":
            return {-1: "-", 0: "0", 1: "+"}[self._value]
        elif format_type == "logical":
            return {-1: "F", 0: "U", 1: "T"}[self._value]
        else:
            raise ValueError(f"Invalid format type: {format_type}")
    
    # Class methods for common operations
    @classmethod
    def zero(cls) -> 'Trit':
        """Create a neutral trit."""
        return cls(0)
    
    @classmethod
    def one(cls) -> 'Trit':
        """Create a positive trit."""
        return cls(1)
    
    @classmethod
    def negative_one(cls) -> 'Trit':
        """Create a negative trit."""
        return cls(-1)
    
    @classmethod
    def random(cls) -> 'Trit':
        """Create a random trit."""
        import random
        return cls(random.choice([-1, 0, 1]))
    
    # Constants for easy access
    ZERO = None
    ONE = None
    NEGATIVE_ONE = None
    
    def __new__(cls, value: Union[int, 'Trit', str, None] = 0):
        """Override __new__ to implement singleton pattern for common values."""
        if value == 0 and cls.ZERO is None:
            instance = super().__new__(cls)
            instance._value = 0
            cls.ZERO = instance
            return instance
        elif value == 1 and cls.ONE is None:
            instance = super().__new__(cls)
            instance._value = 1
            cls.ONE = instance
            return instance
        elif value == -1 and cls.NEGATIVE_ONE is None:
            instance = super().__new__(cls)
            instance._value = -1
            cls.NEGATIVE_ONE = instance
            return instance
        else:
            return super().__new__(cls)


# Initialize class constants
Trit.ZERO = Trit(0)
Trit.ONE = Trit(1)
Trit.NEGATIVE_ONE = Trit(-1)
