"""
Basic tests for Lambda³ core functionality
"""

import pytest
from lambda3.parser.lambda_parser import Var, Abs, App, parse
from lambda3.engine.reducer import reduce
from lambda3.ternary.encoder import encode, Trit


class TestParser:
    """Test lambda parser"""
    
    def test_parse_identity(self):
        """Test parsing identity function"""
        term = parse("λx.x")
        assert isinstance(term, Abs)
        assert term.var == "x"
        assert isinstance(term.body, Var)
        assert term.body.name == "x"
    
    def test_parse_application(self):
        """Test parsing application"""
        term = parse("(λx.x) y")
        assert isinstance(term, App)
        assert isinstance(term.func, Abs)
        assert isinstance(term.arg, Var)


class TestReducer:
    """Test beta reduction"""
    
    def test_reduce_identity(self):
        """Test reducing (λx.x) y → y"""
        term = parse("(λx.x) y")
        result = reduce(term)
        assert isinstance(result, Var)
        assert result.name == "y"


class TestTernary:
    """Test ternary encoding"""
    
    def test_encode_variable(self):
        """Test encoding variable"""
        term = Var("x")
        trits = encode(term)
        assert trits == [Trit.T0]
    
    def test_encode_abstraction(self):
        """Test encoding abstraction"""
        term = Abs("x", Var("x"))
        trits = encode(term)
        assert trits[0] == Trit.T1
        assert trits[1] == Trit.T0
    
    def test_encode_application(self):
        """Test encoding application"""
        term = App(Var("f"), Var("x"))
        trits = encode(term)
        assert trits[0] == Trit.T2


# TODO: Add more comprehensive tests
# See list_todo5.md Phase 1.5 for complete test suite specification

