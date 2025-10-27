"""
T3_Instruction - T3-ISA instruction format and operations.

This module defines the instruction format for the T3-ISA (Ternary 3-Instruction Set Architecture)
and provides methods for encoding, decoding, and executing instructions.
"""

from typing import Union, Optional, Dict, Any, List
from .trit import Trit
from .tritarray import TritArray


class T3_Instruction:
    """
    T3-ISA instruction implementation.
    
    T3-ISA uses 27-trit instructions with the following format:
    - 3 trits: Opcode
    - 3 trits: Register 1
    - 3 trits: Register 2  
    - 3 trits: Register 3
    - 15 trits: Immediate/Address
    """
    
    # Instruction format constants
    INSTRUCTION_SIZE = 27  # 27 trits per instruction
    OPCODE_SIZE = 3
    REGISTER_SIZE = 3
    IMMEDIATE_SIZE = 15
    
    # Opcode definitions
    # Data Movement Instructions
    LOAD = 0b000  # 0
    STORE = 0b001  # 1
    MOVE = 0b010   # 2
    LOADI = 0b011  # 3
    PUSH = 0b100   # 4
    POP = 0b101    # 5
    
    # Arithmetic Instructions
    ADD = 0b110    # 6
    SUB = 0b111    # 7
    MUL = 0b1000   # 8
    DIV = 0b1001   # 9
    NEG = 0b1010   # 10
    ABS = 0b1011   # 11
    
    # Logic Instructions
    NAND = 0b1100  # 12
    CONS = 0b1101  # 13
    ANY = 0b1110   # 14
    NOT = 0b1111   # 15
    
    # Comparison Instructions
    CMP = 0b10000  # 16
    TEST = 0b10001 # 17
    
    # Control Flow Instructions
    JMP = 0b10010  # 18
    JZ = 0b10011   # 19
    JN = 0b10100   # 20
    JP = 0b10101   # 21
    CALL = 0b10110 # 22
    RET = 0b10111  # 23
    CALLI = 0b11000 # 24
    
    # Shift Instructions
    TSHL = 0b11001 # 25
    TSHR = 0b11010 # 26
    ROTL = 0b11011 # 27
    ROTR = 0b11100 # 28
    
    # System Instructions
    SYSCALL = 0b11101 # 29
    HALT = 0b11110   # 30
    NOP = 0b11111    # 31
    BREAK = 0b100000 # 32
    
    # I/O Instructions
    PRINT = 0b100001  # 33
    INPUT = 0b100010  # 34
    PRINTI = 0b100011 # 35
    PRINTS = 0b100100 # 36
    
    # Register definitions
    R0 = 0
    R1 = 1
    R2 = 2
    R3 = 3
    R4 = 4
    R5 = 5
    R6 = 6
    R7 = 7
    PC = 8  # Program Counter
    SP = 9  # Stack Pointer
    FP = 10 # Frame Pointer
    
    # Instruction categories
    DATA_MOVEMENT = [LOAD, STORE, MOVE, LOADI, PUSH, POP]
    ARITHMETIC = [ADD, SUB, MUL, DIV, NEG, ABS]
    LOGIC = [NAND, CONS, ANY, NOT]
    COMPARISON = [CMP, TEST]
    CONTROL_FLOW = [JMP, JZ, JN, JP, CALL, RET, CALLI]
    SHIFT = [TSHL, TSHR, ROTL, ROTR]
    SYSTEM = [SYSCALL, HALT, NOP, BREAK]
    IO = [PRINT, INPUT, PRINTI, PRINTS]
    
    def __init__(self, opcode: int = 0, reg1: int = 0, reg2: int = 0, reg3: int = 0, 
                 immediate: Union[int, TritArray] = 0):
        """
        Initialize a T3-ISA instruction.
        
        Args:
            opcode: Instruction opcode
            reg1: First register
            reg2: Second register
            reg3: Third register
            immediate: Immediate value or address
        """
        self.opcode = opcode
        self.reg1 = reg1
        self.reg2 = reg2
        self.reg3 = reg3
        
        if isinstance(immediate, TritArray):
            self.immediate = immediate
        else:
            self.immediate = TritArray(immediate)
    
    def encode(self) -> TritArray:
        """Encode instruction as TritArray."""
        # Create 27-trit instruction
        instruction = TritArray([0] * self.INSTRUCTION_SIZE)
        
        # Encode opcode (3 trits)
        opcode_trits = self._encode_value(self.opcode, self.OPCODE_SIZE)
        for i in range(self.OPCODE_SIZE):
            instruction[i] = opcode_trits[i]
        
        # Encode registers (3 trits each)
        reg1_trits = self._encode_value(self.reg1, self.REGISTER_SIZE)
        for i in range(self.REGISTER_SIZE):
            instruction[i + self.OPCODE_SIZE] = reg1_trits[i]
        
        reg2_trits = self._encode_value(self.reg2, self.REGISTER_SIZE)
        for i in range(self.REGISTER_SIZE):
            instruction[i + self.OPCODE_SIZE + self.REGISTER_SIZE] = reg2_trits[i]
        
        reg3_trits = self._encode_value(self.reg3, self.REGISTER_SIZE)
        for i in range(self.REGISTER_SIZE):
            instruction[i + self.OPCODE_SIZE + 2 * self.REGISTER_SIZE] = reg3_trits[i]
        
        # Encode immediate/address (15 trits)
        immediate_trits = self._encode_value(self.immediate.to_decimal(), self.IMMEDIATE_SIZE)
        for i in range(self.IMMEDIATE_SIZE):
            instruction[i + self.OPCODE_SIZE + 3 * self.REGISTER_SIZE] = immediate_trits[i]
        
        return instruction
    
    @classmethod
    def decode(cls, instruction: TritArray) -> 'T3_Instruction':
        """Decode TritArray instruction."""
        if len(instruction) != cls.INSTRUCTION_SIZE:
            raise ValueError(f"Invalid instruction size: {len(instruction)}")
        
        # Decode opcode
        opcode_trits = [instruction[i] for i in range(cls.OPCODE_SIZE)]
        opcode = cls._decode_value(opcode_trits)
        
        # Decode registers
        reg1_trits = [instruction[i] for i in range(cls.OPCODE_SIZE, cls.OPCODE_SIZE + cls.REGISTER_SIZE)]
        reg1 = cls._decode_value(reg1_trits)
        
        reg2_trits = [instruction[i] for i in range(cls.OPCODE_SIZE + cls.REGISTER_SIZE, 
                                                    cls.OPCODE_SIZE + 2 * cls.REGISTER_SIZE)]
        reg2 = cls._decode_value(reg2_trits)
        
        reg3_trits = [instruction[i] for i in range(cls.OPCODE_SIZE + 2 * cls.REGISTER_SIZE,
                                                   cls.OPCODE_SIZE + 3 * cls.REGISTER_SIZE)]
        reg3 = cls._decode_value(reg3_trits)
        
        # Decode immediate/address
        immediate_trits = [instruction[i] for i in range(cls.OPCODE_SIZE + 3 * cls.REGISTER_SIZE,
                                                          cls.INSTRUCTION_SIZE)]
        immediate = cls._decode_value(immediate_trits)
        
        return cls(opcode, reg1, reg2, reg3, immediate)
    
    def _encode_value(self, value: int, size: int) -> List[int]:
        """Encode a value as trits."""
        # Convert to balanced ternary
        trits = []
        abs_value = abs(value)
        
        while abs_value > 0:
            trits.append(abs_value % 3)
            abs_value //= 3
        
        # Convert to balanced ternary (-1, 0, 1)
        for i in range(len(trits)):
            if trits[i] == 2:
                trits[i] = -1
                if i + 1 < len(trits):
                    trits[i + 1] += 1
                else:
                    trits.append(1)
        
        # Pad to required size
        while len(trits) < size:
            trits.append(0)
        
        # Truncate if too long
        trits = trits[:size]
        
        # Handle negative values
        if value < 0:
            trits = [-t for t in trits]
        
        return trits
    
    @staticmethod
    def _decode_value(trits: List[int]) -> int:
        """Decode trits to integer value."""
        result = 0
        for i, trit in enumerate(trits):
            if hasattr(trit, 'value'):
                result += trit.value * (3 ** i)
            else:
                result += trit * (3 ** i)
        return result
    
    def get_opcode_name(self) -> str:
        """Get the opcode name."""
        opcode_names = {
            self.LOAD: "LOAD",
            self.STORE: "STORE",
            self.MOVE: "MOVE",
            self.LOADI: "LOADI",
            self.PUSH: "PUSH",
            self.POP: "POP",
            self.ADD: "ADD",
            self.SUB: "SUB",
            self.MUL: "MUL",
            self.DIV: "DIV",
            self.NEG: "NEG",
            self.ABS: "ABS",
            self.NAND: "NAND",
            self.CONS: "CONS",
            self.ANY: "ANY",
            self.NOT: "NOT",
            self.CMP: "CMP",
            self.TEST: "TEST",
            self.JMP: "JMP",
            self.JZ: "JZ",
            self.JN: "JN",
            self.JP: "JP",
            self.CALL: "CALL",
            self.RET: "RET",
            self.CALLI: "CALLI",
            self.TSHL: "TSHL",
            self.TSHR: "TSHR",
            self.ROTL: "ROTL",
            self.ROTR: "ROTR",
            self.SYSCALL: "SYSCALL",
            self.HALT: "HALT",
            self.NOP: "NOP",
            self.BREAK: "BREAK",
            self.PRINT: "PRINT",
            self.INPUT: "INPUT",
            self.PRINTI: "PRINTI",
            self.PRINTS: "PRINTS"
        }
        return opcode_names.get(self.opcode, f"UNKNOWN({self.opcode})")
    
    def get_register_name(self, reg: int) -> str:
        """Get register name."""
        register_names = {
            self.R0: "R0", self.R1: "R1", self.R2: "R2", self.R3: "R3",
            self.R4: "R4", self.R5: "R5", self.R6: "R6", self.R7: "R7",
            self.PC: "PC", self.SP: "SP", self.FP: "FP"
        }
        return register_names.get(reg, f"R{reg}")
    
    def disassemble(self) -> str:
        """Disassemble instruction to assembly string."""
        opcode_name = self.get_opcode_name()
        
        if self.opcode in self.DATA_MOVEMENT:
            if self.opcode == self.LOAD:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, [{self.get_register_name(self.reg2)}]"
            elif self.opcode == self.STORE:
                return f"{opcode_name} [{self.get_register_name(self.reg1)}], {self.get_register_name(self.reg2)}"
            elif self.opcode == self.MOVE:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}"
            elif self.opcode == self.LOADI:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, #{self.immediate.to_decimal()}"
            elif self.opcode == self.PUSH:
                return f"{opcode_name} {self.get_register_name(self.reg1)}"
            elif self.opcode == self.POP:
                return f"{opcode_name} {self.get_register_name(self.reg1)}"
        
        elif self.opcode in self.ARITHMETIC:
            if self.opcode in [self.ADD, self.SUB, self.MUL, self.DIV]:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}, {self.get_register_name(self.reg3)}"
            elif self.opcode in [self.NEG, self.ABS]:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}"
        
        elif self.opcode in self.LOGIC:
            if self.opcode in [self.NAND, self.CONS, self.ANY]:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}, {self.get_register_name(self.reg3)}"
            elif self.opcode == self.NOT:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}"
        
        elif self.opcode in self.COMPARISON:
            return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}"
        
        elif self.opcode in self.CONTROL_FLOW:
            if self.opcode == self.JMP:
                return f"{opcode_name} {self.immediate.to_decimal()}"
            elif self.opcode in [self.JZ, self.JN, self.JP]:
                return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.immediate.to_decimal()}"
            elif self.opcode == self.CALL:
                return f"{opcode_name} {self.get_register_name(self.reg1)}"
            elif self.opcode == self.RET:
                return f"{opcode_name}"
            elif self.opcode == self.CALLI:
                return f"{opcode_name} {self.immediate.to_decimal()}"
        
        elif self.opcode in self.SHIFT:
            return f"{opcode_name} {self.get_register_name(self.reg1)}, {self.get_register_name(self.reg2)}, {self.immediate.to_decimal()}"
        
        elif self.opcode in self.SYSTEM:
            if self.opcode == self.SYSCALL:
                return f"{opcode_name} {self.immediate.to_decimal()}"
            else:
                return f"{opcode_name}"
        
        elif self.opcode in self.IO:
            if self.opcode == self.PRINT:
                return f"{opcode_name} {self.get_register_name(self.reg1)}"
            elif self.opcode == self.INPUT:
                return f"{opcode_name} {self.get_register_name(self.reg1)}"
            elif self.opcode == self.PRINTI:
                return f"{opcode_name} {self.immediate.to_decimal()}"
            elif self.opcode == self.PRINTS:
                return f"{opcode_name} {self.immediate.to_decimal()}"
        
        return f"{opcode_name} {self.reg1}, {self.reg2}, {self.reg3}, {self.immediate.to_decimal()}"
    
    def is_data_movement(self) -> bool:
        """Check if instruction is data movement."""
        return self.opcode in self.DATA_MOVEMENT
    
    def is_arithmetic(self) -> bool:
        """Check if instruction is arithmetic."""
        return self.opcode in self.ARITHMETIC
    
    def is_logic(self) -> bool:
        """Check if instruction is logic."""
        return self.opcode in self.LOGIC
    
    def is_comparison(self) -> bool:
        """Check if instruction is comparison."""
        return self.opcode in self.COMPARISON
    
    def is_control_flow(self) -> bool:
        """Check if instruction is control flow."""
        return self.opcode in self.CONTROL_FLOW
    
    def is_shift(self) -> bool:
        """Check if instruction is shift."""
        return self.opcode in self.SHIFT
    
    def is_system(self) -> bool:
        """Check if instruction is system."""
        return self.opcode in self.SYSTEM
    
    def is_io(self) -> bool:
        """Check if instruction is I/O."""
        return self.opcode in self.IO
    
    def __str__(self) -> str:
        """String representation."""
        return self.disassemble()
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"T3_Instruction(opcode={self.opcode}, reg1={self.reg1}, "
                f"reg2={self.reg2}, reg3={self.reg3}, immediate={self.immediate.to_decimal()})")
    
    def __eq__(self, other) -> bool:
        """Equality comparison."""
        if isinstance(other, T3_Instruction):
            return (self.opcode == other.opcode and
                    self.reg1 == other.reg1 and
                    self.reg2 == other.reg2 and
                    self.reg3 == other.reg3 and
                    self.immediate == other.immediate)
        return False
    
    def __hash__(self) -> int:
        """Hash for use in sets and dictionaries."""
        return hash((self.opcode, self.reg1, self.reg2, self.reg3, self.immediate.to_decimal()))
