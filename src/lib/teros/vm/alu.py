"""
TernaryALU - Arithmetic Logic Unit for ternary operations.

This module implements the Arithmetic Logic Unit (ALU) for the TVM,
providing optimized ternary arithmetic and logic operations.
"""

from typing import Union, Optional
import numpy as np
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryALU:
    """
    Ternary Arithmetic Logic Unit implementation.
    
    The ALU provides optimized ternary arithmetic and logic operations
    using lookup tables and efficient algorithms.
    """
    
    def __init__(self):
        """Initialize the Ternary ALU."""
        # Initialize lookup tables for performance
        self._init_lookup_tables()
    
    def _init_lookup_tables(self) -> None:
        """Initialize lookup tables for fast operations."""
        # Ternary addition lookup table
        self._add_table = np.array([
            [[-1, -1, 0], [-1, 0, -1], [-1, 1, 0]],
            [[0, -1, -1], [0, 0, 0], [0, 1, 1]],
            [[1, -1, 0], [1, 0, 1], [1, 1, 1]]
        ], dtype=np.int8)
        
        # Ternary multiplication lookup table
        self._mul_table = np.array([
            [[1, 0, -1], [0, 0, 0], [-1, 0, 1]],
            [[0, 0, 0], [0, 0, 0], [0, 0, 0]],
            [[-1, 0, 1], [0, 0, 0], [1, 0, -1]]
        ], dtype=np.int8)
        
        # Ternary AND lookup table
        self._and_table = np.array([
            [[-1, -1, -1], [-1, 0, 0], [-1, 1, -1]],
            [[0, -1, -1], [0, 0, 0], [0, 1, 0]],
            [[-1, -1, -1], [0, 0, 0], [1, 1, 1]]
        ], dtype=np.int8)
        
        # Ternary OR lookup table
        self._or_table = np.array([
            [[-1, 0, 1], [0, 0, 1], [1, 1, 1]],
            [[0, 0, 1], [0, 0, 1], [1, 1, 1]],
            [[1, 1, 1], [1, 1, 1], [1, 1, 1]]
        ], dtype=np.int8)
        
        # Ternary XOR lookup table
        self._xor_table = np.array([
            [[0, -1, 1], [-1, 0, 1], [1, 1, 0]],
            [[-1, 0, 1], [0, 0, 0], [1, 0, -1]],
            [[1, 1, 0], [1, 0, -1], [0, -1, 1]]
        ], dtype=np.int8)
    
    def add(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary addition with carry.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Sum of a and b
        """
        return self._ternary_add(a, b)
    
    def sub(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary subtraction with borrow.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Difference of a and b
        """
        return self._ternary_add(a, self.neg(b))
    
    def mul(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary multiplication.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Product of a and b
        """
        return self._ternary_mul(a, b)
    
    def div(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary division.
        
        Args:
            a: Dividend
            b: Divisor
            
        Returns:
            Quotient of a and b
        """
        if b.is_zero():
            raise ZeroDivisionError("Division by zero")
        
        # Simple division algorithm
        result = TritArray(0)
        remainder = a
        
        while not remainder.is_zero() and remainder.to_decimal() >= b.to_decimal():
            remainder = self._ternary_sub(remainder, b)
            result = self._ternary_add(result, TritArray(1))
        
        return result
    
    def neg(self, a: TritArray) -> TritArray:
        """
        Ternary negation.
        
        Args:
            a: Operand
            
        Returns:
            Negation of a
        """
        return -a
    
    def abs(self, a: TritArray) -> TritArray:
        """
        Ternary absolute value.
        
        Args:
            a: Operand
            
        Returns:
            Absolute value of a
        """
        return abs(a)
    
    def nand(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary NAND operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            NAND of a and b
        """
        return self.not_op(self.and_op(a, b))
    
    def cons(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary CONS (consequence) operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            CONS of a and b
        """
        # CONS(a, b) = a -> b (implication)
        return self.or_op(self.not_op(a), b)
    
    def any(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary ANY operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            ANY of a and b
        """
        # ANY(a, b) = a | b (disjunction)
        return self.or_op(a, b)
    
    def not_op(self, a: TritArray) -> TritArray:
        """
        Ternary NOT operation.
        
        Args:
            a: Operand
            
        Returns:
            NOT of a
        """
        return -a
    
    def and_op(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary AND operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            AND of a and b
        """
        return self._ternary_and(a, b)
    
    def or_op(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary OR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            OR of a and b
        """
        return self._ternary_or(a, b)
    
    def xor_op(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary XOR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            XOR of a and b
        """
        return self._ternary_xor(a, b)
    
    def cmp(self, a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary comparison.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Comparison result (-1 if a < b, 0 if a == b, 1 if a > b)
        """
        a_val = a.to_decimal()
        b_val = b.to_decimal()
        
        if a_val < b_val:
            return TritArray(-1)
        elif a_val > b_val:
            return TritArray(1)
        else:
            return TritArray(0)
    
    def test(self, a: TritArray) -> TritArray:
        """
        Ternary test operation.
        
        Args:
            a: Operand
            
        Returns:
            Test result (-1 if negative, 0 if zero, 1 if positive)
        """
        if a.is_negative():
            return TritArray(-1)
        elif a.is_zero():
            return TritArray(0)
        else:
            return TritArray(1)
    
    def tshl(self, a: TritArray, shift: int) -> TritArray:
        """
        Ternary left shift.
        
        Args:
            a: Operand
            shift: Number of positions to shift
            
        Returns:
            Shifted value
        """
        if shift <= 0:
            return a
        
        # Left shift by multiplying by 3^shift
        multiplier = 3 ** shift
        return a * TritArray(multiplier)
    
    def tshr(self, a: TritArray, shift: int) -> TritArray:
        """
        Ternary right shift.
        
        Args:
            a: Operand
            shift: Number of positions to shift
            
        Returns:
            Shifted value
        """
        if shift <= 0:
            return a
        
        # Right shift by dividing by 3^shift
        divisor = 3 ** shift
        return a // TritArray(divisor)
    
    def rotl(self, a: TritArray, shift: int) -> TritArray:
        """
        Ternary left rotation.
        
        Args:
            a: Operand
            shift: Number of positions to rotate
            
        Returns:
            Rotated value
        """
        if shift <= 0:
            return a
        
        # Rotate left by shifting and wrapping
        size = len(a)
        shift = shift % size
        
        if shift == 0:
            return a
        
        # Create rotated array
        rotated = [0] * size
        for i in range(size):
            rotated[i] = a[(i + shift) % size].value
        
        return TritArray(rotated)
    
    def rotr(self, a: TritArray, shift: int) -> TritArray:
        """
        Ternary right rotation.
        
        Args:
            a: Operand
            shift: Number of positions to rotate
            
        Returns:
            Rotated value
        """
        if shift <= 0:
            return a
        
        # Rotate right by shifting and wrapping
        size = len(a)
        shift = shift % size
        
        if shift == 0:
            return a
        
        # Create rotated array
        rotated = [0] * size
        for i in range(size):
            rotated[i] = a[(i - shift) % size].value
        
        return TritArray(rotated)
    
    # Internal helper methods
    def _ternary_add(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary addition with carry."""
        max_len = max(len(a), len(b))
        result = []
        carry = 0
        
        for i in range(max_len):
            a_val = a._trits[i] if i < len(a) else 0
            b_val = b._trits[i] if i < len(b) else 0
            
            # Use lookup table for addition
            sum_val = self._add_table[a_val + 1, b_val + 1, 0] + carry
            
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
    
    def _ternary_sub(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary subtraction with borrow."""
        return self._ternary_add(a, self.neg(b))
    
    def _ternary_mul(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary multiplication."""
        result = TritArray(0)
        
        for i, trit in enumerate(b._trits):
            if trit != 0:
                partial = self._shift_left(a, i)
                if trit == -1:
                    partial = self.neg(partial)
                result = self._ternary_add(result, partial)
        
        return result
    
    def _ternary_and(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary AND operation."""
        max_len = max(len(a), len(b))
        result = []
        
        for i in range(max_len):
            a_val = a._trits[i] if i < len(a) else 0
            b_val = b._trits[i] if i < len(b) else 0
            
            # Use lookup table for AND
            and_val = self._and_table[a_val + 1, b_val + 1, 0]
            result.append(and_val)
        
        return TritArray(result)
    
    def _ternary_or(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary OR operation."""
        max_len = max(len(a), len(b))
        result = []
        
        for i in range(max_len):
            a_val = a._trits[i] if i < len(a) else 0
            b_val = b._trits[i] if i < len(b) else 0
            
            # Use lookup table for OR
            or_val = self._or_table[a_val + 1, b_val + 1, 0]
            result.append(or_val)
        
        return TritArray(result)
    
    def _ternary_xor(self, a: TritArray, b: TritArray) -> TritArray:
        """Internal ternary XOR operation."""
        max_len = max(len(a), len(b))
        result = []
        
        for i in range(max_len):
            a_val = a._trits[i] if i < len(a) else 0
            b_val = b._trits[i] if i < len(b) else 0
            
            # Use lookup table for XOR
            xor_val = self._xor_table[a_val + 1, b_val + 1, 0]
            result.append(xor_val)
        
        return TritArray(result)
    
    def _shift_left(self, a: TritArray, positions: int) -> TritArray:
        """Internal left shift operation."""
        if positions == 0:
            return a
        
        result = [0] * positions + a._trits
        return TritArray(result)
    
    def _shift_right(self, a: TritArray, positions: int) -> TritArray:
        """Internal right shift operation."""
        if positions >= len(a._trits):
            return TritArray(0)
        
        result = a._trits[positions:]
        return TritArray(result)
    
    def __str__(self) -> str:
        """String representation."""
        return "TernaryALU()"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return "TernaryALU()"
