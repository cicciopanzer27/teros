"""
Trit Encoder/Decoder for Hardware Abstraction Layer.

This module provides efficient conversion between ternary trits and binary
representation for hardware integration.
"""

from typing import List, Union, Tuple
import struct
import sys
from enum import Enum
from ..core.trit import Trit


class Endianness(Enum):
    """Byte order for trit encoding."""
    LITTLE_ENDIAN = "little"
    BIG_ENDIAN = "big"


class TritEncoder:
    """
    Trit Encoder - Converts trits to binary representation.
    
    Uses 2-bit encoding: -1→00, 0→01, 1→10
    Optimized with lookup tables for performance.
    """
    
    # Lookup tables for encoding
    TRIT_TO_BINARY = {
        -1: 0b00,  # NEGATIVE
        0:  0b01,  # ZERO  
        1:  0b10   # POSITIVE
    }
    
    BINARY_TO_TRIT = {v: k for k, v in TRIT_TO_BINARY.items()}
    
    def __init__(self, endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize trit encoder.
        
        Args:
            endianness: Byte order for encoding
        """
        self.endianness = endianness
        self._validate_system_endianness()
    
    def _validate_system_endianness(self) -> None:
        """Validate system endianness compatibility."""
        system_endianness = sys.byteorder
        if self.endianness.value != system_endianness:
            print(f"Warning: Encoder endianness ({self.endianness.value}) "
                  f"differs from system ({system_endianness})")
    
    def encode_trit(self, trit: Union[Trit, int]) -> int:
        """
        Encode single trit to binary.
        
        Args:
            trit: Trit value (-1, 0, 1)
            
        Returns:
            2-bit binary representation
            
        Raises:
            ValueError: If trit value is invalid
        """
        if isinstance(trit, Trit):
            value = trit.value
        else:
            value = trit
            
        if value not in self.TRIT_TO_BINARY:
            raise ValueError(f"Invalid trit value: {value}")
            
        return self.TRIT_TO_BINARY[value]
    
    def encode_tritarray(self, trits: List[Union[Trit, int]], 
                        pad_to_bytes: bool = True) -> bytes:
        """
        Encode trit array to binary bytes.
        
        Args:
            trits: List of trit values
            pad_to_bytes: Whether to pad to byte boundary
            
        Returns:
            Binary representation as bytes
        """
        if not trits:
            return b''
        
        # Encode each trit to 2 bits
        binary_bits = []
        for trit in trits:
            binary_bits.append(self.encode_trit(trit))
        
        # Convert to bytes
        return self._bits_to_bytes(binary_bits, pad_to_bytes)
    
    def _bits_to_bytes(self, bits: List[int], pad_to_bytes: bool = True) -> bytes:
        """Convert list of 2-bit values to bytes."""
        if not bits:
            return b''
        
        # Pack bits into bytes (4 trits per byte)
        byte_data = []
        for i in range(0, len(bits), 4):
            byte_bits = bits[i:i+4]
            
            # Pad if necessary
            if len(byte_bits) < 4 and pad_to_bytes:
                byte_bits.extend([0] * (4 - len(byte_bits)))
            
            # Pack into single byte
            byte_value = 0
            for j, bit_pair in enumerate(byte_bits):
                byte_value |= (bit_pair << (j * 2))
            
            byte_data.append(byte_value)
        
        return bytes(byte_data)
    
    def encode_with_metadata(self, trits: List[Union[Trit, int]], 
                           metadata: dict = None) -> bytes:
        """
        Encode trits with metadata header.
        
        Args:
            trits: List of trit values
            metadata: Optional metadata dictionary
            
        Returns:
            Binary data with metadata header
        """
        if metadata is None:
            metadata = {}
        
        # Create header
        header = {
            'trit_count': len(trits),
            'endianness': self.endianness.value,
            'version': 1,
            **metadata
        }
        
        # Encode header
        header_data = self._encode_header(header)
        
        # Encode trits
        trit_data = self.encode_tritarray(trits)
        
        # Combine header and data
        return header_data + trit_data
    
    def _encode_header(self, header: dict) -> bytes:
        """Encode metadata header."""
        # Simple header format: [count][endianness][version][data_length]
        count = header.get('trit_count', 0)
        endianness = header.get('endianness', 'little')
        version = header.get('version', 1)
        
        # Pack header (4 bytes: count, endianness, version, reserved)
        header_bytes = struct.pack('<I', count)  # trit count
        header_bytes += endianness.encode('ascii')[:1]  # endianness
        header_bytes += struct.pack('<B', version)  # version
        header_bytes += b'\x00'  # reserved
        
        return header_bytes


class TritDecoder:
    """
    Trit Decoder - Converts binary to trits.
    
    Handles 2-bit decoding with endianness support.
    """
    
    def __init__(self, endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize trit decoder.
        
        Args:
            endianness: Byte order for decoding
        """
        self.endianness = endianness
        self.encoder = TritEncoder(endianness)
    
    def decode_trit(self, binary_value: int) -> Trit:
        """
        Decode single binary value to trit.
        
        Args:
            binary_value: 2-bit binary value (0-3)
            
        Returns:
            Trit object
            
        Raises:
            ValueError: If binary value is invalid
        """
        if binary_value not in self.encoder.BINARY_TO_TRIT:
            raise ValueError(f"Invalid binary value: {binary_value}")
        
        trit_value = self.encoder.BINARY_TO_TRIT[binary_value]
        return Trit(trit_value)
    
    def decode_bytes(self, data: bytes, trit_count: int = None) -> List[Trit]:
        """
        Decode binary bytes to trit array.
        
        Args:
            data: Binary data to decode
            trit_count: Expected number of trits (if None, decode all)
            
        Returns:
            List of Trit objects
        """
        if not data:
            return []
        
        trits = []
        for byte in data:
            # Extract 4 trits from each byte
            for i in range(4):
                bit_pair = (byte >> (i * 2)) & 0b11
                if bit_pair in self.encoder.BINARY_TO_TRIT:
                    trit_value = self.encoder.BINARY_TO_TRIT[bit_pair]
                    trits.append(Trit(trit_value))
        
        # Trim to requested count if specified
        if trit_count is not None:
            trits = trits[:trit_count]
        
        return trits
    
    def decode_with_metadata(self, data: bytes) -> Tuple[List[Trit], dict]:
        """
        Decode binary data with metadata header.
        
        Args:
            data: Binary data with header
            
        Returns:
            Tuple of (trits, metadata)
        """
        if len(data) < 8:  # Minimum header size
            raise ValueError("Data too short for metadata header")
        
        # Decode header
        header = self._decode_header(data[:8])
        trit_data = data[8:]
        
        # Decode trits
        trits = self.decode_bytes(trit_data, header.get('trit_count'))
        
        return trits, header
    
    def _decode_header(self, header_data: bytes) -> dict:
        """Decode metadata header."""
        if len(header_data) < 8:
            raise ValueError("Header data too short")
        
        # Unpack header
        count = struct.unpack('<I', header_data[:4])[0]
        endianness = header_data[4:5].decode('ascii')
        version = struct.unpack('<B', header_data[5:6])[0]
        
        return {
            'trit_count': count,
            'endianness': endianness,
            'version': version
        }


class TritCodec:
    """
    Combined Trit Encoder/Decoder for convenience.
    
    Provides unified interface for trit encoding/decoding operations.
    """
    
    def __init__(self, endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize trit codec.
        
        Args:
            endianness: Byte order for encoding/decoding
        """
        self.encoder = TritEncoder(endianness)
        self.decoder = TritDecoder(endianness)
        self.endianness = endianness
    
    def encode(self, trits: List[Union[Trit, int]], 
               with_metadata: bool = False,
               metadata: dict = None) -> bytes:
        """
        Encode trits to binary.
        
        Args:
            trits: List of trit values
            with_metadata: Whether to include metadata header
            metadata: Optional metadata dictionary
            
        Returns:
            Binary representation
        """
        if with_metadata:
            return self.encoder.encode_with_metadata(trits, metadata)
        else:
            return self.encoder.encode_tritarray(trits)
    
    def decode(self, data: bytes, 
               with_metadata: bool = False) -> Union[List[Trit], Tuple[List[Trit], dict]]:
        """
        Decode binary to trits.
        
        Args:
            data: Binary data to decode
            with_metadata: Whether data includes metadata header
            
        Returns:
            List of trits or (trits, metadata) tuple
        """
        if with_metadata:
            return self.decoder.decode_with_metadata(data)
        else:
            return self.decoder.decode_bytes(data)
    
    def get_encoding_info(self) -> dict:
        """Get encoding information."""
        return {
            'endianness': self.endianness.value,
            'trit_encoding': '2-bit',
            'trits_per_byte': 4,
            'version': 1
        }
