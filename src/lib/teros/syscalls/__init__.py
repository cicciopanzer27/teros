"""
Ternary System Calls (T-SYSCALLs) implementation.

This module provides system call interface for TEROS,
including user-kernel space communication and system services.
"""

from .syscall_manager import TernarySyscallManager
from .syscall_interface import TernarySyscallInterface
from .syscall_handlers import TernarySyscallHandlers

__all__ = [
    "TernarySyscallManager",
    "TernarySyscallInterface",
    "TernarySyscallHandlers",
]
