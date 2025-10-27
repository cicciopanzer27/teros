"""
Lambda Calculus Parser
Recursive descent parser for lambda terms
"""

from typing import List, Optional

# Handle both package import and standalone execution
try:
    from .lexer import Token, TokenType, LambdaLexer
    from .lambda_parser import Var, Abs, App, LambdaTerm
except ImportError:
    from lexer import Token, TokenType, LambdaLexer
    from lambda_parser import Var, Abs, App, LambdaTerm


class ParseError(Exception):
    """Parse error with position info"""
    def __init__(self, message: str, token: Token):
        self.message = message
        self.token = token
        super().__init__(f"{message} at {token.line}:{token.column}")


class LambdaParser:
    """
    Recursive descent parser for lambda calculus
    
    Grammar:
        term   ::= lambda | app | atom
        lambda ::= (LAMBDA VAR DOT term)
        app    ::= atom atom+
        atom   ::= VAR | LPAREN term RPAREN
    """
    
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0
    
    def current_token(self) -> Token:
        """Get current token"""
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return self.tokens[-1]  # EOF
    
    def peek_token(self, offset: int = 1) -> Token:
        """Peek ahead"""
        pos = self.pos + offset
        if pos < len(self.tokens):
            return self.tokens[pos]
        return self.tokens[-1]  # EOF
    
    def advance(self):
        """Move to next token"""
        if self.pos < len(self.tokens) - 1:
            self.pos += 1
    
    def expect(self, token_type: TokenType) -> Token:
        """Expect specific token type"""
        token = self.current_token()
        if token.type != token_type:
            raise ParseError(
                f"Expected {token_type.name}, got {token.type.name}",
                token
            )
        self.advance()
        return token
    
    def parse_term(self) -> LambdaTerm:
        """
        Parse a term
        term ::= lambda | app | atom
        """
        token = self.current_token()
        
        # Lambda abstraction
        if token.type == TokenType.LAMBDA:
            return self.parse_lambda()
        
        # Application or atom
        return self.parse_app()
    
    def parse_lambda(self) -> Abs:
        """
        Parse lambda abstraction
        lambda ::= LAMBDA VAR DOT term
        """
        self.expect(TokenType.LAMBDA)
        
        # Get variable
        var_token = self.expect(TokenType.VAR)
        var_name = var_token.value
        
        # Convert variable name to ID
        if var_name.startswith('x') and len(var_name) > 1:
            var_id = int(var_name[1:])
        elif len(var_name) == 1:
            var_id = ord(var_name) - ord('a')
        else:
            var_id = 0
        
        self.expect(TokenType.DOT)
        
        # Parse body
        body = self.parse_term()
        
        return Abs(var=var_id, body=body)
    
    def parse_app(self) -> LambdaTerm:
        """
        Parse application or atom
        app ::= atom atom+
        """
        # Parse first atom
        left = self.parse_atom()
        
        # Check if there are more atoms (application)
        while self.current_token().type in [TokenType.VAR, TokenType.LPAREN, TokenType.LAMBDA]:
            right = self.parse_atom()
            left = App(func=left, arg=right)
        
        return left
    
    def parse_atom(self) -> LambdaTerm:
        """
        Parse atomic term
        atom ::= VAR | LPAREN term RPAREN
        """
        token = self.current_token()
        
        # Variable
        if token.type == TokenType.VAR:
            var_name = token.value
            self.advance()
            
            # Convert variable name to ID
            if var_name.startswith('x') and len(var_name) > 1:
                var_id = int(var_name[1:])
            elif len(var_name) == 1:
                var_id = ord(var_name) - ord('a')
            else:
                var_id = 0
            
            return Var(name=var_id)
        
        # Parenthesized term
        elif token.type == TokenType.LPAREN:
            self.expect(TokenType.LPAREN)
            term = self.parse_term()
            self.expect(TokenType.RPAREN)
            return term
        
        # Lambda (for cases like: x \y.y)
        elif token.type == TokenType.LAMBDA:
            return self.parse_lambda()
        
        else:
            raise ParseError(f"Unexpected token {token.type.name}", token)
    
    def parse(self) -> LambdaTerm:
        """
        Parse entire input
        """
        if not self.tokens or self.tokens[0].type == TokenType.EOF:
            raise ParseError("Empty input", self.tokens[0] if self.tokens else Token(TokenType.EOF, '', 0, 0))
        
        term = self.parse_term()
        
        # Expect EOF
        if self.current_token().type != TokenType.EOF:
            raise ParseError(
                f"Unexpected token after term: {self.current_token().type.name}",
                self.current_token()
            )
        
        return term


def parse(source: str) -> LambdaTerm:
    """
    Parse lambda term from string
    
    Args:
        source: Lambda term as string
        
    Returns:
        Parsed lambda term (AST)
        
    Raises:
        ParseError: If syntax is invalid
        
    Example:
        >>> term = parse("\\x.x")
        >>> isinstance(term, Abs)
        True
        >>> term = parse("(\\x.x) y")
        >>> isinstance(term, App)
        True
    """
    # Tokenize
    lexer = LambdaLexer(source)
    tokens = lexer.tokenize()
    
    # Parse
    parser = LambdaParser(tokens)
    return parser.parse()


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    import sys
    
    test_cases = [
        ("x", "Variable"),
        ("\\x.x", "Identity"),
        ("(\\x.x) y", "Application"),
        ("\\x.\\y.x", "Const"),
        ("\\f.\\x.f x", "Application in body"),
        ("(\\x.\\y.x) a b", "Multiple application"),
    ]
    
    print("Lambda Parser Test Cases:")
    print("=" * 60)
    
    all_passed = True
    
    for source, description in test_cases:
        try:
            print(f"\nTest: {description}")
            print(f"Input:  '{source}'")
            term = parse(source)
            print(f"Parsed: {term}")
            print("PASS")
        except ParseError as e:
            print(f"FAIL: {e}")
            all_passed = False
        except Exception as e:
            print(f"ERROR: {e}")
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)

