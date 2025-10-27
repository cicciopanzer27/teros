"""
libternary.so - Advanced ternary arithmetic library.

This module provides advanced ternary arithmetic operations and functions.
"""

from typing import List, Union, Tuple, Optional
import math
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryMath:
    """
    Ternary Mathematics - Advanced ternary arithmetic operations.
    
    Provides mathematical functions optimized for ternary logic.
    """
    
    @staticmethod
    def ternary_add(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary addition with carry propagation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Sum as TritArray
        """
        # Ensure same length
        max_len = max(len(a), len(b))
        a_padded = a.pad(max_len)
        b_padded = b.pad(max_len)
        
        result = []
        carry = 0
        
        for i in range(max_len - 1, -1, -1):
            a_val = a_padded[i].value
            b_val = b_padded[i].value
            
            # Ternary addition with carry
            sum_val = a_val + b_val + carry
            
            # Handle carry
            if sum_val > 1:
                carry = 1
                sum_val -= 3
            elif sum_val < -1:
                carry = -1
                sum_val += 3
            else:
                carry = 0
            
            result.insert(0, Trit(sum_val))
        
        # Add final carry if needed
        if carry != 0:
            result.insert(0, Trit(carry))
        
        return TritArray(result)
    
    @staticmethod
    def ternary_subtract(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary subtraction.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Difference as TritArray
        """
        # Negate b and add
        b_neg = TernaryMath.ternary_negate(b)
        return TernaryMath.ternary_add(a, b_neg)
    
    @staticmethod
    def ternary_multiply(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary multiplication.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            Product as TritArray
        """
        # Convert to decimal for multiplication
        a_decimal = a.to_decimal()
        b_decimal = b.to_decimal()
        
        # Multiply
        product = a_decimal * b_decimal
        
        # Convert back to ternary
        result_size = len(a) + len(b)
        return TritArray.from_int(product, result_size)
    
    @staticmethod
    def ternary_divide(a: TritArray, b: TritArray) -> Tuple[TritArray, TritArray]:
        """
        Ternary division.
        
        Args:
            a: Dividend
            b: Divisor
            
        Returns:
            Tuple of (quotient, remainder)
        """
        # Convert to decimal for division
        a_decimal = a.to_decimal()
        b_decimal = b.to_decimal()
        
        if b_decimal == 0:
            raise ValueError("Division by zero")
        
        # Divide
        quotient = a_decimal // b_decimal
        remainder = a_decimal % b_decimal
        
        # Convert back to ternary
        quotient_size = max(len(a), len(b))
        remainder_size = len(b)
        
        return (TritArray.from_int(quotient, quotient_size),
                TritArray.from_int(remainder, remainder_size))
    
    @staticmethod
    def ternary_negate(a: TritArray) -> TritArray:
        """
        Ternary negation.
        
        Args:
            a: Operand
            
        Returns:
            Negated value as TritArray
        """
        result = []
        for trit in a:
            result.append(Trit(-trit.value))
        return TritArray(result)
    
    @staticmethod
    def ternary_abs(a: TritArray) -> TritArray:
        """
        Ternary absolute value.
        
        Args:
            a: Operand
            
        Returns:
            Absolute value as TritArray
        """
        if a.is_negative():
            return TernaryMath.ternary_negate(a)
        else:
            return a.copy()
    
    @staticmethod
    def ternary_power(base: TritArray, exponent: TritArray) -> TritArray:
        """
        Ternary exponentiation.
        
        Args:
            base: Base
            exponent: Exponent
            
        Returns:
            Result as TritArray
        """
        # Convert to decimal
        base_decimal = base.to_decimal()
        exp_decimal = exponent.to_decimal()
        
        # Calculate power
        result = base_decimal ** exp_decimal
        
        # Convert back to ternary
        result_size = len(base) * (exp_decimal + 1)
        return TritArray.from_int(result, result_size)
    
    @staticmethod
    def ternary_sqrt(a: TritArray) -> TritArray:
        """
        Ternary square root.
        
        Args:
            a: Operand
            
        Returns:
            Square root as TritArray
        """
        # Convert to decimal
        a_decimal = a.to_decimal()
        
        if a_decimal < 0:
            raise ValueError("Square root of negative number")
        
        # Calculate square root
        result = int(math.sqrt(abs(a_decimal)))
        
        # Convert back to ternary
        return TritArray.from_int(result, len(a))
    
    @staticmethod
    def ternary_log(a: TritArray, base: TritArray = None) -> TritArray:
        """
        Ternary logarithm.
        
        Args:
            a: Operand
            base: Logarithm base (default: natural log)
            
        Returns:
            Logarithm as TritArray
        """
        # Convert to decimal
        a_decimal = a.to_decimal()
        
        if a_decimal <= 0:
            raise ValueError("Logarithm of non-positive number")
        
        # Calculate logarithm
        if base is None:
            result = math.log(a_decimal)
        else:
            base_decimal = base.to_decimal()
            if base_decimal <= 0 or base_decimal == 1:
                raise ValueError("Invalid logarithm base")
            result = math.log(a_decimal) / math.log(base_decimal)
        
        # Convert back to ternary
        result_int = int(result)
        return TritArray.from_int(result_int, len(a))
    
    @staticmethod
    def ternary_sin(a: TritArray) -> TritArray:
        """
        Ternary sine function.
        
        Args:
            a: Operand in radians
            
        Returns:
            Sine as TritArray
        """
        # Convert to decimal
        a_decimal = a.to_decimal()
        
        # Calculate sine
        result = math.sin(a_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(a))
    
    @staticmethod
    def ternary_cos(a: TritArray) -> TritArray:
        """
        Ternary cosine function.
        
        Args:
            a: Operand in radians
            
        Returns:
            Cosine as TritArray
        """
        # Convert to decimal
        a_decimal = a.to_decimal()
        
        # Calculate cosine
        result = math.cos(a_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(a))
    
    @staticmethod
    def ternary_tan(a: TritArray) -> TritArray:
        """
        Ternary tangent function.
        
        Args:
            a: Operand in radians
            
        Returns:
            Tangent as TritArray
        """
        # Convert to decimal
        a_decimal = a.to_decimal()
        
        # Calculate tangent
        result = math.tan(a_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(a))


class TernaryStatistics:
    """
    Ternary Statistics - Statistical functions for ternary data.
    """
    
    @staticmethod
    def ternary_mean(data: List[TritArray]) -> TritArray:
        """
        Calculate ternary mean.
        
        Args:
            data: List of TritArray values
            
        Returns:
            Mean as TritArray
        """
        if not data:
            raise ValueError("Empty data set")
        
        # Sum all values
        total = data[0].copy()
        for value in data[1:]:
            total = TernaryMath.ternary_add(total, value)
        
        # Divide by count
        count = TritArray.from_int(len(data), len(total))
        mean, _ = TernaryMath.ternary_divide(total, count)
        
        return mean
    
    @staticmethod
    def ternary_median(data: List[TritArray]) -> TritArray:
        """
        Calculate ternary median.
        
        Args:
            data: List of TritArray values
            
        Returns:
            Median as TritArray
        """
        if not data:
            raise ValueError("Empty data set")
        
        # Sort data
        sorted_data = sorted(data, key=lambda x: x.to_decimal())
        
        # Find median
        n = len(sorted_data)
        if n % 2 == 1:
            return sorted_data[n // 2]
        else:
            mid1 = sorted_data[n // 2 - 1]
            mid2 = sorted_data[n // 2]
            return TernaryMath.ternary_add(mid1, mid2)
    
    @staticmethod
    def ternary_variance(data: List[TritArray]) -> TritArray:
        """
        Calculate ternary variance.
        
        Args:
            data: List of TritArray values
            
        Returns:
            Variance as TritArray
        """
        if not data:
            raise ValueError("Empty data set")
        
        # Calculate mean
        mean = TernaryStatistics.ternary_mean(data)
        
        # Calculate squared differences
        squared_diffs = []
        for value in data:
            diff = TernaryMath.ternary_subtract(value, mean)
            squared_diff = TernaryMath.ternary_multiply(diff, diff)
            squared_diffs.append(squared_diff)
        
        # Calculate variance
        variance = TernaryStatistics.ternary_mean(squared_diffs)
        
        return variance
    
    @staticmethod
    def ternary_std_dev(data: List[TritArray]) -> TritArray:
        """
        Calculate ternary standard deviation.
        
        Args:
            data: List of TritArray values
            
        Returns:
            Standard deviation as TritArray
        """
        variance = TernaryStatistics.ternary_variance(data)
        return TernaryMath.ternary_sqrt(variance)


class TernaryLogic:
    """
    Ternary Logic - Advanced logical operations.
    """
    
    @staticmethod
    def ternary_and(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary AND operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            AND result as TritArray
        """
        result = []
        for i in range(min(len(a), len(b))):
            a_trit = a[i]
            b_trit = b[i]
            result.append(a_trit & b_trit)
        return TritArray(result)
    
    @staticmethod
    def ternary_or(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary OR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            OR result as TritArray
        """
        result = []
        for i in range(min(len(a), len(b))):
            a_trit = a[i]
            b_trit = b[i]
            result.append(a_trit | b_trit)
        return TritArray(result)
    
    @staticmethod
    def ternary_xor(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary XOR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            XOR result as TritArray
        """
        result = []
        for i in range(min(len(a), len(b))):
            a_trit = a[i]
            b_trit = b[i]
            result.append(a_trit ^ b_trit)
        return TritArray(result)
    
    @staticmethod
    def ternary_not(a: TritArray) -> TritArray:
        """
        Ternary NOT operation.
        
        Args:
            a: Operand
            
        Returns:
            NOT result as TritArray
        """
        result = []
        for trit in a:
            result.append(~trit)
        return TritArray(result)
    
    @staticmethod
    def ternary_nand(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary NAND operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            NAND result as TritArray
        """
        and_result = TernaryLogic.ternary_and(a, b)
        return TernaryLogic.ternary_not(and_result)
    
    @staticmethod
    def ternary_nor(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary NOR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            NOR result as TritArray
        """
        or_result = TernaryLogic.ternary_or(a, b)
        return TernaryLogic.ternary_not(or_result)
    
    @staticmethod
    def ternary_xnor(a: TritArray, b: TritArray) -> TritArray:
        """
        Ternary XNOR operation.
        
        Args:
            a: First operand
            b: Second operand
            
        Returns:
            XNOR result as TritArray
        """
        xor_result = TernaryLogic.ternary_xor(a, b)
        return TernaryLogic.ternary_not(xor_result)
