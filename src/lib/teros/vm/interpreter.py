"""
TVMInterpreter - High-level interpreter for the Ternary Virtual Machine.

This module provides a high-level interface for executing ternary programs
with debugging, profiling, and interactive features.
"""

from typing import List, Optional, Dict, Any, Callable, Union
import time
import json
from ..core.t3_instruction import T3_Instruction
from .tvm import TVM


class TVMInterpreter:
    """
    High-level interpreter for the Ternary Virtual Machine.
    
    Provides debugging, profiling, and interactive features for
    executing ternary programs.
    """
    
    def __init__(self, memory_size: int = 729):
        """
        Initialize the TVM interpreter.
        
        Args:
            memory_size: Size of the virtual memory in trits
        """
        self.tvm = TVM(memory_size)
        self.debug_mode = False
        self.step_mode = False
        self.breakpoints = set()
        self.watchpoints = {}
        self.callbacks = {}
        
        # Execution history
        self.history = []
        self.max_history = 1000
        
        # Profiling data
        self.profile_data = {
            'instruction_counts': {},
            'execution_times': {},
            'memory_accesses': 0,
            'io_operations': 0
        }
    
    def load_program(self, program: List[T3_Instruction]) -> None:
        """
        Load a program into the interpreter.
        
        Args:
            program: List of T3-ISA instructions
        """
        self.tvm.load_program(program)
        self.history.clear()
        self.profile_data['instruction_counts'].clear()
        self.profile_data['execution_times'].clear()
    
    def run(self, max_instructions: Optional[int] = None, 
            debug: bool = False) -> Dict[str, Any]:
        """
        Run the loaded program.
        
        Args:
            max_instructions: Maximum number of instructions to execute
            debug: Enable debug mode
            
        Returns:
            Execution results and statistics
        """
        self.debug_mode = debug
        start_time = time.time()
        
        try:
            if debug:
                self._run_debug(max_instructions)
            else:
                self.tvm.run(max_instructions)
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e),
                'execution_time': time.time() - start_time,
                'instructions_executed': self.tvm.instruction_count
            }
        
        execution_time = time.time() - start_time
        
        return {
            'success': True,
            'execution_time': execution_time,
            'instructions_executed': self.tvm.instruction_count,
            'cycles': self.tvm.cycle_count,
            'stats': self.tvm.get_stats(),
            'output': self.tvm.output_buffer.copy()
        }
    
    def step(self) -> Optional[T3_Instruction]:
        """
        Execute a single instruction step.
        
        Returns:
            The executed instruction, or None if execution is complete
        """
        if not self.tvm.running or self.tvm.halted:
            return None
        
        # Get current instruction
        pc = self.tvm.registers['PC'].to_decimal()
        if pc >= len(self.tvm.program):
            return None
        
        instruction = self.tvm.program[pc]
        
        # Record in history
        self._record_step(instruction)
        
        # Execute instruction
        self.tvm.step()
        
        # Update profiling
        self._update_profiling(instruction)
        
        return instruction
    
    def set_breakpoint(self, address: int) -> None:
        """Set a breakpoint at the specified address."""
        self.breakpoints.add(address)
        self.tvm.set_breakpoint(address)
    
    def clear_breakpoint(self, address: int) -> None:
        """Clear a breakpoint at the specified address."""
        self.breakpoints.discard(address)
        self.tvm.clear_breakpoint(address)
    
    def set_watchpoint(self, register: str, callback: Callable) -> None:
        """Set a watchpoint on a register."""
        self.watchpoints[register] = callback
        self.tvm.set_watchpoint(register, callback)
    
    def get_register(self, name: str) -> Any:
        """Get a register value."""
        return self.tvm.get_register(name)
    
    def set_register(self, name: str, value: Any) -> None:
        """Set a register value."""
        self.tvm.set_register(name, value)
    
    def get_memory(self, address: int, size: int = 1) -> Any:
        """Get memory contents."""
        return self.tvm.get_memory(address, size)
    
    def set_memory(self, address: int, value: Any) -> None:
        """Set memory contents."""
        self.tvm.set_memory(address, value)
    
    def get_state(self) -> Dict[str, Any]:
        """Get the current interpreter state."""
        return {
            'running': self.tvm.running,
            'halted': self.tvm.halted,
            'pc': self.tvm.registers['PC'].to_decimal(),
            'sp': self.tvm.registers['SP'].to_decimal(),
            'fp': self.tvm.registers['FP'].to_decimal(),
            'registers': {name: reg.to_decimal() for name, reg in self.tvm.registers.items()},
            'stats': self.tvm.get_stats()
        }
    
    def get_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get execution history."""
        if limit is None:
            return self.history.copy()
        return self.history[-limit:]
    
    def get_profile_data(self) -> Dict[str, Any]:
        """Get profiling data."""
        return self.profile_data.copy()
    
    def reset(self) -> None:
        """Reset the interpreter to initial state."""
        self.tvm.reset()
        self.history.clear()
        self.profile_data['instruction_counts'].clear()
        self.profile_data['execution_times'].clear()
        self.profile_data['memory_accesses'] = 0
        self.profile_data['io_operations'] = 0
    
    def disassemble(self, start: int = 0, end: Optional[int] = None) -> List[str]:
        """
        Disassemble the loaded program.
        
        Args:
            start: Start address
            end: End address (None for end of program)
            
        Returns:
            List of disassembled instructions
        """
        if not self.tvm.program:
            return []
        
        if end is None:
            end = len(self.tvm.program)
        
        disassembly = []
        for i in range(start, min(end, len(self.tvm.program))):
            instruction = self.tvm.program[i]
            disassembly.append(f"{i:04x}: {instruction.disassemble()}")
        
        return disassembly
    
    def trace_execution(self, max_steps: int = 100) -> List[Dict[str, Any]]:
        """
        Trace program execution.
        
        Args:
            max_steps: Maximum number of steps to trace
            
        Returns:
            List of execution steps
        """
        trace = []
        original_running = self.tvm.running
        
        # Reset to beginning
        self.tvm.registers['PC'] = self.tvm.registers['PC'].__class__(0)
        
        for step in range(max_steps):
            if not self.tvm.running or self.tvm.halted:
                break
            
            pc = self.tvm.registers['PC'].to_decimal()
            instruction = self.tvm.program[pc] if pc < len(self.tvm.program) else None
            
            trace.append({
                'step': step,
                'pc': pc,
                'instruction': instruction.disassemble() if instruction else "END",
                'registers': {name: reg.to_decimal() for name, reg in self.tvm.registers.items()},
                'memory_usage': self.tvm.memory.get_memory_stats()
            })
            
            self.step()
        
        # Restore original state
        self.tvm.running = original_running
        
        return trace
    
    def benchmark(self, iterations: int = 100) -> Dict[str, Any]:
        """
        Benchmark program execution.
        
        Args:
            iterations: Number of iterations to run
            
        Returns:
            Benchmark results
        """
        if not self.tvm.program:
            return {'error': 'No program loaded'}
        
        times = []
        instruction_counts = []
        
        for i in range(iterations):
            self.reset()
            start_time = time.time()
            
            self.run()
            
            end_time = time.time()
            times.append(end_time - start_time)
            instruction_counts.append(self.tvm.instruction_count)
        
        return {
            'iterations': iterations,
            'avg_time': sum(times) / len(times),
            'min_time': min(times),
            'max_time': max(times),
            'avg_instructions': sum(instruction_counts) / len(instruction_counts),
            'instructions_per_second': sum(instruction_counts) / sum(times)
        }
    
    def save_state(self, filename: str) -> None:
        """Save interpreter state to file."""
        state = {
            'tvm_state': self.tvm.dump_state(),
            'registers': {name: reg.to_decimal() for name, reg in self.tvm.registers.items()},
            'memory_stats': self.tvm.memory.get_memory_stats(),
            'history': self.history,
            'profile_data': self.profile_data
        }
        
        with open(filename, 'w') as f:
            json.dump(state, f, indent=2)
    
    def load_state(self, filename: str) -> None:
        """Load interpreter state from file."""
        with open(filename, 'r') as f:
            state = json.load(f)
        
        # Restore registers
        for name, value in state['registers'].items():
            self.tvm.registers[name] = self.tvm.registers[name].__class__(value)
        
        # Restore history
        self.history = state.get('history', [])
        
        # Restore profile data
        self.profile_data = state.get('profile_data', {})
    
    def _run_debug(self, max_instructions: Optional[int] = None) -> None:
        """Run program in debug mode."""
        step_count = 0
        
        while self.tvm.running and not self.tvm.halted:
            if max_instructions and step_count >= max_instructions:
                break
            
            # Check for breakpoints
            pc = self.tvm.registers['PC'].to_decimal()
            if pc in self.breakpoints:
                print(f"Breakpoint hit at PC={pc}")
                break
            
            # Execute step
            instruction = self.step()
            if instruction is None:
                break
            
            step_count += 1
            
            # Print debug info
            if self.debug_mode:
                print(f"Step {step_count}: PC={pc}, {instruction.disassemble()}")
                print(f"  Registers: {self._format_registers()}")
    
    def _record_step(self, instruction: T3_Instruction) -> None:
        """Record a step in execution history."""
        step_info = {
            'step': len(self.history),
            'pc': self.tvm.registers['PC'].to_decimal(),
            'instruction': instruction.disassemble(),
            'registers': {name: reg.to_decimal() for name, reg in self.tvm.registers.items()},
            'timestamp': time.time()
        }
        
        self.history.append(step_info)
        
        # Limit history size
        if len(self.history) > self.max_history:
            self.history = self.history[-self.max_history:]
    
    def _update_profiling(self, instruction: T3_Instruction) -> None:
        """Update profiling data."""
        opcode_name = instruction.get_opcode_name()
        
        # Count instruction executions
        if opcode_name not in self.profile_data['instruction_counts']:
            self.profile_data['instruction_counts'][opcode_name] = 0
        self.profile_data['instruction_counts'][opcode_name] += 1
        
        # Update memory and I/O counts
        self.profile_data['memory_accesses'] += self.tvm.stats['memory_accesses']
        self.profile_data['io_operations'] += self.tvm.stats['io_operations']
    
    def _format_registers(self) -> str:
        """Format registers for display."""
        reg_strs = []
        for name, reg in self.tvm.registers.items():
            reg_strs.append(f"{name}={reg.to_decimal()}")
        return ", ".join(reg_strs)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TVMInterpreter(debug={self.debug_mode}, running={self.tvm.running})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TVMInterpreter(debug={self.debug_mode}, running={self.tvm.running}, "
                f"history={len(self.history)})")
