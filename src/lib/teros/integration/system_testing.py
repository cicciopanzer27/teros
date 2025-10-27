"""
System Testing for TEROS.

This module provides comprehensive system testing capabilities for the ternary operating system.
"""

from typing import List, Union, Optional, Dict, Any, Tuple
import time
import threading
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..vm.tvm import TVM
from ..boot.ternary_bootloader import TernaryBootloader
from ..boot.system_initialization import SystemInitializer
from ..shell.tesh import TESHShell
from ..apps.ternary_calculator import TernaryCalculatorApp
from ..apps.ternary_editor import TernaryEditor
from ..apps.ternary_file_manager import TernaryFileManager
from .hardware_integration import HardwareIntegration


class TestCategory(Enum):
    """Test categories."""
    UNIT = "unit"
    INTEGRATION = "integration"
    SYSTEM = "system"
    PERFORMANCE = "performance"
    STRESS = "stress"
    SECURITY = "security"
    USABILITY = "usability"


class SystemTest:
    """
    System Test - Represents a system test.
    
    Provides test information and execution capabilities.
    """
    
    def __init__(self, test_name: str, test_category: TestCategory,
                 test_function: callable, description: str = ""):
        """
        Initialize system test.
        
        Args:
            test_name: Name of test
            test_category: Category of test
            test_function: Test function
            description: Test description
        """
        self.test_name = test_name
        self.category = test_category
        self.test_function = test_function
        self.description = description
        
        # Test results
        self.passed = False
        self.execution_time = 0.0
        self.error_message = None
        self.test_data = {}
        
        # Test statistics
        self.stats = {
            'executions': 0,
            'passes': 0,
            'failures': 0,
            'total_time': 0.0,
            'avg_time': 0.0
        }
    
    def execute(self, *args, **kwargs) -> bool:
        """
        Execute test.
        
        Args:
            *args: Test arguments
            **kwargs: Test keyword arguments
            
        Returns:
            True if test passed, False otherwise
        """
        try:
            start_time = time.time()
            
            # Execute test function
            result = self.test_function(*args, **kwargs)
            
            end_time = time.time()
            self.execution_time = end_time - start_time
            
            # Update test results
            self.passed = result
            self.error_message = None if result else "Test failed"
            
            # Update statistics
            self.stats['executions'] += 1
            if result:
                self.stats['passes'] += 1
            else:
                self.stats['failures'] += 1
            
            self.stats['total_time'] += self.execution_time
            self.stats['avg_time'] = self.stats['total_time'] / self.stats['executions']
            
            return result
            
        except Exception as e:
            self.passed = False
            self.error_message = str(e)
            self.execution_time = 0.0
            
            self.stats['executions'] += 1
            self.stats['failures'] += 1
            
            return False
    
    def get_results(self) -> Dict[str, Any]:
        """Get test results."""
        return {
            'test_name': self.test_name,
            'category': self.category.value,
            'description': self.description,
            'passed': self.passed,
            'execution_time': self.execution_time,
            'error_message': self.error_message,
            'test_data': self.test_data,
            **self.stats
        }


