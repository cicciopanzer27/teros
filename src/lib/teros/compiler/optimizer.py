"""
Lambda³ Optimizer - Code optimization for Lambda³ language.

This module provides code optimization for the Lambda³ language,
including constant folding, dead code elimination, and other optimizations.
"""

from typing import List, Optional, Dict, Any, Union
from .parser import ASTNode, LiteralNode, IdentifierNode, BinaryOpNode, UnaryOpNode, FunctionCallNode, LambdaNode, IfExprNode, LetStmtNode, FunctionDeclNode


class OptimizationPass:
    """Base class for optimization passes."""
    
    def __init__(self, name: str):
        """
        Initialize optimization pass.
        
        Args:
            name: Name of the optimization pass
        """
        self.name = name
        self.applied = 0
    
    def apply(self, ast: ASTNode) -> ASTNode:
        """
        Apply optimization to AST.
        
        Args:
            ast: AST to optimize
            
        Returns:
            Optimized AST
        """
        raise NotImplementedError("Subclasses must implement apply method")
    
    def __str__(self) -> str:
        """String representation."""
        return f"OptimizationPass({self.name})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"OptimizationPass({self.name}, applied={self.applied})"


class ConstantFoldingPass(OptimizationPass):
    """Constant folding optimization pass."""
    
    def __init__(self):
        super().__init__("ConstantFolding")
    
    def apply(self, ast: ASTNode) -> ASTNode:
        """Apply constant folding optimization."""
        return self._fold_constants(ast)
    
    def _fold_constants(self, node: ASTNode) -> ASTNode:
        """Fold constants in a node."""
        if node.node_type == "BINARY_OP":
            return self._fold_binary_op(node)
        elif node.node_type == "UNARY_OP":
            return self._fold_unary_op(node)
        elif node.node_type == "IF_EXPR":
            return self._fold_if_expr(node)
        else:
            # Recursively apply to children
            if hasattr(node, 'children'):
                for i, child in enumerate(node.children):
                    node.children[i] = self._fold_constants(child)
            return node
    
    def _fold_binary_op(self, node: BinaryOpNode) -> ASTNode:
        """Fold binary operation constants."""
        left = self._fold_constants(node.left)
        right = self._fold_constants(node.right)
        
        # Check if both operands are literals
        if (isinstance(left, LiteralNode) and isinstance(right, LiteralNode) and
            left.literal_type in ["integer", "float", "trit"] and
            right.literal_type in ["integer", "float", "trit"]):
            
            # Perform constant folding
            result = self._evaluate_binary_op(left.value, node.operator, right.value, left.literal_type)
            if result is not None:
                self.applied += 1
                return LiteralNode(result, left.literal_type)
        
        # Create new binary op with folded children
        new_node = BinaryOpNode(node.operator, left, right)
        new_node.line = node.line
        new_node.column = node.column
        return new_node
    
    def _fold_unary_op(self, node: UnaryOpNode) -> ASTNode:
        """Fold unary operation constants."""
        operand = self._fold_constants(node.operand)
        
        # Check if operand is literal
        if isinstance(operand, LiteralNode) and operand.literal_type in ["integer", "float", "trit"]:
            # Perform constant folding
            result = self._evaluate_unary_op(operand.value, node.operator, operand.literal_type)
            if result is not None:
                self.applied += 1
                return LiteralNode(result, operand.literal_type)
        
        # Create new unary op with folded operand
        new_node = UnaryOpNode(node.operator, operand)
        new_node.line = node.line
        new_node.column = node.column
        return new_node
    
    def _fold_if_expr(self, node: IfExprNode) -> ASTNode:
        """Fold if expression constants."""
        condition = self._fold_constants(node.condition)
        
        # Check if condition is constant
        if isinstance(condition, LiteralNode) and condition.literal_type in ["boolean", "trit"]:
            if condition.value:
                self.applied += 1
                return self._fold_constants(node.then_expr)
            elif node.else_expr:
                self.applied += 1
                return self._fold_constants(node.else_expr)
            else:
                # No else branch, return void
                return LiteralNode(None, "void")
        
        # Create new if expr with folded condition
        new_node = IfExprNode(condition, self._fold_constants(node.then_expr), 
                              self._fold_constants(node.else_expr) if node.else_expr else None)
        new_node.line = node.line
        new_node.column = node.column
        return new_node
    
    def _evaluate_binary_op(self, left: Any, operator: str, right: Any, type_name: str) -> Optional[Any]:
        """Evaluate binary operation on constants."""
        try:
            if operator == "+":
                return left + right
            elif operator == "-":
                return left - right
            elif operator == "*":
                return left * right
            elif operator == "/":
                if right == 0:
                    return None  # Division by zero
                return left / right
            elif operator == "%":
                if right == 0:
                    return None  # Modulo by zero
                return left % right
            elif operator == "^":
                return left ** right
            elif operator == "==":
                return left == right
            elif operator == "!=":
                return left != right
            elif operator == "<":
                return left < right
            elif operator == "<=":
                return left <= right
            elif operator == ">":
                return left > right
            elif operator == ">=":
                return left >= right
            elif operator == "&&":
                return left and right
            elif operator == "||":
                return left or right
            elif operator == "&":
                return left & right
            elif operator == "|":
                return left | right
            elif operator == "^":
                return left ^ right
            else:
                return None
        except (ZeroDivisionError, OverflowError, TypeError):
            return None
    
    def _evaluate_unary_op(self, operand: Any, operator: str, type_name: str) -> Optional[Any]:
        """Evaluate unary operation on constants."""
        try:
            if operator == "-":
                return -operand
            elif operator == "!":
                return not operand
            elif operator == "~":
                return ~operand
            else:
                return None
        except (OverflowError, TypeError):
            return None


