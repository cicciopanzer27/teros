"""
Lambda³ Compiler - High-level programming language compiler for ternary code.

This module provides a complete compiler for the Lambda³ programming language.
"""

from typing import List, Union, Optional, Dict, Any, Tuple
import re
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..isa.t3_isa import T3_ISA
from ..vm.tvm import TVM


class TokenType(Enum):
    """Token types for Lambda³ language."""
    # Keywords
    LET = "let"
    IN = "in"
    IF = "if"
    THEN = "then"
    ELSE = "else"
    FUN = "fun"
    MATCH = "match"
    WITH = "with"
    CASE = "case"
    OF = "of"
    END = "end"
    REC = "rec"
    AND = "and"
    OR = "or"
    NOT = "not"
    TRUE = "true"
    FALSE = "false"
    NULL = "null"
    
    # Operators
    PLUS = "+"
    MINUS = "-"
    MULTIPLY = "*"
    DIVIDE = "/"
    MODULO = "%"
    POWER = "^"
    EQUALS = "="
    NOT_EQUALS = "!="
    LESS = "<"
    LESS_EQUAL = "<="
    GREATER = ">"
    GREATER_EQUAL = ">="
    AND_OP = "&&"
    OR_OP = "||"
    NOT_OP = "!"
    
    # Delimiters
    LEFT_PAREN = "("
    RIGHT_PAREN = ")"
    LEFT_BRACE = "{"
    RIGHT_BRACE = "}"
    LEFT_BRACKET = "["
    RIGHT_BRACKET = "]"
    SEMICOLON = ";"
    COMMA = ","
    DOT = "."
    COLON = ":"
    ARROW = "->"
    PIPE = "|"
    
    # Literals
    INTEGER = "integer"
    FLOAT = "float"
    STRING = "string"
    IDENTIFIER = "identifier"
    
    # Special
    NEWLINE = "newline"
    EOF = "eof"
    ERROR = "error"


class Token:
    """
    Token - Represents a lexical token.
    
    Provides token information for the compiler.
    """
    
    def __init__(self, type: TokenType, value: str, line: int = 0, column: int = 0):
        """
        Initialize token.
        
        Args:
            type: Token type
            value: Token value
            line: Line number
            column: Column number
        """
        self.type = type
        self.value = value
        self.line = line
        self.column = column
    
    def __str__(self) -> str:
        """Get string representation."""
        return f"Token({self.type.value}, '{self.value}', {self.line}:{self.column})"
    
    def __repr__(self) -> str:
        """Get representation."""
        return self.__str__()


