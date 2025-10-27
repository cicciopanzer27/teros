"""
TritArray - Multi-trit number implementation.

A TritArray represents a multi-trit number in ternary arithmetic,
similar to how binary numbers work but with base-3 arithmetic.
"""

from typing import Union, List, Optional, Iterator
import numpy as np
from .trit import Trit


class TritArray:
    """
    Multi-trit number implementation for ternary arithmetic.
    
    A TritArray represents a number in base-3 using ternary digits (trits).
    The least significant trit is at index 0, most significant at the end.
    """
    
    def __init__(self, trits: Union[List[int], List[Trit], 'TritArray', int, str, None] = None, 
                 size: Optional[int] = None):
        """
        Initialize a TritArray.
        
        Args:
            trits: List of trit values, another TritArray, integer, string, or None
            size: Optional size for fixed-length arrays
        """
        if trits is None:
            self._trits = [0] if size is None else [0] * size
        elif isinstance(trits, TritArray):
            self._trits = trits._trits.copy()
        elif isinstance(trits, int):
            self._trits = self._from_integer(trits, size)
        elif isinstance(trits, str):
            self._trits = self._from_string(trits, size)
        elif isinstance(trits, (list, tuple)):
            self._trits = [int(t) if isinstance(t, Trit) else int(t) for t in trits]
            self._validate_trits()
        else:
            raise ValueError(f"Invalid input type: {type(trits)}")
        
        if size is not None and len(self._trits) != size:
            self._resize(size)
    
    def _validate_trits(self) -> None:
        """Validate that all trits are valid ternary values."""
        for i, trit in enumerate(self._trits):
            if trit not in [-1, 0, 1]:
                raise ValueError(f"Invalid trit at index {i}: {trit}. Must be -1, 0, or 1")
    
    def _from_integer(self, value: int, size: Optional[int] = None) -> List[int]:
        """Convert integer to ternary representation."""
        if value == 0:
            return [0] if size is None else [0] * size
        
        trits = []
        abs_value = abs(value)
        
        # Convert to base-3
        while abs_value > 0:
            trits.append(abs_value % 3)
            abs_value //= 3
        
        # Convert to balanced ternary (-1, 0, 1)
        i = 0
        while i < len(trits):
            if trits[i] == 2:
                trits[i] = -1
                if i + 1 < len(trits):
                    trits[i + 1] += 1
                else:
                    trits.append(1)
            elif trits[i] == 3:
                trits[i] = 0
                if i + 1 < len(trits):
                    trits[i + 1] += 1
                else:
                    trits.append(1)
            elif trits[i] > 3:
                remainder = trits[i] % 3
                carry = trits[i] // 3
                trits[i] = remainder
                if i + 1 < len(trits):
                    trits[i + 1] += carry
                else:
                    trits.append(carry)
            else:
                i += 1
        
        # Handle negative numbers
        if value < 0:
            trits = [-t for t in trits]
        
        # Pad to size if specified
        if size is not None:
            while len(trits) < size:
                trits.append(0)
            trits = trits[:size]
        
        return trits
    
    def _from_string(self, value: str, size: Optional[int] = None) -> List[int]:
        """Convert string representation to ternary."""
        value = value.strip()
        if not value:
            return [0] if size is None else [0] * size
        
        # Handle different string formats
        if value.startswith('0t') or value.startswith('0T'):
            # Ternary literal: 0t10-1
            trits = []
            for char in value[2:]:
                if char == '-':
                    trits.append(-1)
                elif char == '0':
                    trits.append(0)
                elif char == '+':
                    trits.append(1)
                else:
                    raise ValueError(f"Invalid ternary character: {char}")
            return trits
        else:
            # Try to parse as integer
            try:
                int_value = int(value)
                return self._from_integer(int_value, size)
            except ValueError:
                raise ValueError(f"Invalid string format: {value}")
    
    def _resize(self, new_size: int) -> None:
        """Resize the trit array."""
        if new_size < len(self._trits):
            self._trits = self._trits[:new_size]
        else:
            self._trits.extend([0] * (new_size - len(self._trits)))
    
    def __len__(self) -> int:
        """Get the number of trits."""
        return len(self._trits)
    
    def __getitem__(self, index: int) -> Trit:
        """Get a trit at the specified index."""
        return Trit(self._trits[index])
    
    def __setitem__(self, index: int, value: Union[int, Trit]) -> None:
        """Set a trit at the specified index."""
        if isinstance(value, Trit):
            self._trits[index] = value.value
        else:
            self._trits[index] = int(value)
        self._validate_trits()
    
    def __iter__(self) -> Iterator[Trit]:
        """Iterate over trits."""
        return (Trit(t) for t in self._trits)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TritArray({self._trits})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"TritArray({self._trits})"
    
    def __eq__(self, other) -> bool:
        """Equality comparison."""
        if isinstance(other, TritArray):
            return self._trits == other._trits
        return False
    
    def __ne__(self, other) -> bool:
        """Inequality comparison."""
        return not self.__eq__(other)
    
    def __lt__(self, other) -> bool:
        """Less than comparison."""
        if isinstance(other, TritArray):
            return self.to_decimal() < other.to_decimal()
        return False
    
    def __le__(self, other) -> bool:
        """Less than or equal comparison."""
        if isinstance(other, TritArray):
            return self.to_decimal() <= other.to_decimal()
        return False
    
    def __gt__(self, other) -> bool:
        """Greater than comparison."""
        if isinstance(other, TritArray):
            return self.to_decimal() > other.to_decimal()
        return False
    
    def __ge__(self, other) -> bool:
        """Greater than or equal comparison."""
        if isinstance(other, TritArray):
            return self.to_decimal() >= other.to_decimal()
        return False
    
    def __hash__(self) -> int:
        """Hash for use in sets and dictionaries."""
        return hash(tuple(self._trits))
    
    # Arithmetic operations
    def __add__(self, other) -> 'TritArray':
        """Ternary addition."""
        if isinstance(other, TritArray):
            return self._ternary_add(other)
        elif isinstance(other, (int, Trit)):
            return self._ternary_add(TritArray(other))
        else:
            return NotImplemented
    
    def __sub__(self, other) -> 'TritArray':
        """Ternary subtraction."""
        if isinstance(other, TritArray):
            return self._ternary_sub(other)
        elif isinstance(other, (int, Trit)):
            return self._ternary_sub(TritArray(other))
        else:
            return NotImplemented
    
    def __mul__(self, other) -> 'TritArray':
        """Ternary multiplication."""
        if isinstance(other, TritArray):
            return self._ternary_mul(other)
        elif isinstance(other, (int, Trit)):
            return self._ternary_mul(TritArray(other))
        else:
            return NotImplemented
    
    def __neg__(self) -> 'TritArray':
        """Unary negation."""
        return TritArray([-t for t in self._trits])
    
    def __abs__(self) -> 'TritArray':
        """Absolute value."""
        return TritArray([abs(t) for t in self._trits])
    
    def _ternary_add(self, other: 'TritArray') -> 'TritArray':
        """Ternary addition with carry."""
        max_len = max(len(self), len(other))
        result = []
        carry = 0
        
        for i in range(max_len):
            a = self._trits[i] if i < len(self) else 0
            b = other._trits[i] if i < len(other) else 0
            
            # Ternary addition with carry
            sum_val = a + b + carry
            if sum_val > 1:
                result.append(sum_val - 3)
                carry = 1
            elif sum_val < -1:
                result.append(sum_val + 3)
                carry = -1
            else:
                result.append(sum_val)
                carry = 0
        
        if carry != 0:
            result.append(carry)
        
        return TritArray(result)
    
    def _ternary_sub(self, other: 'TritArray') -> 'TritArray':
        """Ternary subtraction with borrow."""
        return self._ternary_add(-other)
    
    def _ternary_mul(self, other: 'TritArray') -> 'TritArray':
        """Ternary multiplication."""
        result = TritArray(0)
        
        for i, trit in enumerate(other._trits):
            if trit != 0:
                partial = self._shift_left(i)
                if trit == -1:
                    partial = -partial
                result = result._ternary_add(partial)
        
        return result
    
    def _shift_left(self, positions: int) -> 'TritArray':
        """Left shift by specified positions."""
        if positions == 0:
            return TritArray(self._trits.copy())
        
        result = [0] * positions + self._trits
        return TritArray(result)
    
    def _shift_right(self, positions: int) -> 'TritArray':
        """Right shift by specified positions."""
        if positions >= len(self._trits):
            return TritArray(0)
        
        result = self._trits[positions:]
        return TritArray(result)
    
    # Conversion methods
    def to_decimal(self) -> int:
        """Convert to decimal integer."""
        result = 0
        for i, trit in enumerate(self._trits):
            result += trit * (3 ** i)
        return result
    
    def to_binary(self) -> int:
        """Convert to binary representation."""
        return self.to_decimal()
    
    def to_string(self, format_type: str = "decimal") -> str:
        """Convert to string representation."""
        if format_type == "decimal":
            return str(self.to_decimal())
        elif format_type == "ternary":
            return "0t" + "".join([{1: "+", 0: "0", -1: "-"}[t.value if hasattr(t, 'value') else t] for t in reversed(self._trits)])
        elif format_type == "trits":
            return "[" + ", ".join(map(str, self._trits)) + "]"
        else:
            raise ValueError(f"Invalid format type: {format_type}")
    
    # Utility methods
    def is_zero(self) -> bool:
        """Check if the array represents zero."""
        return all(t == 0 for t in self._trits)
    
    def is_positive(self) -> bool:
        """Check if the array represents a positive number."""
        return self.to_decimal() > 0
    
    def is_negative(self) -> bool:
        """Check if the array represents a negative number."""
        return self.to_decimal() < 0
    
    def normalize(self) -> 'TritArray':
        """Remove leading zeros."""
        # Find the last non-zero trit
        last_nonzero = len(self._trits) - 1
        while last_nonzero >= 0 and self._trits[last_nonzero] == 0:
            last_nonzero -= 1
        
        if last_nonzero < 0:
            return TritArray(0)
        
        return TritArray(self._trits[:last_nonzero + 1])
    
    def pad(self, size: int, value: int = 0) -> 'TritArray':
        """Pad the array to the specified size."""
        if size <= len(self._trits):
            return TritArray(self._trits.copy())
        
        result = self._trits.copy()
        result.extend([value] * (size - len(self._trits)))
        return TritArray(result)
    
    def reverse(self) -> 'TritArray':
        """Reverse the order of trits."""
        return TritArray(list(reversed(self._trits)))
    
    # Class methods
    @classmethod
    def zero(cls, size: int = 1) -> 'TritArray':
        """Create a zero TritArray."""
        return cls([0] * size)
    
    @classmethod
    def one(cls, size: int = 1) -> 'TritArray':
        """Create a one TritArray."""
        result = [0] * size
        result[0] = 1
        return cls(result)
    
    @classmethod
    def from_decimal(cls, value: int, size: Optional[int] = None) -> 'TritArray':
        """Create TritArray from decimal value."""
        return cls(value, size)
    
    @classmethod
    def from_binary(cls, value: int, size: Optional[int] = None) -> 'TritArray':
        """Create TritArray from binary value."""
        return cls(value, size)
    
    @classmethod
    def random(cls, size: int) -> 'TritArray':
        """Create a random TritArray."""
        import random
        trits = [random.choice([-1, 0, 1]) for _ in range(size)]
        return cls(trits)
    
    # Constants
    ZERO = None
    ONE = None
    
    def __new__(cls, trits: Union[List[int], List[Trit], 'TritArray', int, str, None] = None, 
                size: Optional[int] = None):
        """Override __new__ to implement singleton pattern for common values."""
        if trits == 0 and cls.ZERO is None:
            instance = super().__new__(cls)
            instance._trits = [0]
            cls.ZERO = instance
            return instance
        elif trits == 1 and cls.ONE is None:
            instance = super().__new__(cls)
            instance._trits = [1]
            cls.ONE = instance
            return instance
        else:
            return super().__new__(cls)


# Initialize class constants
TritArray.ZERO = TritArray(0)
TritArray.ONE = TritArray(1)
