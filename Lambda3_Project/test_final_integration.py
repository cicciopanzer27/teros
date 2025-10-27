"""
Final Integration Test for Lambda³
Test all components together
"""

import sys
import os
import time
from pathlib import Path

# Add Lambda³ to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_core_imports():
    """Test core Lambda³ imports"""
    print("Testing core imports...")
    
    try:
        import lambda3
        print("  [PASS] lambda3 core import")
        
        from lambda3.parser.parser import parse
        print("  [PASS] parser import")
        
        from lambda3.engine.reducer import reduce
        print("  [PASS] reducer import")
        
        from lambda3.ternary.encoder import encode
        print("  [PASS] encoder import")
        
        from lambda3.repl.lambda_repl import LambdaREPL
        print("  [PASS] REPL import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Core import error: {e}")
        return False

def test_neural_components():
    """Test neural components"""
    print("Testing neural components...")
    
    try:
        from lambda3.neural.model import TacticSuggestionModel
        print("  [PASS] Neural model import")
        
        from lambda3.neural.dataset import DatasetGenerator
        print("  [PASS] Dataset generator import")
        
        from lambda3.neural.embeddings import LambdaEmbedding
        print("  [PASS] Embeddings import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Neural import error: {e}")
        return False

def test_type_system():
    """Test type system"""
    print("Testing type system...")
    
    try:
        from lambda3.types.inference import infer_type
        print("  [PASS] Type inference import")
        
        from lambda3.types.dependent import DependentTypeChecker
        print("  [PASS] Dependent types import")
        
        from lambda3.types.advanced import AdvancedTypeChecker
        print("  [PASS] Advanced types import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Type system import error: {e}")
        return False

def test_nlu_nlg():
    """Test NLU/NLG components"""
    print("Testing NLU/NLG components...")
    
    try:
        from lambda3.nlu.parser import NLUParser
        print("  [PASS] NLU parser import")
        
        from lambda3.nlg.generator import NLGGenerator
        print("  [PASS] NLG generator import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] NLU/NLG import error: {e}")
        return False

def test_monitoring():
    """Test monitoring components"""
    print("Testing monitoring components...")
    
    try:
        from lambda3.monitoring.metrics import LambdaMetricsCollector
        print("  [PASS] Metrics collector import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Monitoring import error: {e}")
        return False

def test_applications():
    """Test application components"""
    print("Testing application components...")
    
    try:
        from lambda3.applications.qcd import QCDLambdaFormalization
        print("  [PASS] QCD application import")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Applications import error: {e}")
        return False

def test_core_functionality():
    """Test core functionality"""
    print("Testing core functionality...")
    
    try:
        from lambda3.parser.parser import parse
        from lambda3.engine.reducer import reduce
        
        # Test parsing
        term = parse("\\x.x")
        print(f"  [PASS] Parse test: {term}")
        
        # Test reduction
        result = reduce(term)
        print(f"  [PASS] Reduce test: {result}")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Core functionality error: {e}")
        return False

def test_neural_functionality():
    """Test neural functionality"""
    print("Testing neural functionality...")
    
    try:
        from lambda3.neural.dataset import DatasetGenerator
        
        # Test dataset generation
        generator = DatasetGenerator()
        generator.generate_synthetic_proofs(count=10)
        print("  [PASS] Dataset generation test")
        
        return True
    except Exception as e:
        print(f"  [FAIL] Neural functionality error: {e}")
        return False

def test_monitoring_functionality():
    """Test monitoring functionality"""
    print("Testing monitoring functionality...")
    
    try:
        from lambda3.monitoring.metrics import LambdaMetricsCollector
        
        # Test metrics collection
        collector = LambdaMetricsCollector()
        print("  [PASS] Metrics collector test")
        
        # Stop monitoring
        collector.stop_monitoring()
        
        return True
    except Exception as e:
        print(f"  [FAIL] Monitoring functionality error: {e}")
        return False

def main():
    """Run final integration test"""
    print("="*60)
    print("  Lambda³ Final Integration Test")
    print("  Testing All Components Together")
    print("="*60)
    
    tests = [
        ("Core Imports", test_core_imports),
        ("Neural Components", test_neural_components),
        ("Type System", test_type_system),
        ("NLU/NLG", test_nlu_nlg),
        ("Monitoring", test_monitoring),
        ("Applications", test_applications),
        ("Core Functionality", test_core_functionality),
        ("Neural Functionality", test_neural_functionality),
        ("Monitoring Functionality", test_monitoring_functionality)
    ]
    
    results = {}
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        try:
            success = test_func()
            results[test_name] = success
        except Exception as e:
            print(f"  [FAIL] Test failed: {e}")
            results[test_name] = False
    
    # Summary
    print("\n" + "="*60)
    print("  INTEGRATION TEST SUMMARY")
    print("="*60)
    
    passed = sum(1 for success in results.values() if success)
    total = len(results)
    
    print(f"Tests passed: {passed}/{total}")
    print(f"Success rate: {passed/total*100:.1f}%")
    
    print("\nDetailed results:")
    for test_name, success in results.items():
        status = "PASS" if success else "FAIL"
        print(f"  {test_name}: {status}")
    
    if passed == total:
        print("\nSUCCESS: ALL TESTS PASSED! Lambda³ is fully integrated!")
    else:
        print(f"\nWARNING: {total-passed} tests failed. Check errors above.")
    
    print("="*60)
    
    return passed == total

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
