"""
Lambda Calculus Lexer
Tokenizes lambda term strings
"""

from enum import Enum, auto
from dataclasses import dataclass
from typing import List, Optional


class TokenType(Enum):
    """Token types"""
    VAR = auto()        # x, y, z, x0, x1, ...
    LAMBDA = auto()     # λ or \
    DOT = auto()        # .
    LPAREN = auto()     # (
    RPAREN = auto()     # )
    EOF = auto()        # End of input
    ERROR = auto()      # Error token


@dataclass
class Token:
    """Token with type, value, and position"""
    type: TokenType
    value: str
    line: int
    column: int
    
    def __repr__(self) -> str:
        return f"Token({self.type.name}, '{self.value}', {self.line}:{self.column})"


class LambdaLexer:
    """
    Lexer for lambda calculus expressions
    
    Grammar:
        VAR    ::= [a-z] | [a-z][0-9]+
        LAMBDA ::= 'λ' | '\\'
        DOT    ::= '.'
        LPAREN ::= '('
        RPAREN ::= ')'
    """
    
    def __init__(self, source: str):
        self.source = source
        self.pos = 0
        self.line = 1
        self.column = 1
        self.tokens: List[Token] = []
    
    def current_char(self) -> Optional[str]:
        """Get current character"""
        if self.pos >= len(self.source):
            return None
        return self.source[self.pos]
    
    def peek_char(self, offset: int = 1) -> Optional[str]:
        """Peek ahead"""
        pos = self.pos + offset
        if pos >= len(self.source):
            return None
        return self.source[pos]
    
    def advance(self):
        """Move to next character"""
        if self.pos < len(self.source):
            if self.source[self.pos] == '\n':
                self.line += 1
                self.column = 1
            else:
                self.column += 1
            self.pos += 1
    
    def skip_whitespace(self):
        """Skip whitespace and comments"""
        while self.current_char() is not None:
            ch = self.current_char()
            if ch in ' \t\n\r':
                self.advance()
            elif ch == '#':  # Comment to end of line
                while self.current_char() is not None and self.current_char() != '\n':
                    self.advance()
            else:
                break
    
    def read_variable(self) -> Token:
        """Read variable name"""
        start_line = self.line
        start_column = self.column
        value = ''
        
        # Read letter
        if self.current_char() and self.current_char().isalpha():
            value += self.current_char()
            self.advance()
        
        # Read optional digits
        while self.current_char() and self.current_char().isdigit():
            value += self.current_char()
            self.advance()
        
        return Token(TokenType.VAR, value, start_line, start_column)
    
    def make_token(self, token_type: TokenType, value: str) -> Token:
        """Create token at current position"""
        token = Token(token_type, value, self.line, self.column)
        self.advance()
        return token
    
    def tokenize(self) -> List[Token]:
        """Tokenize the entire source"""
        self.tokens = []
        
        while self.current_char() is not None:
            self.skip_whitespace()
            
            if self.current_char() is None:
                break
            
            ch = self.current_char()
            
            # Lambda
            if ch == 'λ' or ch == '\\':
                self.tokens.append(self.make_token(TokenType.LAMBDA, ch))
            
            # Dot
            elif ch == '.':
                self.tokens.append(self.make_token(TokenType.DOT, ch))
            
            # Left paren
            elif ch == '(':
                self.tokens.append(self.make_token(TokenType.LPAREN, ch))
            
            # Right paren
            elif ch == ')':
                self.tokens.append(self.make_token(TokenType.RPAREN, ch))
            
            # Variable
            elif ch.isalpha():
                self.tokens.append(self.read_variable())
            
            # Error
            else:
                error_token = Token(
                    TokenType.ERROR,
                    ch,
                    self.line,
                    self.column
                )
                self.tokens.append(error_token)
                self.advance()
        
        # Add EOF token
        self.tokens.append(Token(TokenType.EOF, '', self.line, self.column))
        
        return self.tokens


def lex(source: str) -> List[Token]:
    """
    Convenience function to tokenize source
    
    Args:
        source: Lambda term as string
        
    Returns:
        List of tokens
        
    Example:
        >>> tokens = lex("λx.x")
        >>> [t.type.name for t in tokens]
        ['LAMBDA', 'VAR', 'DOT', 'VAR', 'EOF']
    """
    lexer = LambdaLexer(source)
    return lexer.tokenize()


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    # Test cases
    test_cases = [
        "x",
        "\\x.x",
        "(\\x.x) y",
        "\\f.\\x.f x",
        "x0",
        "x1",
    ]
    
    print("Lambda Lexer Test Cases:")
    print("=" * 60)
    
    for test in test_cases:
        print(f"\nInput: '{test}'")
        tokens = lex(test)
        for tok in tokens:
            if tok.type != TokenType.EOF:
                print(f"  {tok}")
        print(f"  Total tokens: {len(tokens)-1}")  # Exclude EOF

