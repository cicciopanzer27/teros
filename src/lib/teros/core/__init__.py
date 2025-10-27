"""
Core ternary data structures and utilities.

This module provides the fundamental building blocks for ternary computing:
- Trit: Basic ternary value (-1, 0, 1)
- TritArray: Multi-trit numbers and arrays
- TernaryMemory: Memory layout and management
- T3_PCB: Process Control Block
- T3_Instruction: T3-ISA instruction format
"""

from .trit import Trit
from .tritarray import TritArray
from .ternary_memory import TernaryMemory
from .t3_pcb import T3_PCB
from .t3_instruction import T3_Instruction

__all__ = [
    "Trit",
    "TritArray", 
    "TernaryMemory",
    "T3_PCB",
    "T3_Instruction",
]