class Lambda3Lexer:
    """
    Lambda³ Lexer - Tokenizes Lambda³ source code.
    
    Provides lexical analysis for the Lambda³ programming language.
    """
    
    def __init__(self, source: str):
        """
        Initialize lexer.
        
        Args:
            source: Source code to tokenize
        """
        self.source = source
        self.position = 0
        self.line = 1
        self.column = 1
        self.tokens = []
        
        # Keyword mapping
        self.keywords = {
            'let': TokenType.LET,
            'in': TokenType.IN,
            'if': TokenType.IF,
            'then': TokenType.THEN,
            'else': TokenType.ELSE,
            'fun': TokenType.FUN,
            'match': TokenType.MATCH,
            'with': TokenType.WITH,
            'case': TokenType.CASE,
            'of': TokenType.OF,
            'end': TokenType.END,
            'rec': TokenType.REC,
            'and': TokenType.AND,
            'or': TokenType.OR,
            'not': TokenType.NOT,
            'true': TokenType.TRUE,
            'false': TokenType.FALSE,
            'null': TokenType.NULL
        }
        
        # Operator mapping
        self.operators = {
            '+': TokenType.PLUS,
            '-': TokenType.MINUS,
            '*': TokenType.MULTIPLY,
            '/': TokenType.DIVIDE,
            '%': TokenType.MODULO,
            '^': TokenType.POWER,
            '=': TokenType.EQUALS,
            '!=': TokenType.NOT_EQUALS,
            '<': TokenType.LESS,
            '<=': TokenType.LESS_EQUAL,
            '>': TokenType.GREATER,
            '>=': TokenType.GREATER_EQUAL,
            '&&': TokenType.AND_OP,
            '||': TokenType.OR_OP,
            '!': TokenType.NOT_OP
        }
        
        # Delimiter mapping
        self.delimiters = {
            '(': TokenType.LEFT_PAREN,
            ')': TokenType.RIGHT_PAREN,
            '{': TokenType.LEFT_BRACE,
            '}': TokenType.RIGHT_BRACE,
            '[': TokenType.LEFT_BRACKET,
            ']': TokenType.RIGHT_BRACKET,
            ';': TokenType.SEMICOLON,
            ',': TokenType.COMMA,
            '.': TokenType.DOT,
            ':': TokenType.COLON,
            '->': TokenType.ARROW,
            '|': TokenType.PIPE
        }
    
    def tokenize(self) -> List[Token]:
        """
        Tokenize source code.
        
        Returns:
            List of tokens
        """
        self.tokens = []
        self.position = 0
        self.line = 1
        self.column = 1
        
        while self.position < len(self.source):
            self._skip_whitespace()
            
            if self.position >= len(self.source):
                break
            
            # Try to match different token types
            if self._match_identifier():
                continue
            elif self._match_number():
                continue
            elif self._match_string():
                continue
            elif self._match_operator():
                continue
            elif self._match_delimiter():
                continue
            else:
                # Unknown character
                char = self.source[self.position]
                self._add_token(TokenType.ERROR, char)
                self.position += 1
                self.column += 1
        
        # Add EOF token
        self._add_token(TokenType.EOF, "")
        
        return self.tokens
    
    def _skip_whitespace(self) -> None:
        """Skip whitespace characters."""
        while self.position < len(self.source):
            char = self.source[self.position]
            
            if char == ' ' or char == '\t':
                self.position += 1
                self.column += 1
            elif char == '\n':
                self.position += 1
                self.line += 1
                self.column = 1
            elif char == '\r':
                self.position += 1
                if self.position < len(self.source) and self.source[self.position] == '\n':
                    self.position += 1
                self.line += 1
                self.column = 1
            else:
                break
    
    def _match_identifier(self) -> bool:
        """Match identifier or keyword."""
        if not self._is_alpha(self.source[self.position]):
            return False
        
        start = self.position
        while (self.position < len(self.source) and 
               (self._is_alpha(self.source[self.position]) or 
                self._is_digit(self.source[self.position]) or 
                self.source[self.position] == '_')):
            self.position += 1
            self.column += 1
        
        value = self.source[start:self.position]
        
        # Check if it's a keyword
        if value in self.keywords:
            self._add_token(self.keywords[value], value)
        else:
            self._add_token(TokenType.IDENTIFIER, value)
        
        return True
    
    def _match_number(self) -> bool:
        """Match number literal."""
        if not self._is_digit(self.source[self.position]):
            return False
        
        start = self.position
        is_float = False
        
        # Integer part
        while (self.position < len(self.source) and 
               self._is_digit(self.source[self.position])):
            self.position += 1
            self.column += 1
        
        # Decimal part
        if (self.position < len(self.source) and 
            self.source[self.position] == '.'):
            is_float = True
            self.position += 1
            self.column += 1
            
            while (self.position < len(self.source) and 
                   self._is_digit(self.source[self.position])):
                self.position += 1
                self.column += 1
        
        value = self.source[start:self.position]
        
        if is_float:
            self._add_token(TokenType.FLOAT, value)
        else:
            self._add_token(TokenType.INTEGER, value)
        
        return True
    
    def _match_string(self) -> bool:
        """Match string literal."""
        if self.source[self.position] != '"':
            return False
        
        start = self.position
        self.position += 1
        self.column += 1
        
        while (self.position < len(self.source) and 
               self.source[self.position] != '"'):
            if self.source[self.position] == '\\':
                self.position += 1
                self.column += 1
            self.position += 1
            self.column += 1
        
        if self.position < len(self.source):
            self.position += 1
            self.column += 1
        
        value = self.source[start:self.position]
        self._add_token(TokenType.STRING, value)
        
        return True
    
    def _match_operator(self) -> bool:
        """Match operator."""
        for op in sorted(self.operators.keys(), key=len, reverse=True):
            if self.source[self.position:].startswith(op):
                self._add_token(self.operators[op], op)
                self.position += len(op)
                self.column += len(op)
                return True
        
        return False
    
    def _match_delimiter(self) -> bool:
        """Match delimiter."""
        for delim in sorted(self.delimiters.keys(), key=len, reverse=True):
            if self.source[self.position:].startswith(delim):
                self._add_token(self.delimiters[delim], delim)
                self.position += len(delim)
                self.column += len(delim)
                return True
        
        return False
    
    def _is_alpha(self, char: str) -> bool:
        """Check if character is alphabetic."""
        return char.isalpha()
    
    def _is_digit(self, char: str) -> bool:
        """Check if character is digit."""
        return char.isdigit()
    
    def _add_token(self, type: TokenType, value: str) -> None:
        """Add token to list."""
        token = Token(type, value, self.line, self.column)
        self.tokens.append(token)


