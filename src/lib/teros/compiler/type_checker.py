"""
Lambda³ Type Checker - Type checking for Lambda³ language.

This module provides type checking for the Lambda³ language,
ensuring type safety and correctness of expressions.
"""

from typing import List, Optional, Dict, Any, Union
from .parser import ASTNode, LiteralNode, IdentifierNode, BinaryOpNode, UnaryOpNode, FunctionCallNode, LambdaNode, IfExprNode, LetStmtNode, FunctionDeclNode


class TernaryType:
    """Ternary type system for Lambda³ language."""
    
    # Basic types
    INTEGER = "integer"
    FLOAT = "float"
    STRING = "string"
    BOOLEAN = "boolean"
    TRIT = "trit"
    TRITARRAY = "tritarray"
    VOID = "void"
    
    # Complex types
    FUNCTION = "function"
    ARRAY = "array"
    TUPLE = "tuple"
    RECORD = "record"
    
    # Special types
    UNKNOWN = "unknown"
    ERROR = "error"


class TypeInfo:
    """Type information for AST nodes."""
    
    def __init__(self, type_name: str, is_mutable: bool = False, 
                 element_type: Optional['TypeInfo'] = None,
                 parameter_types: Optional[List['TypeInfo']] = None,
                 return_type: Optional['TypeInfo'] = None):
        """
        Initialize type information.
        
        Args:
            type_name: Name of the type
            is_mutable: Whether the type is mutable
            element_type: Element type for arrays
            parameter_types: Parameter types for functions
            return_type: Return type for functions
        """
        self.type_name = type_name
        self.is_mutable = is_mutable
        self.element_type = element_type
        self.parameter_types = parameter_types or []
        self.return_type = return_type
    
    def is_function(self) -> bool:
        """Check if this is a function type."""
        return self.type_name == TernaryType.FUNCTION
    
    def is_array(self) -> bool:
        """Check if this is an array type."""
        return self.type_name == TernaryType.ARRAY
    
    def is_basic_type(self) -> bool:
        """Check if this is a basic type."""
        return self.type_name in [
            TernaryType.INTEGER, TernaryType.FLOAT, TernaryType.STRING,
            TernaryType.BOOLEAN, TernaryType.TRIT, TernaryType.TRITARRAY, TernaryType.VOID
        ]
    
    def is_compatible_with(self, other: 'TypeInfo') -> bool:
        """Check if this type is compatible with another type."""
        if self.type_name == other.type_name:
            return True
        
        # Numeric types are compatible
        if self.type_name in [TernaryType.INTEGER, TernaryType.FLOAT] and \
           other.type_name in [TernaryType.INTEGER, TernaryType.FLOAT]:
            return True
        
        # Trit and TritArray are compatible
        if self.type_name == TernaryType.TRIT and other.type_name == TernaryType.TRITARRAY:
            return True
        if self.type_name == TernaryType.TRITARRAY and other.type_name == TernaryType.TRIT:
            return True
        
        return False
    
    def __str__(self) -> str:
        """String representation."""
        if self.is_function():
            param_types = [str(t) for t in self.parameter_types]
            return_type = str(self.return_type) if self.return_type else "void"
            return f"({', '.join(param_types)}) -> {return_type}"
        elif self.is_array():
            element_type = str(self.element_type) if self.element_type else "unknown"
            return f"{element_type}[]"
        else:
            return self.type_name
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"TypeInfo({self.type_name}, mutable={self.is_mutable})"


class Symbol:
    """Symbol in the symbol table."""
    
    def __init__(self, name: str, type_info: TypeInfo, 
                 is_constant: bool = False, value: Any = None):
        """
        Initialize a symbol.
        
        Args:
            name: Symbol name
            type_info: Type information
            is_constant: Whether the symbol is constant
            value: Symbol value
        """
        self.name = name
        self.type_info = type_info
        self.is_constant = is_constant
        self.value = value
    
    def __str__(self) -> str:
        """String representation."""
        return f"Symbol({self.name}, {self.type_info})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Symbol({self.name}, {self.type_info}, constant={self.is_constant})"


class SymbolTable:
    """Symbol table for managing scopes and symbols."""
    
    def __init__(self, parent: Optional['SymbolTable'] = None):
        """
        Initialize symbol table.
        
        Args:
            parent: Parent symbol table
        """
        self.symbols = {}
        self.parent = parent
    
    def define(self, name: str, type_info: TypeInfo, 
               is_constant: bool = False, value: Any = None) -> Symbol:
        """
        Define a symbol.
        
        Args:
            name: Symbol name
            type_info: Type information
            is_constant: Whether the symbol is constant
            value: Symbol value
            
        Returns:
            Created symbol
        """
        symbol = Symbol(name, type_info, is_constant, value)
        self.symbols[name] = symbol
        return symbol
    
    def lookup(self, name: str) -> Optional[Symbol]:
        """
        Look up a symbol.
        
        Args:
            name: Symbol name
            
        Returns:
            Symbol if found, None otherwise
        """
        if name in self.symbols:
            return self.symbols[name]
        elif self.parent:
            return self.parent.lookup(name)
        return None
    
    def lookup_local(self, name: str) -> Optional[Symbol]:
        """
        Look up a symbol in current scope only.
        
        Args:
            name: Symbol name
            
        Returns:
            Symbol if found, None otherwise
        """
        return self.symbols.get(name)
    
    def enter_scope(self) -> 'SymbolTable':
        """Enter a new scope."""
        return SymbolTable(self)
    
    def exit_scope(self) -> Optional['SymbolTable']:
        """Exit current scope."""
        return self.parent