class DeadCodeEliminationPass(OptimizationPass):
    """Dead code elimination optimization pass."""
    
    def __init__(self):
        super().__init__("DeadCodeElimination")
    
    def apply(self, ast: ASTNode) -> ASTNode:
        """Apply dead code elimination optimization."""
        return self._eliminate_dead_code(ast)
    
    def _eliminate_dead_code(self, node: ASTNode) -> ASTNode:
        """Eliminate dead code in a node."""
        if node.node_type == "IF_EXPR":
            return self._eliminate_if_dead_code(node)
        elif node.node_type == "LET_STMT":
            return self._eliminate_let_dead_code(node)
        else:
            # Recursively apply to children
            if hasattr(node, 'children'):
                for i, child in enumerate(node.children):
                    node.children[i] = self._eliminate_dead_code(child)
            return node
    
    def _eliminate_if_dead_code(self, node: IfExprNode) -> ASTNode:
        """Eliminate dead code in if expressions."""
        condition = self._eliminate_dead_code(node.condition)
        then_expr = self._eliminate_dead_code(node.then_expr)
        else_expr = self._eliminate_dead_code(node.else_expr) if node.else_expr else None
        
        # Check if condition is constant
        if isinstance(condition, LiteralNode) and condition.literal_type in ["boolean", "trit"]:
            if condition.value:
                self.applied += 1
                return then_expr
            elif else_expr:
                self.applied += 1
                return else_expr
            else:
                # No else branch, return void
                return LiteralNode(None, "void")
        
        # Create new if expr
        new_node = IfExprNode(condition, then_expr, else_expr)
        new_node.line = node.line
        new_node.column = node.column
        return new_node
    
    def _eliminate_let_dead_code(self, node: LetStmtNode) -> ASTNode:
        """Eliminate dead code in let statements."""
        value = self._eliminate_dead_code(node.value)
        body = self._eliminate_dead_code(node.body) if node.body else None
        
        # Check if variable is used in body
        if body and self._is_variable_used(node.variable, body):
            # Variable is used, keep the let statement
            new_node = LetStmtNode(node.variable, value, body)
            new_node.line = node.line
            new_node.column = node.column
            return new_node
        else:
            # Variable is not used, eliminate the let statement
            self.applied += 1
            return body or LiteralNode(None, "void")
    
    def _is_variable_used(self, variable: str, node: ASTNode) -> bool:
        """Check if a variable is used in a node."""
        if node.node_type == "IDENTIFIER" and node.value == variable:
            return True
        
        # Recursively check children
        if hasattr(node, 'children'):
            for child in node.children:
                if self._is_variable_used(variable, child):
                    return True
        
        return False


