"""
T3-ISA (Ternary 3-Instruction Set Architecture)

This module implements the complete T3-ISA instruction set,
including data movement, arithmetic, logic, control flow, system, and I/O instructions.
"""

from typing import Dict, List, Any, Optional, Tuple
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..core.t3_instruction import T3_Instruction


class T3_ISA:
    """Ternary 3-Instruction Set Architecture implementation."""
    
    def __init__(self):
        """Initialize T3-ISA."""
        self.instructions: Dict[str, Dict[str, Any]] = {}
        self._init_instruction_set()
    
    def _init_instruction_set(self):
        """Initialize the complete instruction set."""
        # Data Movement Instructions
        self.instructions.update({
            'LOAD': {
                'opcode': 0x01,
                'format': 'RRI',
                'description': 'Load immediate value into register',
                'operands': ['rd', 'immediate'],
                'cycles': 1
            },
            'STORE': {
                'opcode': 0x02,
                'format': 'RRI',
                'description': 'Store register value to memory',
                'operands': ['rs', 'address'],
                'cycles': 1
            },
            'MOVE': {
                'opcode': 0x03,
                'format': 'RR',
                'description': 'Move value between registers',
                'operands': ['rd', 'rs'],
                'cycles': 1
            },
            'PUSH': {
                'opcode': 0x04,
                'format': 'R',
                'description': 'Push register onto stack',
                'operands': ['rs'],
                'cycles': 1
            },
            'POP': {
                'opcode': 0x05,
                'format': 'R',
                'description': 'Pop value from stack to register',
                'operands': ['rd'],
                'cycles': 1
            },
            'LOADI': {
                'opcode': 0x06,
                'format': 'RRI',
                'description': 'Load indirect from memory',
                'operands': ['rd', 'rs', 'offset'],
                'cycles': 2
            },
            'STOREI': {
                'opcode': 0x07,
                'format': 'RRI',
                'description': 'Store indirect to memory',
                'operands': ['rs', 'rd', 'offset'],
                'cycles': 2
            }
        })
        
        # Arithmetic Instructions
        self.instructions.update({
            'ADD': {
                'opcode': 0x10,
                'format': 'RRR',
                'description': 'Add two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'SUB': {
                'opcode': 0x11,
                'format': 'RRR',
                'description': 'Subtract two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'MUL': {
                'opcode': 0x12,
                'format': 'RRR',
                'description': 'Multiply two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 2
            },
            'DIV': {
                'opcode': 0x13,
                'format': 'RRR',
                'description': 'Divide two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 3
            },
            'MOD': {
                'opcode': 0x14,
                'format': 'RRR',
                'description': 'Modulo operation',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 3
            },
            'ADDI': {
                'opcode': 0x15,
                'format': 'RRI',
                'description': 'Add immediate value',
                'operands': ['rd', 'rs', 'immediate'],
                'cycles': 1
            },
            'SUBI': {
                'opcode': 0x16,
                'format': 'RRI',
                'description': 'Subtract immediate value',
                'operands': ['rd', 'rs', 'immediate'],
                'cycles': 1
            },
            'MULI': {
                'opcode': 0x17,
                'format': 'RRI',
                'description': 'Multiply by immediate value',
                'operands': ['rd', 'rs', 'immediate'],
                'cycles': 2
            },
            'DIVI': {
                'opcode': 0x18,
                'format': 'RRI',
                'description': 'Divide by immediate value',
                'operands': ['rd', 'rs', 'immediate'],
                'cycles': 3
            },
            'NEG': {
                'opcode': 0x19,
                'format': 'RR',
                'description': 'Negate register value',
                'operands': ['rd', 'rs'],
                'cycles': 1
            },
            'ABS': {
                'opcode': 0x1A,
                'format': 'RR',
                'description': 'Absolute value of register',
                'operands': ['rd', 'rs'],
                'cycles': 1
            }
        })
        
        # Logic Instructions
        self.instructions.update({
            'AND': {
                'opcode': 0x20,
                'format': 'RRR',
                'description': 'Bitwise AND of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'OR': {
                'opcode': 0x21,
                'format': 'RRR',
                'description': 'Bitwise OR of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'XOR': {
                'opcode': 0x22,
                'format': 'RRR',
                'description': 'Bitwise XOR of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'NOT': {
                'opcode': 0x23,
                'format': 'RR',
                'description': 'Bitwise NOT of register',
                'operands': ['rd', 'rs'],
                'cycles': 1
            },
            'NAND': {
                'opcode': 0x24,
                'format': 'RRR',
                'description': 'Bitwise NAND of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'NOR': {
                'opcode': 0x25,
                'format': 'RRR',
                'description': 'Bitwise NOR of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'XNOR': {
                'opcode': 0x26,
                'format': 'RRR',
                'description': 'Bitwise XNOR of two registers',
                'operands': ['rd', 'rs1', 'rs2'],
                'cycles': 1
            },
            'SHL': {
                'opcode': 0x27,
                'format': 'RRI',
                'description': 'Shift left by immediate',
                'operands': ['rd', 'rs', 'shift'],
                'cycles': 1
            },
            'SHR': {
                'opcode': 0x28,
                'format': 'RRI',
                'description': 'Shift right by immediate',
                'operands': ['rd', 'rs', 'shift'],
                'cycles': 1
            },
            'ROL': {
                'opcode': 0x29,
                'format': 'RRI',
                'description': 'Rotate left by immediate',
                'operands': ['rd', 'rs', 'shift'],
                'cycles': 1
            },
            'ROR': {
                'opcode': 0x2A,
                'format': 'RRI',
                'description': 'Rotate right by immediate',
                'operands': ['rd', 'rs', 'shift'],
                'cycles': 1
            }
        })
        
        # Control Flow Instructions
        self.instructions.update({
            'JMP': {
                'opcode': 0x30,
                'format': 'I',
                'description': 'Unconditional jump',
                'operands': ['address'],
                'cycles': 1
            },
            'JZ': {
                'opcode': 0x31,
                'format': 'RI',
                'description': 'Jump if zero',
                'operands': ['rs', 'address'],
                'cycles': 2
            },
            'JNZ': {
                'opcode': 0x32,
                'format': 'RI',
                'description': 'Jump if not zero',
                'operands': ['rs', 'address'],
                'cycles': 2
            },
            'JN': {
                'opcode': 0x33,
                'format': 'RI',
                'description': 'Jump if negative',
                'operands': ['rs', 'address'],
                'cycles': 2
            },
            'JP': {
                'opcode': 0x34,
                'format': 'RI',
                'description': 'Jump if positive',
                'operands': ['rs', 'address'],
                'cycles': 2
            },
            'CALL': {
                'opcode': 0x35,
                'format': 'I',
                'description': 'Call subroutine',
                'operands': ['address'],
                'cycles': 2
            },
            'RET': {
                'opcode': 0x36,
                'format': 'N',
                'description': 'Return from subroutine',
                'operands': [],
                'cycles': 2
            },
            'CMP': {
                'opcode': 0x37,
                'format': 'RR',
                'description': 'Compare two registers',
                'operands': ['rs1', 'rs2'],
                'cycles': 1
            },
            'TEST': {
                'opcode': 0x38,
                'format': 'RR',
                'description': 'Test register value',
                'operands': ['rs1', 'rs2'],
                'cycles': 1
            }
        })
        
        # System Instructions
        self.instructions.update({
            'HALT': {
                'opcode': 0x40,
                'format': 'N',
                'description': 'Halt execution',
                'operands': [],
                'cycles': 1
            },
            'NOP': {
                'opcode': 0x41,
                'format': 'N',
                'description': 'No operation',
                'operands': [],
                'cycles': 1
            },
            'SYSCALL': {
                'opcode': 0x42,
                'format': 'I',
                'description': 'System call',
                'operands': ['syscall_number'],
                'cycles': 10
            },
            'INT': {
                'opcode': 0x43,
                'format': 'I',
                'description': 'Software interrupt',
                'operands': ['interrupt_number'],
                'cycles': 5
            },
            'IRET': {
                'opcode': 0x44,
                'format': 'N',
                'description': 'Return from interrupt',
                'operands': [],
                'cycles': 5
            },
            'CLI': {
                'opcode': 0x45,
                'format': 'N',
                'description': 'Clear interrupt flag',
                'operands': [],
                'cycles': 1
            },
            'STI': {
                'opcode': 0x46,
                'format': 'N',
                'description': 'Set interrupt flag',
                'operands': [],
                'cycles': 1
            },
            'PUSHFL': {
                'opcode': 0x47,
                'format': 'N',
                'description': 'Push flags to stack',
                'operands': [],
                'cycles': 1
            },
            'POPFL': {
                'opcode': 0x48,
                'format': 'N',
                'description': 'Pop flags from stack',
                'operands': [],
                'cycles': 1
            }
        })
        
        # I/O Instructions
        self.instructions.update({
            'IN': {
                'opcode': 0x50,
                'format': 'RI',
                'description': 'Input from port',
                'operands': ['rd', 'port'],
                'cycles': 3
            },
            'OUT': {
                'opcode': 0x51,
                'format': 'RI',
                'description': 'Output to port',
                'operands': ['rs', 'port'],
                'cycles': 3
            },
            'INB': {
                'opcode': 0x52,
                'format': 'RI',
                'description': 'Input byte from port',
                'operands': ['rd', 'port'],
                'cycles': 2
            },
            'OUTB': {
                'opcode': 0x53,
                'format': 'RI',
                'description': 'Output byte to port',
                'operands': ['rs', 'port'],
                'cycles': 2
            },
            'INW': {
                'opcode': 0x54,
                'format': 'RI',
                'description': 'Input word from port',
                'operands': ['rd', 'port'],
                'cycles': 3
            },
            'OUTW': {
                'opcode': 0x55,
                'format': 'RI',
                'description': 'Output word to port',
                'operands': ['rs', 'port'],
                'cycles': 3
            },
            'IND': {
                'opcode': 0x56,
                'format': 'RI',
                'description': 'Input dword from port',
                'operands': ['rd', 'port'],
                'cycles': 4
            },
            'OUTD': {
                'opcode': 0x57,
                'format': 'RI',
                'description': 'Output dword to port',
                'operands': ['rs', 'port'],
                'cycles': 4
            }
        })
    
    def get_instruction_info(self, instruction_name: str) -> Optional[Dict[str, Any]]:
        """Get information about an instruction."""
        return self.instructions.get(instruction_name.upper())
    
    def get_all_instructions(self) -> Dict[str, Dict[str, Any]]:
        """Get all instructions."""
        return self.instructions.copy()
    
    def get_instructions_by_category(self, category: str) -> Dict[str, Dict[str, Any]]:
        """Get instructions by category."""
        categories = {
            'data_movement': ['LOAD', 'STORE', 'MOVE', 'PUSH', 'POP', 'LOADI', 'STOREI'],
            'arithmetic': ['ADD', 'SUB', 'MUL', 'DIV', 'MOD', 'ADDI', 'SUBI', 'MULI', 'DIVI', 'NEG', 'ABS'],
            'logic': ['AND', 'OR', 'XOR', 'NOT', 'NAND', 'NOR', 'XNOR', 'SHL', 'SHR', 'ROL', 'ROR'],
            'control_flow': ['JMP', 'JZ', 'JNZ', 'JN', 'JP', 'CALL', 'RET', 'CMP', 'TEST'],
            'system': ['HALT', 'NOP', 'SYSCALL', 'INT', 'IRET', 'CLI', 'STI', 'PUSHFL', 'POPFL'],
            'io': ['IN', 'OUT', 'INB', 'OUTB', 'INW', 'OUTW', 'IND', 'OUTD']
        }
        
        if category not in categories:
            return {}
        
        result = {}
        for instruction_name in categories[category]:
            if instruction_name in self.instructions:
                result[instruction_name] = self.instructions[instruction_name]
        
        return result
    
    def get_instruction_count(self) -> int:
        """Get total number of instructions."""
        return len(self.instructions)
    
    def get_instruction_categories(self) -> List[str]:
        """Get list of instruction categories."""
        return ['data_movement', 'arithmetic', 'logic', 'control_flow', 'system', 'io']
    
    def validate_instruction(self, instruction_name: str, operands: List[Any]) -> bool:
        """Validate instruction and operands."""
        instruction_info = self.get_instruction_info(instruction_name)
        if not instruction_info:
            return False
        
        expected_operands = instruction_info['operands']
        if len(operands) != len(expected_operands):
            return False
        
        # Additional validation could be added here
        return True
    
    def get_instruction_cycles(self, instruction_name: str) -> int:
        """Get number of cycles for an instruction."""
        instruction_info = self.get_instruction_info(instruction_name)
        if instruction_info:
            return instruction_info['cycles']
        return 1
    
    def get_instruction_format(self, instruction_name: str) -> str:
        """Get instruction format."""
        instruction_info = self.get_instruction_info(instruction_name)
        if instruction_info:
            return instruction_info['format']
        return 'N'
    
    def get_instruction_description(self, instruction_name: str) -> str:
        """Get instruction description."""
        instruction_info = self.get_instruction_info(instruction_name)
        if instruction_info:
            return instruction_info['description']
        return 'Unknown instruction'
    
    def get_instruction_operands(self, instruction_name: str) -> List[str]:
        """Get instruction operands."""
        instruction_info = self.get_instruction_info(instruction_name)
        if instruction_info:
            return instruction_info['operands']
        return []
    
    def get_instruction_opcode(self, instruction_name: str) -> int:
        """Get instruction opcode."""
        instruction_info = self.get_instruction_info(instruction_name)
        if instruction_info:
            return instruction_info['opcode']
        return 0
    
    def get_instruction_stats(self) -> Dict[str, Any]:
        """Get instruction set statistics."""
        categories = self.get_instruction_categories()
        category_counts = {}
        
        for category in categories:
            category_instructions = self.get_instructions_by_category(category)
            category_counts[category] = len(category_instructions)
        
        return {
            'total_instructions': self.get_instruction_count(),
            'categories': category_counts,
            'instruction_formats': {
                'R': 0,    # Register
                'I': 0,    # Immediate
                'RI': 0,   # Register-Immediate
                'RR': 0,   # Register-Register
                'RRI': 0,  # Register-Register-Immediate
                'RRR': 0,  # Register-Register-Register
                'N': 0     # No operands
            }
        }
    
    def print_instruction_set(self):
        """Print the complete instruction set."""
        print("=== T3-ISA Instruction Set ===")
        print()
        
        categories = self.get_instruction_categories()
        for category in categories:
            print(f"## {category.replace('_', ' ').title()} Instructions")
            print()
            
            category_instructions = self.get_instructions_by_category(category)
            for instruction_name, info in category_instructions.items():
                print(f"### {instruction_name}")
                print(f"  Opcode: 0x{info['opcode']:02X}")
                print(f"  Format: {info['format']}")
                print(f"  Description: {info['description']}")
                print(f"  Operands: {', '.join(info['operands'])}")
                print(f"  Cycles: {info['cycles']}")
                print()
        
        # Print statistics
        stats = self.get_instruction_stats()
        print("## Instruction Set Statistics")
        print(f"Total Instructions: {stats['total_instructions']}")
        print("Category Breakdown:")
        for category, count in stats['categories'].items():
            print(f"  {category.replace('_', ' ').title()}: {count}")
        print()


