"""
Lambda Formatter for Lambda³ NLG
Format lambda terms and proofs for natural language output
"""

from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import re

class FormatStyle(Enum):
    """Formatting styles"""
    PLAIN = "plain"
    MARKDOWN = "markdown"
    LATEX = "latex"
    HTML = "html"
    COLORED = "colored"

@dataclass
class FormatOptions:
    """Formatting options"""
    style: FormatStyle
    max_line_length: int = 80
    indent_size: int = 2
    show_types: bool = True
    show_steps: bool = True
    colorize: bool = False

class LambdaFormatter:
    """
    Lambda formatter for Lambda³ NLG
    
    Formats lambda terms for:
    - Natural language output
    - Markdown documentation
    - LaTeX papers
    - HTML web pages
    - Colored terminal output
    """
    
    def __init__(self):
        self.colors = {
            'lambda': '\033[94m',      # Blue
            'variable': '\033[92m',    # Green
            'application': '\033[93m', # Yellow
            'type': '\033[95m',        # Magenta
            'reset': '\033[0m'          # Reset
        }
        
        self.latex_symbols = {
            'lambda': '\\lambda',
            'arrow': '\\rightarrow',
            'times': '\\times',
            'forall': '\\forall',
            'exists': '\\exists'
        }
    
    def format_term(self, term: str, options: FormatOptions = None) -> str:
        """Format a lambda term"""
        if options is None:
            options = FormatOptions(FormatStyle.PLAIN)
        
        if options.style == FormatStyle.PLAIN:
            return self._format_plain(term)
        elif options.style == FormatStyle.MARKDOWN:
            return self._format_markdown(term)
        elif options.style == FormatStyle.LATEX:
            return self._format_latex(term)
        elif options.style == FormatStyle.HTML:
            return self._format_html(term)
        elif options.style == FormatStyle.COLORED:
            return self._format_colored(term)
        else:
            return term
    
    def _format_plain(self, term: str) -> str:
        """Format as plain text"""
        # Replace lambda symbols with ASCII
        term = term.replace('λ', '\\')
        return term
    
    def _format_markdown(self, term: str) -> str:
        """Format as Markdown"""
        # Escape backslashes for Markdown
        term = term.replace('\\', '\\\\')
        return f"`{term}`"
    
    def _format_latex(self, term: str) -> str:
        """Format as LaTeX"""
        # Replace lambda with LaTeX symbol
        term = term.replace('\\', '\\lambda')
        # Replace arrows
        term = term.replace('->', '\\rightarrow')
        return f"\\texttt{{{term}}}"
    
    def _format_html(self, term: str) -> str:
        """Format as HTML"""
        # Escape HTML characters
        term = term.replace('&', '&amp;')
        term = term.replace('<', '&lt;')
        term = term.replace('>', '&gt;')
        return f"<code>{term}</code>"
    
    def _format_colored(self, term: str) -> str:
        """Format with colors"""
        # Colorize different parts
        colored = term
        
        # Color lambda symbols
        colored = re.sub(r'\\', f"{self.colors['lambda']}\\\\{self.colors['reset']}", colored)
        
        # Color variables
        colored = re.sub(r'([a-zA-Z]\w*)', f"{self.colors['variable']}\\1{self.colors['reset']}", colored)
        
        # Color applications
        colored = re.sub(r'\(', f"{self.colors['application']}({self.colors['reset']}", colored)
        colored = re.sub(r'\)', f"{self.colors['application']}){self.colors['reset']}", colored)
        
        return colored
    
    def format_reduction_steps(self, steps: List[Tuple[str, str]], options: FormatOptions = None) -> str:
        """Format reduction steps"""
        if options is None:
            options = FormatOptions(FormatStyle.PLAIN)
        
        if options.style == FormatStyle.PLAIN:
            return self._format_steps_plain(steps)
        elif options.style == FormatStyle.MARKDOWN:
            return self._format_steps_markdown(steps)
        elif options.style == FormatStyle.LATEX:
            return self._format_steps_latex(steps)
        else:
            return self._format_steps_plain(steps)
    
    def _format_steps_plain(self, steps: List[Tuple[str, str]]) -> str:
        """Format steps as plain text"""
        result = []
        for i, (term, rule) in enumerate(steps):
            result.append(f"Step {i+1}: {term} (using {rule})")
        return "\n".join(result)
    
    def _format_steps_markdown(self, steps: List[Tuple[str, str]]) -> str:
        """Format steps as Markdown"""
        result = ["## Reduction Steps\n"]
        for i, (term, rule) in enumerate(steps):
            result.append(f"**Step {i+1}:** `{term}` (using {rule})")
        return "\n".join(result)
    
    def _format_steps_latex(self, steps: List[Tuple[str, str]]) -> str:
        """Format steps as LaTeX"""
        result = ["\\begin{enumerate}"]
        for i, (term, rule) in enumerate(steps):
            formatted_term = self._format_latex(term)
            result.append(f"\\item {formatted_term} (using {rule})")
        result.append("\\end{enumerate}")
        return "\n".join(result)
    
    def format_type_signature(self, term: str, type_str: str, options: FormatOptions = None) -> str:
        """Format type signature"""
        if options is None:
            options = FormatOptions(FormatStyle.PLAIN)
        
        if options.style == FormatStyle.PLAIN:
            return f"{term} : {type_str}"
        elif options.style == FormatStyle.MARKDOWN:
            return f"`{term}` : `{type_str}`"
        elif options.style == FormatStyle.LATEX:
            formatted_term = self._format_latex(term)
            return f"{formatted_term} : {type_str}"
        else:
            return f"{term} : {type_str}"
    
    def format_proof(self, proof: Dict, options: FormatOptions = None) -> str:
        """Format a complete proof"""
        if options is None:
            options = FormatOptions(FormatStyle.PLAIN)
        
        title = proof.get('title', 'Proof')
        steps = proof.get('steps', [])
        conclusion = proof.get('conclusion', '')
        
        if options.style == FormatStyle.MARKDOWN:
            return self._format_proof_markdown(title, steps, conclusion)
        elif options.style == FormatStyle.LATEX:
            return self._format_proof_latex(title, steps, conclusion)
        else:
            return self._format_proof_plain(title, steps, conclusion)
    
    def _format_proof_plain(self, title: str, steps: List[Dict], conclusion: str) -> str:
        """Format proof as plain text"""
        result = [f"Proof: {title}", "=" * len(title)]
        
        for i, step in enumerate(steps):
            result.append(f"Step {i+1}: {step.get('description', '')}")
            if 'term' in step:
                result.append(f"  Term: {step['term']}")
            if 'rule' in step:
                result.append(f"  Rule: {step['rule']}")
        
        result.append(f"Conclusion: {conclusion}")
        return "\n".join(result)
    
    def _format_proof_markdown(self, title: str, steps: List[Dict], conclusion: str) -> str:
        """Format proof as Markdown"""
        result = [f"# {title}", ""]
        
        for i, step in enumerate(steps):
            result.append(f"## Step {i+1}: {step.get('description', '')}")
            if 'term' in step:
                result.append(f"```\n{step['term']}\n```")
            if 'rule' in step:
                result.append(f"**Rule:** {step['rule']}")
            result.append("")
        
        result.append(f"## Conclusion\n{conclusion}")
        return "\n".join(result)
    
    def _format_proof_latex(self, title: str, steps: List[Dict], conclusion: str) -> str:
        """Format proof as LaTeX"""
        result = [f"\\section{{{title}}}", ""]
        
        for i, step in enumerate(steps):
            result.append(f"\\subsection{{Step {i+1}: {step.get('description', '')}}}")
            if 'term' in step:
                formatted_term = self._format_latex(step['term'])
                result.append(f"\\texttt{{{formatted_term}}}")
            if 'rule' in step:
                result.append(f"\\textbf{{Rule:}} {step['rule']}")
            result.append("")
        
        result.append(f"\\subsection{{Conclusion}}\n{conclusion}")
        return "\n".join(result)
    
    def format_tutorial(self, tutorial: Dict, options: FormatOptions = None) -> str:
        """Format tutorial content"""
        if options is None:
            options = FormatOptions(FormatStyle.PLAIN)
        
        title = tutorial.get('title', 'Tutorial')
        sections = tutorial.get('sections', [])
        
        if options.style == FormatStyle.MARKDOWN:
            return self._format_tutorial_markdown(title, sections)
        else:
            return self._format_tutorial_plain(title, sections)
    
    def _format_tutorial_plain(self, title: str, sections: List[Dict]) -> str:
        """Format tutorial as plain text"""
        result = [f"Tutorial: {title}", "=" * len(title), ""]
        
        for section in sections:
            result.append(f"## {section.get('title', '')}")
            result.append(section.get('content', ''))
            result.append("")
        
        return "\n".join(result)
    
    def _format_tutorial_markdown(self, title: str, sections: List[Dict]) -> str:
        """Format tutorial as Markdown"""
        result = [f"# {title}", ""]
        
        for section in sections:
            result.append(f"## {section.get('title', '')}")
            result.append(section.get('content', ''))
            result.append("")
        
        return "\n".join(result)

