"""
Integration and Testing for TEROS.

This module provides hardware integration and system testing capabilities.
"""

from .hardware_integration import HardwareIntegration, HardwareTest, HardwareTestType
from .system_testing import SystemTesting, SystemTest, TestCategory

__all__ = [
    # Hardware Integration
    "HardwareIntegration",
    "HardwareTest",
    "HardwareTestType",
    
    # System Testing
    "SystemTesting",
    "SystemTest",
    "TestCategory"
]
