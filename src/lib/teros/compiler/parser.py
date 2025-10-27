"""
Lambda³ Parser - Syntax analysis for Lambda³ language.

This module provides the parser for the Lambda³ language,
building an Abstract Syntax Tree (AST) from tokens.
"""

from typing import List, Optional, Dict, Any, Union
from .lexer import Token, TokenType


class ASTNodeType:
    """AST node types for Lambda³ language."""
    
    # Expressions
    LITERAL = "LITERAL"
    IDENTIFIER = "IDENTIFIER"
    BINARY_OP = "BINARY_OP"
    UNARY_OP = "UNARY_OP"
    FUNCTION_CALL = "FUNCTION_CALL"
    LAMBDA = "LAMBDA"
    IF_EXPR = "IF_EXPR"
    MATCH_EXPR = "MATCH_EXPR"
    CASE = "CASE"
    
    # Statements
    LET_STMT = "LET_STMT"
    FUNCTION_DECL = "FUNCTION_DECL"
    RETURN_STMT = "RETURN_STMT"
    EXPR_STMT = "EXPR_STMT"
    
    # Program
    PROGRAM = "PROGRAM"
    BLOCK = "BLOCK"


class ASTNode:
    """Base class for AST nodes."""
    
    def __init__(self, node_type: str, value: Any = None, children: List['ASTNode'] = None):
        """
        Initialize an AST node.
        
        Args:
            node_type: Type of the node
            value: Node value
            children: Child nodes
        """
        self.node_type = node_type
        self.value = value
        self.children = children or []
        self.line = 0
        self.column = 0
    
    def add_child(self, child: 'ASTNode') -> None:
        """Add a child node."""
        self.children.append(child)
    
    def get_child(self, index: int) -> Optional['ASTNode']:
        """Get a child node by index."""
        if 0 <= index < len(self.children):
            return self.children[index]
        return None
    
    def __str__(self) -> str:
        """String representation."""
        return f"ASTNode({self.node_type}, {self.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"ASTNode({self.node_type}, {self.value}, {len(self.children)} children)"


class LiteralNode(ASTNode):
    """AST node for literals."""
    
    def __init__(self, value: Any, literal_type: str):
        super().__init__(ASTNodeType.LITERAL, value)
        self.literal_type = literal_type


class IdentifierNode(ASTNode):
    """AST node for identifiers."""
    
    def __init__(self, name: str):
        super().__init__(ASTNodeType.IDENTIFIER, name)


class BinaryOpNode(ASTNode):
    """AST node for binary operations."""
    
    def __init__(self, operator: str, left: ASTNode, right: ASTNode):
        super().__init__(ASTNodeType.BINARY_OP, operator, [left, right])
        self.operator = operator
        self.left = left
        self.right = right


class UnaryOpNode(ASTNode):
    """AST node for unary operations."""
    
    def __init__(self, operator: str, operand: ASTNode):
        super().__init__(ASTNodeType.UNARY_OP, operator, [operand])
        self.operator = operator
        self.operand = operand


class FunctionCallNode(ASTNode):
    """AST node for function calls."""
    
    def __init__(self, function: ASTNode, arguments: List[ASTNode]):
        super().__init__(ASTNodeType.FUNCTION_CALL, function, arguments)
        self.function = function
        self.arguments = arguments


class LambdaNode(ASTNode):
    """AST node for lambda expressions."""
    
    def __init__(self, parameters: List[str], body: ASTNode):
        super().__init__(ASTNodeType.LAMBDA, parameters, [body])
        self.parameters = parameters
        self.body = body


class IfExprNode(ASTNode):
    """AST node for if expressions."""
    
    def __init__(self, condition: ASTNode, then_expr: ASTNode, else_expr: Optional[ASTNode] = None):
        children = [condition, then_expr]
        if else_expr:
            children.append(else_expr)
        super().__init__(ASTNodeType.IF_EXPR, None, children)
        self.condition = condition
        self.then_expr = then_expr
        self.else_expr = else_expr