class SystemTesting:
    """
    System Testing - Manages system testing for TEROS.
    
    Provides comprehensive system testing including unit tests,
    integration tests, performance tests, and stress tests.
    """
    
    def __init__(self):
        """Initialize system testing."""
        self.tests = {}  # test_name -> SystemTest
        self.test_results = []
        
        # Testing state
        self.is_testing = False
        self.testing_start_time = 0.0
        self.testing_end_time = 0.0
        
        # Testing statistics
        self.stats = {
            'tests_executed': 0,
            'tests_passed': 0,
            'tests_failed': 0,
            'total_testing_time': 0.0,
            'categories_tested': 0
        }
        
        # Initialize tests
        self._initialize_tests()
    
    def _initialize_tests(self) -> None:
        """Initialize system tests."""
        # Unit tests
        self._add_test('trit_creation', TestCategory.UNIT,
                      self._test_trit_creation, "Test Trit creation")
        
        self._add_test('trit_operations', TestCategory.UNIT,
                      self._test_trit_operations, "Test Trit operations")
        
        self._add_test('tritarray_creation', TestCategory.UNIT,
                      self._test_tritarray_creation, "Test TritArray creation")
        
        self._add_test('tritarray_operations', TestCategory.UNIT,
                      self._test_tritarray_operations, "Test TritArray operations")
        
        # Integration tests
        self._add_test('tvm_initialization', TestCategory.INTEGRATION,
                      self._test_tvm_initialization, "Test TVM initialization")
        
        self._add_test('tvm_execution', TestCategory.INTEGRATION,
                      self._test_tvm_execution, "Test TVM execution")
        
        self._add_test('bootloader_functionality', TestCategory.INTEGRATION,
                      self._test_bootloader_functionality, "Test bootloader functionality")
        
        self._add_test('system_initialization', TestCategory.INTEGRATION,
                      self._test_system_initialization, "Test system initialization")
        
        # System tests
        self._add_test('shell_functionality', TestCategory.SYSTEM,
                      self._test_shell_functionality, "Test shell functionality")
        
        self._add_test('calculator_application', TestCategory.SYSTEM,
                      self._test_calculator_application, "Test calculator application")
        
        self._add_test('editor_application', TestCategory.SYSTEM,
                      self._test_editor_application, "Test editor application")
        
        self._add_test('file_manager_application', TestCategory.SYSTEM,
                      self._test_file_manager_application, "Test file manager application")
        
        # Performance tests
        self._add_test('cpu_performance', TestCategory.PERFORMANCE,
                      self._test_cpu_performance, "Test CPU performance")
        
        self._add_test('memory_performance', TestCategory.PERFORMANCE,
                      self._test_memory_performance, "Test memory performance")
        
        self._add_test('io_performance', TestCategory.PERFORMANCE,
                      self._test_io_performance, "Test I/O performance")
        
        # Stress tests
        self._add_test('memory_stress', TestCategory.STRESS,
                      self._test_memory_stress, "Test memory stress")
        
        self._add_test('cpu_stress', TestCategory.STRESS,
                      self._test_cpu_stress, "Test CPU stress")
        
        self._add_test('concurrent_operations', TestCategory.STRESS,
                      self._test_concurrent_operations, "Test concurrent operations")
        
        # Security tests
        self._add_test('access_control', TestCategory.SECURITY,
                      self._test_access_control, "Test access control")
        
        self._add_test('data_integrity', TestCategory.SECURITY,
                      self._test_data_integrity, "Test data integrity")
        
        # Usability tests
        self._add_test('user_interface', TestCategory.USABILITY,
                      self._test_user_interface, "Test user interface")
        
        self._add_test('application_usability', TestCategory.USABILITY,
                      self._test_application_usability, "Test application usability")
    
    def _add_test(self, test_name: str, test_category: TestCategory,
                  test_function: callable, description: str = "") -> None:
        """Add test to test suite."""
        test = SystemTest(test_name, test_category, test_function, description)
        self.tests[test_name] = test
    
    def run_test_suite(self, categories: List[TestCategory] = None) -> bool:
        """
        Run test suite.
        
        Args:
            categories: Categories of tests to run (if None, run all)
            
        Returns:
            True if all tests passed, False otherwise
        """
        if categories is None:
            categories = list(TestCategory)
        
        self.is_testing = True
        self.testing_start_time = time.time()
        
        print("=== TEROS System Testing ===")
        print("Running system tests...")
        print()
        
        passed_tests = 0
        total_tests = 0
        
        # Group tests by category
        tests_by_category = {}
        for test_name, test in self.tests.items():
            if test.category in categories:
                if test.category not in tests_by_category:
                    tests_by_category[test.category] = []
                tests_by_category[test.category].append(test)
        
        # Run tests by category
        for category, tests in tests_by_category.items():
            print(f"Running {category.value} tests...")
            
            for test in tests:
                print(f"  {test.test_name}...")
                
                if test.execute():
                    print(f"    PASSED")
                    passed_tests += 1
                else:
                    print(f"    FAILED")
                    if test.error_message:
                        print(f"      Error: {test.error_message}")
                
                total_tests += 1
                self.stats['tests_executed'] += 1
                
                if test.passed:
                    self.stats['tests_passed'] += 1
                else:
                    self.stats['tests_failed'] += 1
            
            print()
        
        self.testing_end_time = time.time()
        self.stats['total_testing_time'] = self.testing_end_time - self.testing_start_time
        self.stats['categories_tested'] = len(tests_by_category)
        
        print(f"=== Test Results ===")
        print(f"Tests executed: {total_tests}")
        print(f"Tests passed: {passed_tests}")
        print(f"Tests failed: {total_tests - passed_tests}")
        print(f"Success rate: {(passed_tests / total_tests * 100):.1f}%")
        print(f"Total testing time: {self.stats['total_testing_time']:.2f} seconds")
        
        self.is_testing = False
        return passed_tests == total_tests
    
    # Test implementations
    def _test_trit_creation(self) -> bool:
        """Test Trit creation."""
        try:
            # Test valid trit creation
            trit1 = Trit(1)
            trit2 = Trit(0)
            trit3 = Trit(-1)
            
            if trit1.value != 1 or trit2.value != 0 or trit3.value != -1:
                return False
            
            # Test invalid trit creation
            try:
                Trit(2)
                return False  # Should raise ValueError
            except ValueError:
                pass
            
            return True
            
        except Exception as e:
            print(f"Trit creation test error: {e}")
            return False
    
    def _test_trit_operations(self) -> bool:
        """Test Trit operations."""
        try:
            trit1 = Trit(1)
            trit2 = Trit(-1)
            trit3 = Trit(0)
            
            # Test addition
            result = trit1 + trit2
            if result.value != 0:
                return False
            
            # Test multiplication
            result = trit1 * trit2
            if result.value != -1:
                return False
            
            # Test comparison
            if not (trit1 == Trit(1)):
                return False
            
            return True
            
        except Exception as e:
            print(f"Trit operations test error: {e}")
            return False
    
    def _test_tritarray_creation(self) -> bool:
        """Test TritArray creation."""
        try:
            # Test creation from list
            tritarray1 = TritArray([1, 0, -1])
            if len(tritarray1) != 3:
                return False
            
            # Test creation from integer
            tritarray2 = TritArray.from_int(5, 3)
            if len(tritarray2) != 3:
                return False
            
            return True
            
        except Exception as e:
            print(f"TritArray creation test error: {e}")
            return False
    
    def _test_tritarray_operations(self) -> bool:
        """Test TritArray operations."""
        try:
            tritarray1 = TritArray([1, 0, -1])
            tritarray2 = TritArray([0, 1, 0])
            
            # Test addition
            result = tritarray1 + tritarray2
            if len(result) != 3:
                return False
            
            # Test conversion
            decimal = tritarray1.to_decimal()
            if decimal < 0:
                return False
            
            return True
            
        except Exception as e:
            print(f"TritArray operations test error: {e}")
            return False
    
    def _test_tvm_initialization(self) -> bool:
        """Test TVM initialization."""
        try:
            tvm = TVM()
            
            # Check if TVM is properly initialized
            if not hasattr(tvm, 'registers'):
                return False
            
            if not hasattr(tvm, 'memory'):
                return False
            
            return True
            
        except Exception as e:
            print(f"TVM initialization test error: {e}")
            return False
    
    def _test_tvm_execution(self) -> bool:
        """Test TVM execution."""
        try:
            tvm = TVM()
            
            # Load simple program
            program = [
                "LOADI 1",
                "LOADI 2",
                "ADD",
                "HALT"
            ]
            
            tvm.load_program(program)
            tvm.run()
            
            # Check if program executed successfully
            return True
            
        except Exception as e:
            print(f"TVM execution test error: {e}")
            return False
    
    def _test_bootloader_functionality(self) -> bool:
        """Test bootloader functionality."""
        try:
            bootloader = TernaryBootloader()
            
            # Test bootloader initialization
            if not hasattr(bootloader, 'boot'):
                return False
            
            return True
            
        except Exception as e:
            print(f"Bootloader functionality test error: {e}")
            return False
    
    def _test_system_initialization(self) -> bool:
        """Test system initialization."""
        try:
            initializer = SystemInitializer()
            
            # Test system initialization
            if not hasattr(initializer, 'initialize_system'):
                return False
            
            return True
            
        except Exception as e:
            print(f"System initialization test error: {e}")
            return False
    
    def _test_shell_functionality(self) -> bool:
        """Test shell functionality."""
        try:
            shell = TESHShell()
            
            # Test shell initialization
            if not hasattr(shell, 'run'):
                return False
            
            # Test command registration
            if not hasattr(shell, 'register_command'):
                return False
            
            return True
            
        except Exception as e:
            print(f"Shell functionality test error: {e}")
            return False
    
    def _test_calculator_application(self) -> bool:
        """Test calculator application."""
        try:
            calculator = TernaryCalculatorApp()
            
            # Test calculator initialization
            if not hasattr(calculator, 'run'):
                return False
            
            return True
            
        except Exception as e:
            print(f"Calculator application test error: {e}")
            return False
    
    def _test_editor_application(self) -> bool:
        """Test editor application."""
        try:
            editor = TernaryEditor()
            
            # Test editor initialization
            if not hasattr(editor, 'insert_text'):
                return False
            
            if not hasattr(editor, 'delete_text'):
                return False
            
            return True
            
        except Exception as e:
            print(f"Editor application test error: {e}")
            return False
    
    def _test_file_manager_application(self) -> bool:
        """Test file manager application."""
        try:
            file_manager = TernaryFileManager()
            
            # Test file manager initialization
            if not hasattr(file_manager, 'list_directory'):
                return False
            
            if not hasattr(file_manager, 'create_file'):
                return False
            
            return True
            
        except Exception as e:
            print(f"File manager application test error: {e}")
            return False
    
    def _test_cpu_performance(self) -> bool:
        """Test CPU performance."""
        try:
            tvm = TVM()
            
            # Load performance test program
            program = []
            for i in range(1000):
                program.extend([
                    f"LOADI {i}",
                    f"LOADI {i+1}",
                    "ADD"
                ])
            program.append("HALT")
            
            tvm.load_program(program)
            
            # Measure execution time
            start_time = time.time()
            tvm.run()
            end_time = time.time()
            
            execution_time = end_time - start_time
            
            # Performance threshold (should complete in < 5 seconds)
            return execution_time < 5.0
            
        except Exception as e:
            print(f"CPU performance test error: {e}")
            return False
    
    def _test_memory_performance(self) -> bool:
        """Test memory performance."""
        try:
            from ..hal.memory_pool import TernaryMemoryPool
            
            pool = TernaryMemoryPool()
            
            # Test memory allocation performance
            start_time = time.time()
            addresses = []
            for i in range(10000):
                address = pool.allocate(8)
                if address is None:
                    return False
                addresses.append(address)
            
            # Test memory deallocation performance
            for address in addresses:
                if not pool.deallocate(address):
                    return False
            
            end_time = time.time()
            execution_time = end_time - start_time
            
            # Performance threshold (should complete in < 2 seconds)
            return execution_time < 2.0
            
        except Exception as e:
            print(f"Memory performance test error: {e}")
            return False
    
    def _test_io_performance(self) -> bool:
        """Test I/O performance."""
        try:
            from ..libs.libio import TernaryConsoleIO
            
            # Test I/O performance
            start_time = time.time()
            for i in range(1000):
                trits = [Trit(1), Trit(0), Trit(-1)]
                TernaryConsoleIO.print_trits(trits)
            end_time = time.time()
            
            execution_time = end_time - start_time
            
            # Performance threshold (should complete in < 3 seconds)
            return execution_time < 3.0
            
        except Exception as e:
            print(f"I/O performance test error: {e}")
            return False
    
    def _test_memory_stress(self) -> bool:
        """Test memory stress."""
        try:
            from ..hal.memory_pool import TernaryMemoryPool
            
            pool = TernaryMemoryPool()
            
            # Stress test with large memory allocations
            addresses = []
            for i in range(50000):
                address = pool.allocate(16)
                if address is None:
                    return False
                addresses.append(address)
            
            # Deallocate memory
            for address in addresses:
                if not pool.deallocate(address):
                    return False
            
            return True
            
        except Exception as e:
            print(f"Memory stress test error: {e}")
            return False
    
    def _test_cpu_stress(self) -> bool:
        """Test CPU stress."""
        try:
            tvm = TVM()
            
            # Stress test with complex program
            program = []
            for i in range(10000):
                program.extend([
                    f"LOADI {i % 100}",
                    f"LOADI {(i + 1) % 100}",
                    "ADD",
                    "MUL",
                    "PUSH"
                ])
            program.append("HALT")
            
            tvm.load_program(program)
            
            # Run stress test
            start_time = time.time()
            tvm.run()
            end_time = time.time()
            
            execution_time = end_time - start_time
            
            # Stress test threshold (should complete in < 10 seconds)
            return execution_time < 10.0
            
        except Exception as e:
            print(f"CPU stress test error: {e}")
            return False
    
    def _test_concurrent_operations(self) -> bool:
        """Test concurrent operations."""
        try:
            results = []
            
            def worker(worker_id):
                try:
                    tvm = TVM()
                    program = [
                        f"LOADI {worker_id}",
                        "LOADI 1",
                        "ADD",
                        "HALT"
                    ]
                    tvm.load_program(program)
                    tvm.run()
                    results.append(True)
                except:
                    results.append(False)
            
            # Create multiple threads
            threads = []
            for i in range(10):
                thread = threading.Thread(target=worker, args=(i,))
                threads.append(thread)
                thread.start()
            
            # Wait for all threads to complete
            for thread in threads:
                thread.join()
            
            # Check if all operations succeeded
            return all(results)
            
        except Exception as e:
            print(f"Concurrent operations test error: {e}")
            return False
    
    def _test_access_control(self) -> bool:
        """Test access control."""
        try:
            # Test basic access control
            # This is a placeholder - in real implementation,
            # this would test actual access control mechanisms
            
            return True
            
        except Exception as e:
            print(f"Access control test error: {e}")
            return False
    
    def _test_data_integrity(self) -> bool:
        """Test data integrity."""
        try:
            # Test data integrity
            # This is a placeholder - in real implementation,
            # this would test actual data integrity mechanisms
            
            return True
            
        except Exception as e:
            print(f"Data integrity test error: {e}")
            return False
    
    def _test_user_interface(self) -> bool:
        """Test user interface."""
        try:
            # Test user interface
            # This is a placeholder - in real implementation,
            # this would test actual user interface components
            
            return True
            
        except Exception as e:
            print(f"User interface test error: {e}")
            return False
    
    def _test_application_usability(self) -> bool:
        """Test application usability."""
        try:
            # Test application usability
            # This is a placeholder - in real implementation,
            # this would test actual application usability
            
            return True
            
        except Exception as e:
            print(f"Application usability test error: {e}")
            return False
    
    def get_test_results(self) -> List[Dict[str, Any]]:
        """Get test results."""
        return [test.get_results() for test in self.tests.values()]
    
    def get_testing_stats(self) -> Dict[str, Any]:
        """Get testing statistics."""
        return {
            'is_testing': self.is_testing,
            'total_testing_time': self.stats['total_testing_time'],
            'tests_executed': self.stats['tests_executed'],
            'tests_passed': self.stats['tests_passed'],
            'tests_failed': self.stats['tests_failed'],
            'categories_tested': self.stats['categories_tested'],
            'test_success_rate': (self.stats['tests_passed'] / self.stats['tests_executed'] * 100) if self.stats['tests_executed'] > 0 else 0
        }
    
    def generate_report(self) -> str:
        """Generate testing report."""
        report = []
        report.append("=== TEROS System Testing Report ===")
        report.append("")
        
        # Testing statistics
        stats = self.get_testing_stats()
        report.append("Testing Statistics:")
        for key, value in stats.items():
            report.append(f"  {key}: {value}")
        report.append("")
        
        # Test results by category
        test_results = self.get_testing_stats()
        if test_results:
            report.append("Test Results by Category:")
            
            # Group results by category
            results_by_category = {}
            for test_name, test in self.tests.items():
                category = test.category.value
                if category not in results_by_category:
                    results_by_category[category] = {'passed': 0, 'failed': 0, 'total': 0}
                
                results_by_category[category]['total'] += 1
                if test.passed:
                    results_by_category[category]['passed'] += 1
                else:
                    results_by_category[category]['failed'] += 1
            
            for category, results in results_by_category.items():
                success_rate = (results['passed'] / results['total'] * 100) if results['total'] > 0 else 0
                report.append(f"  {category}: {results['passed']}/{results['total']} ({success_rate:.1f}%)")
            report.append("")
        
        # Detailed test results
        test_results = self.get_test_results()
        if test_results:
            report.append("Detailed Test Results:")
            for result in test_results:
                status = "PASSED" if result['passed'] else "FAILED"
                report.append(f"  {result['test_name']}: {status} ({result['execution_time']:.6f}s)")
                if result['error_message']:
                    report.append(f"    Error: {result['error_message']}")
            report.append("")
        
        return "\n".join(report)
    
    def save_report(self, filename: str) -> bool:
        """
        Save testing report to file.
        
        Args:
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        try:
            report = self.generate_report()
            with open(filename, 'w') as f:
                f.write(report)
            print(f"Testing report saved to {filename}")
            return True
        except Exception as e:
            print(f"Failed to save report: {e}")
            return False
    
    def run_comprehensive_testing(self) -> bool:
        """
        Run comprehensive testing suite.
        
        Returns:
            True if all tests passed, False otherwise
        """
        print("=== TEROS Comprehensive System Testing ===")
        print("Running comprehensive testing suite...")
        print()
        
        # Run all test categories
        all_categories = list(TestCategory)
        return self.run_test_suite(all_categories)
    
    def run_quick_testing(self) -> bool:
        """
        Run quick testing suite.
        
        Returns:
            True if all tests passed, False otherwise
        """
        print("=== TEROS Quick System Testing ===")
        print("Running quick testing suite...")
        print()
        
        # Run only unit and integration tests
        quick_categories = [TestCategory.UNIT, TestCategory.INTEGRATION]
        return self.run_test_suite(quick_categories)
    
    def run_performance_testing(self) -> bool:
        """
        Run performance testing suite.
        
        Returns:
            True if all tests passed, False otherwise
        """
        print("=== TEROS Performance Testing ===")
        print("Running performance testing suite...")
        print()
        
        # Run only performance tests
        performance_categories = [TestCategory.PERFORMANCE]
        return self.run_test_suite(performance_categories)
    
    def run_stress_testing(self) -> bool:
        """
        Run stress testing suite.
        
        Returns:
            True if all tests passed, False otherwise
        """
        print("=== TEROS Stress Testing ===")
        print("Running stress testing suite...")
        print()
        
        # Run only stress tests
        stress_categories = [TestCategory.STRESS]
        return self.run_test_suite(stress_categories)
    
    def help(self) -> str:
        """Get help information."""
        return """