class ASTNode:
    """
    AST Node - Base class for Abstract Syntax Tree nodes.
    
    Provides base functionality for AST nodes.
    """
    
    def __init__(self, node_type: str):
        """
        Initialize AST node.
        
        Args:
            node_type: Type of AST node
        """
        self.node_type = node_type
        self.children = []
        self.value = None
        self.line = 0
        self.column = 0
    
    def add_child(self, child: 'ASTNode') -> None:
        """Add child node."""
        self.children.append(child)
    
    def get_child(self, index: int) -> Optional['ASTNode']:
        """Get child node by index."""
        if 0 <= index < len(self.children):
            return self.children[index]
        return None
    
    def __str__(self) -> str:
        """Get string representation."""
        return f"{self.node_type}({self.value})"
    
    def __repr__(self) -> str:
        """Get representation."""
        return self.__str__()


class Lambda3Parser:
    """
    Lambda³ Parser - Parses Lambda³ source code into AST.
    
    Provides syntactic analysis for the Lambda³ programming language.
    """
    
    def __init__(self, tokens: List[Token]):
        """
        Initialize parser.
        
        Args:
            tokens: List of tokens to parse
        """
        self.tokens = tokens
        self.position = 0
        self.current_token = None
        
        # Initialize parser
        self._advance()
    
    def parse(self) -> ASTNode:
        """
        Parse tokens into AST.
            
        Returns:
            Root AST node
        """
        return self._parse_program()
    
    def _advance(self) -> None:
        """Advance to next token."""
        if self.position < len(self.tokens):
            self.current_token = self.tokens[self.position]
            self.position += 1
        else:
            self.current_token = None
    
    def _expect(self, expected_type: TokenType) -> Token:
        """Expect token of specific type."""
        if self.current_token and self.current_token.type == expected_type:
            token = self.current_token
            self._advance()
            return token
        else:
            raise SyntaxError(f"Expected {expected_type.value}, got {self.current_token.type.value if self.current_token else 'EOF'}")
    
    def _parse_program(self) -> ASTNode:
        """Parse program."""
        program = ASTNode("Program")
        
        while self.current_token and self.current_token.type != TokenType.EOF:
            if self.current_token.type == TokenType.LET:
                program.add_child(self._parse_let())
            elif self.current_token.type == TokenType.FUN:
                program.add_child(self._parse_function())
            else:
                program.add_child(self._parse_expression())
        
        return program
    
    def _parse_let(self) -> ASTNode:
        """Parse let expression."""
        self._expect(TokenType.LET)
        
        let_node = ASTNode("Let")
        
        # Parse variable name
        if self.current_token and self.current_token.type == TokenType.IDENTIFIER:
            let_node.value = self.current_token.value
            self._advance()
        else:
            raise SyntaxError("Expected identifier in let expression")
        
        # Parse equals
        self._expect(TokenType.EQUALS)
        
        # Parse expression
        let_node.add_child(self._parse_expression())
        
        # Parse in
        if self.current_token and self.current_token.type == TokenType.IN:
            self._advance()
            let_node.add_child(self._parse_expression())
        
        return let_node
    
    def _parse_function(self) -> ASTNode:
        """Parse function definition."""
        self._expect(TokenType.FUN)
        
        fun_node = ASTNode("Function")
        
        # Parse function name
        if self.current_token and self.current_token.type == TokenType.IDENTIFIER:
            fun_node.value = self.current_token.value
            self._advance()
        else:
            raise SyntaxError("Expected function name")
        
        # Parse parameters
        if self.current_token and self.current_token.type == TokenType.LEFT_PAREN:
            self._advance()
            params = []
            
            while (self.current_token and 
                   self.current_token.type != TokenType.RIGHT_PAREN):
                if self.current_token.type == TokenType.IDENTIFIER:
                    params.append(self.current_token.value)
                    self._advance()
                
                if (self.current_token and 
                    self.current_token.type == TokenType.COMMA):
                    self._advance()
            
            self._expect(TokenType.RIGHT_PAREN)
            fun_node.children = params
        
        # Parse arrow
        self._expect(TokenType.ARROW)
        
        # Parse body
        fun_node.add_child(self._parse_expression())
        
        return fun_node
    
    def _parse_expression(self) -> ASTNode:
        """Parse expression."""
        return self._parse_or_expression()
    
    def _parse_or_expression(self) -> ASTNode:
        """Parse OR expression."""
        left = self._parse_and_expression()
        
        while (self.current_token and 
               self.current_token.type == TokenType.OR_OP):
            self._advance()
            right = self._parse_and_expression()
            
            or_node = ASTNode("Or")
            or_node.add_child(left)
            or_node.add_child(right)
            left = or_node
        
        return left
    
    def _parse_and_expression(self) -> ASTNode:
        """Parse AND expression."""
        left = self._parse_equality_expression()
        
        while (self.current_token and 
               self.current_token.type == TokenType.AND_OP):
            self._advance()
            right = self._parse_equality_expression()
            
            and_node = ASTNode("And")
            and_node.add_child(left)
            and_node.add_child(right)
            left = and_node
        
        return left
    
    def _parse_equality_expression(self) -> ASTNode:
        """Parse equality expression."""
        left = self._parse_relational_expression()
        
        while (self.current_token and 
               self.current_token.type in [TokenType.EQUALS, TokenType.NOT_EQUALS]):
            op = self.current_token.type
            self._advance()
            right = self._parse_relational_expression()
            
            eq_node = ASTNode("Equality")
            eq_node.value = op.value
            eq_node.add_child(left)
            eq_node.add_child(right)
            left = eq_node
        
        return left
    
    def _parse_relational_expression(self) -> ASTNode:
        """Parse relational expression."""
        left = self._parse_additive_expression()
        
        while (self.current_token and 
               self.current_token.type in [TokenType.LESS, TokenType.LESS_EQUAL, 
                                          TokenType.GREATER, TokenType.GREATER_EQUAL]):
            op = self.current_token.type
            self._advance()
            right = self._parse_additive_expression()
            
            rel_node = ASTNode("Relational")
            rel_node.value = op.value
            rel_node.add_child(left)
            rel_node.add_child(right)
            left = rel_node
        
        return left
    
    def _parse_additive_expression(self) -> ASTNode:
        """Parse additive expression."""
        left = self._parse_multiplicative_expression()
        
        while (self.current_token and 
               self.current_token.type in [TokenType.PLUS, TokenType.MINUS]):
            op = self.current_token.type
            self._advance()
            right = self._parse_multiplicative_expression()
            
            add_node = ASTNode("Additive")
            add_node.value = op.value
            add_node.add_child(left)
            add_node.add_child(right)
            left = add_node
        
        return left
    
    def _parse_multiplicative_expression(self) -> ASTNode:
        """Parse multiplicative expression."""
        left = self._parse_unary_expression()
        
        while (self.current_token and 
               self.current_token.type in [TokenType.MULTIPLY, TokenType.DIVIDE, TokenType.MODULO]):
            op = self.current_token.type
            self._advance()
            right = self._parse_unary_expression()
            
            mul_node = ASTNode("Multiplicative")
            mul_node.value = op.value
            mul_node.add_child(left)
            mul_node.add_child(right)
            left = mul_node
        
        return left
    
    def _parse_unary_expression(self) -> ASTNode:
        """Parse unary expression."""
        if (self.current_token and 
            self.current_token.type in [TokenType.MINUS, TokenType.NOT_OP]):
            op = self.current_token.type
            self._advance()
            operand = self._parse_primary_expression()
            
            unary_node = ASTNode("Unary")
            unary_node.value = op.value
            unary_node.add_child(operand)
            return unary_node
        
        return self._parse_primary_expression()
    
    def _parse_primary_expression(self) -> ASTNode:
        """Parse primary expression."""
        if not self.current_token:
            raise SyntaxError("Unexpected end of input")
        
        if self.current_token.type == TokenType.LEFT_PAREN:
            self._advance()
            expr = self._parse_expression()
            self._expect(TokenType.RIGHT_PAREN)
            return expr
        
        elif self.current_token.type == TokenType.INTEGER:
            value = self.current_token.value
            self._advance()
            
            int_node = ASTNode("Integer")
            int_node.value = int(value)
            return int_node
        
        elif self.current_token.type == TokenType.FLOAT:
            value = self.current_token.value
            self._advance()
            
            float_node = ASTNode("Float")
            float_node.value = float(value)
            return float_node
        
        elif self.current_token.type == TokenType.STRING:
            value = self.current_token.value
            self._advance()
            
            str_node = ASTNode("String")
            str_node.value = value[1:-1]  # Remove quotes
            return str_node
        
        elif self.current_token.type == TokenType.TRUE:
            self._advance()
            bool_node = ASTNode("Boolean")
            bool_node.value = True
            return bool_node
        
        elif self.current_token.type == TokenType.FALSE:
            self._advance()
            bool_node = ASTNode("Boolean")
            bool_node.value = False
            return bool_node
        
        elif self.current_token.type == TokenType.IDENTIFIER:
            value = self.current_token.value
            self._advance()
            
            # Check if it's a function call
            if (self.current_token and 
                self.current_token.type == TokenType.LEFT_PAREN):
                self._advance()
                args = []
                
                while (self.current_token and 
                       self.current_token.type != TokenType.RIGHT_PAREN):
                    args.append(self._parse_expression())
                    
                    if (self.current_token and 
                        self.current_token.type == TokenType.COMMA):
                        self._advance()
                
                self._expect(TokenType.RIGHT_PAREN)
                
                call_node = ASTNode("FunctionCall")
                call_node.value = value
                call_node.children = args
                return call_node
            
            else:
                var_node = ASTNode("Variable")
                var_node.value = value
                return var_node
        
        else:
            raise SyntaxError(f"Unexpected token: {self.current_token.type.value}")


