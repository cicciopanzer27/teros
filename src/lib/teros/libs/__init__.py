"""
System Libraries for TEROS.

This module provides system libraries for ternary computing.
"""

from .libternary import TernaryMath, TernaryStatistics, TernaryLogic
from .libio import TernaryInputStream, TernaryOutputStream, TernaryFileIO, TernaryConsoleIO
from .libmath import TernaryTrigonometry, TernaryExponentials, TernaryHyperbolics, TernaryConstants, TernarySpecialFunctions
from .libstring import TernaryString, TernaryStringBuilder, TernaryStringUtils
from .libgraphics import TernaryGraphics, TernaryCanvas, TernaryColor, TernaryShape

__all__ = [
    # Ternary arithmetic
    "TernaryMath",
    "TernaryStatistics", 
    "TernaryLogic",
    
    # I/O operations
    "TernaryInputStream",
    "TernaryOutputStream",
    "TernaryFileIO",
    "TernaryConsoleIO",
    
    # Mathematical functions
    "TernaryTrigonometry",
    "TernaryExponentials",
    "TernaryHyperbolics",
    "TernaryConstants",
    "TernarySpecialFunctions",
    
    # String operations
    "TernaryString",
    "TernaryStringBuilder",
    "TernaryStringUtils",
    
    # Graphics operations
    "TernaryGraphics",
    "TernaryCanvas",
    "TernaryColor",
    "TernaryShape"
]
