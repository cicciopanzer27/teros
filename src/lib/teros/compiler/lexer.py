"""
Lambda³ Lexer - Lexical analysis for Lambda³ language.

This module provides the lexical analyzer for the Lambda³ language,
tokenizing source code into a stream of tokens.
"""

from typing import List, Optional, Dict, Any, Iterator
import re
from enum import Enum


class TokenType(Enum):
    """Token types for Lambda³ language."""
    
    # Literals
    INTEGER = "INTEGER"
    FLOAT = "FLOAT"
    STRING = "STRING"
    TRIT = "TRIT"
    TRITARRAY = "TRITARRAY"
    BOOLEAN = "BOOLEAN"
    
    # Identifiers
    IDENTIFIER = "IDENTIFIER"
    
    # Keywords
    LET = "LET"
    IN = "IN"
    IF = "IF"
    THEN = "THEN"
    ELSE = "ELSE"
    FUN = "FUN"
    LAMBDA = "LAMBDA"
    MATCH = "MATCH"
    WITH = "WITH"
    CASE = "CASE"
    OF = "OF"
    REC = "REC"
    AND = "AND"
    OR = "OR"
    NOT = "NOT"
    TRUE = "TRUE"
    FALSE = "FALSE"
    NEGATIVE = "NEGATIVE"
    NEUTRAL = "NEUTRAL"
    POSITIVE = "POSITIVE"
    
    # Operators
    PLUS = "PLUS"
    MINUS = "MINUS"
    MULTIPLY = "MULTIPLY"
    DIVIDE = "DIVIDE"
    MODULO = "MODULO"
    POWER = "POWER"
    EQUAL = "EQUAL"
    NOT_EQUAL = "NOT_EQUAL"
    LESS = "LESS"
    LESS_EQUAL = "LESS_EQUAL"
    GREATER = "GREATER"
    GREATER_EQUAL = "GREATER_EQUAL"
    AND_OP = "AND_OP"
    OR_OP = "OR_OP"
    NOT_OP = "NOT_OP"
    TERNARY_AND = "TERNARY_AND"
    TERNARY_OR = "TERNARY_OR"
    TERNARY_NOT = "TERNARY_NOT"
    TERNARY_XOR = "TERNARY_XOR"
    TERNARY_CONS = "TERNARY_CONS"
    TERNARY_ANY = "TERNARY_ANY"
    TERNARY_NAND = "TERNARY_NAND"
    
    # Punctuation
    LEFT_PAREN = "LEFT_PAREN"
    RIGHT_PAREN = "RIGHT_PAREN"
    LEFT_BRACKET = "LEFT_BRACKET"
    RIGHT_BRACKET = "RIGHT_BRACKET"
    LEFT_BRACE = "LEFT_BRACE"
    RIGHT_BRACE = "RIGHT_BRACE"
    COMMA = "COMMA"
    SEMICOLON = "SEMICOLON"
    COLON = "COLON"
    DOT = "DOT"
    ARROW = "ARROW"
    PIPE = "PIPE"
    ASSIGN = "ASSIGN"
    
    # Special
    NEWLINE = "NEWLINE"
    WHITESPACE = "WHITESPACE"
    COMMENT = "COMMENT"
    EOF = "EOF"
    ERROR = "ERROR"


class Token:
    """Token class for Lambda³ language."""
    
    def __init__(self, type_: TokenType, value: str, line: int = 1, column: int = 1):
        """
        Initialize a token.
        
        Args:
            type_: Token type
            value: Token value
            line: Line number
            column: Column number
        """
        self.type = type_
        self.value = value
        self.line = line
        self.column = column
    
    def __str__(self) -> str:
        """String representation."""
        return f"Token({self.type.value}, '{self.value}', line={self.line}, col={self.column})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Token({self.type.value}, '{self.value}', line={self.line}, col={self.column})"
    
    def __eq__(self, other) -> bool:
        """Equality comparison."""
        if isinstance(other, Token):
            return (self.type == other.type and 
                    self.value == other.value and 
                    self.line == other.line and 
                    self.column == other.column)
        return False


