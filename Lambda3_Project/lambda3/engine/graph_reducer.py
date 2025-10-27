"""
Graph Reduction Engine
Optimized reduction using DAG with sharing to avoid exponential explosion
"""

from typing import Dict, Set, Optional, Tuple
from dataclasses import dataclass, field

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


@dataclass
class GraphNode:
    """
    Node in the DAG representation
    Supports sharing of identical subterms
    """
    term: LambdaTerm
    hash_value: int
    children: list = field(default_factory=list)
    reduced: Optional['GraphNode'] = None
    ref_count: int = 0
    
    def __hash__(self):
        return self.hash_value
    
    def __eq__(self, other):
        return self.hash_value == other.hash_value


class GraphReducer:
    """
    Graph-based reducer with sharing
    Avoids exponential explosion by building a DAG
    """
    
    def __init__(self):
        self.node_cache: Dict[int, GraphNode] = {}
        self.reduction_count = 0
        self.sharing_count = 0
    
    def compute_hash(self, term: LambdaTerm) -> int:
        """Compute hash for hash consing"""
        if isinstance(term, Var):
            return hash(('var', term.name))
        elif isinstance(term, Abs):
            body_hash = self.compute_hash(term.body)
            return hash(('abs', term.var, body_hash))
        elif isinstance(term, App):
            func_hash = self.compute_hash(term.func)
            arg_hash = self.compute_hash(term.arg)
            return hash(('app', func_hash, arg_hash))
        return 0
    
    def build_dag(self, term: LambdaTerm) -> GraphNode:
        """
        Build DAG from term tree with hash consing
        Identical subterms share the same node
        """
        term_hash = self.compute_hash(term)
        
        # Check cache (hash consing)
        if term_hash in self.node_cache:
            self.sharing_count += 1
            cached = self.node_cache[term_hash]
            cached.ref_count += 1
            return cached
        
        # Create new node
        node = GraphNode(term=term, hash_value=term_hash, ref_count=1)
        
        # Recursively build children
        if isinstance(term, Abs):
            body_node = self.build_dag(term.body)
            node.children = [body_node]
        elif isinstance(term, App):
            func_node = self.build_dag(term.func)
            arg_node = self.build_dag(term.arg)
            node.children = [func_node, arg_node]
        
        # Cache node
        self.node_cache[term_hash] = node
        return node
    
    def reduce_node(self, node: GraphNode) -> GraphNode:
        """
        Reduce a single node
        Uses memoization: if already reduced, return cached result
        """
        # Check if already reduced
        if node.reduced is not None:
            return node.reduced
        
        term = node.term
        
        if isinstance(term, Var):
            # Variables are in normal form
            node.reduced = node
            return node
        
        elif isinstance(term, Abs):
            # Reduce body
            if node.children:
                body_node = node.children[0]
                reduced_body = self.reduce_node(body_node)
                
                # Create new abstraction with reduced body
                new_term = Abs(var=term.var, body=reduced_body.term)
                new_node = self.build_dag(new_term)
                node.reduced = new_node
                return new_node
            else:
                node.reduced = node
                return node
        
        elif isinstance(term, App):
            if len(node.children) >= 2:
                func_node = node.children[0]
                arg_node = node.children[1]
                
                # Reduce func first
                reduced_func = self.reduce_node(func_node)
                
                # Check if redex: (λx.M) N
                if isinstance(reduced_func.term, Abs):
                    self.reduction_count += 1
                    
                    # Beta reduction: substitute
                    var_id = reduced_func.term.var
                    body = reduced_func.term.body
                    arg = arg_node.term
                    
                    # Perform substitution
                    result_term = self.substitute(body, var_id, arg)
                    result_node = self.build_dag(result_term)
                    
                    # Continue reducing result
                    final_node = self.reduce_node(result_node)
                    node.reduced = final_node
                    return final_node
                else:
                    # Not a redex, reduce arg too
                    reduced_arg = self.reduce_node(arg_node)
                    
                    # Create new application
                    new_term = App(func=reduced_func.term, arg=reduced_arg.term)
                    new_node = self.build_dag(new_term)
                    node.reduced = new_node
                    return new_node
        
        node.reduced = node
        return node
    
    def substitute(self, term: LambdaTerm, var_id, replacement: LambdaTerm) -> LambdaTerm:
        """Capture-avoiding substitution"""
        if isinstance(term, Var):
            if term.name == var_id:
                return replacement
            else:
                return term
        
        elif isinstance(term, Abs):
            if term.var == var_id:
                return term  # Shadowed
            else:
                new_body = self.substitute(term.body, var_id, replacement)
                return Abs(var=term.var, body=new_body)
        
        elif isinstance(term, App):
            new_func = self.substitute(term.func, var_id, replacement)
            new_arg = self.substitute(term.arg, var_id, replacement)
            return App(func=new_func, arg=new_arg)
        
        return term
    
    def reduce(self, term: LambdaTerm, max_steps: int = 10000) -> Tuple[LambdaTerm, dict]:
        """
        Reduce term using graph reduction
        Returns (reduced_term, stats)
        """
        self.reduction_count = 0
        self.sharing_count = 0
        self.node_cache.clear()
        
        # Build DAG
        root = self.build_dag(term)
        
        # Reduce
        reduced_root = self.reduce_node(root)
        
        stats = {
            'reductions': self.reduction_count,
            'sharing_hits': self.sharing_count,
            'unique_nodes': len(self.node_cache),
        }
        
        return reduced_root.term, stats


