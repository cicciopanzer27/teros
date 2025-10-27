"""
Lambda Calculus Commands for TESH
Extends TESH with lambda calculus support
"""

import sys
import os

# Add paths for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', 'Lambda3_Project'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

try:
    from lambda3.parser.parser import parse
    from lambda3.engine.reducer import reduce
    from lambda3.engine.graph_reducer import reduce_with_sharing
    from lambda3.ternary.encoder import encode, decode, encoding_efficiency
    from lambda3.types.inference import infer_type
except ImportError as e:
    print(f"Warning: Lambda3 import failed: {e}")
    parse = None


# ============================================================================
# PREDEFINED LAMBDA LIBRARY
# ============================================================================

LAMBDA_LIBRARY = {
    # Church Numerals
    ':0': r'\f.\x.x',
    ':1': r'\f.\x.f x',
    ':2': r'\f.\x.f (f x)',
    ':3': r'\f.\x.f (f (f x))',
    ':4': r'\f.\x.f (f (f (f x)))',
    ':5': r'\f.\x.f (f (f (f (f x))))',
    
    # Church Booleans
    ':true': r'\x.\y.x',
    ':false': r'\x.\y.y',
    
    # Combinators
    ':I': r'\x.x',                        # Identity
    ':K': r'\x.\y.x',                     # Const
    ':S': r'\x.\y.\z.x z (y z)',         # S combinator
    ':Y': r'\f.(\x.f (x x)) (\x.f (x x))', # Y combinator
    
    # Boolean operations
    ':not': r'\p.\a.\b.p b a',
    ':and': r'\p.\q.p q p',
    ':or': r'\p.\q.p p q',
    
    # Church arithmetic
    ':succ': r'\n.\f.\x.f (n f x)',
    ':add': r'\m.\n.\f.\x.m f (n f x)',
    ':mult': r'\m.\n.\f.m (n f)',
    ':pred': r'\n.\f.\x.n (\g.\h.h (g f)) (\u.x) (\u.u)',
    
    # List operations
    ':cons': r'\h.\t.\c.\n.c h (t c n)',
    ':nil': r'\c.\n.n',
    ':car': r'\p.p (\h.\t.h)',
    ':cdr': r'\p.p (\h.\t.t)',
}


# ============================================================================
# LAMBDA COMMANDS
# ============================================================================

