#!/usr/bin/env python3
"""
TEROS Ternary Debugger

This module provides debugging functionality for TEROS,
including breakpoints, step execution, variable inspection, and memory analysis.
"""

import sys
import traceback
from typing import Any, Dict, List, Optional, Set, Tuple
from ...core.trit import Trit
from ...core.tritarray import TritArray
from ...vm.tvm import TernaryVirtualMachine
from ...process.scheduler import TernaryScheduler
from ...memory.memory_manager import TernaryMemoryManager


class TernaryDebugger:
    """Ternary debugger for TEROS."""
    
    def __init__(self):
        """Initialize the debugger."""
        self.breakpoints: Set[int] = set()
        self.watchpoints: Dict[str, Any] = {}
        self.call_stack: List[Dict[str, Any]] = []
        self.variables: Dict[str, Any] = {}
        self.memory_snapshots: List[Dict[str, Any]] = []
        self.execution_history: List[Dict[str, Any]] = []
        self.is_running = False
        self.is_paused = False
        self.current_line = 0
        self.current_file = None
        
    def set_breakpoint(self, line: int, file: Optional[str] = None) -> bool:
        """Set a breakpoint at the specified line."""
        try:
            if file:
                breakpoint_key = f"{file}:{line}"
            else:
                breakpoint_key = f"{self.current_file}:{line}" if self.current_file else str(line)
            
            self.breakpoints.add(breakpoint_key)
            print(f"Breakpoint set at {breakpoint_key}")
            return True
        except Exception as e:
            print(f"Error setting breakpoint: {e}")
            return False
    
    def remove_breakpoint(self, line: int, file: Optional[str] = None) -> bool:
        """Remove a breakpoint at the specified line."""
        try:
            if file:
                breakpoint_key = f"{file}:{line}"
            else:
                breakpoint_key = f"{self.current_file}:{line}" if self.current_file else str(line)
            
            if breakpoint_key in self.breakpoints:
                self.breakpoints.remove(breakpoint_key)
                print(f"Breakpoint removed at {breakpoint_key}")
                return True
            else:
                print(f"No breakpoint found at {breakpoint_key}")
                return False
        except Exception as e:
            print(f"Error removing breakpoint: {e}")
            return False
    
    def set_watchpoint(self, variable: str, value: Any = None) -> bool:
        """Set a watchpoint for a variable."""
        try:
            self.watchpoints[variable] = value
            print(f"Watchpoint set for variable '{variable}'")
            return True
        except Exception as e:
            print(f"Error setting watchpoint: {e}")
            return False
    
    def remove_watchpoint(self, variable: str) -> bool:
        """Remove a watchpoint for a variable."""
        try:
            if variable in self.watchpoints:
                del self.watchpoints[variable]
                print(f"Watchpoint removed for variable '{variable}'")
                return True
            else:
                print(f"No watchpoint found for variable '{variable}'")
                return False
        except Exception as e:
            print(f"Error removing watchpoint: {e}")
            return False
    
    def step_into(self) -> bool:
        """Step into the next instruction."""
        try:
            if not self.is_running:
                print("Debugger is not running")
                return False
            
            # Execute one step
            self._execute_step()
            
            # Check for breakpoints
            if self._check_breakpoints():
                self.is_paused = True
                print("Breakpoint hit!")
                return True
            
            # Check watchpoints
            if self._check_watchpoints():
                self.is_paused = True
                print("Watchpoint hit!")
                return True
            
            return True
        except Exception as e:
            print(f"Error stepping into: {e}")
            return False
    
    def step_over(self) -> bool:
        """Step over the current instruction."""
        try:
            if not self.is_running:
                print("Debugger is not running")
                return False
            
            # Execute current instruction
            self._execute_step()
            
            # Check for breakpoints
            if self._check_breakpoints():
                self.is_paused = True
                print("Breakpoint hit!")
                return True
            
            return True
        except Exception as e:
            print(f"Error stepping over: {e}")
            return False
    
    def step_out(self) -> bool:
        """Step out of the current function."""
        try:
            if not self.is_running:
                print("Debugger is not running")
                return False
            
            # Execute until we return from current function
            while self.is_running and not self.is_paused:
                self._execute_step()
                
                # Check for breakpoints
                if self._check_breakpoints():
                    self.is_paused = True
                    print("Breakpoint hit!")
                    return True
            
            return True
        except Exception as e:
            print(f"Error stepping out: {e}")
            return False
    
    def continue_execution(self) -> bool:
        """Continue execution until next breakpoint."""
        try:
            if not self.is_running:
                print("Debugger is not running")
                return False
            
            self.is_paused = False
            
            # Execute until breakpoint or end
            while self.is_running and not self.is_paused:
                self._execute_step()
                
                # Check for breakpoints
                if self._check_breakpoints():
                    self.is_paused = True
                    print("Breakpoint hit!")
                    return True
            
            return True
        except Exception as e:
            print(f"Error continuing execution: {e}")
            return False
    
    def pause_execution(self) -> bool:
        """Pause execution."""
        try:
            if not self.is_running:
                print("Debugger is not running")
                return False
            
            self.is_paused = True
            print("Execution paused")
            return True
        except Exception as e:
            print(f"Error pausing execution: {e}")
            return False
    
    def stop_execution(self) -> bool:
        """Stop execution."""
        try:
            self.is_running = False
            self.is_paused = False
            print("Execution stopped")
            return True
        except Exception as e:
            print(f"Error stopping execution: {e}")
            return False
    
    def inspect_variable(self, variable: str) -> Optional[Any]:
        """Inspect a variable's value."""
        try:
            if variable in self.variables:
                value = self.variables[variable]
                print(f"Variable '{variable}': {value}")
                return value
            else:
                print(f"Variable '{variable}' not found")
                return None
        except Exception as e:
            print(f"Error inspecting variable: {e}")
            return None
    
    def inspect_memory(self, address: int, size: int = 1) -> Optional[bytes]:
        """Inspect memory at the specified address."""
        try:
            # This would require access to the memory manager
            # For now, return a placeholder
            print(f"Memory at address {address}: [placeholder]")
            return b"placeholder"
        except Exception as e:
            print(f"Error inspecting memory: {e}")
            return None
    
    def inspect_call_stack(self) -> List[Dict[str, Any]]:
        """Inspect the call stack."""
        try:
            print("Call stack:")
            for i, frame in enumerate(self.call_stack):
                print(f"  {i}: {frame}")
            return self.call_stack
        except Exception as e:
            print(f"Error inspecting call stack: {e}")
            return []
    
    def take_memory_snapshot(self) -> bool:
        """Take a snapshot of current memory state."""
        try:
            snapshot = {
                'timestamp': self._get_timestamp(),
                'variables': self.variables.copy(),
                'call_stack': self.call_stack.copy(),
                'current_line': self.current_line,
                'current_file': self.current_file
            }
            self.memory_snapshots.append(snapshot)
            print("Memory snapshot taken")
            return True
        except Exception as e:
            print(f"Error taking memory snapshot: {e}")
            return False
    
    def restore_memory_snapshot(self, index: int) -> bool:
        """Restore a memory snapshot."""
        try:
            if 0 <= index < len(self.memory_snapshots):
                snapshot = self.memory_snapshots[index]
                self.variables = snapshot['variables'].copy()
                self.call_stack = snapshot['call_stack'].copy()
                self.current_line = snapshot['current_line']
                self.current_file = snapshot['current_file']
                print(f"Memory snapshot {index} restored")
                return True
            else:
                print(f"Invalid snapshot index: {index}")
                return False
        except Exception as e:
            print(f"Error restoring memory snapshot: {e}")
            return False
    
    def list_breakpoints(self) -> List[str]:
        """List all breakpoints."""
        try:
            breakpoints = list(self.breakpoints)
            print(f"Breakpoints: {breakpoints}")
            return breakpoints
        except Exception as e:
            print(f"Error listing breakpoints: {e}")
            return []
    
    def list_watchpoints(self) -> List[str]:
        """List all watchpoints."""
        try:
            watchpoints = list(self.watchpoints.keys())
            print(f"Watchpoints: {watchpoints}")
            return watchpoints
        except Exception as e:
            print(f"Error listing watchpoints: {e}")
            return []
    
    def list_variables(self) -> List[str]:
        """List all variables."""
        try:
            variables = list(self.variables.keys())
            print(f"Variables: {variables}")
            return variables
        except Exception as e:
            print(f"Error listing variables: {e}")
            return []
    
    def _execute_step(self) -> bool:
        """Execute one step of the program."""
        try:
            # This would contain the actual execution logic
            # For now, just simulate a step
            self.current_line += 1
            
            # Record execution history
            step_info = {
                'line': self.current_line,
                'file': self.current_file,
                'timestamp': self._get_timestamp()
            }
            self.execution_history.append(step_info)
            
            return True
        except Exception as e:
            print(f"Error executing step: {e}")
            return False
    
    def _check_breakpoints(self) -> bool:
        """Check if any breakpoints are hit."""
        try:
            current_breakpoint = f"{self.current_file}:{self.current_line}" if self.current_file else str(self.current_line)
            return current_breakpoint in self.breakpoints
        except Exception as e:
            print(f"Error checking breakpoints: {e}")
            return False
    
    def _check_watchpoints(self) -> bool:
        """Check if any watchpoints are hit."""
        try:
            for variable, expected_value in self.watchpoints.items():
                if variable in self.variables:
                    current_value = self.variables[variable]
                    if expected_value is not None and current_value != expected_value:
                        return True
            return False
        except Exception as e:
            print(f"Error checking watchpoints: {e}")
            return False
    
    def _get_timestamp(self) -> str:
        """Get current timestamp."""
        import datetime
        return datetime.datetime.now().isoformat()
    
    def start_debugging(self, program: str) -> bool:
        """Start debugging a program."""
        try:
            self.is_running = True
            self.is_paused = False
            self.current_file = program
            self.current_line = 0
            
            print(f"Started debugging program: {program}")
            return True
        except Exception as e:
            print(f"Error starting debugging: {e}")
            return False
    
    def get_debugger_status(self) -> Dict[str, Any]:
        """Get current debugger status."""
        return {
            'is_running': self.is_running,
            'is_paused': self.is_paused,
            'current_line': self.current_line,
            'current_file': self.current_file,
            'breakpoints': list(self.breakpoints),
            'watchpoints': list(self.watchpoints.keys()),
            'variables': list(self.variables.keys()),
            'call_stack_depth': len(self.call_stack),
            'memory_snapshots': len(self.memory_snapshots),
            'execution_history': len(self.execution_history)
        }


