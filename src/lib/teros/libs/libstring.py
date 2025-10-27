"""
libstring.so - Ternary string manipulation library.

This module provides string operations optimized for ternary data.
"""

from typing import List, Union, Optional, Tuple, Dict
import re
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryString:
    """
    Ternary String - String operations for ternary data.
    
    Provides string manipulation, encoding, and conversion operations.
    """
    
    def __init__(self, trits: List[Trit] = None):
        """
        Initialize ternary string.
        
        Args:
            trits: List of Trit objects
        """
        self.trits = trits or []
        self.length = len(self.trits)
    
    def __len__(self) -> int:
        """Get string length."""
        return self.length
    
    def __getitem__(self, index: int) -> Trit:
        """Get trit at index."""
        if 0 <= index < self.length:
            return self.trits[index]
        raise IndexError("String index out of range")
    
    def __setitem__(self, index: int, value: Trit) -> None:
        """Set trit at index."""
        if 0 <= index < self.length:
            self.trits[index] = value
        else:
            raise IndexError("String index out of range")
    
    def __str__(self) -> str:
        """Convert to string representation."""
        return ''.join(self._trit_to_char(trit) for trit in self.trits)
    
    def __repr__(self) -> str:
        """Get string representation."""
        return f"TernaryString({self.__str__()})"
    
    def _trit_to_char(self, trit: Trit) -> str:
        """Convert trit to character."""
        if trit.value == 1:
            return '1'
        elif trit.value == -1:
            return '-'
        else:
            return '0'
    
    def _char_to_trit(self, char: str) -> Trit:
        """Convert character to trit."""
        if char == '1':
            return Trit(1)
        elif char == '-':
            return Trit(-1)
        else:
            return Trit(0)
    
    def append(self, trit: Trit) -> None:
        """Append trit to string."""
        self.trits.append(trit)
        self.length += 1
    
    def insert(self, index: int, trit: Trit) -> None:
        """Insert trit at index."""
        if 0 <= index <= self.length:
            self.trits.insert(index, trit)
            self.length += 1
        else:
            raise IndexError("String index out of range")
    
    def remove(self, index: int) -> Trit:
        """Remove trit at index."""
        if 0 <= index < self.length:
            trit = self.trits.pop(index)
            self.length -= 1
            return trit
        else:
            raise IndexError("String index out of range")
    
    def substring(self, start: int, end: int = None) -> 'TernaryString':
        """Get substring."""
        if end is None:
            end = self.length
        
        if start < 0 or end > self.length or start > end:
            raise ValueError("Invalid substring range")
        
        return TernaryString(self.trits[start:end])
    
    def find(self, pattern: 'TernaryString', start: int = 0) -> int:
        """Find pattern in string."""
        if start < 0 or start >= self.length:
            return -1
        
        pattern_len = len(pattern)
        if pattern_len == 0:
            return start
        
        for i in range(start, self.length - pattern_len + 1):
            if self.trits[i:i+pattern_len] == pattern.trits:
                return i
        
        return -1
    
    def replace(self, old: 'TernaryString', new: 'TernaryString') -> 'TernaryString':
        """Replace pattern in string."""
        result = TernaryString(self.trits.copy())
        
        while True:
            index = result.find(old)
            if index == -1:
                break
            
            # Replace pattern
            result.trits = (result.trits[:index] + 
                           new.trits + 
                           result.trits[index+len(old):])
            result.length = len(result.trits)
        
        return result
    
    def split(self, delimiter: 'TernaryString') -> List['TernaryString']:
        """Split string by delimiter."""
        if len(delimiter) == 0:
            return [TernaryString([trit]) for trit in self.trits]
        
        result = []
        current = TernaryString()
        
        i = 0
        while i < self.length:
            if i + len(delimiter) <= self.length:
                if self.trits[i:i+len(delimiter)] == delimiter.trits:
                    if len(current) > 0:
                        result.append(current)
                        current = TernaryString()
                    i += len(delimiter)
                    continue
            
            current.append(self.trits[i])
            i += 1
        
        if len(current) > 0:
            result.append(current)
        
        return result
    
    def join(self, strings: List['TernaryString']) -> 'TernaryString':
        """Join strings with this string as separator."""
        if not strings:
            return TernaryString()
        
        result = strings[0].trits.copy()
        
        for string in strings[1:]:
            result.extend(self.trits)
            result.extend(string.trits)
        
        return TernaryString(result)
    
    def reverse(self) -> 'TernaryString':
        """Reverse string."""
        return TernaryString(self.trits[::-1])
    
    def to_upper(self) -> 'TernaryString':
        """Convert to uppercase (ternary equivalent)."""
        # In ternary, "uppercase" means converting -1 to 1
        result_trits = []
        for trit in self.trits:
            if trit.value == -1:
                result_trits.append(Trit(1))
            else:
                result_trits.append(trit)
        return TernaryString(result_trits)
    
    def to_lower(self) -> 'TernaryString':
        """Convert to lowercase (ternary equivalent)."""
        # In ternary, "lowercase" means converting 1 to -1
        result_trits = []
        for trit in self.trits:
            if trit.value == 1:
                result_trits.append(Trit(-1))
            else:
                result_trits.append(trit)
        return TernaryString(result_trits)
    
    def strip(self, chars: 'TernaryString' = None) -> 'TernaryString':
        """Strip characters from ends."""
        if chars is None:
            chars = TernaryString([Trit(0)])  # Strip zeros by default
        
        # Strip from start
        start = 0
        while start < self.length and self.trits[start] in chars.trits:
            start += 1
        
        # Strip from end
        end = self.length
        while end > start and self.trits[end-1] in chars.trits:
            end -= 1
        
        return TernaryString(self.trits[start:end])
    
    def lstrip(self, chars: 'TernaryString' = None) -> 'TernaryString':
        """Strip characters from left end."""
        if chars is None:
            chars = TernaryString([Trit(0)])
        
        start = 0
        while start < self.length and self.trits[start] in chars.trits:
            start += 1
        
        return TernaryString(self.trits[start:])
    
    def rstrip(self, chars: 'TernaryString' = None) -> 'TernaryString':
        """Strip characters from right end."""
        if chars is None:
            chars = TernaryString([Trit(0)])
        
        end = self.length
        while end > 0 and self.trits[end-1] in chars.trits:
            end -= 1
        
        return TernaryString(self.trits[:end])
    
    def count(self, pattern: 'TernaryString') -> int:
        """Count occurrences of pattern."""
        count = 0
        start = 0
        
        while True:
            index = self.find(pattern, start)
            if index == -1:
                break
            count += 1
            start = index + 1
        
        return count
    
    def startswith(self, prefix: 'TernaryString') -> bool:
        """Check if string starts with prefix."""
        if len(prefix) > self.length:
            return False
        
        return self.trits[:len(prefix)] == prefix.trits
    
    def endswith(self, suffix: 'TernaryString') -> bool:
        """Check if string ends with suffix."""
        if len(suffix) > self.length:
            return False
        
        return self.trits[-len(suffix):] == suffix.trits
    
    def isdigit(self) -> bool:
        """Check if string contains only digits (ternary equivalent)."""
        for trit in self.trits:
            if trit.value not in [-1, 0, 1]:
                return False
        return True
    
    def isalpha(self) -> bool:
        """Check if string contains only letters (ternary equivalent)."""
        # In ternary, "letters" are non-zero trits
        for trit in self.trits:
            if trit.value == 0:
                return False
        return True
    
    def isnumeric(self) -> bool:
        """Check if string is numeric (ternary equivalent)."""
        return self.isdigit()
    
    def to_tritarray(self) -> TritArray:
        """Convert to TritArray."""
        return TritArray(self.trits)
    
    @classmethod
    def from_tritarray(cls, tritarray: TritArray) -> 'TernaryString':
        """Create from TritArray."""
        return cls(tritarray.trits)
    
    @classmethod
    def from_string(cls, string: str) -> 'TernaryString':
        """Create from string."""
        trits = []
        for char in string:
            if char == '1':
                trits.append(Trit(1))
            elif char == '-':
                trits.append(Trit(-1))
            else:
                trits.append(Trit(0))
        return cls(trits)
    
    def copy(self) -> 'TernaryString':
        """Create copy of string."""
        return TernaryString(self.trits.copy())


