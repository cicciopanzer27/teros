"""
Ternary Process Management implementation.

This module provides process management for the TEROS system,
including scheduling, context switching, and inter-process communication.
"""

from .scheduler import TernaryScheduler
from .context_switch import ContextSwitchManager
from .ipc import TernaryIPCManager, TernaryIPCQueue, TernaryIPCMessage

__all__ = [
    "TernaryScheduler",
    "ContextSwitchManager", 
    "TernaryIPCManager",
    "TernaryIPCQueue",
    "TernaryIPCMessage",
]
