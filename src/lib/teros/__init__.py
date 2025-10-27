"""
TEROS - Ternary Operating System
===============================

Sistema operativo ternario completo con supporto per:
- Core: Trit, TritArray, T3-ISA, TVM
- Compiler: Lambda³ compiler
- Memory: Memory management, paging, garbage collection
- Process: Process management, scheduling, IPC
- Security: Access control, capabilities, audit logging
- Apps: Calculator, editor, file manager, system monitor
- Libs: Graphics, I/O, math, string libraries

Usage:
    from teros.core.trit import Trit
    from teros.apps.ternary_calculator import TernaryCalculator
    from teros.compiler.lambda3_compiler import Lambda3Compiler
"""

__version__ = "1.0.0"
__author__ = "TEROS Development Team"
__email__ = "teros@example.com"

# Simple imports - only import what works
try:
    from .core.trit import Trit
    __all__ = ['Trit']
except ImportError as e:
    print(f"Warning: Could not import Trit: {e}")
    __all__ = []