class TernaryStringBuilder:
    """
    Ternary String Builder - Efficient string building.
    
    Provides efficient string concatenation and building.
    """
    
    def __init__(self, initial_capacity: int = 16):
        """
        Initialize string builder.
        
        Args:
            initial_capacity: Initial capacity
        """
        self.trits = []
        self.capacity = initial_capacity
        self.length = 0
    
    def append(self, trit: Trit) -> None:
        """Append trit to builder."""
        if self.length >= self.capacity:
            self._expand_capacity()
        
        self.trits.append(trit)
        self.length += 1
    
    def append_string(self, string: TernaryString) -> None:
        """Append string to builder."""
        for trit in string.trits:
            self.append(trit)
    
    def insert(self, index: int, trit: Trit) -> None:
        """Insert trit at index."""
        if index < 0 or index > self.length:
            raise IndexError("Index out of range")
        
        if self.length >= self.capacity:
            self._expand_capacity()
        
        self.trits.insert(index, trit)
        self.length += 1
    
    def remove(self, index: int) -> Trit:
        """Remove trit at index."""
        if index < 0 or index >= self.length:
            raise IndexError("Index out of range")
        
        trit = self.trits.pop(index)
        self.length -= 1
        return trit
    
    def clear(self) -> None:
        """Clear builder."""
        self.trits.clear()
        self.length = 0
    
    def to_string(self) -> TernaryString:
        """Convert to TernaryString."""
        return TernaryString(self.trits.copy())
    
    def _expand_capacity(self) -> None:
        """Expand capacity."""
        self.capacity *= 2
    
    def __len__(self) -> int:
        """Get length."""
        return self.length
    
    def __str__(self) -> str:
        """Get string representation."""
        return ''.join(self._trit_to_char(trit) for trit in self.trits)
    
    def _trit_to_char(self, trit: Trit) -> str:
        """Convert trit to character."""
        if trit.value == 1:
            return '1'
        elif trit.value == -1:
            return '-'
        else:
            return '0'