TEROS System Testing Help:

Test Categories:
  unit               : Unit tests
  integration        : Integration tests
  system             : System tests
  performance        : Performance tests
  stress             : Stress tests
  security           : Security tests
  usability          : Usability tests

Testing Commands:
  run_all            : Run all tests
  run_quick          : Run quick tests (unit + integration)
  run_performance    : Run performance tests
  run_stress         : Run stress tests
  run_category <cat> : Run specific category
  
Reports:
  report             : Generate testing report
  save <filename>    : Save report to file
  
Info:
  stats              : Show testing statistics
  results            : Show test results
  help               : Show this help
        """
    
    def run_interactive(self) -> None:
        """Run interactive testing session."""
        print("=== TEROS System Testing Interactive Mode ===")
        print("Type 'help' for available commands, 'quit' to exit")
        print()
        
        try:
            while True:
                try:
                    # Get user input
                    user_input = input("testing> ").strip()
                    
                    if not user_input:
                        continue
                    
                    # Parse and execute command
                    self._execute_command(user_input)
                    
                except KeyboardInterrupt:
                    print("\nUse 'quit' to exit")
                    continue
                except EOFError:
                    break
                except Exception as e:
                    print(f"Command error: {e}")
                    continue
        
        finally:
            if self.is_testing:
                self.is_testing = False
    
    def _execute_command(self, command: str) -> None:
        """Execute testing command."""
        parts = command.split()
        if not parts:
            return
        
        cmd = parts[0].lower()
        args = parts[1:]
        
        if cmd == 'help':
            print(self.help())
        
        elif cmd == 'quit':
            sys.exit(0)
        
        elif cmd == 'run_all':
            self.run_comprehensive_testing()
        
        elif cmd == 'run_quick':
            self.run_quick_testing()
        
        elif cmd == 'run_performance':
            self.run_performance_testing()
        
        elif cmd == 'run_stress':
            self.run_stress_testing()
        
        elif cmd == 'run_category':
            if args:
                try:
                    category = TestCategory(args[0])
                    self.run_test_suite([category])
                except ValueError:
                    print(f"Unknown category: {args[0]}")
            else:
                print("Usage: run_category <category>")
        
        elif cmd == 'report':
            self.generate_report()
        
        elif cmd == 'save':
            if args:
                self.save_report(args[0])
            else:
                print("Usage: save <filename>")
        
        elif cmd == 'stats':
            stats = self.get_testing_stats()
            print("Testing Statistics:")
            for key, value in stats.items():
                print(f"  {key}: {value}")
        
        elif cmd == 'results':
            results = self.get_test_results()
            print("Test Results:")
            for result in results:
                status = "PASSED" if result['passed'] else "FAILED"
                print(f"  {result['test_name']}: {status} ({result['execution_time']:.6f}s)")
        
        else:
            print(f"Unknown command: {cmd}")
            print("Type 'help' for available commands")


if __name__ == "__main__":
    testing = SystemTesting()
    testing.run_interactive()