class LambdaCommands:
    """Lambda calculus commands for TESH"""
    
    def __init__(self):
        self.step_mode = False
        self.show_types = True
        self.show_encoding = False
        self.max_steps = 1000
    
    def handle_command(self, command_line: str) -> str:
        """
        Handle lambda-related commands
        
        Commands:
        - :lambda <term>  - Parse and evaluate
        - :reduce <term>  - Show reduction steps
        - :type <term>    - Show type
        - :encode <term>  - Show ternary encoding
        - :step           - Toggle step mode
        - :library        - Show predefined terms
        - <term>          - Direct evaluation
        """
        
        command_line = command_line.strip()
        
        # Check if it's a library lookup
        if command_line in LAMBDA_LIBRARY:
            term_str = LAMBDA_LIBRARY[command_line]
            return f"{command_line} = {term_str}"
        
        # Parse command
        parts = command_line.split(maxsplit=1)
        if not parts:
            return ""
        
        cmd = parts[0]
        arg = parts[1] if len(parts) > 1 else ""
        
        # Expand library references in arg
        for key, value in LAMBDA_LIBRARY.items():
            arg = arg.replace(key, value)
        
        # Handle commands
        if cmd == ':lambda':
            return self.cmd_lambda(arg)
        elif cmd == ':reduce':
            return self.cmd_reduce(arg)
        elif cmd == ':type':
            return self.cmd_type(arg)
        elif cmd == ':encode':
            return self.cmd_encode(arg)
        elif cmd == ':step':
            self.step_mode = not self.step_mode
            return f"Step mode: {'ON' if self.step_mode else 'OFF'}"
        elif cmd == ':library':
            return self.cmd_library()
        elif cmd.startswith(':'):
            if cmd in LAMBDA_LIBRARY:
                return f"{cmd} = {LAMBDA_LIBRARY[cmd]}"
            else:
                return f"Unknown command: {cmd}"
        else:
            # Direct lambda term evaluation
            return self.cmd_lambda(command_line)
    
    def cmd_lambda(self, term_str: str) -> str:
        """Evaluate a lambda term"""
        if not parse:
            return "Error: Lambda3 not available"
        
        try:
            # Parse
            term = parse(term_str)
            result_lines = [f"Term: {term}"]
            
            # Type inference
            if self.show_types:
                try:
                    type_ = infer_type(term)
                    result_lines.append(f"Type: {type_}")
                except Exception as e:
                    result_lines.append(f"Type: (error: {e})")
            
            # Reduce
            reduced = reduce(term, max_steps=self.max_steps)
            result_lines.append(f"Result: {reduced}")
            
            # Encoding
            if self.show_encoding:
                trits = encode(reduced)
                eff = encoding_efficiency(reduced)
                result_lines.append(f"Trits: {len(trits)}, Savings: {eff['savings_percent']:.1f}%")
            
            return "\n".join(result_lines)
        
        except Exception as e:
            return f"Error: {e}"
    
    def cmd_reduce(self, term_str: str) -> str:
        """Show reduction steps"""
        if not parse:
            return "Error: Lambda3 not available"
        
        try:
            term = parse(term_str)
            result_lines = [f"Reducing: {term}", "-" * 60]
            
            # For now, just show final result
            # TODO: Implement step-by-step reduction
            reduced = reduce(term, max_steps=self.max_steps)
            result_lines.append(f"→ {reduced}")
            
            return "\n".join(result_lines)
        
        except Exception as e:
            return f"Error: {e}"
    
    def cmd_type(self, term_str: str) -> str:
        """Show type of a term"""
        if not parse:
            return "Error: Lambda3 not available"
        
        try:
            term = parse(term_str)
            type_ = infer_type(term)
            return f"{term} : {type_}"
        
        except Exception as e:
            return f"Error: {e}"
    
    def cmd_encode(self, term_str: str) -> str:
        """Show ternary encoding"""
        if not parse:
            return "Error: Lambda3 not available"
        
        try:
            term = parse(term_str)
            trits = encode(term)
            eff = encoding_efficiency(term)
            
            result_lines = [
                f"Term: {term}",
                f"Trits: {trits}",
                f"Count: {eff['num_trits']} trits",
                f"Ternary: {eff['ternary_bits']:.2f} bits",
                f"Binary: {eff['binary_bits']} bits",
                f"Savings: {eff['savings_percent']:.1f}%"
            ]
            
            return "\n".join(result_lines)
        
        except Exception as e:
            return f"Error: {e}"
    
    def cmd_library(self) -> str:
        """Show predefined library"""
        result_lines = ["Lambda Library:", "=" * 60]
        
        # Group by category
        categories = {
            'Numbers': [':0', ':1', ':2', ':3', ':4', ':5'],
            'Booleans': [':true', ':false', ':not', ':and', ':or'],
            'Combinators': [':I', ':K', ':S', ':Y'],
            'Arithmetic': [':succ', ':add', ':mult', ':pred'],
            'Lists': [':cons', ':nil', ':car', ':cdr'],
        }
        
        for category, keys in categories.items():
            result_lines.append(f"\n{category}:")
            for key in keys:
                if key in LAMBDA_LIBRARY:
                    result_lines.append(f"  {key:8s} = {LAMBDA_LIBRARY[key]}")
        
        return "\n".join(result_lines)


# ============================================================================
# MAIN (for testing)
# ============================================================================

if __name__ == '__main__':
    print("Lambda Commands for TESH")
    print("=" * 60)
    
    lc = LambdaCommands()
    
    test_commands = [
        ':library',
        ':I',
        r'(\x.x) y',
        ':type \x.x',
        ':encode :I',
        ':0',
        ':true',
    ]
    
    for cmd in test_commands:
        print(f"\n> {cmd}")
        result = lc.handle_command(cmd)
        print(result)
    
    print("\n" + "=" * 60)
    print("Lambda Commands Test Complete")