class MatchExprNode(ASTNode):
    """AST node for match expressions."""
    
    def __init__(self, expression: ASTNode, cases: List['CaseNode']):
        super().__init__(ASTNodeType.MATCH_EXPR, expression, cases)
        self.expression = expression
        self.cases = cases


class CaseNode(ASTNode):
    """AST node for case expressions."""
    
    def __init__(self, pattern: ASTNode, body: ASTNode):
        super().__init__(ASTNodeType.CASE, pattern, [body])
        self.pattern = pattern
        self.body = body


class LetStmtNode(ASTNode):
    """AST node for let statements."""
    
    def __init__(self, variable: str, value: ASTNode, body: Optional[ASTNode] = None):
        children = [value]
        if body:
            children.append(body)
        super().__init__(ASTNodeType.LET_STMT, variable, children)
        self.variable = variable
        self.value = value
        self.body = body


class FunctionDeclNode(ASTNode):
    """AST node for function declarations."""
    
    def __init__(self, name: str, parameters: List[str], body: ASTNode):
        super().__init__(ASTNodeType.FUNCTION_DECL, name, [body])
        self.name = name
        self.parameters = parameters
        self.body = body


class ReturnStmtNode(ASTNode):
    """AST node for return statements."""
    
    def __init__(self, expression: Optional[ASTNode] = None):
        super().__init__(ASTNodeType.RETURN_STMT, None, [expression] if expression else [])
        self.expression = expression


class ExprStmtNode(ASTNode):
    """AST node for expression statements."""
    
    def __init__(self, expression: ASTNode):
        super().__init__(ASTNodeType.EXPR_STMT, None, [expression])
        self.expression = expression


class ProgramNode(ASTNode):
    """AST node for programs."""
    
    def __init__(self, statements: List[ASTNode]):
        super().__init__(ASTNodeType.PROGRAM, None, statements)
        self.statements = statements


class BlockNode(ASTNode):
    """AST node for blocks."""
    
    def __init__(self, statements: List[ASTNode]):
        super().__init__(ASTNodeType.BLOCK, None, statements)
        self.statements = statements


