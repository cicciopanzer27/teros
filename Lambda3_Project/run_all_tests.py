#!/usr/bin/env python3
"""
Lambda3 Complete Test Suite
Run all tests for Lambda3 project
"""

import sys
import os
import subprocess

# Test files
TESTS = [
    ("Parser", "examples/test_parser.py"),
    ("Reducer", "examples/test_reducer.py"),
    ("Encoder", "examples/test_encoder.py"),
    ("REPL", "examples/test_repl.py"),
]


def run_test(name, path):
    """Run single test file"""
    print(f"\n{'='*60}")
    print(f"Running: {name}")
    print('='*60)
    
    result = subprocess.run([sys.executable, path], capture_output=False)
    return result.returncode == 0


def main():
    print("="*60)
    print("  Lambda3 Complete Test Suite")
    print("  Running all integration tests")
    print("="*60)
    
    results = []
    
    for name, path in TESTS:
        if os.path.exists(path):
            success = run_test(name, path)
            results.append((name, success))
        else:
            print(f"\nWARNING: Test file not found: {path}")
            results.append((name, False))
    
    # Summary
    print(f"\n\n{'='*60}")
    print("  TEST SUMMARY")
    print('='*60)
    
    for name, success in results:
        status = "PASS" if success else "FAIL"
        symbol = "[PASS]" if success else "[FAIL]"
        print(f"  {symbol} {name:15s}: {status}")
    
    passed = sum(1 for _, s in results if s)
    total = len(results)
    
    print('='*60)
    print(f"  Total: {passed}/{total} test suites passed")
    
    if passed == total:
        print("  ALL TEST SUITES PASSED!")
        print("  Lambda3 is fully functional!")
    else:
        print(f"  {total - passed} test suite(s) failed")
    
    print('='*60)
    
    return 0 if passed == total else 1


if __name__ == '__main__':
    sys.exit(main())

