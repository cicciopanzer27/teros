"""
TESH (Ternary Shell) - Interactive ternary shell for TEROS.

This module provides a complete interactive shell for the ternary operating system.
"""

from typing import Dict, List, Optional, Any, Callable, Tuple
import os
import sys
import time
import threading
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..hal.driver_framework import ConsoleDriver


class CommandType(Enum):
    """Command types."""
    BUILTIN = "builtin"
    EXTERNAL = "external"
    SCRIPT = "script"
    ALIAS = "alias"


class CommandResult(Enum):
    """Command execution results."""
    SUCCESS = "success"
    ERROR = "error"
    EXIT = "exit"
    CONTINUE = "continue"


class TESHCommand:
    """
    TESH Command - Represents a shell command.
    
    Provides command parsing, execution, and result handling.
    """
    
    def __init__(self, name: str, command_type: CommandType, 
                 handler: Callable = None, description: str = ""):
        """
        Initialize TESH command.
        
        Args:
            name: Command name
            command_type: Type of command
            handler: Command handler function
            description: Command description
        """
        self.name = name
        self.command_type = command_type
        self.handler = handler
        self.description = description
        
        # Command statistics
        self.stats = {
            'executions': 0,
            'successes': 0,
            'errors': 0,
            'last_execution': 0
        }
    
    def execute(self, args: List[str], shell) -> CommandResult:
        """
        Execute the command.
        
        Args:
            args: Command arguments
            shell: Shell instance
            
        Returns:
            Command execution result
        """
        try:
            self.stats['executions'] += 1
            self.stats['last_execution'] = time.time()
            
            if self.handler:
                result = self.handler(args, shell)
                if result:
                    self.stats['successes'] += 1
                    return CommandResult.SUCCESS
                else:
                    self.stats['errors'] += 1
                    return CommandResult.ERROR
            else:
                self.stats['errors'] += 1
                return CommandResult.ERROR
                
        except Exception as e:
            self.stats['errors'] += 1
            print(f"Command execution error: {e}")
            return CommandResult.ERROR
    
    def get_stats(self) -> Dict[str, Any]:
        """Get command statistics."""
        return {
            'name': self.name,
            'type': self.command_type.value,
            'description': self.description,
            **self.stats
        }


