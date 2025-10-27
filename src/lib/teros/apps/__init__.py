"""
User Applications for TEROS.

This module provides user applications for the ternary operating system.
"""

from .ternary_calculator import TernaryCalculator, TernaryCalculatorApp
from .ternary_editor import TernaryEditor
from .ternary_file_manager import TernaryFileManager, FileInfo, FileType
from .ternary_system_monitor import TernarySystemMonitor

__all__ = [
    # Calculator
    "TernaryCalculator",
    "TernaryCalculatorApp",
    
    # Editor
    "TernaryEditor",
    
    # File Manager
    "TernaryFileManager",
    "FileInfo",
    "FileType",
    
    # System Monitor
    "TernarySystemMonitor"
]