def main():
    """Main debugger function."""
    debugger = TernaryDebugger()
    
    print("=== TEROS Ternary Debugger ===")
    print("Type 'help' for available commands")
    print()
    
    while True:
        try:
            command = input("(teros-debugger) ").strip().split()
            
            if not command:
                continue
            
            cmd = command[0].lower()
            
            if cmd == 'help':
                print("Available commands:")
                print("  break <line> [file]  - Set breakpoint")
                print("  clear <line> [file] - Remove breakpoint")
                print("  watch <variable>     - Set watchpoint")
                print("  unwatch <variable>   - Remove watchpoint")
                print("  step                 - Step into")
                print("  next                 - Step over")
                print("  finish               - Step out")
                print("  continue             - Continue execution")
                print("  pause                - Pause execution")
                print("  stop                 - Stop execution")
                print("  inspect <variable>   - Inspect variable")
                print("  memory <address>     - Inspect memory")
                print("  stack                - Show call stack")
                print("  snapshot             - Take memory snapshot")
                print("  restore <index>      - Restore snapshot")
                print("  list breakpoints     - List breakpoints")
                print("  list watchpoints     - List watchpoints")
                print("  list variables       - List variables")
                print("  status               - Show debugger status")
                print("  quit                 - Exit debugger")
            
            elif cmd == 'break':
                if len(command) >= 2:
                    line = int(command[1])
                    file = command[2] if len(command) > 2 else None
                    debugger.set_breakpoint(line, file)
                else:
                    print("Usage: break <line> [file]")
            
            elif cmd == 'clear':
                if len(command) >= 2:
                    line = int(command[1])
                    file = command[2] if len(command) > 2 else None
                    debugger.remove_breakpoint(line, file)
                else:
                    print("Usage: clear <line> [file]")
            
            elif cmd == 'watch':
                if len(command) >= 2:
                    variable = command[1]
                    debugger.set_watchpoint(variable)
                else:
                    print("Usage: watch <variable>")
            
            elif cmd == 'unwatch':
                if len(command) >= 2:
                    variable = command[1]
                    debugger.remove_watchpoint(variable)
                else:
                    print("Usage: unwatch <variable>")
            
            elif cmd == 'step':
                debugger.step_into()
            
            elif cmd == 'next':
                debugger.step_over()
            
            elif cmd == 'finish':
                debugger.step_out()
            
            elif cmd == 'continue':
                debugger.continue_execution()
            
            elif cmd == 'pause':
                debugger.pause_execution()
            
            elif cmd == 'stop':
                debugger.stop_execution()
            
            elif cmd == 'inspect':
                if len(command) >= 2:
                    variable = command[1]
                    debugger.inspect_variable(variable)
                else:
                    print("Usage: inspect <variable>")
            
            elif cmd == 'memory':
                if len(command) >= 2:
                    address = int(command[1])
                    debugger.inspect_memory(address)
                else:
                    print("Usage: memory <address>")
            
            elif cmd == 'stack':
                debugger.inspect_call_stack()
            
            elif cmd == 'snapshot':
                debugger.take_memory_snapshot()
            
            elif cmd == 'restore':
                if len(command) >= 2:
                    index = int(command[1])
                    debugger.restore_memory_snapshot(index)
                else:
                    print("Usage: restore <index>")
            
            elif cmd == 'list':
                if len(command) >= 2:
                    subcmd = command[1].lower()
                    if subcmd == 'breakpoints':
                        debugger.list_breakpoints()
                    elif subcmd == 'watchpoints':
                        debugger.list_watchpoints()
                    elif subcmd == 'variables':
                        debugger.list_variables()
                    else:
                        print("Usage: list <breakpoints|watchpoints|variables>")
                else:
                    print("Usage: list <breakpoints|watchpoints|variables>")
            
            elif cmd == 'status':
                status = debugger.get_debugger_status()
                for key, value in status.items():
                    print(f"{key}: {value}")
            
            elif cmd == 'quit':
                break
            
            else:
                print(f"Unknown command: {cmd}")
                print("Type 'help' for available commands")
        
        except KeyboardInterrupt:
            print("\nUse 'quit' to exit")
        except Exception as e:
            print(f"Error: {e}")
    
    print("Goodbye!")


if __name__ == "__main__":
    main()
