"""
Lambda³ Code Generator - Code generation for Lambda³ language.

This module provides code generation for the Lambda³ language,
converting the optimized AST into T3-ISA bytecode.
"""

from typing import List, Optional, Dict, Any, Union
from ..core.t3_instruction import T3_Instruction
from ..core.tritarray import TritArray
from .parser import ASTNode, LiteralNode, IdentifierNode, BinaryOpNode, UnaryOpNode, FunctionCallNode, LambdaNode, IfExprNode, LetStmtNode, FunctionDeclNode


class CodeGenerator:
    """
    Code generator for Lambda³ language.
    
    Converts optimized AST into T3-ISA bytecode instructions.
    """
    
    def __init__(self):
        """Initialize the code generator."""
        self.instructions = []
        self.label_counter = 0
        self.variable_map = {}
        self.function_map = {}
        self.stack_depth = 0
        self.max_stack_depth = 0
        
        # Register allocation
        self.registers = ['R0', 'R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7']
        self.allocated_registers = set()
        self.register_map = {}
    
    def generate(self, ast: ASTNode) -> List[T3_Instruction]:
        """
        Generate T3-ISA bytecode from AST.
        
        Args:
            ast: Optimized AST
            
        Returns:
            List of T3-ISA instructions
        """
        self.instructions.clear()
        self.label_counter = 0
        self.variable_map.clear()
        self.function_map.clear()
        self.stack_depth = 0
        self.max_stack_depth = 0
        self.allocated_registers.clear()
        self.register_map.clear()
        
        # Generate code for the AST
        self._generate_node(ast)
        
        return self.instructions.copy()
    
    def _generate_node(self, node: ASTNode) -> str:
        """
        Generate code for a node.
        
        Args:
            node: AST node
            
        Returns:
            Register containing the result
        """
        if node.node_type == "LITERAL":
            return self._generate_literal(node)
        elif node.node_type == "IDENTIFIER":
            return self._generate_identifier(node)
        elif node.node_type == "BINARY_OP":
            return self._generate_binary_op(node)
        elif node.node_type == "UNARY_OP":
            return self._generate_unary_op(node)
        elif node.node_type == "FUNCTION_CALL":
            return self._generate_function_call(node)
        elif node.node_type == "LAMBDA":
            return self._generate_lambda(node)
        elif node.node_type == "IF_EXPR":
            return self._generate_if_expr(node)
        elif node.node_type == "LET_STMT":
            return self._generate_let_stmt(node)
        elif node.node_type == "FUNCTION_DECL":
            return self._generate_function_decl(node)
        elif node.node_type == "PROGRAM":
            return self._generate_program(node)
        else:
            return "R0"  # Default register
    
    def _generate_literal(self, node: LiteralNode) -> str:
        """Generate code for literal node."""
        result_reg = self._allocate_register()
        
        if node.literal_type == "integer":
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, node.value)
        elif node.literal_type == "float":
            # Convert float to integer for now
            int_value = int(node.value)
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, int_value)
        elif node.literal_type == "string":
            # Store string in memory and load address
            # For now, just load a placeholder
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, 0)
        elif node.literal_type == "boolean":
            value = 1 if node.value else 0
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, value)
        elif node.literal_type == "trit":
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, node.value)
        elif node.literal_type == "tritarray":
            # For now, treat as integer
            if isinstance(node.value, str):
                # Parse tritarray string
                value = self._parse_tritarray(node.value)
                self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, value)
            else:
                self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, node.value)
        else:
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, 0)
        
        return result_reg
    
    def _generate_identifier(self, node: IdentifierNode) -> str:
        """Generate code for identifier node."""
        if node.value in self.variable_map:
            return self.variable_map[node.value]
        else:
            # Undefined variable, return R0
            return "R0"
    
    def _generate_binary_op(self, node: BinaryOpNode) -> str:
        """Generate code for binary operation node."""
        left_reg = self._generate_node(node.left)
        right_reg = self._generate_node(node.right)
        result_reg = self._allocate_register()
        
        # Generate appropriate instruction based on operator
        if node.operator == "+":
            self._emit_instruction(T3_Instruction.ADD, result_reg, left_reg, right_reg, 0)
        elif node.operator == "-":
            self._emit_instruction(T3_Instruction.SUB, result_reg, left_reg, right_reg, 0)
        elif node.operator == "*":
            self._emit_instruction(T3_Instruction.MUL, result_reg, left_reg, right_reg, 0)
        elif node.operator == "/":
            self._emit_instruction(T3_Instruction.DIV, result_reg, left_reg, right_reg, 0)
        elif node.operator == "%":
            # Modulo not directly supported, implement as a - (a / b) * b
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.DIV, temp_reg, left_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.MUL, temp_reg, temp_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.SUB, result_reg, left_reg, temp_reg, 0)
            self._free_register(temp_reg)
        elif node.operator == "^":
            # Power not directly supported, implement as repeated multiplication
            # For now, just use multiplication
            self._emit_instruction(T3_Instruction.MUL, result_reg, left_reg, right_reg, 0)
        elif node.operator == "==":
            # Comparison not directly supported, implement as subtraction and test
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, left_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == "!=":
            # Not equal
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, left_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, temp_reg, temp_reg, 0, 0)
            self._emit_instruction(T3_Instruction.NOT, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == "<":
            # Less than
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, left_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == "<=":
            # Less than or equal
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, left_reg, right_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == ">":
            # Greater than
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, right_reg, left_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == ">=":
            # Greater than or equal
            temp_reg = self._allocate_register()
            self._emit_instruction(T3_Instruction.SUB, temp_reg, right_reg, left_reg, 0)
            self._emit_instruction(T3_Instruction.TEST, result_reg, temp_reg, 0, 0)
            self._free_register(temp_reg)
        elif node.operator == "&&":
            # Logical AND
            self._emit_instruction(T3_Instruction.AND, result_reg, left_reg, right_reg, 0)
        elif node.operator == "||":
            # Logical OR
            self._emit_instruction(T3_Instruction.OR, result_reg, left_reg, right_reg, 0)
        elif node.operator == "&":
            # Ternary AND
            self._emit_instruction(T3_Instruction.TERNARY_AND, result_reg, left_reg, right_reg, 0)
        elif node.operator == "|":
            # Ternary OR
            self._emit_instruction(T3_Instruction.TERNARY_OR, result_reg, left_reg, right_reg, 0)
        elif node.operator == "^":
            # Ternary XOR
            self._emit_instruction(T3_Instruction.TERNARY_XOR, result_reg, left_reg, right_reg, 0)
        elif node.operator == "->":
            # Ternary CONS (implication)
            self._emit_instruction(T3_Instruction.TERNARY_CONS, result_reg, left_reg, right_reg, 0)
        elif node.operator == "?":
            # Ternary ANY
            self._emit_instruction(T3_Instruction.TERNARY_ANY, result_reg, left_reg, right_reg, 0)
        elif node.operator == "!&":
            # Ternary NAND
            self._emit_instruction(T3_Instruction.TERNARY_NAND, result_reg, left_reg, right_reg, 0)
        else:
            # Unknown operator, use addition as fallback
            self._emit_instruction(T3_Instruction.ADD, result_reg, left_reg, right_reg, 0)
        
        self._free_register(left_reg)
        self._free_register(right_reg)
        return result_reg
    
    def _generate_unary_op(self, node: UnaryOpNode) -> str:
        """Generate code for unary operation node."""
        operand_reg = self._generate_node(node.operand)
        result_reg = self._allocate_register()
        
        if node.operator == "-":
            self._emit_instruction(T3_Instruction.NEG, result_reg, operand_reg, 0, 0)
        elif node.operator == "!":
            self._emit_instruction(T3_Instruction.NOT, result_reg, operand_reg, 0, 0)
        elif node.operator == "~":
            self._emit_instruction(T3_Instruction.TERNARY_NOT, result_reg, operand_reg, 0, 0)
        else:
            # Unknown operator, just copy
            self._emit_instruction(T3_Instruction.MOVE, result_reg, operand_reg, 0, 0)
        
        self._free_register(operand_reg)
        return result_reg
    
    def _generate_function_call(self, node: FunctionCallNode) -> str:
        """Generate code for function call node."""
        # For now, just return R0
        # In a full implementation, this would handle function calls properly
        return "R0"
    
    def _generate_lambda(self, node: LambdaNode) -> str:
        """Generate code for lambda node."""
        # For now, just generate the body
        return self._generate_node(node.body)
    
    def _generate_if_expr(self, node: IfExprNode) -> str:
        """Generate code for if expression node."""
        condition_reg = self._generate_node(node.condition)
        result_reg = self._allocate_register()
        
        # Generate labels
        else_label = self._generate_label()
        end_label = self._generate_label()
        
        # Jump to else if condition is false
        self._emit_instruction(T3_Instruction.JZ, condition_reg, 0, 0, else_label)
        
        # Generate then branch
        then_reg = self._generate_node(node.then_expr)
        self._emit_instruction(T3_Instruction.MOVE, result_reg, then_reg, 0, 0)
        self._emit_instruction(T3_Instruction.JMP, 0, 0, 0, end_label)
        
        # Generate else branch
        self._emit_label(else_label)
        if node.else_expr:
            else_reg = self._generate_node(node.else_expr)
            self._emit_instruction(T3_Instruction.MOVE, result_reg, else_reg, 0, 0)
        else:
            self._emit_instruction(T3_Instruction.LOADI, result_reg, 0, 0, 0)
        
        # End label
        self._emit_label(end_label)
        
        self._free_register(condition_reg)
        return result_reg
    
    def _generate_let_stmt(self, node: LetStmtNode) -> str:
        """Generate code for let statement node."""
        value_reg = self._generate_node(node.value)
        
        # Store variable in register map
        self.variable_map[node.variable] = value_reg
        
        if node.body:
            return self._generate_node(node.body)
        else:
            return value_reg
    
    def _generate_function_decl(self, node: FunctionDeclNode) -> str:
        """Generate code for function declaration node."""
        # Store function in function map
        self.function_map[node.name] = node
        
        # Generate function body
        return self._generate_node(node.body)
    
    def _generate_program(self, node: ASTNode) -> str:
        """Generate code for program node."""
        result_reg = "R0"
        
        for stmt in node.children:
            result_reg = self._generate_node(stmt)
        
        return result_reg
    
    def _allocate_register(self) -> str:
        """Allocate a free register."""
        for reg in self.registers:
            if reg not in self.allocated_registers:
                self.allocated_registers.add(reg)
                return reg
        
        # No free registers, use R0
        return "R0"
    
    def _free_register(self, reg: str) -> None:
        """Free a register."""
        self.allocated_registers.discard(reg)
    
    def _generate_label(self) -> int:
        """Generate a unique label."""
        self.label_counter += 1
        return self.label_counter
    
    def _emit_label(self, label: int) -> None:
        """Emit a label."""
        # Labels are handled by the instruction addresses
        pass
    
    def _emit_instruction(self, opcode: int, reg1: str, reg2: str, reg3: str, immediate: int) -> None:
        """Emit a T3-ISA instruction."""
        # Convert register names to numbers
        reg1_num = self._register_to_number(reg1)
        reg2_num = self._register_to_number(reg2)
        reg3_num = self._register_to_number(reg3)
        
        instruction = T3_Instruction(opcode, reg1_num, reg2_num, reg3_num, immediate)
        self.instructions.append(instruction)
    
    def _register_to_number(self, reg: str) -> int:
        """Convert register name to number."""
        if reg.startswith('R'):
            return int(reg[1:])
        elif reg == 'PC':
            return 8
        elif reg == 'SP':
            return 9
        elif reg == 'FP':
            return 10
        else:
            return 0
    
    def _parse_tritarray(self, value: str) -> int:
        """Parse tritarray string to integer."""
        if value.startswith('0t'):
            # Parse ternary literal
            trits = value[2:]
            result = 0
            for i, trit in enumerate(trits):
                if trit == '+':
                    result += 3 ** i
                elif trit == '-':
                    result -= 3 ** i
                # '0' adds nothing
            return result
        else:
            return 0
    
    def get_instructions(self) -> List[T3_Instruction]:
        """Get generated instructions."""
        return self.instructions.copy()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get code generation statistics."""
        return {
            'instructions_count': len(self.instructions),
            'max_stack_depth': self.max_stack_depth,
            'allocated_registers': len(self.allocated_registers),
            'variables_defined': len(self.variable_map),
            'functions_defined': len(self.function_map)
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"CodeGenerator(instructions={len(self.instructions)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"CodeGenerator(instructions={len(self.instructions)}, "
                f"max_stack_depth={self.max_stack_depth})")


class Lambda3CodeGenerator:
    """
    Main code generator for Lambda³ language.
    
    Provides a high-level interface for code generation.
    """
    
    def __init__(self):
        """Initialize the code generator."""
        self.generator = CodeGenerator()
    
    def generate(self, ast: ASTNode) -> List[T3_Instruction]:
        """
        Generate T3-ISA bytecode from AST.
        
        Args:
            ast: Optimized AST
            
        Returns:
            List of T3-ISA instructions
        """
        return self.generator.generate(ast)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get code generation statistics."""
        return self.generator.get_stats()
    
    def __str__(self) -> str:
        """String representation."""
        return f"Lambda3CodeGenerator(instructions={len(self.generator.instructions)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return f"Lambda3CodeGenerator(instructions={len(self.generator.instructions)})"
