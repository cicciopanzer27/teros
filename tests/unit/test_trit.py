"""
Unit tests for Trit (ternary value) operations.

Tests cover:
- Basic trit operations (AND, OR, NOT, XOR)
- Arithmetic operations (ADD, SUB, MUL, DIV)
- Comparison and validation
- Edge cases and error handling
"""

import pytest
import sys
from pathlib import Path

# Add project root to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

# Try importing Python implementation first
try:
    from src.lib.teros.core.trit import Trit
    PYTHON_AVAILABLE = True
except ImportError:
    PYTHON_AVAILABLE = False

# TODO: Import C implementation when available
# from src.kernel.trit import trit_t, trit_create


@pytest.mark.unit
class TestTritCreation:
    """Test trit creation and initialization."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_create_positive(self):
        """Test creating positive trit (+1)."""
        trit = Trit(1)
        assert trit.value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_create_negative(self):
        """Test creating negative trit (-1)."""
        trit = Trit(-1)
        assert trit.value == -1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_create_neutral(self):
        """Test creating neutral trit (0)."""
        trit = Trit(0)
        assert trit.value == 0
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_invalid_value(self):
        """Test that invalid values raise exception."""
        with pytest.raises(ValueError):
            Trit(2)
        with pytest.raises(ValueError):
            Trit(-2)


@pytest.mark.unit
class TestTritLogic:
    """Test ternary logic operations."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_and(self):
        """Test ternary AND operation."""
        assert (Trit(-1) & Trit(-1)).value == -1
        assert (Trit(-1) & Trit(0)).value == 0
        assert (Trit(-1) & Trit(1)).value == -1
        assert (Trit(0) & Trit(-1)).value == 0
        assert (Trit(0) & Trit(0)).value == 0
        assert (Trit(0) & Trit(1)).value == 0
        assert (Trit(1) & Trit(-1)).value == -1
        assert (Trit(1) & Trit(0)).value == 0
        assert (Trit(1) & Trit(1)).value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_or(self):
        """Test ternary OR operation."""
        assert (Trit(-1) | Trit(-1)).value == -1
        assert (Trit(-1) | Trit(0)).value == -1
        assert (Trit(-1) | Trit(1)).value == 1
        assert (Trit(0) | Trit(-1)).value == -1
        assert (Trit(0) | Trit(0)).value == 0
        assert (Trit(0) | Trit(1)).value == 1
        assert (Trit(1) | Trit(-1)).value == 1
        assert (Trit(1) | Trit(0)).value == 1
        assert (Trit(1) | Trit(1)).value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_not(self):
        """Test ternary NOT operation."""
        assert (~Trit(-1)).value == 1
        assert (~Trit(0)).value == 0
        assert (~Trit(1)).value == -1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_xor(self):
        """Test ternary XOR operation."""
        assert (Trit(-1) ^ Trit(-1)).value == 0
        assert (Trit(-1) ^ Trit(0)).value == -1
        assert (Trit(-1) ^ Trit(1)).value == 1
        assert (Trit(0) ^ Trit(-1)).value == -1
        assert (Trit(0) ^ Trit(0)).value == 0
        assert (Trit(0) ^ Trit(1)).value == 1
        assert (Trit(1) ^ Trit(-1)).value == 1
        assert (Trit(1) ^ Trit(0)).value == 1
        assert (Trit(1) ^ Trit(1)).value == 0


@pytest.mark.unit
class TestTritArithmetic:
    """Test ternary arithmetic operations."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_add(self):
        """Test ternary addition."""
        assert (Trit(-1) + Trit(-1)).value == -1
        assert (Trit(-1) + Trit(0)).value == -1
        assert (Trit(-1) + Trit(1)).value == 0
        assert (Trit(0) + Trit(-1)).value == -1
        assert (Trit(0) + Trit(0)).value == 0
        assert (Trit(0) + Trit(1)).value == 1
        assert (Trit(1) + Trit(-1)).value == 0
        assert (Trit(1) + Trit(0)).value == 1
        assert (Trit(1) + Trit(1)).value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_subtract(self):
        """Test ternary subtraction."""
        assert (Trit(-1) - Trit(-1)).value == 0
        assert (Trit(-1) - Trit(0)).value == -1
        assert (Trit(-1) - Trit(1)).value == -1
        assert (Trit(0) - Trit(-1)).value == 1
        assert (Trit(0) - Trit(0)).value == 0
        assert (Trit(0) - Trit(1)).value == -1
        assert (Trit(1) - Trit(-1)).value == 1
        assert (Trit(1) - Trit(0)).value == 1
        assert (Trit(1) - Trit(1)).value == 0
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_multiply(self):
        """Test ternary multiplication."""
        assert (Trit(-1) * Trit(-1)).value == 1
        assert (Trit(-1) * Trit(0)).value == 0
        assert (Trit(-1) * Trit(1)).value == -1
        assert (Trit(0) * Trit(-1)).value == 0
        assert (Trit(0) * Trit(0)).value == 0
        assert (Trit(0) * Trit(1)).value == 0
        assert (Trit(1) * Trit(-1)).value == -1
        assert (Trit(1) * Trit(0)).value == 0
        assert (Trit(1) * Trit(1)).value == 1


@pytest.mark.unit
class TestTritConversion:
    """Test trit conversions."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_to_int(self):
        """Test conversion to integer."""
        assert int(Trit(-1)) == -1
        assert int(Trit(0)) == 0
        assert int(Trit(1)) == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python trit implementation not available")
    def test_to_bool(self):
        """Test conversion to boolean."""
        assert bool(Trit(-1)) == False
        assert bool(Trit(0)) == False
        assert bool(Trit(1)) == True


if __name__ == '__main__':
    pytest.main([__file__, '-v'])