class Lambda3Lexer:
    """
    Lexical analyzer for Lambda³ language.
    
    Tokenizes Lambda³ source code into a stream of tokens.
    """
    
    def __init__(self):
        """Initialize the lexer."""
        # Define token patterns
        self.patterns = [
            # Keywords
            (r'\blet\b', TokenType.LET),
            (r'\bin\b', TokenType.IN),
            (r'\bif\b', TokenType.IF),
            (r'\bthen\b', TokenType.THEN),
            (r'\belse\b', TokenType.ELSE),
            (r'\bfun\b', TokenType.FUN),
            (r'\blambda\b', TokenType.LAMBDA),
            (r'\bmatch\b', TokenType.MATCH),
            (r'\bwith\b', TokenType.WITH),
            (r'\bcase\b', TokenType.CASE),
            (r'\bof\b', TokenType.OF),
            (r'\brec\b', TokenType.REC),
            (r'\band\b', TokenType.AND),
            (r'\bor\b', TokenType.OR),
            (r'\bnot\b', TokenType.NOT),
            (r'\btrue\b', TokenType.TRUE),
            (r'\bfalse\b', TokenType.FALSE),
            (r'\bnegative\b', TokenType.NEGATIVE),
            (r'\bneutral\b', TokenType.NEUTRAL),
            (r'\bpositive\b', TokenType.POSITIVE),
            
            # Literals
            (r'\b\d+\.\d+\b', TokenType.FLOAT),
            (r'\b\d+\b', TokenType.INTEGER),
            (r'"[^"]*"', TokenType.STRING),
            (r"'[^']*'", TokenType.STRING),
            (r'\b0t[+-0]+\b', TokenType.TRITARRAY),
            (r'\b[+-]\b', TokenType.TRIT),
            (r'\b0\b', TokenType.TRIT),
            
            # Operators
            (r'\+', TokenType.PLUS),
            (r'-', TokenType.MINUS),
            (r'\*', TokenType.MULTIPLY),
            (r'/', TokenType.DIVIDE),
            (r'%', TokenType.MODULO),
            (r'\^', TokenType.POWER),
            (r'==', TokenType.EQUAL),
            (r'!=', TokenType.NOT_EQUAL),
            (r'<', TokenType.LESS),
            (r'<=', TokenType.LESS_EQUAL),
            (r'>', TokenType.GREATER),
            (r'>=', TokenType.GREATER_EQUAL),
            (r'&&', TokenType.AND_OP),
            (r'\|\|', TokenType.OR_OP),
            (r'!', TokenType.NOT_OP),
            (r'&', TokenType.TERNARY_AND),
            (r'\|', TokenType.TERNARY_OR),
            (r'~', TokenType.TERNARY_NOT),
            (r'\^', TokenType.TERNARY_XOR),
            (r'->', TokenType.TERNARY_CONS),
            (r'\?', TokenType.TERNARY_ANY),
            (r'!&', TokenType.TERNARY_NAND),
            
            # Punctuation
            (r'\(', TokenType.LEFT_PAREN),
            (r'\)', TokenType.RIGHT_PAREN),
            (r'\[', TokenType.LEFT_BRACKET),
            (r'\]', TokenType.RIGHT_BRACKET),
            (r'\{', TokenType.LEFT_BRACE),
            (r'\}', TokenType.RIGHT_BRACE),
            (r',', TokenType.COMMA),
            (r';', TokenType.SEMICOLON),
            (r':', TokenType.COLON),
            (r'\.', TokenType.DOT),
            (r'=>', TokenType.ARROW),
            (r'\|', TokenType.PIPE),
            (r'=', TokenType.ASSIGN),
            
            # Special
            (r'\n', TokenType.NEWLINE),
            (r'\s+', TokenType.WHITESPACE),
            (r'//.*', TokenType.COMMENT),
            (r'/\*.*?\*/', TokenType.COMMENT),
        ]
        
        # Compile patterns
        self.compiled_patterns = []
        for pattern, token_type in self.patterns:
            self.compiled_patterns.append((re.compile(pattern), token_type))
    
    def tokenize(self, source_code: str) -> List[Token]:
        """
        Tokenize source code.
        
        Args:
            source_code: Source code string
            
        Returns:
            List of tokens
        """
        tokens = []
        position = 0
        line = 1
        column = 1
        
        while position < len(source_code):
            matched = False
            
            # Try to match each pattern
            for pattern, token_type in self.compiled_patterns:
                match = pattern.match(source_code, position)
                if match:
                    value = match.group(0)
                    
                    # Skip whitespace and comments
                    if token_type in [TokenType.WHITESPACE, TokenType.COMMENT]:
                        # Update position and line/column
                        position = match.end()
                        if token_type == TokenType.NEWLINE:
                            line += 1
                            column = 1
                        else:
                            column += len(value)
                        matched = True
                        break
                    
                    # Create token
                    token = Token(token_type, value, line, column)
                    tokens.append(token)
                    
                    # Update position and line/column
                    position = match.end()
                    if token_type == TokenType.NEWLINE:
                        line += 1
                        column = 1
                    else:
                        column += len(value)
                    
                    matched = True
                    break
            
            if not matched:
                # No pattern matched, create error token
                char = source_code[position]
                error_token = Token(TokenType.ERROR, char, line, column)
                tokens.append(error_token)
                position += 1
                column += 1
        
        # Add EOF token
        eof_token = Token(TokenType.EOF, "", line, column)
        tokens.append(eof_token)
        
        return tokens
    
    def tokenize_file(self, filename: str) -> List[Token]:
        """
        Tokenize a source file.
        
        Args:
            filename: Path to source file
            
        Returns:
            List of tokens
        """
        with open(filename, 'r', encoding='utf-8') as f:
            source_code = f.read()
        
        return self.tokenize(source_code)
    
    def get_keywords(self) -> List[str]:
        """Get list of keywords."""
        keywords = [
            'let', 'in', 'if', 'then', 'else', 'fun', 'lambda',
            'match', 'with', 'case', 'of', 'rec', 'and', 'or', 'not',
            'true', 'false', 'negative', 'neutral', 'positive'
        ]
        return keywords
    
    def get_operators(self) -> List[str]:
        """Get list of operators."""
        operators = [
            '+', '-', '*', '/', '%', '^',
            '==', '!=', '<', '<=', '>', '>=',
            '&&', '||', '!', '&', '|', '~', '^',
            '->', '?', '!&'
        ]
        return operators
    
    def get_punctuation(self) -> List[str]:
        """Get list of punctuation."""
        punctuation = [
            '(', ')', '[', ']', '{', '}',
            ',', ';', ':', '.', '=>', '|', '='
        ]
        return punctuation
    
    def __str__(self) -> str:
        """String representation."""
        return "Lambda3Lexer()"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return "Lambda3Lexer()"
