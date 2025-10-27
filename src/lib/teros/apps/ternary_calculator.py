"""
Ternary Calculator - Advanced calculator for ternary arithmetic.

This module provides a complete calculator application for ternary mathematics.
"""

from typing import List, Union, Optional, Dict, Any
import sys
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..libs.libternary import TernaryMath, TernaryLogic
from ..libs.libmath import TernaryTrigonometry, TernaryExponentials, TernaryConstants
from ..libs.libstring import TernaryString


class TernaryCalculator:
    """
    Ternary Calculator - Advanced calculator for ternary arithmetic.
    
    Provides comprehensive ternary mathematical operations and functions.
    """
    
    def __init__(self):
        """Initialize ternary calculator."""
        self.memory = {}  # Memory storage
        self.history = []  # Calculation history
        self.variables = {}  # Variable storage
        
        # Calculator state
        self.last_result = None
        self.display_precision = 8
        
        # Statistics
        self.stats = {
            'calculations': 0,
            'operations': 0,
            'errors': 0
        }
    
    def calculate(self, expression: str) -> Union[TritArray, str]:
        """
        Calculate ternary expression.
        
        Args:
            expression: Mathematical expression
            
        Returns:
            Calculation result or error message
        """
        try:
            # Parse and evaluate expression
            result = self._evaluate_expression(expression)
            
            # Store result
            self.last_result = result
            
            # Add to history
            self.history.append({
                'expression': expression,
                'result': result,
                'timestamp': self._get_timestamp()
            })
            
            # Update statistics
            self.stats['calculations'] += 1
            self.stats['operations'] += 1
            
            return result
            
        except Exception as e:
            self.stats['errors'] += 1
            return f"Error: {str(e)}"
    
    def _evaluate_expression(self, expression: str) -> TritArray:
        """Evaluate mathematical expression."""
        # Remove whitespace
        expression = expression.replace(' ', '')
        
        # Handle parentheses
        while '(' in expression:
            start = expression.rfind('(')
            end = expression.find(')', start)
            if end == -1:
                raise ValueError("Mismatched parentheses")
            
            sub_expr = expression[start+1:end]
            sub_result = self._evaluate_expression(sub_expr)
            expression = expression[:start] + self._tritarray_to_string(sub_result) + expression[end+1:]
        
        # Handle operators in order of precedence
        expression = self._handle_operators(expression, ['^'], self._power)
        expression = self._handle_operators(expression, ['*', '/'], self._multiply_divide)
        expression = self._handle_operators(expression, ['+', '-'], self._add_subtract)
        
        # Convert final result
        return self._string_to_tritarray(expression)
    
    def _handle_operators(self, expression: str, operators: List[str], 
                         operation_func) -> str:
        """Handle operators in expression."""
        for operator in operators:
            while operator in expression:
                index = expression.find(operator)
                
                # Find operands
                left_start = self._find_operand_start(expression, index - 1)
                right_end = self._find_operand_end(expression, index + 1)
                
                left_operand = expression[left_start:index]
                right_operand = expression[index+1:right_end]
                
                # Perform operation
                result = operation_func(left_operand, right_operand)
                
                # Replace in expression
                expression = (expression[:left_start] + 
                            self._tritarray_to_string(result) + 
                            expression[right_end:])
        
        return expression
    
    def _find_operand_start(self, expression: str, index: int) -> int:
        """Find start of operand."""
        while index >= 0 and expression[index] in '0123456789-+':
            index -= 1
        return index + 1
    
    def _find_operand_end(self, expression: str, index: int) -> int:
        """Find end of operand."""
        while index < len(expression) and expression[index] in '0123456789-+':
            index += 1
        return index
    
    def _add_subtract(self, left: str, right: str) -> TritArray:
        """Add or subtract operands."""
        left_val = self._string_to_tritarray(left)
        right_val = self._string_to_tritarray(right)
        
        return TernaryMath.ternary_add(left_val, right_val)
    
    def _multiply_divide(self, left: str, right: str) -> TritArray:
        """Multiply or divide operands."""
        left_val = self._string_to_tritarray(left)
        right_val = self._string_to_tritarray(right)
        
        # Check for division
        if '/' in left or '/' in right:
            quotient, remainder = TernaryMath.ternary_divide(left_val, right_val)
            return quotient
        else:
            return TernaryMath.ternary_multiply(left_val, right_val)
    
    def _power(self, left: str, right: str) -> TritArray:
        """Power operation."""
        left_val = self._string_to_tritarray(left)
        right_val = self._string_to_tritarray(right)
        
        return TernaryMath.ternary_power(left_val, right_val)
    
    def _string_to_tritarray(self, string: str) -> TritArray:
        """Convert string to TritArray."""
        if string in self.variables:
            return self.variables[string]
        
        # Handle constants
        if string == 'pi':
            return TernaryConstants.PI
        elif string == 'e':
            return TernaryConstants.E
        elif string == 'phi':
            return TernaryConstants.PHI
        
        # Convert number
        try:
            value = int(string)
            return TritArray.from_int(value, 8)
        except ValueError:
            raise ValueError(f"Invalid operand: {string}")
    
    def _tritarray_to_string(self, tritarray: TritArray) -> str:
        """Convert TritArray to string."""
        return str(tritarray.to_decimal())
    
    def _get_timestamp(self) -> str:
        """Get current timestamp."""
        import time
        return time.strftime("%Y-%m-%d %H:%M:%S")
    
    def store_memory(self, name: str, value: TritArray) -> None:
        """Store value in memory."""
        self.memory[name] = value.copy()
    
    def recall_memory(self, name: str) -> Optional[TritArray]:
        """Recall value from memory."""
        return self.memory.get(name)
    
    def clear_memory(self) -> None:
        """Clear all memory."""
        self.memory.clear()
    
    def set_variable(self, name: str, value: TritArray) -> None:
        """Set variable value."""
        self.variables[name] = value.copy()
    
    def get_variable(self, name: str) -> Optional[TritArray]:
        """Get variable value."""
        return self.variables.get(name)
    
    def clear_variables(self) -> None:
        """Clear all variables."""
        self.variables.clear()
    
    def get_history(self) -> List[Dict[str, Any]]:
        """Get calculation history."""
        return self.history.copy()
    
    def clear_history(self) -> None:
        """Clear calculation history."""
        self.history.clear()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get calculator statistics."""
        return {
            'calculations': self.stats['calculations'],
            'operations': self.stats['operations'],
            'errors': self.stats['errors'],
            'memory_entries': len(self.memory),
            'variables': len(self.variables),
            'history_entries': len(self.history)
        }
    
    def help(self) -> str:
        """Get help information."""
        return """
