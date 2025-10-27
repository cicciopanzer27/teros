"""
Lambda REPL - Interactive Lambda Calculus REPL for TESH
"""

from .tvm_backend import (
    LambdaTerm, lambda_reduce, lambda_parse,
    church_numeral, church_boolean,
    COMBINATOR_I, COMBINATOR_K, COMBINATOR_S,
    CHURCH_ZERO, CHURCH_ONE, CHURCH_TWO,
    CHURCH_TRUE, CHURCH_FALSE
)


class LambdaREPL:
    """Interactive Lambda Calculus REPL"""
    
    def __init__(self):
        self.env = {}
        self.history = []
        self.verbose = False
        
        # Preload standard library
        self.env[':0'] = CHURCH_ZERO
        self.env[':1'] = CHURCH_ONE
        self.env[':2'] = CHURCH_TWO
        self.env[':3'] = church_numeral(3)
        self.env[':4'] = church_numeral(4)
        self.env[':5'] = church_numeral(5)
        
        self.env[':true'] = CHURCH_TRUE
        self.env[':false'] = CHURCH_FALSE
        
        self.env[':I'] = COMBINATOR_I
        self.env[':K'] = COMBINATOR_K
        self.env[':S'] = COMBINATOR_S
    
    def run(self):
        """Run interactive REPL"""
        print("╔═══════════════════════════════════════════════════╗")
        print("║   Lambda³ REPL - Ternary Lambda Calculus         ║")
        print("║   Running on TEROS Native                        ║")
        print("╚═══════════════════════════════════════════════════╝")
        print()
        print("Commands:")
        print("  :parse <term>    - Parse and show AST")
        print("  :reduce <term>   - Reduce to normal form")
        print("  :step <term>     - Show reduction steps")
        print("  :help            - Show this help")
        print("  :quit            - Exit REPL")
        print()
        print("Predefined:")
        print("  :0, :1, :2, ...  - Church numerals")
        print("  :true, :false    - Church booleans")
        print("  :I, :K, :S       - Combinators")
        print()
        
        while True:
            try:
                line = input("λ> ").strip()
                if not line:
                    continue
                
                self.history.append(line)
                
                if line == ':quit' or line == ':q':
                    print("Goodbye!")
                    break
                elif line == ':help' or line == ':h':
                    self.show_help()
                elif line.startswith(':parse '):
                    self.cmd_parse(line[7:])
                elif line.startswith(':reduce '):
                    self.cmd_reduce(line[8:])
                elif line.startswith(':step '):
                    self.cmd_step(line[6:])
                elif line.startswith(':encode '):
                    self.cmd_encode(line[8:])
                elif line.startswith(':'):
                    # Check if it's a predefined term
                    if line in self.env:
                        print(f"{line} = {self.env[line]}")
                    else:
                        print(f"Unknown command: {line}")
                else:
                    # Default: parse and reduce
                    self.cmd_reduce(line)
            
            except KeyboardInterrupt:
                print("\nInterrupted. Type :quit to exit.")
            except Exception as e:
                print(f"Error: {e}")
    
    def cmd_parse(self, source: str):
        """Parse and show AST"""
        term = self.resolve(source)
        if term:
            print(f"Parsed: {term}")
        else:
            print("Parse error")
    
    def cmd_reduce(self, source: str):
        """Reduce to normal form"""
        term = self.resolve(source)
        if not term:
            print("Parse error")
            return
        
        print(f"Input:  {term}")
        result = lambda_reduce(term, max_steps=1000)
        print(f"Result: {result}")
    
    def cmd_step(self, source: str):
        """Show reduction steps"""
        from .tvm_backend import lambda_reduce_step
        
        term = self.resolve(source)
        if not term:
            print("Parse error")
            return
        
        print(f"Step 0: {term}")
        
        current = term
        for step in range(1, 20):
            next_term, changed = lambda_reduce_step(current)
            if not changed:
                print(f"Normal form reached at step {step-1}")
                break
            print(f"Step {step}: {next_term}")
            current = next_term
    
    def cmd_encode(self, source: str):
        """Show ternary encoding"""
        term = self.resolve(source)
        if not term:
            print("Parse error")
            return
        
        # Encode as ternary sequence
        encoding = self.encode_ternary(term)
        print(f"Term:     {term}")
        print(f"Ternary:  {encoding}")
        print(f"Length:   {len(encoding)} trits")
    
    def encode_ternary(self, term: LambdaTerm) -> str:
        """
        Encode lambda term as ternary string
        -1 (Var), 0 (Abs), 1 (App)
        """
        from .tvm_backend import LambdaTermType
        
        if term.term_type == LambdaTermType.VAR:
            return f"-1[{term.data}]"
        elif term.term_type == LambdaTermType.ABS:
            var_id, body = term.data
            body_enc = self.encode_ternary(body)
            return f"0[{var_id},{body_enc}]"
        elif term.term_type == LambdaTermType.APP:
            func, arg = term.data
            func_enc = self.encode_ternary(func)
            arg_enc = self.encode_ternary(arg)
            return f"1[{func_enc},{arg_enc}]"
        return "?"
    
    def resolve(self, source: str) -> LambdaTerm:
        """
        Resolve source to lambda term
        Checks predefined terms first
        """
        source = source.strip()
        
        # Check if it's a predefined term
        if source in self.env:
            return self.env[source]
        
        # Try to parse
        return lambda_parse(source)
    
    def show_help(self):
        """Show detailed help"""
        print("""
Lambda Calculus Syntax:
  λx.M   or   \\x.M    - Abstraction (lambda)
  M N                  - Application
  x, y, z              - Variables
  (...)                - Grouping

Examples:
  λx.x                 - Identity function
  (λx.x) y             - Apply identity to y
  λx.λy.x              - Constant function
  (λx.λy.x) a b        - Const applied twice
  
  :0                   - Church numeral 0
  :1                   - Church numeral 1
  (:I) y               - Identity applied to y
  
Commands:
  :parse <term>        - Parse and show structure
  :reduce <term>       - Reduce to normal form
  :step <term>         - Show reduction steps
  :encode <term>       - Show ternary encoding
  :help                - This help
  :quit                - Exit
""")


def main():
    """Entry point for Lambda REPL"""
    repl = LambdaREPL()
    repl.run()


if __name__ == '__main__':
    main()