# Global T3-ISA instance
t3_isa = T3_ISA()


def get_instruction_info(instruction_name: str) -> Optional[Dict[str, Any]]:
    """Get information about an instruction."""
    return t3_isa.get_instruction_info(instruction_name)


def get_all_instructions() -> Dict[str, Dict[str, Any]]:
    """Get all instructions."""
    return t3_isa.get_all_instructions()


def get_instructions_by_category(category: str) -> Dict[str, Dict[str, Any]]:
    """Get instructions by category."""
    return t3_isa.get_instructions_by_category(category)


def validate_instruction(instruction_name: str, operands: List[Any]) -> bool:
    """Validate instruction and operands."""
    return t3_isa.validate_instruction(instruction_name, operands)


def get_instruction_cycles(instruction_name: str) -> int:
    """Get number of cycles for an instruction."""
    return t3_isa.get_instruction_cycles(instruction_name)


def get_instruction_format(instruction_name: str) -> str:
    """Get instruction format."""
    return t3_isa.get_instruction_format(instruction_name)


def get_instruction_description(instruction_name: str) -> str:
    """Get instruction description."""
    return t3_isa.get_instruction_description(instruction_name)


def get_instruction_operands(instruction_name: str) -> List[str]:
    """Get instruction operands."""
    return t3_isa.get_instruction_operands(instruction_name)


def get_instruction_opcode(instruction_name: str) -> int:
    """Get instruction opcode."""
    return t3_isa.get_instruction_opcode(instruction_name)


def get_instruction_stats() -> Dict[str, Any]:
    """Get instruction set statistics."""
    return t3_isa.get_instruction_stats()


def print_instruction_set():
    """Print the complete instruction set."""
    t3_isa.print_instruction_set()
