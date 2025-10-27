"""
TVM - Ternary Virtual Machine implementation.

The TVM is the core execution engine for ternary programs, providing
register management, instruction execution, and memory access.
"""

from typing import Dict, List, Optional, Union, Any
import time
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..core.ternary_memory import TernaryMemory
from ..core.t3_pcb import T3_PCB
from ..core.t3_instruction import T3_Instruction
from .alu import TernaryALU


class TVM:
    """
    Ternary Virtual Machine implementation.
    
    The TVM executes T3-ISA instructions and manages the execution state
    of ternary programs, including registers, memory, and I/O.
    """
    
    def __init__(self, memory_size: int = 729):
        """
        Initialize the TVM.
        
        Args:
            memory_size: Size of the virtual memory in trits
        """
        self.memory = TernaryMemory(memory_size)
        self.alu = TernaryALU()
        self.registers = {
            'R0': TritArray(0),
            'R1': TritArray(0),
            'R2': TritArray(0),
            'R3': TritArray(0),
            'R4': TritArray(0),
            'R5': TritArray(0),
            'R6': TritArray(0),
            'R7': TritArray(0),
            'PC': TritArray(0),  # Program Counter
            'SP': TritArray(0),  # Stack Pointer
            'FP': TritArray(0),  # Frame Pointer
            'FLAGS': TritArray(0)  # Status Flags
        }
        
        # Execution state
        self.running = False
        self.halted = False
        self.instruction_count = 0
        self.cycle_count = 0
        self.start_time = None
        self.end_time = None
        
        # Program storage
        self.program = []
        self.program_size = 0
        
        # I/O buffers
        self.input_buffer = []
        self.output_buffer = []
        
        # Debug mode
        self.debug_mode = False
        self.breakpoints = set()
        self.watchpoints = {}
        
        # Performance counters
        self.stats = {
            'instructions_executed': 0,
            'memory_accesses': 0,
            'io_operations': 0,
            'branches_taken': 0,
            'function_calls': 0,
            'exceptions': 0
        }
    
    def load_program(self, program: List[T3_Instruction]) -> None:
        """
        Load a program into the TVM.
        
        Args:
            program: List of T3-ISA instructions
        """
        self.program = program
        self.program_size = len(program)
        
        # Store program in memory
        for i, instruction in enumerate(program):
            encoded = instruction.encode()
            start_addr = i * T3_Instruction.INSTRUCTION_SIZE
            self.memory.store_tritarray(start_addr, encoded)
        
        # Reset execution state
        self.reset()
    
    def reset(self) -> None:
        """Reset the TVM to initial state."""
        # Reset registers
        for reg in self.registers:
            self.registers[reg] = TritArray(0)
        
        # Reset execution state
        self.running = False
        self.halted = False
        self.instruction_count = 0
        self.cycle_count = 0
        self.start_time = None
        self.end_time = None
        
        # Reset I/O buffers
        self.input_buffer.clear()
        self.output_buffer.clear()
        
        # Reset stats
        for key in self.stats:
            self.stats[key] = 0
    
    def run(self, max_instructions: Optional[int] = None) -> None:
        """
        Run the loaded program.
        
        Args:
            max_instructions: Maximum number of instructions to execute
        """
        if not self.program:
            raise RuntimeError("No program loaded")
        
        self.running = True
        self.halted = False
        self.start_time = time.time()
        
        try:
            while self.running and not self.halted:
                if max_instructions and self.instruction_count >= max_instructions:
                    break
                
                self.step()
                
        except Exception as e:
            self.running = False
            self.stats['exceptions'] += 1
            raise RuntimeError(f"TVM execution error: {e}")
        
        finally:
            self.running = False
            self.end_time = time.time()
    
    def step(self) -> None:
        """Execute a single instruction."""
        if not self.running or self.halted:
            return
        
        # Get current program counter
        pc = self.registers['PC'].to_decimal()
        
        # Check if we're at the end of the program
        if pc >= self.program_size:
            self.halted = True
            return
        
        # Load instruction from memory
        instruction_addr = pc * T3_Instruction.INSTRUCTION_SIZE
        encoded_instruction = self.memory.load_tritarray(
            instruction_addr, T3_Instruction.INSTRUCTION_SIZE
        )
        
        # Decode instruction
        instruction = T3_Instruction.decode(encoded_instruction)
        
        # Check for breakpoints
        if pc in self.breakpoints:
            self.debug_mode = True
            print(f"Breakpoint hit at PC={pc}")
        
        # Execute instruction
        self.execute_instruction(instruction)
        
        # Update program counter (unless instruction modified it)
        if instruction.opcode not in [T3_Instruction.JMP, T3_Instruction.CALL, T3_Instruction.RET]:
            self.registers['PC'] = TritArray(pc + 1)
        
        # Update counters
        self.instruction_count += 1
        self.cycle_count += 1
        self.stats['instructions_executed'] += 1
    
    def execute_instruction(self, instruction: T3_Instruction) -> None:
        """
        Execute a single instruction.
        
        Args:
            instruction: T3-ISA instruction to execute
        """
        opcode = instruction.opcode
        
        # Data Movement Instructions
        if opcode == T3_Instruction.LOAD:
            self._execute_load(instruction)
        elif opcode == T3_Instruction.STORE:
            self._execute_store(instruction)
        elif opcode == T3_Instruction.MOVE:
            self._execute_move(instruction)
        elif opcode == T3_Instruction.LOADI:
            self._execute_loadi(instruction)
        elif opcode == T3_Instruction.PUSH:
            self._execute_push(instruction)
        elif opcode == T3_Instruction.POP:
            self._execute_pop(instruction)
        
        # Arithmetic Instructions
        elif opcode == T3_Instruction.ADD:
            self._execute_add(instruction)
        elif opcode == T3_Instruction.SUB:
            self._execute_sub(instruction)
        elif opcode == T3_Instruction.MUL:
            self._execute_mul(instruction)
        elif opcode == T3_Instruction.DIV:
            self._execute_div(instruction)
        elif opcode == T3_Instruction.NEG:
            self._execute_neg(instruction)
        elif opcode == T3_Instruction.ABS:
            self._execute_abs(instruction)
        
        # Logic Instructions
        elif opcode == T3_Instruction.NAND:
            self._execute_nand(instruction)
        elif opcode == T3_Instruction.CONS:
            self._execute_cons(instruction)
        elif opcode == T3_Instruction.ANY:
            self._execute_any(instruction)
        elif opcode == T3_Instruction.NOT:
            self._execute_not(instruction)
        
        # Comparison Instructions
        elif opcode == T3_Instruction.CMP:
            self._execute_cmp(instruction)
        elif opcode == T3_Instruction.TEST:
            self._execute_test(instruction)
        
        # Control Flow Instructions
        elif opcode == T3_Instruction.JMP:
            self._execute_jmp(instruction)
        elif opcode == T3_Instruction.JZ:
            self._execute_jz(instruction)
        elif opcode == T3_Instruction.JN:
            self._execute_jn(instruction)
        elif opcode == T3_Instruction.JP:
            self._execute_jp(instruction)
        elif opcode == T3_Instruction.CALL:
            self._execute_call(instruction)
        elif opcode == T3_Instruction.RET:
            self._execute_ret(instruction)
        elif opcode == T3_Instruction.CALLI:
            self._execute_calli(instruction)
        
        # Shift Instructions
        elif opcode == T3_Instruction.TSHL:
            self._execute_tshl(instruction)
        elif opcode == T3_Instruction.TSHR:
            self._execute_tshr(instruction)
        elif opcode == T3_Instruction.ROTL:
            self._execute_rotl(instruction)
        elif opcode == T3_Instruction.ROTR:
            self._execute_rotr(instruction)
        
        # System Instructions
        elif opcode == T3_Instruction.SYSCALL:
            self._execute_syscall(instruction)
        elif opcode == T3_Instruction.HALT:
            self._execute_halt(instruction)
        elif opcode == T3_Instruction.NOP:
            self._execute_nop(instruction)
        elif opcode == T3_Instruction.BREAK:
            self._execute_break(instruction)
        
        # I/O Instructions
        elif opcode == T3_Instruction.PRINT:
            self._execute_print(instruction)
        elif opcode == T3_Instruction.INPUT:
            self._execute_input(instruction)
        elif opcode == T3_Instruction.PRINTI:
            self._execute_printi(instruction)
        elif opcode == T3_Instruction.PRINTS:
            self._execute_prints(instruction)
        
        else:
            raise RuntimeError(f"Unknown opcode: {opcode}")
    
    # Data Movement Instructions
    def _execute_load(self, instruction: T3_Instruction) -> None:
        """Execute LOAD instruction."""
        addr_reg = self.registers[f'R{instruction.reg2}']
        addr = addr_reg.to_decimal()
        value = self.memory.load_tritarray(addr, 1)
        self.registers[f'R{instruction.reg1}'] = value
        self.stats['memory_accesses'] += 1
    
    def _execute_store(self, instruction: T3_Instruction) -> None:
        """Execute STORE instruction."""
        addr_reg = self.registers[f'R{instruction.reg1}']
        value_reg = self.registers[f'R{instruction.reg2}']
        addr = addr_reg.to_decimal()
        self.memory.store_tritarray(addr, value_reg)
        self.stats['memory_accesses'] += 1
    
    def _execute_move(self, instruction: T3_Instruction) -> None:
        """Execute MOVE instruction."""
        value = self.registers[f'R{instruction.reg2}']
        self.registers[f'R{instruction.reg1}'] = value
    
    def _execute_loadi(self, instruction: T3_Instruction) -> None:
        """Execute LOADI instruction."""
        immediate = instruction.immediate
        self.registers[f'R{instruction.reg1}'] = immediate
    
    def _execute_push(self, instruction: T3_Instruction) -> None:
        """Execute PUSH instruction."""
        sp = self.registers['SP'].to_decimal()
        value = self.registers[f'R{instruction.reg1}']
        self.memory.store_tritarray(sp, value)
        self.registers['SP'] = TritArray(sp + 1)
        self.stats['memory_accesses'] += 1
    
    def _execute_pop(self, instruction: T3_Instruction) -> None:
        """Execute POP instruction."""
        sp = self.registers['SP'].to_decimal() - 1
        value = self.memory.load_tritarray(sp, 1)
        self.registers[f'R{instruction.reg1}'] = value
        self.registers['SP'] = TritArray(sp)
        self.stats['memory_accesses'] += 1
    
    # Arithmetic Instructions
    def _execute_add(self, instruction: T3_Instruction) -> None:
        """Execute ADD instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.add(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_sub(self, instruction: T3_Instruction) -> None:
        """Execute SUB instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.sub(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_mul(self, instruction: T3_Instruction) -> None:
        """Execute MUL instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.mul(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_div(self, instruction: T3_Instruction) -> None:
        """Execute DIV instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.div(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_neg(self, instruction: T3_Instruction) -> None:
        """Execute NEG instruction."""
        value = self.registers[f'R{instruction.reg2}']
        result = self.alu.neg(value)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_abs(self, instruction: T3_Instruction) -> None:
        """Execute ABS instruction."""
        value = self.registers[f'R{instruction.reg2}']
        result = self.alu.abs(value)
        self.registers[f'R{instruction.reg1}'] = result
    
    # Logic Instructions
    def _execute_nand(self, instruction: T3_Instruction) -> None:
        """Execute NAND instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.nand(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_cons(self, instruction: T3_Instruction) -> None:
        """Execute CONS instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.cons(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_any(self, instruction: T3_Instruction) -> None:
        """Execute ANY instruction."""
        a = self.registers[f'R{instruction.reg2}']
        b = self.registers[f'R{instruction.reg3}']
        result = self.alu.any(a, b)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_not(self, instruction: T3_Instruction) -> None:
        """Execute NOT instruction."""
        value = self.registers[f'R{instruction.reg2}']
        result = self.alu.not_op(value)
        self.registers[f'R{instruction.reg1}'] = result
    
    # Comparison Instructions
    def _execute_cmp(self, instruction: T3_Instruction) -> None:
        """Execute CMP instruction."""
        a = self.registers[f'R{instruction.reg1}']
        b = self.registers[f'R{instruction.reg2}']
        result = self.alu.cmp(a, b)
        self.registers['FLAGS'] = result
    
    def _execute_test(self, instruction: T3_Instruction) -> None:
        """Execute TEST instruction."""
        value = self.registers[f'R{instruction.reg1}']
        result = self.alu.test(value)
        self.registers['FLAGS'] = result
    
    # Control Flow Instructions
    def _execute_jmp(self, instruction: T3_Instruction) -> None:
        """Execute JMP instruction."""
        addr = instruction.immediate.to_decimal()
        self.registers['PC'] = TritArray(addr)
        self.stats['branches_taken'] += 1
    
    def _execute_jz(self, instruction: T3_Instruction) -> None:
        """Execute JZ instruction."""
        value = self.registers[f'R{instruction.reg1}']
        if value.is_zero():
            addr = instruction.immediate.to_decimal()
            self.registers['PC'] = TritArray(addr)
            self.stats['branches_taken'] += 1
    
    def _execute_jn(self, instruction: T3_Instruction) -> None:
        """Execute JN instruction."""
        value = self.registers[f'R{instruction.reg1}']
        if value.is_negative():
            addr = instruction.immediate.to_decimal()
            self.registers['PC'] = TritArray(addr)
            self.stats['branches_taken'] += 1
    
    def _execute_jp(self, instruction: T3_Instruction) -> None:
        """Execute JP instruction."""
        value = self.registers[f'R{instruction.reg1}']
        if value.is_positive():
            addr = instruction.immediate.to_decimal()
            self.registers['PC'] = TritArray(addr)
            self.stats['branches_taken'] += 1
    
    def _execute_call(self, instruction: T3_Instruction) -> None:
        """Execute CALL instruction."""
        # Save return address
        pc = self.registers['PC'].to_decimal()
        sp = self.registers['SP'].to_decimal()
        self.memory.store_tritarray(sp, TritArray(pc))
        self.registers['SP'] = TritArray(sp + 1)
        
        # Jump to function
        addr = self.registers[f'R{instruction.reg1}'].to_decimal()
        self.registers['PC'] = TritArray(addr)
        self.stats['function_calls'] += 1
        self.stats['memory_accesses'] += 1
    
    def _execute_ret(self, instruction: T3_Instruction) -> None:
        """Execute RET instruction."""
        # Restore return address
        sp = self.registers['SP'].to_decimal() - 1
        return_addr = self.memory.load_tritarray(sp, 1)
        self.registers['PC'] = return_addr
        self.registers['SP'] = TritArray(sp)
        self.stats['memory_accesses'] += 1
    
    def _execute_calli(self, instruction: T3_Instruction) -> None:
        """Execute CALLI instruction."""
        # Save return address
        pc = self.registers['PC'].to_decimal()
        sp = self.registers['SP'].to_decimal()
        self.memory.store_tritarray(sp, TritArray(pc))
        self.registers['SP'] = TritArray(sp + 1)
        
        # Jump to immediate address
        addr = instruction.immediate.to_decimal()
        self.registers['PC'] = TritArray(addr)
        self.stats['function_calls'] += 1
        self.stats['memory_accesses'] += 1
    
    # Shift Instructions
    def _execute_tshl(self, instruction: T3_Instruction) -> None:
        """Execute TSHL instruction."""
        value = self.registers[f'R{instruction.reg2}']
        shift = instruction.immediate.to_decimal()
        result = self.alu.tshl(value, shift)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_tshr(self, instruction: T3_Instruction) -> None:
        """Execute TSHR instruction."""
        value = self.registers[f'R{instruction.reg2}']
        shift = instruction.immediate.to_decimal()
        result = self.alu.tshr(value, shift)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_rotl(self, instruction: T3_Instruction) -> None:
        """Execute ROTL instruction."""
        value = self.registers[f'R{instruction.reg2}']
        shift = instruction.immediate.to_decimal()
        result = self.alu.rotl(value, shift)
        self.registers[f'R{instruction.reg1}'] = result
    
    def _execute_rotr(self, instruction: T3_Instruction) -> None:
        """Execute ROTR instruction."""
        value = self.registers[f'R{instruction.reg2}']
        shift = instruction.immediate.to_decimal()
        result = self.alu.rotr(value, shift)
        self.registers[f'R{instruction.reg1}'] = result
    
    # System Instructions
    def _execute_syscall(self, instruction: T3_Instruction) -> None:
        """Execute SYSCALL instruction."""
        syscall_num = instruction.immediate.to_decimal()
        # TODO: Implement system call handling
        print(f"System call {syscall_num} not implemented")
    
    def _execute_halt(self, instruction: T3_Instruction) -> None:
        """Execute HALT instruction."""
        self.halted = True
        self.running = False
    
    def _execute_nop(self, instruction: T3_Instruction) -> None:
        """Execute NOP instruction."""
        pass
    
    def _execute_break(self, instruction: T3_Instruction) -> None:
        """Execute BREAK instruction."""
        self.debug_mode = True
        print("Breakpoint hit")
    
    # I/O Instructions
    def _execute_print(self, instruction: T3_Instruction) -> None:
        """Execute PRINT instruction."""
        value = self.registers[f'R{instruction.reg1}']
        self.output_buffer.append(value.to_decimal())
        print(f"Output: {value.to_decimal()}")
        self.stats['io_operations'] += 1
    
    def _execute_input(self, instruction: T3_Instruction) -> None:
        """Execute INPUT instruction."""
        if self.input_buffer:
            value = self.input_buffer.pop(0)
            self.registers[f'R{instruction.reg1}'] = TritArray(value)
        else:
            # Default to 0 if no input available
            self.registers[f'R{instruction.reg1}'] = TritArray(0)
        self.stats['io_operations'] += 1
    
    def _execute_printi(self, instruction: T3_Instruction) -> None:
        """Execute PRINTI instruction."""
        value = instruction.immediate.to_decimal()
        self.output_buffer.append(value)
        print(f"Output: {value}")
        self.stats['io_operations'] += 1
    
    def _execute_prints(self, instruction: T3_Instruction) -> None:
        """Execute PRINTS instruction."""
        # TODO: Implement string printing
        print("String printing not implemented")
        self.stats['io_operations'] += 1
    
    # Debug and utility methods
    def set_breakpoint(self, address: int) -> None:
        """Set a breakpoint at the specified address."""
        self.breakpoints.add(address)
    
    def clear_breakpoint(self, address: int) -> None:
        """Clear a breakpoint at the specified address."""
        self.breakpoints.discard(address)
    
    def set_watchpoint(self, register: str, callback) -> None:
        """Set a watchpoint on a register."""
        self.watchpoints[register] = callback
    
    def get_register(self, name: str) -> TritArray:
        """Get a register value."""
        return self.registers.get(name, TritArray(0))
    
    def set_register(self, name: str, value: TritArray) -> None:
        """Set a register value."""
        self.registers[name] = value
    
    def get_memory(self, address: int, size: int = 1) -> TritArray:
        """Get memory contents."""
        return self.memory.load_tritarray(address, size)
    
    def set_memory(self, address: int, value: TritArray) -> None:
        """Set memory contents."""
        self.memory.store_tritarray(address, value)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get execution statistics."""
        stats = self.stats.copy()
        if self.start_time and self.end_time:
            stats['execution_time'] = self.end_time - self.start_time
        elif self.start_time:
            stats['execution_time'] = time.time() - self.start_time
        else:
            stats['execution_time'] = 0
        
        stats['instructions_per_second'] = (
            stats['instructions_executed'] / stats['execution_time']
            if stats['execution_time'] > 0 else 0
        )
        
        return stats
    
    def dump_state(self) -> str:
        """Dump the current VM state."""
        lines = []
        lines.append("=== TVM State ===")
        lines.append(f"Running: {self.running}")
        lines.append(f"Halted: {self.halted}")
        lines.append(f"PC: {self.registers['PC'].to_decimal()}")
        lines.append(f"SP: {self.registers['SP'].to_decimal()}")
        lines.append(f"FP: {self.registers['FP'].to_decimal()}")
        lines.append("")
        lines.append("Registers:")
        for name, value in self.registers.items():
            lines.append(f"  {name}: {value.to_decimal()}")
        lines.append("")
        lines.append("Stats:")
        for name, value in self.stats.items():
            lines.append(f"  {name}: {value}")
        
        return "\n".join(lines)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TVM(running={self.running}, halted={self.halted}, pc={self.registers['PC'].to_decimal()})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TVM(running={self.running}, halted={self.halted}, "
                f"pc={self.registers['PC'].to_decimal()}, "
                f"instructions={self.instruction_count})")
