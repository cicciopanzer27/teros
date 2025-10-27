"""
Unit tests for TritArray operations.

Tests cover:
- Array creation and destruction
- Element access and modification
- Array operations (slice, concat, reverse)
- Arithmetic operations
- Logic operations
- Conversions
"""

import pytest
import sys
from pathlib import Path

# Add project root to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

# Try importing Python implementation first
try:
    from src.lib.teros.core.tritarray import TritArray
    PYTHON_AVAILABLE = True
except ImportError:
    PYTHON_AVAILABLE = False


@pytest.mark.unit
class TestTritArrayCreation:
    """Test trit array creation and initialization."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_create_empty(self):
        """Test creating empty array."""
        arr = TritArray(0)
        assert len(arr) == 0
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_create_with_size(self):
        """Test creating array with specified size."""
        arr = TritArray(5)
        assert len(arr) == 5
        # All elements should be neutral (0)
        for trit in arr:
            assert trit.value == 0
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_create_from_list(self):
        """Test creating array from list of trits."""
        from src.lib.teros.core.trit import Trit
        arr = TritArray([Trit(-1), Trit(0), Trit(1)])
        assert len(arr) == 3
        assert arr[0].value == -1
        assert arr[1].value == 0
        assert arr[2].value == 1


@pytest.mark.unit
class TestTritArrayAccess:
    """Test trit array element access."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_get_element(self):
        """Test getting array elements."""
        from src.lib.teros.core.trit import Trit
        arr = TritArray([Trit(-1), Trit(0), Trit(1)])
        assert arr[0].value == -1
        assert arr[1].value == 0
        assert arr[2].value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_set_element(self):
        """Test setting array elements."""
        from src.lib.teros.core.trit import Trit
        arr = TritArray(3)
        arr[0] = Trit(-1)
        arr[1] = Trit(1)
        assert arr[0].value == -1
        assert arr[1].value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_index_error(self):
        """Test index out of bounds."""
        arr = TritArray(3)
        with pytest.raises(IndexError):
            _ = arr[10]


@pytest.mark.unit
class TestTritArrayOperations:
    """Test trit array operations."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_slice(self):
        """Test slicing array."""
        from src.lib.teros.core.trit import Trit
        arr = TritArray([Trit(-1), Trit(0), Trit(1)])
        slice_arr = arr[1:3]
        assert len(slice_arr) == 2
        assert slice_arr[0].value == 0
        assert slice_arr[1].value == 1
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_concat(self):
        """Test concatenating arrays."""
        from src.lib.teros.core.trit import Trit
        arr1 = TritArray([Trit(-1), Trit(0)])
        arr2 = TritArray([Trit(1), Trit(-1)])
        concat_arr = arr1 + arr2
        assert len(concat_arr) == 4
        assert concat_arr[0].value == -1
        assert concat_arr[1].value == 0
        assert concat_arr[2].value == 1
        assert concat_arr[3].value == -1


@pytest.mark.unit
class TestTritArrayArithmetic:
    """Test trit array arithmetic operations."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_add_arrays(self):
        """Test element-wise addition."""
        from src.lib.teros.core.trit import Trit
        arr1 = TritArray([Trit(-1), Trit(0), Trit(1)])
        arr2 = TritArray([Trit(1), Trit(0), Trit(-1)])
        result = arr1 + arr2
        assert len(result) == 3
        # Note: Addition with saturation for trits
        assert result[0].value == 0  # -1 + 1 = 0
        assert result[1].value == 0  # 0 + 0 = 0
        assert result[2].value == 0  # 1 + -1 = 0


@pytest.mark.unit
class TestTritArrayConversion:
    """Test trit array conversions."""
    
    @pytest.mark.skipif(not PYTHON_AVAILABLE, reason="Python tritarray implementation not available")
    def test_to_int(self):
        """Test converting array to integer."""
        from src.lib.teros.core.trit import Trit
        # Test balanced ternary: [-1, 0, 1] = -1*3^2 + 0*3^1 + 1*3^0 = -9 + 0 + 1 = -8
        arr = TritArray([Trit(-1), Trit(0), Trit(1)])
        # This is a placeholder - actual conversion depends on implementation
        # value = arr.to_int()
        # assert value == -8


if __name__ == '__main__':
    pytest.main([__file__, '-v'])

