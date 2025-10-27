"""
Lambda3 REPL - Interactive Lambda Calculus REPL
Complete Read-Eval-Print Loop for Lambda3
"""

from lambda3.parser.parser import parse
from lambda3.engine.reducer import reduce, is_normal_form, reduce_step
from lambda3.ternary.encoder import encode, decode, encoding_efficiency


# ============================================================================
# CHURCH ENCODINGS
# ============================================================================

def church_numeral(n: int) -> str:
    """Create Church numeral as string"""
    if n == 0:
        return "\\f.\\x.x"
    elif n == 1:
        return "\\f.\\x.f x"
    elif n == 2:
        return "\\f.\\x.f (f x)"
    elif n == 3:
        return "\\f.\\x.f (f (f x))"
    else:
        # Generate for any n
        body = "x"
        for _ in range(n):
            body = f"f ({body})"
        return f"\\f.\\x.{body}"


# Predefined terms
PREDEF = {
    ':0': church_numeral(0),
    ':1': church_numeral(1),
    ':2': church_numeral(2),
    ':3': church_numeral(3),
    ':I': '\\x.x',  # Identity
    ':K': '\\x.\\y.x',  # Const
    ':S': '\\x.\\y.\\z.x z (y z)',  # S combinator
    ':true': '\\x.\\y.x',  # Church true
    ':false': '\\x.\\y.y',  # Church false
}


class LambdaREPL:
    """
    Interactive Lambda Calculus REPL for Lambda3
    """
    
    def __init__(self):
        self.history = []
        self.verbose = False
    
    def run(self):
        """Run interactive REPL"""
        print("="*60)
        print("  Lambda3 REPL - Ternary Lambda Calculus")
        print("  Native Ternary Computing")
        print("="*60)
        print()
        print("Commands:")
        print("  <term>           - Parse and reduce")
        print("  :parse <term>    - Parse and show AST")
        print("  :reduce <term>   - Reduce to normal form")
        print("  :step <term>     - Show reduction steps")
        print("  :encode <term>   - Show ternary encoding")
        print("  :list            - List predefined terms")
        print("  :help            - Show this help")
        print("  :quit            - Exit REPL")
        print()
        print("Predefined:")
        print("  :0, :1, :2, :3   - Church numerals")
        print("  :true, :false    - Church booleans")
        print("  :I, :K, :S       - Combinators")
        print()
        
        while True:
            try:
                line = input("lambda> ").strip()
                if not line:
                    continue
                
                self.history.append(line)
                
                # Commands
                if line == ':quit' or line == ':q':
                    print("Goodbye!")
                    break
                
                elif line == ':help' or line == ':h':
                    self.show_help()
                
                elif line == ':list':
                    self.show_predefined()
                
                elif line.startswith(':parse '):
                    self.cmd_parse(line[7:])
                
                elif line.startswith(':reduce '):
                    self.cmd_reduce(line[8:])
                
                elif line.startswith(':step '):
                    self.cmd_step(line[6:])
                
                elif line.startswith(':encode '):
                    self.cmd_encode(line[8:])
                
                elif line in PREDEF:
                    # Show predefined term
                    print(f"{line} = {PREDEF[line]}")
                    term = parse(PREDEF[line])
                    print(f"Parsed: {term}")
                
                else:
                    # Default: parse and reduce
                    self.cmd_default(line)
            
            except KeyboardInterrupt:
                print("\nInterrupted. Type :quit to exit.")
            except Exception as e:
                print(f"Error: {e}")
                if self.verbose:
                    import traceback
                    traceback.print_exc()
    
    def resolve(self, source: str) -> str:
        """Resolve predefined terms"""
        source = source.strip()
        if source in PREDEF:
            return PREDEF[source]
        return source
    
    def cmd_parse(self, source: str):
        """Parse and show AST"""
        source = self.resolve(source)
        term = parse(source)
        print(f"Source: {source}")
        print(f"AST:    {term}")
    
    def cmd_reduce(self, source: str):
        """Reduce to normal form"""
        source = self.resolve(source)
        term = parse(source)
        print(f"Input:  {term}")
        result = reduce(term)
        print(f"Result: {result}")
        
        # Show if already in normal form
        if str(term) == str(result):
            print("(already in normal form)")
    
    def cmd_step(self, source: str):
        """Show reduction steps"""
        source = self.resolve(source)
        term = parse(source)
        
        print(f"Step 0: {term}")
        
        current = term
        for step in range(1, 20):
            next_term, changed = reduce_step(current)
            if not changed:
                print(f"Normal form reached at step {step-1}")
                break
            print(f"Step {step}: {next_term}")
            current = next_term
    
    def cmd_encode(self, source: str):
        """Show ternary encoding"""
        source = self.resolve(source)
        term = parse(source)
        
        trits = encode(term)
        eff = encoding_efficiency(term)
        
        print(f"Term:         {term}")
        print(f"Trits:        {trits}")
        print(f"  Num trits:  {eff['num_trits']}")
        print(f"  Ternary:    {eff['ternary_bits']:.1f} bits")
        print(f"  Binary:     {eff['binary_bits']} bits")
        print(f"  Savings:    {eff['savings_percent']:.1f}%")
    
    def cmd_default(self, source: str):
        """Default: parse and reduce"""
        source = self.resolve(source)
        term = parse(source)
        result = reduce(term)
        print(f"{term} => {result}")
    
    def show_predefined(self):
        """Show all predefined terms"""
        print("\nPredefined terms:")
        print("-" * 60)
        for name, value in sorted(PREDEF.items()):
            print(f"  {name:10s} = {value}")
        print("-" * 60)
    
    def show_help(self):
        r"""Show detailed help"""
        print(r"""
Lambda Calculus Syntax:
  \x.M                 - Abstraction (lambda)
  M N                  - Application
  x, y, z              - Variables
  (...)                - Grouping

Examples:
  \x.x                 - Identity function
  (\x.x) y             - Apply identity to y
  \x.\y.x              - Constant function
  (\x.\y.x) a b        - Const applied twice
  
  :0                   - Church numeral 0
  :1                   - Church numeral 1
  :I                   - Identity combinator
  :K                   - Const combinator
  :S                   - S combinator
  
Commands:
  <term>               - Parse and reduce
  :parse <term>        - Parse and show AST
  :reduce <term>       - Reduce to normal form
  :step <term>         - Show reduction steps
  :encode <term>       - Show ternary encoding
  :list                - List predefined terms
  :help                - This help
  :quit                - Exit
        """)


def main():
    """Entry point for Lambda3 REPL"""
    repl = LambdaREPL()
    repl.run()


if __name__ == '__main__':
    main()