class Lambda3Compiler:
    """
    Lambda³ Compiler - Compiles Lambda³ source code to T3-ISA.
    
    Provides complete compilation pipeline for the Lambda³ programming language.
    """
    
    def __init__(self):
        """Initialize compiler."""
        self.symbol_table = {}
        self.label_counter = 0
        self.temp_counter = 0
        
        # Compilation statistics
        self.stats = {
            'files_compiled': 0,
            'errors': 0,
            'warnings': 0,
            'instructions_generated': 0
        }
    
    def compile(self, source: str) -> List[str]:
        """
        Compile Lambda³ source code.
        
        Args:
            source: Source code to compile
            
        Returns:
            List of T3-ISA instructions
        """
        try:
            # Lexical analysis
            lexer = Lambda3Lexer(source)
            tokens = lexer.tokenize()
            
            # Syntactic analysis
            parser = Lambda3Parser(tokens)
            ast = parser.parse()
            
            # Code generation
            instructions = self._generate_code(ast)
            
            self.stats['files_compiled'] += 1
            self.stats['instructions_generated'] += len(instructions)
            
            return instructions
            
        except Exception as e:
            self.stats['errors'] += 1
            print(f"Compilation error: {e}")
            return []
    
    def _generate_code(self, ast: ASTNode) -> List[str]:
        """Generate T3-ISA code from AST."""
        instructions = []
        
        if ast.node_type == "Program":
            for child in ast.children:
                instructions.extend(self._generate_code(child))
        
        elif ast.node_type == "Let":
            # Generate code for let expression
            instructions.extend(self._generate_let(ast))
        
        elif ast.node_type == "Function":
            # Generate code for function
            instructions.extend(self._generate_function(ast))
        
        elif ast.node_type == "Integer":
            # Generate code for integer literal
            instructions.extend(self._generate_integer(ast))
        
        elif ast.node_type == "Float":
            # Generate code for float literal
            instructions.extend(self._generate_float(ast))
        
        elif ast.node_type == "String":
            # Generate code for string literal
            instructions.extend(self._generate_string(ast))
        
        elif ast.node_type == "Boolean":
            # Generate code for boolean literal
            instructions.extend(self._generate_boolean(ast))
        
        elif ast.node_type == "Variable":
            # Generate code for variable reference
            instructions.extend(self._generate_variable(ast))
        
        elif ast.node_type == "FunctionCall":
            # Generate code for function call
            instructions.extend(self._generate_function_call(ast))
        
        elif ast.node_type == "Additive":
            # Generate code for addition/subtraction
            instructions.extend(self._generate_additive(ast))
        
        elif ast.node_type == "Multiplicative":
            # Generate code for multiplication/division
            instructions.extend(self._generate_multiplicative(ast))
        
        elif ast.node_type == "Equality":
            # Generate code for equality comparison
            instructions.extend(self._generate_equality(ast))
        
        elif ast.node_type == "Relational":
            # Generate code for relational comparison
            instructions.extend(self._generate_relational(ast))
        
        elif ast.node_type == "And":
            # Generate code for logical AND
            instructions.extend(self._generate_and(ast))
        
        elif ast.node_type == "Or":
            # Generate code for logical OR
            instructions.extend(self._generate_or(ast))
        
        elif ast.node_type == "Unary":
            # Generate code for unary operation
            instructions.extend(self._generate_unary(ast))
        
        return instructions
    
    def _generate_let(self, ast: ASTNode) -> List[str]:
        """Generate code for let expression."""
        instructions = []
        
        # Generate code for expression
        instructions.extend(self._generate_code(ast.get_child(0)))
        
        # Store in variable
        var_name = ast.value
        if var_name not in self.symbol_table:
            self.symbol_table[var_name] = self._get_temp_register()
        
        instructions.append(f"STORE {self.symbol_table[var_name]}")
        
        # Generate code for in expression
        if len(ast.children) > 1:
            instructions.extend(self._generate_code(ast.get_child(1)))
        
        return instructions
    
    def _generate_function(self, ast: ASTNode) -> List[str]:
        """Generate code for function definition."""
        instructions = []
        
        # Function label
        func_name = ast.value
        func_label = f"func_{func_name}"
        instructions.append(f"{func_label}:")
        
        # Function parameters
        params = ast.children[:-1]  # All but last child (body)
        for param in params:
            if param not in self.symbol_table:
                self.symbol_table[param] = self._get_temp_register()
        
        # Function body
        instructions.extend(self._generate_code(ast.get_child(-1)))
        
        # Return
        instructions.append("RET")
        
        return instructions
    
    def _generate_integer(self, ast: ASTNode) -> List[str]:
        """Generate code for integer literal."""
        instructions = []
        value = ast.value
        
        # Load immediate value
        instructions.append(f"LOADI {value}")
        
        return instructions
    
    def _generate_float(self, ast: ASTNode) -> List[str]:
        """Generate code for float literal."""
        instructions = []
        value = ast.value
        
        # Convert float to integer for now
        int_value = int(value)
        instructions.append(f"LOADI {int_value}")
        
        return instructions
    
    def _generate_string(self, ast: ASTNode) -> List[str]:
        """Generate code for string literal."""
        instructions = []
        value = ast.value
        
        # For now, just load string length
        instructions.append(f"LOADI {len(value)}")
        
        return instructions
    
    def _generate_boolean(self, ast: ASTNode) -> List[str]:
        """Generate code for boolean literal."""
        instructions = []
        value = ast.value
        
        # Load boolean value (1 for true, 0 for false)
        bool_value = 1 if value else 0
        instructions.append(f"LOADI {bool_value}")
        
        return instructions
    
    def _generate_variable(self, ast: ASTNode) -> List[str]:
        """Generate code for variable reference."""
        instructions = []
        var_name = ast.value
        
        if var_name in self.symbol_table:
            # Load variable value
            instructions.append(f"LOAD {self.symbol_table[var_name]}")
        else:
            # Undefined variable
            instructions.append(f"LOADI 0")  # Default to 0
        
        return instructions
    
    def _generate_function_call(self, ast: ASTNode) -> List[str]:
        """Generate code for function call."""
        instructions = []
        func_name = ast.value
        args = ast.children
        
        # Push arguments
        for arg in args:
            instructions.extend(self._generate_code(arg))
            instructions.append("PUSH")
        
        # Call function
        func_label = f"func_{func_name}"
        instructions.append(f"CALL {func_label}")
        
        return instructions
    
    def _generate_additive(self, ast: ASTNode) -> List[str]:
        """Generate code for addition/subtraction."""
        instructions = []
        op = ast.value
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform operation
        if op == "+":
            instructions.append("ADD")
        elif op == "-":
            instructions.append("SUB")
        
        return instructions
    
    def _generate_multiplicative(self, ast: ASTNode) -> List[str]:
        """Generate code for multiplication/division."""
        instructions = []
        op = ast.value
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform operation
        if op == "*":
            instructions.append("MUL")
        elif op == "/":
            instructions.append("DIV")
        elif op == "%":
            instructions.append("MOD")
        
        return instructions
    
    def _generate_equality(self, ast: ASTNode) -> List[str]:
        """Generate code for equality comparison."""
        instructions = []
        op = ast.value
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform comparison
        if op == "=":
            instructions.append("CMP")
        elif op == "!=":
            instructions.append("CMP")
            instructions.append("NOT")
        
        return instructions
    
    def _generate_relational(self, ast: ASTNode) -> List[str]:
        """Generate code for relational comparison."""
        instructions = []
        op = ast.value
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform comparison
        if op == "<":
            instructions.append("CMP")
        elif op == "<=":
            instructions.append("CMP")
        elif op == ">":
            instructions.append("CMP")
        elif op == ">=":
            instructions.append("CMP")
        
        return instructions
    
    def _generate_and(self, ast: ASTNode) -> List[str]:
        """Generate code for logical AND."""
        instructions = []
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform AND operation
        instructions.append("AND")
        
        return instructions
    
    def _generate_or(self, ast: ASTNode) -> List[str]:
        """Generate code for logical OR."""
        instructions = []
        
        # Generate code for operands
        instructions.extend(self._generate_code(ast.get_child(0)))
        instructions.append("PUSH")
        instructions.extend(self._generate_code(ast.get_child(1)))
        
        # Perform OR operation
        instructions.append("OR")
        
        return instructions
    
    def _generate_unary(self, ast: ASTNode) -> List[str]:
        """Generate code for unary operation."""
        instructions = []
        op = ast.value
        
        # Generate code for operand
        instructions.extend(self._generate_code(ast.get_child(0)))
        
        # Perform unary operation
        if op == "-":
            instructions.append("NEG")
        elif op == "!":
            instructions.append("NOT")
        
        return instructions
    
    def _get_temp_register(self) -> str:
        """Get temporary register name."""
        self.temp_counter += 1
        return f"R{self.temp_counter}"
    
    def _get_label(self) -> str:
        """Get unique label name."""
        self.label_counter += 1
        return f"label_{self.label_counter}"
    
    def get_stats(self) -> Dict[str, Any]:
        """Get compiler statistics."""
        return {
            'symbol_table_size': len(self.symbol_table),
            'temp_registers_used': self.temp_counter,
            'labels_generated': self.label_counter,
            **self.stats
        }
    
    def reset(self) -> None:
        """Reset compiler state."""
        self.symbol_table.clear()
        self.label_counter = 0
        self.temp_counter = 0
        
        for key in self.stats:
            self.stats[key] = 0