Ternary Calculator Help:

Basic Operations:
  + : Addition
  - : Subtraction  
  * : Multiplication
  / : Division
  ^ : Power

Functions:
  sin(x) : Sine
  cos(x) : Cosine
  tan(x) : Tangent
  sqrt(x) : Square root
  log(x) : Natural logarithm
  exp(x) : Exponential

Constants:
  pi : Pi
  e : Euler's number
  phi : Golden ratio

Memory:
  store(name, value) : Store value in memory
  recall(name) : Recall value from memory
  clear_memory() : Clear all memory

Variables:
  set(name, value) : Set variable
  get(name) : Get variable
  clear_variables() : Clear all variables

Examples:
  1+1-0
  2*3/2
  sqrt(9)
  sin(pi/2)
  store('x', 5)
  recall('x')
        """


class TernaryCalculatorApp:
    """
    Ternary Calculator Application - Interactive calculator interface.
    
    Provides a complete calculator application with user interface.
    """
    
    def __init__(self):
        """Initialize calculator application."""
        self.calculator = TernaryCalculator()
        self.running = False
    
    def run(self) -> None:
        """Run calculator application."""
        self.running = True
        
        print("=== Ternary Calculator ===")
        print("Type 'help' for help, 'quit' to exit")
        print()
        
        try:
            while self.running:
                try:
                    # Get user input
                    user_input = input("calc> ").strip()
                    
                    if not user_input:
                        continue
                    
                    # Handle commands
                    if user_input.lower() == 'quit':
                        break
                    elif user_input.lower() == 'help':
                        print(self.calculator.help())
                        continue
                    elif user_input.lower() == 'clear':
                        self.calculator.clear_history()
                        print("History cleared")
                        continue
                    elif user_input.lower() == 'stats':
                        stats = self.calculator.get_stats()
                        print(f"Calculations: {stats['calculations']}")
                        print(f"Operations: {stats['operations']}")
                        print(f"Errors: {stats['errors']}")
                        continue
                    elif user_input.lower() == 'history':
                        history = self.calculator.get_history()
                        for entry in history[-10:]:  # Show last 10
                            print(f"{entry['timestamp']}: {entry['expression']} = {entry['result']}")
                        continue
                    
                    # Calculate expression
                    result = self.calculator.calculate(user_input)
                    print(f"Result: {result}")
                    
                except KeyboardInterrupt:
                    print("\nUse 'quit' to exit")
                    continue
                except EOFError:
                    break
                except Exception as e:
                    print(f"Error: {e}")
                    continue
        
        finally:
            self._cleanup()
    
    def _cleanup(self) -> None:
        """Cleanup calculator application."""
        self.running = False
        print("Calculator session ended")


if __name__ == "__main__":
    app = TernaryCalculatorApp()
    app.run()
