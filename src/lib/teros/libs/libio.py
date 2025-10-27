"""
libio.so - Ternary I/O library.

This module provides I/O operations for ternary data streams.
"""

from typing import List, Union, Optional, BinaryIO, TextIO
import os
import sys
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..hal.trit_encoder import TritCodec, Endianness


class TernaryInputStream:
    """
    Ternary Input Stream - Reads ternary data from various sources.
    
    Provides streaming input for ternary data with buffering and encoding.
    """
    
    def __init__(self, source: Union[str, BinaryIO, TextIO], 
                 encoding: str = 'utf-8', endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize ternary input stream.
        
        Args:
            source: Input source (file path, file object, or stdin)
            encoding: Text encoding for text sources
            endianness: Byte order for binary sources
        """
        self.source = source
        self.encoding = encoding
        self.endianness = endianness
        self.codec = TritCodec(endianness)
        
        # Stream state
        self.is_open = False
        self.position = 0
        self.buffer = []
        self.buffer_size = 4096
        
        # Statistics
        self.stats = {
            'bytes_read': 0,
            'trits_read': 0,
            'read_operations': 0
        }
        
        # Open stream
        self._open_stream()
    
    def _open_stream(self) -> None:
        """Open the input stream."""
        try:
            if isinstance(self.source, str):
                # File path
                self.file_handle = open(self.source, 'rb')
                self.is_open = True
            elif hasattr(self.source, 'read'):
                # File object
                self.file_handle = self.source
                self.is_open = True
            else:
                # Default to stdin
                self.file_handle = sys.stdin.buffer if hasattr(sys.stdin, 'buffer') else sys.stdin
                self.is_open = True
                
        except Exception as e:
            raise RuntimeError(f"Failed to open input stream: {e}")
    
    def read_trits(self, count: int) -> List[Trit]:
        """
        Read ternary data.
        
        Args:
            count: Number of trits to read
            
        Returns:
            List of Trit objects
        """
        if not self.is_open:
            raise RuntimeError("Stream not open")
        
        try:
            # Read binary data
            bytes_needed = (count + 3) // 4  # 4 trits per byte
            binary_data = self.file_handle.read(bytes_needed)
            
            if not binary_data:
                return []
            
            # Decode to trits
            trits = self.codec.decode(binary_data)
            
            # Trim to requested count
            trits = trits[:count]
            
            # Update statistics
            self.stats['bytes_read'] += len(binary_data)
            self.stats['trits_read'] += len(trits)
            self.stats['read_operations'] += 1
            self.position += len(trits)
            
            return trits
            
        except Exception as e:
            raise RuntimeError(f"Failed to read trits: {e}")
    
    def read_tritarray(self, size: int) -> Optional[TritArray]:
        """
        Read TritArray.
        
        Args:
            size: Size of TritArray
            
        Returns:
            TritArray or None if EOF
        """
        trits = self.read_trits(size)
        if not trits:
            return None
        
        return TritArray(trits)
    
    def read_line(self) -> List[Trit]:
        """
        Read a line of ternary data.
        
        Returns:
            List of Trit objects representing a line
        """
        line_trits = []
        
        while True:
            trit = self.read_trits(1)
            if not trit:
                break
            
            # Check for newline (ternary representation)
            if trit[0].value == 0:  # Assuming 0 represents newline
                break
            
            line_trits.append(trit[0])
        
        return line_trits
    
    def seek(self, position: int) -> None:
        """
        Seek to position in stream.
        
        Args:
            position: Position to seek to
        """
        if not self.is_open:
            raise RuntimeError("Stream not open")
        
        try:
            # Convert trit position to byte position
            byte_position = position // 4
            self.file_handle.seek(byte_position)
            self.position = position
            
        except Exception as e:
            raise RuntimeError(f"Failed to seek: {e}")
    
    def tell(self) -> int:
        """Get current position in stream."""
        return self.position
    
    def close(self) -> None:
        """Close the input stream."""
        if self.is_open and hasattr(self, 'file_handle'):
            self.file_handle.close()
        self.is_open = False
    
    def get_stats(self) -> dict:
        """Get stream statistics."""
        return {
            'is_open': self.is_open,
            'position': self.position,
            **self.stats
        }
    
    def __del__(self):
        """Destructor."""
        self.close()


class TernaryOutputStream:
    """
    Ternary Output Stream - Writes ternary data to various destinations.
    
    Provides streaming output for ternary data with buffering and encoding.
    """
    
    def __init__(self, destination: Union[str, BinaryIO, TextIO], 
                 encoding: str = 'utf-8', endianness: Endianness = Endianness.LITTLE_ENDIAN):
        """
        Initialize ternary output stream.
        
        Args:
            destination: Output destination (file path, file object, or stdout)
            encoding: Text encoding for text destinations
            endianness: Byte order for binary destinations
        """
        self.destination = destination
        self.encoding = encoding
        self.endianness = endianness
        self.codec = TritCodec(endianness)
        
        # Stream state
        self.is_open = False
        self.position = 0
        self.buffer = []
        self.buffer_size = 4096
        
        # Statistics
        self.stats = {
            'bytes_written': 0,
            'trits_written': 0,
            'write_operations': 0
        }
        
        # Open stream
        self._open_stream()
    
    def _open_stream(self) -> None:
        """Open the output stream."""
        try:
            if isinstance(self.destination, str):
                # File path
                self.file_handle = open(self.destination, 'wb')
                self.is_open = True
            elif hasattr(self.destination, 'write'):
                # File object
                self.file_handle = self.destination
                self.is_open = True
            else:
                # Default to stdout
                self.file_handle = sys.stdout.buffer if hasattr(sys.stdout, 'buffer') else sys.stdout
                self.is_open = True
                
        except Exception as e:
            raise RuntimeError(f"Failed to open output stream: {e}")
    
    def write_trits(self, trits: List[Trit]) -> None:
        """
        Write ternary data.
        
        Args:
            trits: List of Trit objects to write
        """
        if not self.is_open:
            raise RuntimeError("Stream not open")
        
        try:
            # Encode to binary
            binary_data = self.codec.encode(trits)
            
            # Write binary data
            self.file_handle.write(binary_data)
            
            # Update statistics
            self.stats['bytes_written'] += len(binary_data)
            self.stats['trits_written'] += len(trits)
            self.stats['write_operations'] += 1
            self.position += len(trits)
            
        except Exception as e:
            raise RuntimeError(f"Failed to write trits: {e}")
    
    def write_tritarray(self, tritarray: TritArray) -> None:
        """
        Write TritArray.
        
        Args:
            tritarray: TritArray to write
        """
        self.write_trits(tritarray.trits)
    
    def write_line(self, trits: List[Trit]) -> None:
        """
        Write a line of ternary data.
        
        Args:
            trits: List of Trit objects representing a line
        """
        # Add newline (ternary representation)
        line_trits = trits + [Trit(0)]  # Assuming 0 represents newline
        self.write_trits(line_trits)
    
    def flush(self) -> None:
        """Flush the output stream."""
        if self.is_open and hasattr(self.file_handle, 'flush'):
            self.file_handle.flush()
    
    def close(self) -> None:
        """Close the output stream."""
        if self.is_open:
            self.flush()
            if hasattr(self, 'file_handle'):
                self.file_handle.close()
        self.is_open = False
    
    def get_stats(self) -> dict:
        """Get stream statistics."""
        return {
            'is_open': self.is_open,
            'position': self.position,
            **self.stats
        }
    
    def __del__(self):
        """Destructor."""
        self.close()


class TernaryFileIO:
    """
    Ternary File I/O - File operations for ternary data.
    
    Provides file operations optimized for ternary data storage.
    """
    
    @staticmethod
    def read_file(file_path: str, endianness: Endianness = Endianness.LITTLE_ENDIAN) -> List[Trit]:
        """
        Read entire file as ternary data.
        
        Args:
            file_path: Path to file
            endianness: Byte order for reading
            
        Returns:
            List of Trit objects
        """
        with TernaryInputStream(file_path, endianness=endianness) as stream:
            return stream.read_trits(os.path.getsize(file_path) * 4)  # 4 trits per byte
    
    @staticmethod
    def write_file(file_path: str, trits: List[Trit], 
                  endianness: Endianness = Endianness.LITTLE_ENDIAN) -> None:
        """
        Write ternary data to file.
        
        Args:
            file_path: Path to file
            trits: List of Trit objects to write
            endianness: Byte order for writing
        """
        with TernaryOutputStream(file_path, endianness=endianness) as stream:
            stream.write_trits(trits)
    
    @staticmethod
    def append_file(file_path: str, trits: List[Trit], 
                   endianness: Endianness = Endianness.LITTLE_ENDIAN) -> None:
        """
        Append ternary data to file.
        
        Args:
            file_path: Path to file
            trits: List of Trit objects to append
            endianness: Byte order for writing
        """
        with open(file_path, 'ab') as f:
            codec = TritCodec(endianness)
            binary_data = codec.encode(trits)
            f.write(binary_data)
    
    @staticmethod
    def copy_file(source_path: str, dest_path: str, 
                 endianness: Endianness = Endianness.LITTLE_ENDIAN) -> None:
        """
        Copy ternary file.
        
        Args:
            source_path: Source file path
            dest_path: Destination file path
            endianness: Byte order for copying
        """
        trits = TernaryFileIO.read_file(source_path, endianness)
        TernaryFileIO.write_file(dest_path, trits, endianness)
    
    @staticmethod
    def get_file_size(file_path: str) -> int:
        """
        Get file size in trits.
        
        Args:
            file_path: Path to file
            
        Returns:
            File size in trits
        """
        byte_size = os.path.getsize(file_path)
        return byte_size * 4  # 4 trits per byte


class TernaryConsoleIO:
    """
    Ternary Console I/O - Console operations for ternary data.
    
    Provides console input/output optimized for ternary data.
    """
    
    @staticmethod
    def print_trits(trits: List[Trit], end: str = '\n') -> None:
        """
        Print trits to console.
        
        Args:
            trits: List of Trit objects to print
            end: End character
        """
        for trit in trits:
            if trit.value == 1:
                print('1', end='')
            elif trit.value == -1:
                print('-', end='')
            else:
                print('0', end='')
        print(end, end='')
    
    @staticmethod
    def print_tritarray(tritarray: TritArray, end: str = '\n') -> None:
        """
        Print TritArray to console.
        
        Args:
            tritarray: TritArray to print
            end: End character
        """
        TernaryConsoleIO.print_trits(tritarray.trits, end)
    
    @staticmethod
    def input_trits(prompt: str = "") -> List[Trit]:
        """
        Get trits from console input.
        
        Args:
            prompt: Input prompt
            
        Returns:
            List of Trit objects
        """
        if prompt:
            print(prompt, end='')
        
        user_input = input()
        trits = []
        
        for char in user_input:
            if char == '1':
                trits.append(Trit(1))
            elif char == '-':
                trits.append(Trit(-1))
            else:
                trits.append(Trit(0))
        
        return trits
    
    @staticmethod
    def input_tritarray(prompt: str = "", size: int = None) -> TritArray:
        """
        Get TritArray from console input.
        
        Args:
            prompt: Input prompt
            size: Expected size (if None, use input length)
            
        Returns:
            TritArray
        """
        trits = TernaryConsoleIO.input_trits(prompt)
        
        if size is not None:
            # Pad or truncate to specified size
            if len(trits) < size:
                trits.extend([Trit(0)] * (size - len(trits)))
            else:
                trits = trits[:size]
        
        return TritArray(trits)