class TernaryStringUtils:
    """
    Ternary String Utils - Utility functions for string operations.
    """
    
    @staticmethod
    def format_string(template: str, *args) -> TernaryString:
        """
        Format string with arguments.
        
        Args:
            template: Format template
            *args: Arguments to format
            
        Returns:
            Formatted TernaryString
        """
        try:
            formatted = template.format(*args)
            return TernaryString.from_string(formatted)
        except Exception as e:
            raise ValueError(f"String formatting error: {e}")
    
    @staticmethod
    def join_strings(strings: List[TernaryString], separator: TernaryString = None) -> TernaryString:
        """
        Join strings with separator.
        
        Args:
            strings: List of strings to join
            separator: Separator string
            
        Returns:
            Joined string
        """
        if not strings:
            return TernaryString()
        
        if separator is None:
            separator = TernaryString()
        
        result = strings[0].copy()
        
        for string in strings[1:]:
            result = result.join([result, separator, string])
        
        return result
    
    @staticmethod
    def split_string(string: TernaryString, delimiter: TernaryString) -> List[TernaryString]:
        """
        Split string by delimiter.
        
        Args:
            string: String to split
            delimiter: Delimiter string
            
        Returns:
            List of split strings
        """
        return string.split(delimiter)
    
    @staticmethod
    def replace_string(string: TernaryString, old: TernaryString, new: TernaryString) -> TernaryString:
        """
        Replace pattern in string.
        
        Args:
            string: String to replace in
            old: Pattern to replace
            new: Replacement pattern
            
        Returns:
            String with replacements
        """
        return string.replace(old, new)
    
    @staticmethod
    def find_string(string: TernaryString, pattern: TernaryString, start: int = 0) -> int:
        """
        Find pattern in string.
        
        Args:
            string: String to search in
            pattern: Pattern to find
            start: Start position
            
        Returns:
            Index of pattern or -1 if not found
        """
        return string.find(pattern, start)
    
    @staticmethod
    def count_string(string: TernaryString, pattern: TernaryString) -> int:
        """
        Count occurrences of pattern.
        
        Args:
            string: String to search in
            pattern: Pattern to count
            
        Returns:
            Number of occurrences
        """
        return string.count(pattern)
    
    @staticmethod
    def reverse_string(string: TernaryString) -> TernaryString:
        """
        Reverse string.
        
        Args:
            string: String to reverse
            
        Returns:
            Reversed string
        """
        return string.reverse()
    
    @staticmethod
    def strip_string(string: TernaryString, chars: TernaryString = None) -> TernaryString:
        """
        Strip characters from ends.
        
        Args:
            string: String to strip
            chars: Characters to strip
            
        Returns:
            Stripped string
        """
        return string.strip(chars)
    
    @staticmethod
    def upper_string(string: TernaryString) -> TernaryString:
        """
        Convert to uppercase.
        
        Args:
            string: String to convert
            
        Returns:
            Uppercase string
        """
        return string.to_upper()
    
    @staticmethod
    def lower_string(string: TernaryString) -> TernaryString:
        """
        Convert to lowercase.
        
        Args:
            string: String to convert
            
        Returns:
            Lowercase string
        """
        return string.to_lower()