class TESHShell:
    """
    TESH (Ternary Shell) - Interactive shell for TEROS.
    
    Provides command parsing, execution, history, and scripting.
    """
    
    def __init__(self, prompt: str = "teros> "):
        """
        Initialize TESH shell.
        
        Args:
            prompt: Shell prompt
        """
        self.prompt = prompt
        self.running = False
        self.exit_requested = False
        
        # Command management
        self.commands = {}  # command_name -> TESHCommand
        self.aliases = {}   # alias -> command_name
        
        # History management
        self.history = []
        self.history_index = 0
        self.max_history = 1000
        
        # Scripting
        self.script_mode = False
        self.script_file = None
        
        # Console driver
        self.console_driver = ConsoleDriver("tesh_console")
        self.console_driver.initialize()
        
        # Shell statistics
        self.stats = {
            'commands_executed': 0,
            'successful_commands': 0,
            'failed_commands': 0,
            'session_start_time': 0,
            'session_duration': 0
        }
        
        # Initialize built-in commands
        self._initialize_builtin_commands()
    
    def _initialize_builtin_commands(self) -> None:
        """Initialize built-in commands."""
        # Help command
        self.register_command('help', CommandType.BUILTIN, self._cmd_help, 
                             "Show help information")
        
        # Exit command
        self.register_command('exit', CommandType.BUILTIN, self._cmd_exit, 
                             "Exit the shell")
        
        # Clear command
        self.register_command('clear', CommandType.BUILTIN, self._cmd_clear, 
                             "Clear the screen")
        
        # History command
        self.register_command('history', CommandType.BUILTIN, self._cmd_history, 
                             "Show command history")
        
        # Pwd command
        self.register_command('pwd', CommandType.BUILTIN, self._cmd_pwd, 
                             "Print working directory")
        
        # Ls command
        self.register_command('ls', CommandType.BUILTIN, self._cmd_ls, 
                             "List directory contents")
        
        # Cd command
        self.register_command('cd', CommandType.BUILTIN, self._cmd_cd, 
                             "Change directory")
        
        # Echo command
        self.register_command('echo', CommandType.BUILTIN, self._cmd_echo, 
                             "Print arguments")
        
        # Ternary calculator
        self.register_command('calc', CommandType.BUILTIN, self._cmd_calc, 
                             "Ternary calculator")
        
        # System info
        self.register_command('sysinfo', CommandType.BUILTIN, self._cmd_sysinfo, 
                             "Show system information")
    
    def register_command(self, name: str, command_type: CommandType, 
                        handler: Callable, description: str = "") -> None:
        """
        Register a command.
        
        Args:
            name: Command name
            command_type: Command type
            handler: Command handler
            description: Command description
        """
        command = TESHCommand(name, command_type, handler, description)
        self.commands[name] = command
    
    def run(self) -> None:
        """Run the shell."""
        self.running = True
        self.stats['session_start_time'] = time.time()
        
        print("=== TESH (Ternary Shell) ===")
        print("Welcome to the Ternary Operating System Shell")
        print("Type 'help' for available commands, 'exit' to quit")
        print()
        
        try:
            while self.running and not self.exit_requested:
                try:
                    # Get user input
                    user_input = self._get_input()
                    
                    if user_input.strip():
                        # Add to history
                        self._add_to_history(user_input)
                        
                        # Parse and execute command
                        result = self._execute_command(user_input)
                        
                        # Update statistics
                        self.stats['commands_executed'] += 1
                        if result == CommandResult.SUCCESS:
                            self.stats['successful_commands'] += 1
                        elif result == CommandResult.ERROR:
                            self.stats['failed_commands'] += 1
                        elif result == CommandResult.EXIT:
                            break
                
                except KeyboardInterrupt:
                    print("\nUse 'exit' to quit the shell")
                    continue
                except EOFError:
                    break
                except Exception as e:
                    print(f"Shell error: {e}")
                    continue
        
        finally:
            self._cleanup()
    
    def _get_input(self) -> str:
        """Get user input."""
        try:
            return input(self.prompt)
        except (KeyboardInterrupt, EOFError):
            raise
    
    def _add_to_history(self, command: str) -> None:
        """Add command to history."""
        self.history.append(command)
        if len(self.history) > self.max_history:
            self.history.pop(0)
        self.history_index = len(self.history)
    
    def _execute_command(self, command_line: str) -> CommandResult:
        """Execute a command line."""
        try:
            # Parse command line
            parts = command_line.strip().split()
            if not parts:
                return CommandResult.CONTINUE
            
            command_name = parts[0]
            args = parts[1:] if len(parts) > 1 else []
            
            # Check for aliases
            if command_name in self.aliases:
                command_name = self.aliases[command_name]
            
            # Execute command
            if command_name in self.commands:
                command = self.commands[command_name]
                return command.execute(args, self)
            else:
                print(f"Command not found: {command_name}")
                print("Type 'help' for available commands")
                return CommandResult.ERROR
                
        except Exception as e:
            print(f"Command execution error: {e}")
            return CommandResult.ERROR
    
    # Built-in command handlers
    def _cmd_help(self, args: List[str], shell) -> bool:
        """Help command handler."""
        print("Available commands:")
        for name, command in self.commands.items():
            print(f"  {name:<12} - {command.description}")
        return True
    
    def _cmd_exit(self, args: List[str], shell) -> bool:
        """Exit command handler."""
        self.exit_requested = True
        self.running = False
        return True
    
    def _cmd_clear(self, args: List[str], shell) -> bool:
        """Clear command handler."""
        os.system('clear' if os.name == 'posix' else 'cls')
        return True
    
    def _cmd_history(self, args: List[str], shell) -> bool:
        """History command handler."""
        if not self.history:
            print("No command history")
            return True
        
        print("Command history:")
        for i, command in enumerate(self.history[-20:], 1):  # Show last 20
            print(f"  {i:3d}: {command}")
        return True
    
    def _cmd_pwd(self, args: List[str], shell) -> bool:
        """Pwd command handler."""
        print(os.getcwd())
        return True
    
    def _cmd_ls(self, args: List[str], shell) -> bool:
        """Ls command handler."""
        try:
            path = args[0] if args else "."
            if os.path.exists(path):
                items = os.listdir(path)
                for item in sorted(items):
                    item_path = os.path.join(path, item)
                    if os.path.isdir(item_path):
                        print(f"  {item}/")
                    else:
                        print(f"  {item}")
            else:
                print(f"Directory not found: {path}")
                return False
        except Exception as e:
            print(f"Error listing directory: {e}")
            return False
        return True
    
    def _cmd_cd(self, args: List[str], shell) -> bool:
        """Cd command handler."""
        try:
            path = args[0] if args else os.path.expanduser("~")
            os.chdir(path)
        except Exception as e:
            print(f"Error changing directory: {e}")
            return False
        return True
    
    def _cmd_echo(self, args: List[str], shell) -> bool:
        """Echo command handler."""
        print(" ".join(args))
        return True
    
    def _cmd_calc(self, args: List[str], shell) -> bool:
        """Ternary calculator command handler."""
        if not args:
            print("Usage: calc <ternary_expression>")
            print("Example: calc 1+1-0")
            return True
        
        try:
            expression = " ".join(args)
            result = self._evaluate_ternary_expression(expression)
            print(f"Result: {result}")
        except Exception as e:
            print(f"Calculation error: {e}")
            return False
        return True
    
    def _cmd_sysinfo(self, args: List[str], shell) -> bool:
        """System information command handler."""
        print("=== TEROS System Information ===")
        print(f"Shell: TESH (Ternary Shell)")
        print(f"Session duration: {time.time() - self.stats['session_start_time']:.2f} seconds")
        print(f"Commands executed: {self.stats['commands_executed']}")
        print(f"Successful commands: {self.stats['successful_commands']}")
        print(f"Failed commands: {self.stats['failed_commands']}")
        print(f"History entries: {len(self.history)}")
        return True
    
    def _evaluate_ternary_expression(self, expression: str) -> str:
        """Evaluate ternary expression."""
        # Simple ternary expression evaluator
        # This is a placeholder - in real implementation,
        # this would be a full ternary expression parser
        
        # Convert to binary for evaluation
        binary_expr = expression.replace('1', '1').replace('-', '0').replace('0', '0')
        
        try:
            # Evaluate binary expression
            result = eval(binary_expr)
            
            # Convert result back to ternary
            if result > 0:
                return "1"
            elif result < 0:
                return "-1"
            else:
                return "0"
        except:
            raise ValueError("Invalid ternary expression")
    
    def _cleanup(self) -> None:
        """Cleanup shell resources."""
        self.stats['session_duration'] = time.time() - self.stats['session_start_time']
        
        # Cleanup console driver
        if self.console_driver:
            self.console_driver.cleanup()
        
        print("TESH shell session ended")
    
    def get_stats(self) -> Dict[str, Any]:
        """Get shell statistics."""
        return {
            'running': self.running,
            'commands_registered': len(self.commands),
            'history_entries': len(self.history),
            'session_duration': time.time() - self.stats['session_start_time'],
            **self.stats
        }
    
    def get_command_stats(self) -> Dict[str, Any]:
        """Get command statistics."""
        return {name: command.get_stats() for name, command in self.commands.items()}