class Lambda3TypeChecker:
    """
    Type checker for Lambda³ language.
    
    Performs type checking on the AST to ensure type safety
    and correctness of expressions.
    """
    
    def __init__(self):
        """Initialize the type checker."""
        self.symbol_table = SymbolTable()
        self.errors = []
        self.warnings = []
    
    def check_types(self, ast: ASTNode) -> ASTNode:
        """
        Check types in the AST.
        
        Args:
            ast: AST to check
            
        Returns:
            Type-checked AST
        """
        self.errors.clear()
        self.warnings.clear()
        
        try:
            self._check_node(ast)
        except TypeCheckError as e:
            self.errors.append(str(e))
        
        return ast
    
    def _check_node(self, node: ASTNode) -> TypeInfo:
        """
        Check types for a node.
        
        Args:
            node: AST node to check
            
        Returns:
            Type information for the node
        """
        if node.node_type == "LITERAL":
            return self._check_literal(node)
        elif node.node_type == "IDENTIFIER":
            return self._check_identifier(node)
        elif node.node_type == "BINARY_OP":
            return self._check_binary_op(node)
        elif node.node_type == "UNARY_OP":
            return self._check_unary_op(node)
        elif node.node_type == "FUNCTION_CALL":
            return self._check_function_call(node)
        elif node.node_type == "LAMBDA":
            return self._check_lambda(node)
        elif node.node_type == "IF_EXPR":
            return self._check_if_expr(node)
        elif node.node_type == "LET_STMT":
            return self._check_let_stmt(node)
        elif node.node_type == "FUNCTION_DECL":
            return self._check_function_decl(node)
        elif node.node_type == "PROGRAM":
            return self._check_program(node)
        else:
            return TypeInfo(TernaryType.UNKNOWN)
    
    def _check_literal(self, node: LiteralNode) -> TypeInfo:
        """Check literal node."""
        if node.literal_type == "integer":
            return TypeInfo(TernaryType.INTEGER)
        elif node.literal_type == "float":
            return TypeInfo(TernaryType.FLOAT)
        elif node.literal_type == "string":
            return TypeInfo(TernaryType.STRING)
        elif node.literal_type == "boolean":
            return TypeInfo(TernaryType.BOOLEAN)
        elif node.literal_type == "trit":
            return TypeInfo(TernaryType.TRIT)
        elif node.literal_type == "tritarray":
            return TypeInfo(TernaryType.TRITARRAY)
        else:
            return TypeInfo(TernaryType.UNKNOWN)
    
    def _check_identifier(self, node: IdentifierNode) -> TypeInfo:
        """Check identifier node."""
        symbol = self.symbol_table.lookup(node.value)
        if symbol:
            return symbol.type_info
        else:
            self.errors.append(f"Undefined variable: {node.value}")
            return TypeInfo(TernaryType.ERROR)
    
    def _check_binary_op(self, node: BinaryOpNode) -> TypeInfo:
        """Check binary operation node."""
        left_type = self._check_node(node.left)
        right_type = self._check_node(node.right)
        
        # Check for type compatibility
        if not left_type.is_compatible_with(right_type):
            self.errors.append(f"Type mismatch in binary operation: {left_type} {node.operator} {right_type}")
            return TypeInfo(TernaryType.ERROR)
        
        # Determine result type based on operation
        if node.operator in ["+", "-", "*", "/", "%", "^"]:
            # Arithmetic operations
            if left_type.type_name == TernaryType.FLOAT or right_type.type_name == TernaryType.FLOAT:
                return TypeInfo(TernaryType.FLOAT)
            elif left_type.type_name == TernaryType.TRITARRAY or right_type.type_name == TernaryType.TRITARRAY:
                return TypeInfo(TernaryType.TRITARRAY)
            elif left_type.type_name == TernaryType.TRIT or right_type.type_name == TernaryType.TRIT:
                return TypeInfo(TernaryType.TRIT)
            else:
                return TypeInfo(TernaryType.INTEGER)
        elif node.operator in ["==", "!=", "<", "<=", ">", ">="]:
            # Comparison operations
            return TypeInfo(TernaryType.BOOLEAN)
        elif node.operator in ["&&", "||", "&", "|", "^", "->", "?", "!&"]:
            # Logical operations
            if left_type.type_name == TernaryType.TRITARRAY or right_type.type_name == TernaryType.TRITARRAY:
                return TypeInfo(TernaryType.TRITARRAY)
            else:
                return TypeInfo(TernaryType.TRIT)
        else:
            return TypeInfo(TernaryType.UNKNOWN)
    
    def _check_unary_op(self, node: UnaryOpNode) -> TypeInfo:
        """Check unary operation node."""
        operand_type = self._check_node(node.operand)
        
        if node.operator in ["-", "~"]:
            # Arithmetic/logic negation
            return operand_type
        elif node.operator == "!":
            # Logical negation
            if operand_type.type_name == TernaryType.BOOLEAN:
                return TypeInfo(TernaryType.BOOLEAN)
            else:
                return TypeInfo(TernaryType.TRIT)
        else:
            return TypeInfo(TernaryType.UNKNOWN)
    
    def _check_function_call(self, node: FunctionCallNode) -> TypeInfo:
        """Check function call node."""
        function_type = self._check_node(node.function)
        
        if not function_type.is_function():
            self.errors.append(f"Cannot call non-function: {function_type}")
            return TypeInfo(TernaryType.ERROR)
        
        # Check argument types
        if len(node.arguments) != len(function_type.parameter_types):
            self.errors.append(f"Argument count mismatch: expected {len(function_type.parameter_types)}, got {len(node.arguments)}")
            return TypeInfo(TernaryType.ERROR)
        
        for i, (arg, param_type) in enumerate(zip(node.arguments, function_type.parameter_types)):
            arg_type = self._check_node(arg)
            if not arg_type.is_compatible_with(param_type):
                self.errors.append(f"Argument {i+1} type mismatch: expected {param_type}, got {arg_type}")
        
        return function_type.return_type or TypeInfo(TernaryType.VOID)
    
    def _check_lambda(self, node: LambdaNode) -> TypeInfo:
        """Check lambda node."""
        # Enter new scope
        old_table = self.symbol_table
        self.symbol_table = self.symbol_table.enter_scope()
        
        # Add parameters to scope
        parameter_types = []
        for param in node.parameters:
            param_type = TypeInfo(TernaryType.UNKNOWN)  # Infer from usage
            self.symbol_table.define(param, param_type)
            parameter_types.append(param_type)
        
        # Check body
        body_type = self._check_node(node.body)
        
        # Restore scope
        self.symbol_table = old_table
        
        return TypeInfo(TernaryType.FUNCTION, parameter_types=parameter_types, return_type=body_type)
    
    def _check_if_expr(self, node: IfExprNode) -> TypeInfo:
        """Check if expression node."""
        condition_type = self._check_node(node.condition)
        
        if condition_type.type_name not in [TernaryType.BOOLEAN, TernaryType.TRIT]:
            self.warnings.append(f"Condition should be boolean or trit, got {condition_type}")
        
        then_type = self._check_node(node.then_expr)
        
        if node.else_expr:
            else_type = self._check_node(node.else_expr)
            if not then_type.is_compatible_with(else_type):
                self.errors.append(f"Type mismatch in if-else: then {then_type}, else {else_type}")
                return TypeInfo(TernaryType.ERROR)
            return then_type
        else:
            return then_type
    
    def _check_let_stmt(self, node: LetStmtNode) -> TypeInfo:
        """Check let statement node."""
        # Check value type
        value_type = self._check_node(node.value)
        
        # Define variable in current scope
        self.symbol_table.define(node.variable, value_type)
        
        # Check body if present
        if node.body:
            return self._check_node(node.body)
        else:
            return TypeInfo(TernaryType.VOID)
    
    def _check_function_decl(self, node: FunctionDeclNode) -> TypeInfo:
        """Check function declaration node."""
        # Enter new scope
        old_table = self.symbol_table
        self.symbol_table = self.symbol_table.enter_scope()
        
        # Add parameters to scope
        parameter_types = []
        for param in node.parameters:
            param_type = TypeInfo(TernaryType.UNKNOWN)  # Infer from usage
            self.symbol_table.define(param, param_type)
            parameter_types.append(param_type)
        
        # Check body
        body_type = self._check_node(node.body)
        
        # Create function type
        function_type = TypeInfo(TernaryType.FUNCTION, parameter_types=parameter_types, return_type=body_type)
        
        # Define function in outer scope
        self.symbol_table = old_table
        self.symbol_table.define(node.name, function_type)
        
        return function_type
    
    def _check_program(self, node: ASTNode) -> TypeInfo:
        """Check program node."""
        for stmt in node.children:
            self._check_node(stmt)
        
        return TypeInfo(TernaryType.VOID)
    
    def get_errors(self) -> List[str]:
        """Get type checking errors."""
        return self.errors.copy()
    
    def get_warnings(self) -> List[str]:
        """Get type checking warnings."""
        return self.warnings.copy()
    
    def has_errors(self) -> bool:
        """Check if type checking has errors."""
        return len(self.errors) > 0
    
    def has_warnings(self) -> bool:
        """Check if type checking has warnings."""
        return len(self.warnings) > 0
    
    def __str__(self) -> str:
        """String representation."""
        return "Lambda3TypeChecker()"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Lambda3TypeChecker(errors={len(self.errors)}, warnings={len(self.warnings)})"


class TypeCheckError(Exception):
    """Exception raised during type checking."""
    pass