class Lambda3Parser:
    """
    Parser for Lambda³ language.
    
    Builds an Abstract Syntax Tree (AST) from tokens using
    recursive descent parsing.
    """
    
    def __init__(self):
        """Initialize the parser."""
        self.tokens = []
        self.current = 0
        self.errors = []
    
    def parse(self, tokens: List[Token]) -> ProgramNode:
        """
        Parse tokens into an AST.
        
        Args:
            tokens: List of tokens
            
        Returns:
            Program AST node
        """
        self.tokens = tokens
        self.current = 0
        self.errors.clear()
        
        statements = []
        
        while not self.is_at_end():
            try:
                stmt = self.statement()
                if stmt:
                    statements.append(stmt)
            except ParseError as e:
                self.errors.append(str(e))
                self.synchronize()
        
        return ProgramNode(statements)
    
    def statement(self) -> Optional[ASTNode]:
        """Parse a statement."""
        if self.match(TokenType.LET):
            return self.let_statement()
        elif self.match(TokenType.FUN):
            return self.function_declaration()
        elif self.match(TokenType.RETURN):
            return self.return_statement()
        else:
            return self.expression_statement()
    
    def let_statement(self) -> LetStmtNode:
        """Parse a let statement."""
        # Parse variable name
        name = self.consume(TokenType.IDENTIFIER, "Expected variable name").value
        
        # Parse assignment
        self.consume(TokenType.ASSIGN, "Expected '=' after variable name")
        
        # Parse value
        value = self.expression()
        
        # Parse optional body
        body = None
        if self.match(TokenType.IN):
            body = self.expression()
        
        return LetStmtNode(name, value, body)
    
    def function_declaration(self) -> FunctionDeclNode:
        """Parse a function declaration."""
        # Parse function name
        name = self.consume(TokenType.IDENTIFIER, "Expected function name").value
        
        # Parse parameters
        parameters = []
        self.consume(TokenType.LEFT_PAREN, "Expected '(' after function name")
        
        if not self.check(TokenType.RIGHT_PAREN):
            parameters.append(self.consume(TokenType.IDENTIFIER, "Expected parameter name").value)
            while self.match(TokenType.COMMA):
                parameters.append(self.consume(TokenType.IDENTIFIER, "Expected parameter name").value)
        
        self.consume(TokenType.RIGHT_PAREN, "Expected ')' after parameters")
        
        # Parse body
        self.consume(TokenType.ASSIGN, "Expected '=' after function declaration")
        body = self.expression()
        
        return FunctionDeclNode(name, parameters, body)
    
    def return_statement(self) -> ReturnStmtNode:
        """Parse a return statement."""
        expression = None
        if not self.check(TokenType.SEMICOLON):
            expression = self.expression()
        
        return ReturnStmtNode(expression)
    
    def expression_statement(self) -> ExprStmtNode:
        """Parse an expression statement."""
        expr = self.expression()
        return ExprStmtNode(expr)
    
    def expression(self) -> ASTNode:
        """Parse an expression."""
        return self.ternary_expression()
    
    def ternary_expression(self) -> ASTNode:
        """Parse ternary expressions (if-then-else)."""
        expr = self.logical_or()
        
        if self.match(TokenType.IF):
            condition = self.expression()
            self.consume(TokenType.THEN, "Expected 'then' after condition")
            then_expr = self.expression()
            else_expr = None
            if self.match(TokenType.ELSE):
                else_expr = self.expression()
            return IfExprNode(condition, then_expr, else_expr)
        
        return expr
    
    def logical_or(self) -> ASTNode:
        """Parse logical OR expressions."""
        expr = self.logical_and()
        
        while self.match(TokenType.OR_OP):
            operator = self.previous()
            right = self.logical_and()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def logical_and(self) -> ASTNode:
        """Parse logical AND expressions."""
        expr = self.equality()
        
        while self.match(TokenType.AND_OP):
            operator = self.previous()
            right = self.equality()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def equality(self) -> ASTNode:
        """Parse equality expressions."""
        expr = self.comparison()
        
        while self.match(TokenType.EQUAL, TokenType.NOT_EQUAL):
            operator = self.previous()
            right = self.comparison()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def comparison(self) -> ASTNode:
        """Parse comparison expressions."""
        expr = self.term()
        
        while self.match(TokenType.LESS, TokenType.LESS_EQUAL, 
                        TokenType.GREATER, TokenType.GREATER_EQUAL):
            operator = self.previous()
            right = self.term()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def term(self) -> ASTNode:
        """Parse addition and subtraction."""
        expr = self.factor()
        
        while self.match(TokenType.PLUS, TokenType.MINUS):
            operator = self.previous()
            right = self.factor()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def factor(self) -> ASTNode:
        """Parse multiplication and division."""
        expr = self.unary()
        
        while self.match(TokenType.MULTIPLY, TokenType.DIVIDE, TokenType.MODULO):
            operator = self.previous()
            right = self.unary()
            expr = BinaryOpNode(operator.value, expr, right)
        
        return expr
    
    def unary(self) -> ASTNode:
        """Parse unary expressions."""
        if self.match(TokenType.MINUS, TokenType.NOT_OP, TokenType.TERNARY_NOT):
            operator = self.previous()
            right = self.unary()
            return UnaryOpNode(operator.value, right)
        
        return self.call()
    
    def call(self) -> ASTNode:
        """Parse function calls."""
        expr = self.primary()
        
        while True:
            if self.match(TokenType.LEFT_PAREN):
                expr = self.finish_call(expr)
            else:
                break
        
        return expr
    
    def finish_call(self, callee: ASTNode) -> FunctionCallNode:
        """Finish parsing a function call."""
        arguments = []
        
        if not self.check(TokenType.RIGHT_PAREN):
            arguments.append(self.expression())
            while self.match(TokenType.COMMA):
                arguments.append(self.expression())
        
        self.consume(TokenType.RIGHT_PAREN, "Expected ')' after arguments")
        return FunctionCallNode(callee, arguments)
    
    def primary(self) -> ASTNode:
        """Parse primary expressions."""
        if self.match(TokenType.TRUE):
            return LiteralNode(True, "boolean")
        elif self.match(TokenType.FALSE):
            return LiteralNode(False, "boolean")
        elif self.match(TokenType.NEGATIVE):
            return LiteralNode(-1, "trit")
        elif self.match(TokenType.NEUTRAL):
            return LiteralNode(0, "trit")
        elif self.match(TokenType.POSITIVE):
            return LiteralNode(1, "trit")
        elif self.match(TokenType.INTEGER):
            return LiteralNode(int(self.previous().value), "integer")
        elif self.match(TokenType.FLOAT):
            return LiteralNode(float(self.previous().value), "float")
        elif self.match(TokenType.STRING):
            value = self.previous().value
            return LiteralNode(value[1:-1], "string")  # Remove quotes
        elif self.match(TokenType.TRITARRAY):
            value = self.previous().value
            return LiteralNode(value, "tritarray")
        elif self.match(TokenType.IDENTIFIER):
            return IdentifierNode(self.previous().value)
        elif self.match(TokenType.LEFT_PAREN):
            expr = self.expression()
            self.consume(TokenType.RIGHT_PAREN, "Expected ')' after expression")
            return expr
        elif self.match(TokenType.LAMBDA):
            return self.lambda_expression()
        else:
            raise ParseError(f"Expected expression, got {self.peek().type.value}")
    
    def lambda_expression(self) -> LambdaNode:
        """Parse lambda expressions."""
        parameters = []
        
        if self.match(TokenType.LEFT_PAREN):
            if not self.check(TokenType.RIGHT_PAREN):
                parameters.append(self.consume(TokenType.IDENTIFIER, "Expected parameter name").value)
                while self.match(TokenType.COMMA):
                    parameters.append(self.consume(TokenType.IDENTIFIER, "Expected parameter name").value)
            self.consume(TokenType.RIGHT_PAREN, "Expected ')' after parameters")
        else:
            parameters.append(self.consume(TokenType.IDENTIFIER, "Expected parameter name").value)
        
        self.consume(TokenType.ARROW, "Expected '->' after lambda parameters")
        body = self.expression()
        
        return LambdaNode(parameters, body)
    
    # Helper methods
    def match(self, *token_types: TokenType) -> bool:
        """Check if current token matches any of the given types."""
        for token_type in token_types:
            if self.check(token_type):
                self.advance()
                return True
        return False
    
    def check(self, token_type: TokenType) -> bool:
        """Check if current token is of given type."""
        if self.is_at_end():
            return False
        return self.peek().type == token_type
    
    def advance(self) -> Token:
        """Advance to next token."""
        if not self.is_at_end():
            self.current += 1
        return self.previous()
    
    def is_at_end(self) -> bool:
        """Check if at end of tokens."""
        return self.peek().type == TokenType.EOF
    
    def peek(self) -> Token:
        """Get current token."""
        return self.tokens[self.current]
    
    def previous(self) -> Token:
        """Get previous token."""
        return self.tokens[self.current - 1]
    
    def consume(self, token_type: TokenType, message: str) -> Token:
        """Consume token of given type."""
        if self.check(token_type):
            return self.advance()
        raise ParseError(f"{message} at line {self.peek().line}, column {self.peek().column}")
    
    def synchronize(self) -> None:
        """Synchronize parser after error."""
        self.advance()
        
        while not self.is_at_end():
            if self.previous().type == TokenType.SEMICOLON:
                return
            
            if self.peek().type in [TokenType.LET, TokenType.FUN, TokenType.IF, TokenType.WHILE, TokenType.FOR, TokenType.RETURN]:
                return
            
            self.advance()
    
    def get_errors(self) -> List[str]:
        """Get parsing errors."""
        return self.errors.copy()
    
    def has_errors(self) -> bool:
        """Check if parsing has errors."""
        return len(self.errors) > 0
    
    def __str__(self) -> str:
        """String representation."""
        return "Lambda3Parser()"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Lambda3Parser(errors={len(self.errors)})"


class ParseError(Exception):
    """Exception raised during parsing."""
    pass
