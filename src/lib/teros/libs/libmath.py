"""
libmath.so - Ternary mathematics library.

This module provides mathematical functions optimized for ternary logic.
"""

from typing import List, Union, Tuple, Optional
import math
from ..core.trit import Trit
from ..core.tritarray import TritArray
from .libternary import TernaryMath


class TernaryTrigonometry:
    """
    Ternary Trigonometry - Trigonometric functions for ternary data.
    """
    
    @staticmethod
    def ternary_sin(x: TritArray) -> TritArray:
        """
        Ternary sine function.
        
        Args:
            x: Input angle in radians
            
        Returns:
            Sine value as TritArray
        """
        return TernaryMath.ternary_sin(x)
    
    @staticmethod
    def ternary_cos(x: TritArray) -> TritArray:
        """
        Ternary cosine function.
        
        Args:
            x: Input angle in radians
            
        Returns:
            Cosine value as TritArray
        """
        return TernaryMath.ternary_cos(x)
    
    @staticmethod
    def ternary_tan(x: TritArray) -> TritArray:
        """
        Ternary tangent function.
        
        Args:
            x: Input angle in radians
            
        Returns:
            Tangent value as TritArray
        """
        return TernaryMath.ternary_tan(x)
    
    @staticmethod
    def ternary_asin(x: TritArray) -> TritArray:
        """
        Ternary arcsine function.
        
        Args:
            x: Input value
            
        Returns:
            Arcsine in radians as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal < -1 or x_decimal > 1:
            raise ValueError("Arcsine input out of range [-1, 1]")
        
        # Calculate arcsine
        result = math.asin(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_acos(x: TritArray) -> TritArray:
        """
        Ternary arccosine function.
        
        Args:
            x: Input value
            
        Returns:
            Arccosine in radians as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal < -1 or x_decimal > 1:
            raise ValueError("Arccosine input out of range [-1, 1]")
        
        # Calculate arccosine
        result = math.acos(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_atan(x: TritArray) -> TritArray:
        """
        Ternary arctangent function.
        
        Args:
            x: Input value
            
        Returns:
            Arctangent in radians as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate arctangent
        result = math.atan(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_atan2(y: TritArray, x: TritArray) -> TritArray:
        """
        Ternary arctangent2 function.
        
        Args:
            y: Y coordinate
            x: X coordinate
            
        Returns:
            Arctangent2 in radians as TritArray
        """
        # Convert to decimal
        y_decimal = y.to_decimal()
        x_decimal = x.to_decimal()
        
        # Calculate arctangent2
        result = math.atan2(y_decimal, x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))


class TernaryExponentials:
    """
    Ternary Exponentials - Exponential and logarithmic functions.
    """
    
    @staticmethod
    def ternary_exp(x: TritArray) -> TritArray:
        """
        Ternary exponential function.
        
        Args:
            x: Input value
            
        Returns:
            e^x as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate exponential
        result = math.exp(x_decimal)
        
        # Convert back to ternary
        result_int = int(result)
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_log(x: TritArray) -> TritArray:
        """
        Ternary natural logarithm.
        
        Args:
            x: Input value
            
        Returns:
            ln(x) as TritArray
        """
        return TernaryMath.ternary_log(x)
    
    @staticmethod
    def ternary_log10(x: TritArray) -> TritArray:
        """
        Ternary base-10 logarithm.
        
        Args:
            x: Input value
            
        Returns:
            log10(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal <= 0:
            raise ValueError("Logarithm of non-positive number")
        
        # Calculate base-10 logarithm
        result = math.log10(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_log2(x: TritArray) -> TritArray:
        """
        Ternary base-2 logarithm.
        
        Args:
            x: Input value
            
        Returns:
            log2(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal <= 0:
            raise ValueError("Logarithm of non-positive number")
        
        # Calculate base-2 logarithm
        result = math.log2(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_pow(base: TritArray, exponent: TritArray) -> TritArray:
        """
        Ternary power function.
        
        Args:
            base: Base value
            exponent: Exponent value
            
        Returns:
            base^exponent as TritArray
        """
        return TernaryMath.ternary_power(base, exponent)


class TernaryHyperbolics:
    """
    Ternary Hyperbolics - Hyperbolic functions.
    """
    
    @staticmethod
    def ternary_sinh(x: TritArray) -> TritArray:
        """
        Ternary hyperbolic sine.
        
        Args:
            x: Input value
            
        Returns:
            sinh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate hyperbolic sine
        result = math.sinh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_cosh(x: TritArray) -> TritArray:
        """
        Ternary hyperbolic cosine.
        
        Args:
            x: Input value
            
        Returns:
            cosh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate hyperbolic cosine
        result = math.cosh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_tanh(x: TritArray) -> TritArray:
        """
        Ternary hyperbolic tangent.
        
        Args:
            x: Input value
            
        Returns:
            tanh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate hyperbolic tangent
        result = math.tanh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_asinh(x: TritArray) -> TritArray:
        """
        Ternary inverse hyperbolic sine.
        
        Args:
            x: Input value
            
        Returns:
            asinh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        # Calculate inverse hyperbolic sine
        result = math.asinh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_acosh(x: TritArray) -> TritArray:
        """
        Ternary inverse hyperbolic cosine.
        
        Args:
            x: Input value
            
        Returns:
            acosh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal < 1:
            raise ValueError("Acosh input must be >= 1")
        
        # Calculate inverse hyperbolic cosine
        result = math.acosh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_atanh(x: TritArray) -> TritArray:
        """
        Ternary inverse hyperbolic tangent.
        
        Args:
            x: Input value
            
        Returns:
            atanh(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal <= -1 or x_decimal >= 1:
            raise ValueError("Atanh input must be in range (-1, 1)")
        
        # Calculate inverse hyperbolic tangent
        result = math.atanh(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))


class TernaryConstants:
    """
    Ternary Constants - Mathematical constants in ternary representation.
    """
    
    # Pi in ternary
    PI = TritArray.from_int(int(math.pi * 1000), 8)
    
    # Euler's number in ternary
    E = TritArray.from_int(int(math.e * 1000), 8)
    
    # Golden ratio in ternary
    PHI = TritArray.from_int(int(1.618033988749895 * 1000), 8)
    
    # Square root of 2 in ternary
    SQRT2 = TritArray.from_int(int(math.sqrt(2) * 1000), 8)
    
    # Square root of 3 in ternary
    SQRT3 = TritArray.from_int(int(math.sqrt(3) * 1000), 8)
    
    # Natural logarithm of 2 in ternary
    LN2 = TritArray.from_int(int(math.log(2) * 1000), 8)
    
    # Natural logarithm of 10 in ternary
    LN10 = TritArray.from_int(int(math.log(10) * 1000), 8)


class TernarySpecialFunctions:
    """
    Ternary Special Functions - Special mathematical functions.
    """
    
    @staticmethod
    def ternary_gamma(x: TritArray) -> TritArray:
        """
        Ternary gamma function.
        
        Args:
            x: Input value
            
        Returns:
            Gamma(x) as TritArray
        """
        # Convert to decimal
        x_decimal = x.to_decimal()
        
        if x_decimal <= 0:
            raise ValueError("Gamma function undefined for non-positive integers")
        
        # Calculate gamma function
        result = math.gamma(x_decimal)
        
        # Convert back to ternary
        result_int = int(result * 1000)  # Scale for precision
        return TritArray.from_int(result_int, len(x))
    
    @staticmethod
    def ternary_factorial(n: TritArray) -> TritArray:
        """
        Ternary factorial function.
        
        Args:
            n: Input value
            
        Returns:
            n! as TritArray
        """
        # Convert to decimal
        n_decimal = n.to_decimal()
        
        if n_decimal < 0:
            raise ValueError("Factorial undefined for negative numbers")
        
        if n_decimal != int(n_decimal):
            raise ValueError("Factorial only defined for integers")
        
        # Calculate factorial
        result = math.factorial(int(n_decimal))
        
        # Convert back to ternary
        return TritArray.from_int(result, len(n))
    
    @staticmethod
    def ternary_combinations(n: TritArray, k: TritArray) -> TritArray:
        """
        Ternary combinations function.
        
        Args:
            n: Total items
            k: Items to choose
            
        Returns:
            C(n,k) as TritArray
        """
        # Convert to decimal
        n_decimal = n.to_decimal()
        k_decimal = k.to_decimal()
        
        if n_decimal < 0 or k_decimal < 0:
            raise ValueError("Combinations undefined for negative values")
        
        if k_decimal > n_decimal:
            raise ValueError("Cannot choose more items than available")
        
        # Calculate combinations
        result = math.comb(int(n_decimal), int(k_decimal))
        
        # Convert back to ternary
        return TritArray.from_int(result, len(n))
    
    @staticmethod
    def ternary_permutations(n: TritArray, k: TritArray) -> TritArray:
        """
        Ternary permutations function.
        
        Args:
            n: Total items
            k: Items to arrange
            
        Returns:
            P(n,k) as TritArray
        """
        # Convert to decimal
        n_decimal = n.to_decimal()
        k_decimal = k.to_decimal()
        
        if n_decimal < 0 or k_decimal < 0:
            raise ValueError("Permutations undefined for negative values")
        
        if k_decimal > n_decimal:
            raise ValueError("Cannot arrange more items than available")
        
        # Calculate permutations
        result = math.perm(int(n_decimal), int(k_decimal))
        
        # Convert back to ternary
        return TritArray.from_int(result, len(n))
