"""
Hardware Abstraction Layer (HAL) for TEROS.

This module provides hardware abstraction for ternary computing on binary hardware.
"""

from .trit_encoder import TritEncoder, TritDecoder, TritCodec, Endianness
from .memory_mapping import TernaryMemoryMapper
from .cpu_emulator import TernaryCPUEmulator
from .device_manager import HALDeviceManager

__all__ = [
    "TritEncoder",
    "TritDecoder", 
    "TritCodec",
    "Endianness",
    "TernaryMemoryMapper",
    "TernaryCPUEmulator",
    "HALDeviceManager"
]