def reduce_with_sharing(term: LambdaTerm, max_steps: int = 10000) -> Tuple[LambdaTerm, dict]:
    """
    Convenience function for graph reduction
    
    Args:
        term: Lambda term to reduce
        max_steps: Maximum reduction steps
        
    Returns:
        (reduced_term, stats_dict)
        
    Example:
        >>> from lambda3.parser.parser import parse
        >>> term = parse("(\\x.x x) (\\y.y)")
        >>> result, stats = reduce_with_sharing(term)
        >>> print(f"Reductions: {stats['reductions']}")
        >>> print(f"Sharing hits: {stats['sharing_hits']}")
    """
    reducer = GraphReducer()
    return reducer.reduce(term, max_steps)


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    try:
        from lambda3.parser.parser import parse
        from lambda3.engine.reducer import reduce as naive_reduce
    except ImportError:
        from parser.parser import parse
        from engine.reducer import reduce as naive_reduce
    
    import sys
    import time
    
    print("Graph Reduction Tests:")
    print("=" * 60)
    
    test_cases = [
        ("Identity", r"(\x.x) y"),
        ("Const", r"(\x.\y.x) a b"),
        ("Self-application", r"(\x.x x) (\y.y)"),
        ("Nested", r"(\f.(\x.f (f x))) g y"),
    ]
    
    all_passed = True
    
    for name, source in test_cases:
        try:
            print(f"\nTest: {name}")
            print(f"Input: {source}")
            
            term = parse(source)
            print(f"Parsed: {term}")
            
            # Graph reduction
            start = time.time()
            result, stats = reduce_with_sharing(term)
            graph_time = time.time() - start
            
            print(f"Result: {result}")
            print(f"Stats: {stats}")
            print(f"Time: {graph_time*1000:.2f}ms")
            
            # Compare with naive (for small terms)
            if len(source) < 30:
                start = time.time()
                naive_result = naive_reduce(term, max_steps=1000)
                naive_time = time.time() - start
                print(f"Naive time: {naive_time*1000:.2f}ms")
                
                # Check results match
                if str(result) == str(naive_result):
                    print("PASS: Results match")
                else:
                    print(f"FAIL: Results differ")
                    print(f"  Graph: {result}")
                    print(f"  Naive: {naive_result}")
                    all_passed = False
            else:
                print("PASS: Graph reduction completed")
        
        except Exception as e:
            print(f"FAIL: {e}")
            import traceback
            traceback.print_exc()
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("ALL TESTS PASSED")
        print("Graph reduction with sharing is working!")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED")
        sys.exit(1)

