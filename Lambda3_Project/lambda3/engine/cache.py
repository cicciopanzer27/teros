"""
Memoization & Caching for Lambda Reduction
LRU cache for reduction results and type information
"""

from typing import Optional, Dict, Tuple, Any
from collections import OrderedDict
from dataclasses import dataclass

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App


@dataclass
class CacheStats:
    """Cache statistics"""
    hits: int = 0
    misses: int = 0
    evictions: int = 0
    
    @property
    def hit_rate(self) -> float:
        total = self.hits + self.misses
        return self.hits / total if total > 0 else 0.0


class LRUCache:
    """
    LRU (Least Recently Used) Cache
    Generic cache implementation
    """
    
    def __init__(self, maxsize: int = 1024):
        self.maxsize = maxsize
        self.cache: OrderedDict = OrderedDict()
        self.stats = CacheStats()
    
    def get(self, key: Any) -> Optional[Any]:
        """Get value from cache"""
        if key in self.cache:
            self.stats.hits += 1
            # Move to end (most recently used)
            self.cache.move_to_end(key)
            return self.cache[key]
        else:
            self.stats.misses += 1
            return None
    
    def put(self, key: Any, value: Any):
        """Put value in cache"""
        if key in self.cache:
            # Update existing
            self.cache.move_to_end(key)
            self.cache[key] = value
        else:
            # Add new
            if len(self.cache) >= self.maxsize:
                # Evict least recently used
                self.cache.popitem(last=False)
                self.stats.evictions += 1
            self.cache[key] = value
    
    def clear(self):
        """Clear cache"""
        self.cache.clear()
        self.stats = CacheStats()
    
    def get_stats(self) -> CacheStats:
        """Get cache statistics"""
        return self.stats


class ReductionCache:
    """
    Cache for lambda reduction results
    Maps (term_hash, context) -> reduced_term
    """
    
    def __init__(self, maxsize: int = 1024):
        self.cache = LRUCache(maxsize=maxsize)
    
    def compute_hash(self, term: LambdaTerm) -> int:
        """Compute hash for term"""
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
    
    def get(self, term: LambdaTerm, context: Optional[Dict] = None) -> Optional[LambdaTerm]:
        """Get cached reduction result"""
        key = (self.compute_hash(term), str(context) if context else '')
        return self.cache.get(key)
    
    def put(self, term: LambdaTerm, result: LambdaTerm, context: Optional[Dict] = None):
        """Cache reduction result"""
        key = (self.compute_hash(term), str(context) if context else '')
        self.cache.put(key, result)
    
    def get_stats(self) -> CacheStats:
        """Get cache statistics"""
        return self.cache.get_stats()
    
    def clear(self):
        """Clear cache"""
        self.cache.clear()


class NormalFormCache:
    """
    Cache for normal form results
    Stores whether a term is in normal form
    """
    
    def __init__(self, maxsize: int = 512):
        self.cache = LRUCache(maxsize=maxsize)
    
    def compute_hash(self, term: LambdaTerm) -> int:
        """Compute hash for term"""
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
    
    def get(self, term: LambdaTerm) -> Optional[bool]:
        """Get cached normal form status"""
        key = self.compute_hash(term)
        return self.cache.get(key)
    
    def put(self, term: LambdaTerm, is_nf: bool):
        """Cache normal form status"""
        key = self.compute_hash(term)
        self.cache.put(key, is_nf)
    
    def get_stats(self) -> CacheStats:
        """Get cache statistics"""
        return self.cache.get_stats()
    
    def clear(self):
        """Clear cache"""
        self.cache.clear()


class CachedReducer:
    """
    Reducer with memoization
    Caches reduction results for performance
    """
    
    def __init__(self, maxsize: int = 1024):
        self.reduction_cache = ReductionCache(maxsize=maxsize)
        self.nf_cache = NormalFormCache(maxsize=maxsize // 2)
    
    def is_normal_form(self, term: LambdaTerm) -> bool:
        """Check if term is in normal form (with caching)"""
        # Check cache
        cached = self.nf_cache.get(term)
        if cached is not None:
            return cached
        
        # Compute
        result = self._compute_normal_form(term)
        
        # Cache result
        self.nf_cache.put(term, result)
        return result
    
    def _compute_normal_form(self, term: LambdaTerm) -> bool:
        """Compute normal form status"""
        if isinstance(term, Var):
            return True
        elif isinstance(term, Abs):
            return self.is_normal_form(term.body)
        elif isinstance(term, App):
            if isinstance(term.func, Abs):
                return False  # Redex
            return self.is_normal_form(term.func) and self.is_normal_form(term.arg)
        return True
    
    def reduce(self, term: LambdaTerm, max_steps: int = 1000) -> Tuple[LambdaTerm, Dict]:
        """Reduce with caching"""
        from .reducer import reduce as naive_reduce
        
        # Check cache
        cached = self.reduction_cache.get(term)
        if cached is not None:
            stats = {
                'cache_hit': True,
                'reduction_stats': self.reduction_cache.get_stats(),
                'nf_stats': self.nf_cache.get_stats(),
            }
            return cached, stats
        
        # Reduce
        result = naive_reduce(term, max_steps=max_steps)
        
        # Cache result
        self.reduction_cache.put(term, result)
        
        stats = {
            'cache_hit': False,
            'reduction_stats': self.reduction_cache.get_stats(),
            'nf_stats': self.nf_cache.get_stats(),
        }
        
        return result, stats
    
    def get_stats(self) -> Dict:
        """Get all cache statistics"""
        return {
            'reduction_cache': self.reduction_cache.get_stats(),
            'nf_cache': self.nf_cache.get_stats(),
        }
    
    def clear(self):
        """Clear all caches"""
        self.reduction_cache.clear()
        self.nf_cache.clear()


# ============================================================================
# TESTS
# ============================================================================

if __name__ == '__main__':
    print("Cache Module Tests:")
    print("=" * 60)
    
    # Test LRU Cache
    print("\n[Test 1: LRU Cache]")
    cache = LRUCache(maxsize=3)
    
    cache.put('a', 1)
    cache.put('b', 2)
    cache.put('c', 3)
    
    assert cache.get('a') == 1
    assert cache.get('b') == 2
    
    # This should evict 'c' (least recently used)
    cache.put('d', 4)
    
    assert cache.get('c') is None  # Evicted
    assert cache.get('d') == 4
    
    stats = cache.get_stats()
    print(f"  Hits: {stats.hits}")
    print(f"  Misses: {stats.misses}")
    print(f"  Evictions: {stats.evictions}")
    print(f"  Hit rate: {stats.hit_rate:.2%}")
    print("PASS: LRU Cache works")
    
    # Test Reduction Cache
    print("\n[Test 2: Reduction Cache]")
    from parser.lambda_parser import Var, Abs, App
    
    rcache = ReductionCache()
    
    term1 = Var(0)
    result1 = Var(1)
    rcache.put(term1, result1)
    
    cached = rcache.get(term1)
    assert cached == result1
    
    stats = rcache.get_stats()
    print(f"  Hits: {stats.hits}")
    print(f"  Hit rate: {stats.hit_rate:.2%}")
    print("PASS: Reduction Cache works")
    
    print("\n" + "=" * 60)
    print("ALL TESTS PASSED")
    print("Caching system is working!")