class InliningPass(OptimizationPass):
    """Function inlining optimization pass."""
    
    def __init__(self):
        super().__init__("Inlining")
        self.function_definitions = {}
    
    def apply(self, ast: ASTNode) -> ASTNode:
        """Apply function inlining optimization."""
        # First pass: collect function definitions
        self._collect_functions(ast)
        
        # Second pass: inline function calls
        return self._inline_functions(ast)
    
    def _collect_functions(self, node: ASTNode) -> None:
        """Collect function definitions."""
        if node.node_type == "FUNCTION_DECL":
            self.function_definitions[node.name] = node
        
        # Recursively collect from children
        if hasattr(node, 'children'):
            for child in node.children:
                self._collect_functions(child)
    
    def _inline_functions(self, node: ASTNode) -> ASTNode:
        """Inline function calls."""
        if node.node_type == "FUNCTION_CALL":
            return self._inline_function_call(node)
        else:
            # Recursively apply to children
            if hasattr(node, 'children'):
                for i, child in enumerate(node.children):
                    node.children[i] = self._inline_functions(child)
            return node
    
    def _inline_function_call(self, node: FunctionCallNode) -> ASTNode:
        """Inline a function call."""
        if node.function.node_type == "IDENTIFIER":
            function_name = node.function.value
            if function_name in self.function_definitions:
                func_def = self.function_definitions[function_name]
                
                # Check if function is simple enough to inline
                if self._is_simple_function(func_def):
                    self.applied += 1
                    return self._inline_function(func_def, node.arguments)
        
        return node
    
    def _is_simple_function(self, func_def: FunctionDeclNode) -> bool:
        """Check if a function is simple enough to inline."""
        # Simple heuristic: function with single expression body
        return (func_def.body.node_type in ["LITERAL", "IDENTIFIER", "BINARY_OP", "UNARY_OP"] and
                len(func_def.parameters) <= 3)
    
    def _inline_function(self, func_def: FunctionDeclNode, arguments: List[ASTNode]) -> ASTNode:
        """Inline a function call."""
        # Create substitution map
        substitutions = {}
        for param, arg in zip(func_def.parameters, arguments):
            substitutions[param] = arg
        
        # Substitute parameters in function body
        return self._substitute_variables(func_def.body, substitutions)
    
    def _substitute_variables(self, node: ASTNode, substitutions: Dict[str, ASTNode]) -> ASTNode:
        """Substitute variables in a node."""
        if node.node_type == "IDENTIFIER" and node.value in substitutions:
            return substitutions[node.value]
        else:
            # Recursively apply to children
            if hasattr(node, 'children'):
                for i, child in enumerate(node.children):
                    node.children[i] = self._substitute_variables(child, substitutions)
            return node


class Lambda3Optimizer:
    """
    Optimizer for Lambda³ language.
    
    Applies various optimization passes to improve code performance
    and reduce code size.
    """
    
    def __init__(self):
        """Initialize the optimizer."""
        self.passes = [
            ConstantFoldingPass(),
            DeadCodeEliminationPass(),
            InliningPass()
        ]
        self.stats = {
            'passes_applied': 0,
            'total_optimizations': 0,
            'optimization_time': 0
        }
    
    def optimize(self, ast: ASTNode) -> ASTNode:
        """
        Optimize an AST.
        
        Args:
            ast: AST to optimize
            
        Returns:
            Optimized AST
        """
        import time
        start_time = time.time()
        
        current_ast = ast
        
        # Apply each optimization pass
        for pass_ in self.passes:
            current_ast = pass_.apply(current_ast)
            self.stats['total_optimizations'] += pass_.applied
            if pass_.applied > 0:
                self.stats['passes_applied'] += 1
        
        self.stats['optimization_time'] = time.time() - start_time
        
        return current_ast
    
    def add_pass(self, pass_: OptimizationPass) -> None:
        """Add an optimization pass."""
        self.passes.append(pass_)
    
    def remove_pass(self, pass_name: str) -> None:
        """Remove an optimization pass by name."""
        self.passes = [p for p in self.passes if p.name != pass_name]
    
    def get_stats(self) -> Dict[str, Any]:
        """Get optimization statistics."""
        return self.stats.copy()
    
    def get_pass_stats(self) -> List[Dict[str, Any]]:
        """Get statistics for each optimization pass."""
        return [
            {
                'name': pass_.name,
                'applied': pass_.applied
            }
            for pass_ in self.passes
        ]
    
    def __str__(self) -> str:
        """String representation."""
        return f"Lambda3Optimizer(passes={len(self.passes)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"Lambda3Optimizer(passes={len(self.passes)}, "
                f"total_optimizations={self.stats['total_optimizations']})")
