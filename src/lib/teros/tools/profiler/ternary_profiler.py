#!/usr/bin/env python3
"""
TEROS Ternary Profiler

This module provides profiling functionality for TEROS,
including performance analysis, memory profiling, and optimization suggestions.
"""

import time
import psutil
import threading
from typing import Dict, List, Any, Optional, Callable
from ...core.trit import Trit
from ...core.tritarray import TritArray
from ...vm.tvm import TernaryVirtualMachine
from ...process.scheduler import TernaryScheduler
from ...memory.memory_manager import TernaryMemoryManager


class TernaryProfiler:
    """Ternary profiler for TEROS."""
    
    def __init__(self):
        """Initialize the profiler."""
        self.profiling_data: Dict[str, Any] = {}
        self.function_calls: Dict[str, int] = {}
        self.execution_times: Dict[str, List[float]] = {}
        self.memory_usage: List[Dict[str, Any]] = []
        self.cpu_usage: List[Dict[str, Any]] = []
        self.is_profiling = False
        self.profiling_thread: Optional[threading.Thread] = None
        self.start_time: Optional[float] = None
        self.end_time: Optional[float] = None
        
    def start_profiling(self) -> bool:
        """Start profiling."""
        try:
            if self.is_profiling:
                print("Profiling already started")
                return False
            
            self.is_profiling = True
            self.start_time = time.time()
            self.profiling_data = {}
            self.function_calls = {}
            self.execution_times = {}
            self.memory_usage = []
            self.cpu_usage = []
            
            # Start profiling thread
            self.profiling_thread = threading.Thread(target=self._profile_system, daemon=True)
            self.profiling_thread.start()
            
            print("Profiling started")
            return True
        except Exception as e:
            print(f"Error starting profiling: {e}")
            return False
    
    def stop_profiling(self) -> bool:
        """Stop profiling."""
        try:
            if not self.is_profiling:
                print("Profiling not started")
                return False
            
            self.is_profiling = False
            self.end_time = time.time()
            
            # Wait for profiling thread to finish
            if self.profiling_thread:
                self.profiling_thread.join(timeout=1.0)
            
            print("Profiling stopped")
            return True
        except Exception as e:
            print(f"Error stopping profiling: {e}")
            return False
    
    def profile_function(self, func: Callable) -> Callable:
        """Decorator to profile a function."""
        def wrapper(*args, **kwargs):
            if not self.is_profiling:
                return func(*args, **kwargs)
            
            func_name = func.__name__
            start_time = time.time()
            
            try:
                result = func(*args, **kwargs)
                return result
            finally:
                end_time = time.time()
                execution_time = end_time - start_time
                
                # Record function call
                if func_name not in self.function_calls:
                    self.function_calls[func_name] = 0
                self.function_calls[func_name] += 1
                
                # Record execution time
                if func_name not in self.execution_times:
                    self.execution_times[func_name] = []
                self.execution_times[func_name].append(execution_time)
        
        return wrapper
    
    def profile_trit_operations(self, iterations: int = 10000) -> Dict[str, Any]:
        """Profile Trit operations."""
        print("Profiling Trit operations...")
        
        results = {}
        
        # Profile trit creation
        start_time = time.time()
        for _ in range(iterations):
            trit = Trit(1)
        creation_time = time.time() - start_time
        
        results['trit_creation'] = {
            'iterations': iterations,
            'total_time': creation_time,
            'avg_time_per_operation': creation_time / iterations,
            'operations_per_second': iterations / creation_time
        }
        
        # Profile trit arithmetic
        trit1 = Trit(1)
        trit2 = Trit(0)
        
        start_time = time.time()
        for _ in range(iterations):
            result = trit1 + trit2
        arithmetic_time = time.time() - start_time
        
        results['trit_arithmetic'] = {
            'iterations': iterations,
            'total_time': arithmetic_time,
            'avg_time_per_operation': arithmetic_time / iterations,
            'operations_per_second': iterations / arithmetic_time
        }
        
        # Profile trit logic
        start_time = time.time()
        for _ in range(iterations):
            result = trit1 & trit2
        logic_time = time.time() - start_time
        
        results['trit_logic'] = {
            'iterations': iterations,
            'total_time': logic_time,
            'avg_time_per_operation': logic_time / iterations,
            'operations_per_second': iterations / logic_time
        }
        
        return results
    
    def profile_tritarray_operations(self, iterations: int = 1000, array_size: int = 1000) -> Dict[str, Any]:
        """Profile TritArray operations."""
        print("Profiling TritArray operations...")
        
        results = {}
        
        # Profile tritarray creation
        start_time = time.time()
        arrays = []
        for _ in range(iterations):
            array = TritArray([1, 0, -1] * (array_size // 3))
            arrays.append(array)
        creation_time = time.time() - start_time
        
        results['tritarray_creation'] = {
            'iterations': iterations,
            'array_size': array_size,
            'total_time': creation_time,
            'avg_time_per_operation': creation_time / iterations,
            'operations_per_second': iterations / creation_time
        }
        
        # Profile tritarray arithmetic
        array1 = TritArray([1, 0, -1] * (array_size // 3))
        array2 = TritArray([0, 1, -1] * (array_size // 3))
        
        start_time = time.time()
        for _ in range(iterations):
            result = array1 + array2
        arithmetic_time = time.time() - start_time
        
        results['tritarray_arithmetic'] = {
            'iterations': iterations,
            'array_size': array_size,
            'total_time': arithmetic_time,
            'avg_time_per_operation': arithmetic_time / iterations,
            'operations_per_second': iterations / arithmetic_time
        }
        
        # Profile tritarray shifts
        start_time = time.time()
        for _ in range(iterations):
            result = array1 << 1
        shift_time = time.time() - start_time
        
        results['tritarray_shift'] = {
            'iterations': iterations,
            'array_size': array_size,
            'total_time': shift_time,
            'avg_time_per_operation': shift_time / iterations,
            'operations_per_second': iterations / shift_time
        }
        
        return results
    
    def profile_memory_operations(self, iterations: int = 10000) -> Dict[str, Any]:
        """Profile memory operations."""
        print("Profiling memory operations...")
        
        results = {}
        memory_manager = TernaryMemoryManager()
        
        # Profile memory allocation
        start_time = time.time()
        addresses = []
        for _ in range(iterations):
            address = memory_manager.allocate_memory(100)
            addresses.append(address)
        allocation_time = time.time() - start_time
        
        results['memory_allocation'] = {
            'iterations': iterations,
            'total_time': allocation_time,
            'avg_time_per_operation': allocation_time / iterations,
            'operations_per_second': iterations / allocation_time
        }
        
        # Profile memory deallocation
        start_time = time.time()
        for address in addresses:
            memory_manager.deallocate_memory(address)
        deallocation_time = time.time() - start_time
        
        results['memory_deallocation'] = {
            'iterations': iterations,
            'total_time': deallocation_time,
            'avg_time_per_operation': deallocation_time / iterations,
            'operations_per_second': iterations / deallocation_time
        }
        
        # Profile memory access
        address = memory_manager.allocate_memory(1000)
        data = b"test data" * 100
        
        start_time = time.time()
        for _ in range(iterations):
            memory_manager.write_memory(address, data)
        write_time = time.time() - start_time
        
        results['memory_write'] = {
            'iterations': iterations,
            'total_time': write_time,
            'avg_time_per_operation': write_time / iterations,
            'operations_per_second': iterations / write_time
        }
        
        start_time = time.time()
        for _ in range(iterations):
            memory_manager.read_memory(address, len(data))
        read_time = time.time() - start_time
        
        results['memory_read'] = {
            'iterations': iterations,
            'total_time': read_time,
            'avg_time_per_operation': read_time / iterations,
            'operations_per_second': iterations / read_time
        }
        
        memory_manager.deallocate_memory(address)
        
        return results
    
    def profile_process_operations(self, iterations: int = 1000) -> Dict[str, Any]:
        """Profile process operations."""
        print("Profiling process operations...")
        
        results = {}
        scheduler = TernaryScheduler()
        
        # Profile process creation
        start_time = time.time()
        pids = []
        for i in range(iterations):
            pid = scheduler.create_process(f"process_{i}")
            pids.append(pid)
        creation_time = time.time() - start_time
        
        results['process_creation'] = {
            'iterations': iterations,
            'total_time': creation_time,
            'avg_time_per_operation': creation_time / iterations,
            'operations_per_second': iterations / creation_time
        }
        
        # Profile process scheduling
        start_time = time.time()
        for _ in range(iterations):
            next_pid = scheduler.schedule()
        scheduling_time = time.time() - start_time
        
        results['process_scheduling'] = {
            'iterations': iterations,
            'total_time': scheduling_time,
            'avg_time_per_operation': scheduling_time / iterations,
            'operations_per_second': iterations / scheduling_time
        }
        
        # Profile process termination
        start_time = time.time()
        for pid in pids:
            scheduler.terminate_process(pid)
        termination_time = time.time() - start_time
        
        results['process_termination'] = {
            'iterations': iterations,
            'total_time': termination_time,
            'avg_time_per_operation': termination_time / iterations,
            'operations_per_second': iterations / termination_time
        }
        
        return results
    
    def get_profiling_report(self) -> Dict[str, Any]:
        """Get comprehensive profiling report."""
        if not self.is_profiling and not self.profiling_data:
            print("No profiling data available")
            return {}
        
        report = {
            'profiling_duration': self.end_time - self.start_time if self.end_time and self.start_time else 0,
            'function_calls': self.function_calls,
            'execution_times': self.execution_times,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage,
            'system_info': self._get_system_info()
        }
        
        return report
    
    def _profile_system(self):
        """Profile system resources."""
        while self.is_profiling:
            try:
                # Memory usage
                memory = psutil.virtual_memory()
                memory_info = {
                    'timestamp': time.time(),
                    'total': memory.total,
                    'available': memory.available,
                    'used': memory.used,
                    'percent': memory.percent
                }
                self.memory_usage.append(memory_info)
                
                # CPU usage
                cpu_percent = psutil.cpu_percent(interval=0.1)
                cpu_info = {
                    'timestamp': time.time(),
                    'percent': cpu_percent
                }
                self.cpu_usage.append(cpu_info)
                
                time.sleep(0.1)  # Profile every 100ms
                
            except Exception as e:
                print(f"Error profiling system: {e}")
                break
    
    def _get_system_info(self) -> Dict[str, Any]:
        """Get system information."""
        return {
            'cpu_count': psutil.cpu_count(),
            'memory_total': psutil.virtual_memory().total,
            'disk_total': psutil.disk_usage('/').total,
            'platform': psutil.platform.system(),
            'python_version': psutil.sys.version
        }
    
    def generate_optimization_suggestions(self) -> List[str]:
        """Generate optimization suggestions based on profiling data."""
        suggestions = []
        
        # Analyze function calls
        if self.function_calls:
            most_called = max(self.function_calls.items(), key=lambda x: x[1])
            if most_called[1] > 1000:
                suggestions.append(f"Function '{most_called[0]}' called {most_called[1]} times - consider optimization")
        
        # Analyze execution times
        if self.execution_times:
            for func_name, times in self.execution_times.items():
                avg_time = sum(times) / len(times)
                if avg_time > 0.1:  # 100ms
                    suggestions.append(f"Function '{func_name}' takes {avg_time:.4f}s on average - consider optimization")
        
        # Analyze memory usage
        if self.memory_usage:
            max_memory = max(usage['percent'] for usage in self.memory_usage)
            if max_memory > 80:
                suggestions.append(f"High memory usage detected ({max_memory:.1f}%) - consider memory optimization")
        
        # Analyze CPU usage
        if self.cpu_usage:
            avg_cpu = sum(usage['percent'] for usage in self.cpu_usage) / len(self.cpu_usage)
            if avg_cpu > 80:
                suggestions.append(f"High CPU usage detected ({avg_cpu:.1f}%) - consider CPU optimization")
        
        return suggestions
    
    def save_profiling_report(self, filename: str = "profiling_report.json"):
        """Save profiling report to file."""
        import json
        
        report = self.get_profiling_report()
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Profiling report saved to {filename}")
    
    def print_profiling_report(self):
        """Print profiling report."""
        report = self.get_profiling_report()
        
        print("=== TEROS Profiling Report ===")
        print()
        
        if report.get('profiling_duration'):
            print(f"Profiling Duration: {report['profiling_duration']:.2f} seconds")
            print()
        
        if report.get('function_calls'):
            print("Function Calls:")
            for func_name, count in report['function_calls'].items():
                print(f"  {func_name}: {count} calls")
            print()
        
        if report.get('execution_times'):
            print("Execution Times:")
            for func_name, times in report['execution_times'].items():
                avg_time = sum(times) / len(times)
                total_time = sum(times)
                print(f"  {func_name}: {avg_time:.6f}s average, {total_time:.6f}s total")
            print()
        
        if report.get('memory_usage'):
            print("Memory Usage:")
            max_memory = max(usage['percent'] for usage in report['memory_usage'])
            avg_memory = sum(usage['percent'] for usage in report['memory_usage']) / len(report['memory_usage'])
            print(f"  Maximum: {max_memory:.1f}%")
            print(f"  Average: {avg_memory:.1f}%")
            print()
        
        if report.get('cpu_usage'):
            print("CPU Usage:")
            max_cpu = max(usage['percent'] for usage in report['cpu_usage'])
            avg_cpu = sum(usage['percent'] for usage in report['cpu_usage']) / len(report['cpu_usage'])
            print(f"  Maximum: {max_cpu:.1f}%")
            print(f"  Average: {avg_cpu:.1f}%")
            print()
        
        # Optimization suggestions
        suggestions = self.generate_optimization_suggestions()
        if suggestions:
            print("Optimization Suggestions:")
            for suggestion in suggestions:
                print(f"  - {suggestion}")
            print()


def main():
    """Main profiler function."""
    profiler = TernaryProfiler()
    
    print("=== TEROS Ternary Profiler ===")
    print("Type 'help' for available commands")
    print()
    
    while True:
        try:
            command = input("(teros-profiler) ").strip().split()
            
            if not command:
                continue
            
            cmd = command[0].lower()
            
            if cmd == 'help':
                print("Available commands:")
                print("  start                - Start profiling")
                print("  stop                 - Stop profiling")
                print("  profile trit         - Profile Trit operations")
                print("  profile tritarray    - Profile TritArray operations")
                print("  profile memory       - Profile memory operations")
                print("  profile process      - Profile process operations")
                print("  report               - Show profiling report")
                print("  save <filename>      - Save profiling report")
                print("  suggestions          - Show optimization suggestions")
                print("  quit                 - Exit profiler")
            
            elif cmd == 'start':
                profiler.start_profiling()
            
            elif cmd == 'stop':
                profiler.stop_profiling()
            
            elif cmd == 'profile':
                if len(command) >= 2:
                    subcmd = command[1].lower()
                    if subcmd == 'trit':
                        results = profiler.profile_trit_operations()
                        print("Trit profiling results:", results)
                    elif subcmd == 'tritarray':
                        results = profiler.profile_tritarray_operations()
                        print("TritArray profiling results:", results)
                    elif subcmd == 'memory':
                        results = profiler.profile_memory_operations()
                        print("Memory profiling results:", results)
                    elif subcmd == 'process':
                        results = profiler.profile_process_operations()
                        print("Process profiling results:", results)
                    else:
                        print("Usage: profile <trit|tritarray|memory|process>")
                else:
                    print("Usage: profile <trit|tritarray|memory|process>")
            
            elif cmd == 'report':
                profiler.print_profiling_report()
            
            elif cmd == 'save':
                if len(command) >= 2:
                    filename = command[1]
                    profiler.save_profiling_report(filename)
                else:
                    print("Usage: save <filename>")
            
            elif cmd == 'suggestions':
                suggestions = profiler.generate_optimization_suggestions()
                if suggestions:
                    print("Optimization Suggestions:")
                    for suggestion in suggestions:
                        print(f"  - {suggestion}")
                else:
                    print("No optimization suggestions available")
            
            elif cmd == 'quit':
                break
            
            else:
                print(f"Unknown command: {cmd}")
                print("Type 'help' for available commands")
        
        except KeyboardInterrupt:
            print("\nUse 'quit' to exit")
        except Exception as e:
            print(f"Error: {e}")
    
    print("Goodbye!")


if __name__ == "__main__":
    main()