# ============================================================================
# DEMO
# ============================================================================

def demo_lambda_formatter():
    """Demonstrate lambda formatting"""
    print("="*60)
    print("  Lambda³ Lambda Formatter Demo")
    print("="*60)
    
    formatter = LambdaFormatter()
    
    # Test different formats
    test_term = "\\x.x"
    
    print("1. Plain Format:")
    plain = formatter.format_term(test_term, FormatOptions(FormatStyle.PLAIN))
    print(f"  {plain}")
    
    print("\n2. Markdown Format:")
    markdown = formatter.format_term(test_term, FormatOptions(FormatStyle.MARKDOWN))
    print(f"  {markdown}")
    
    print("\n3. LaTeX Format:")
    latex = formatter.format_term(test_term, FormatOptions(FormatStyle.LATEX))
    print(f"  {latex}")
    
    print("\n4. Colored Format:")
    colored = formatter.format_term(test_term, FormatOptions(FormatStyle.COLORED))
    print(f"  {colored}")
    
    # Test reduction steps
    print("\n5. Reduction Steps:")
    steps = [("(\\x.x) y", "beta"), ("y", "simplification")]
    steps_formatted = formatter.format_reduction_steps(steps, FormatOptions(FormatStyle.MARKDOWN))
    print(steps_formatted)
    
    # Test type signature
    print("\n6. Type Signature:")
    type_sig = formatter.format_type_signature("\\x.x", "A -> A", FormatOptions(FormatStyle.MARKDOWN))
    print(f"  {type_sig}")

def main():
    print("="*60)
    print("  Lambda³ Lambda Formatter")
    print("  Natural Language Formatting")
    print("="*60)
    
    demo_lambda_formatter()
    
    print("\n" + "="*60)
    print("Formatter Features:")
    print("  Plain text formatting")
    print("  Markdown formatting")
    print("  LaTeX formatting")
    print("  HTML formatting")
    print("  Colored terminal output")
    print("  Proof formatting")
    print("  Tutorial formatting")
    print("="*60)
    
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
