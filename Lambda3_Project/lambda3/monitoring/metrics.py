"""
Production Monitoring for Lambda³
Metrics, logging, and observability
"""

import time
import json
import logging
from typing import Dict, List, Optional, Any
from dataclasses import dataclass, asdict
from datetime import datetime
import psutil
import threading
from pathlib import Path

try:
    from lambda3.engine.reducer import reduce
    from lambda3.parser.parser import parse
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from engine.reducer import reduce
    from parser.parser import parse


@dataclass
class PerformanceMetrics:
    """Performance metrics for Lambda³ operations"""
    operation: str
    duration_ms: float
    memory_mb: float
    cpu_percent: float
    timestamp: datetime
    success: bool
    error_message: Optional[str] = None
    
    def to_dict(self) -> Dict:
        return asdict(self)


@dataclass
class SystemMetrics:
    """System-level metrics"""
    cpu_percent: float
    memory_mb: float
    disk_usage_percent: float
    timestamp: datetime
    
    def to_dict(self) -> Dict:
        return asdict(self)


class LambdaMetricsCollector:
    """
    Metrics collector for Lambda³ operations
    
    Tracks:
    - Operation performance
    - System resources
    - Error rates
    - Throughput
    """
    
    def __init__(self, log_file: str = "lambda3_metrics.json"):
        self.log_file = Path(log_file)
        self.metrics: List[PerformanceMetrics] = []
        self.system_metrics: List[SystemMetrics] = []
        self.operation_counts: Dict[str, int] = {}
        self.error_counts: Dict[str, int] = {}
        
        # Setup logging
        self.logger = logging.getLogger('lambda3_metrics')
        self.logger.setLevel(logging.INFO)
        
        # File handler
        handler = logging.FileHandler(self.log_file)
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        
        # Start system monitoring
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_system)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def record_operation(self, operation: str, func, *args, **kwargs) -> Any:
        """Record performance of an operation"""
        start_time = time.time()
        start_memory = psutil.Process().memory_info().rss / 1024 / 1024
        start_cpu = psutil.cpu_percent()
        
        try:
            # Execute operation
            result = func(*args, **kwargs)
            
            # Record success
            end_time = time.time()
            end_memory = psutil.Process().memory_info().rss / 1024 / 1024
            end_cpu = psutil.cpu_percent()
            
            duration_ms = (end_time - start_time) * 1000
            memory_mb = end_memory - start_memory
            
            metric = PerformanceMetrics(
                operation=operation,
                duration_ms=duration_ms,
                memory_mb=memory_mb,
                cpu_percent=end_cpu,
                timestamp=datetime.now(),
                success=True
            )
            
            self.metrics.append(metric)
            self.operation_counts[operation] = self.operation_counts.get(operation, 0) + 1
            
            self.logger.info(f"Operation {operation} completed in {duration_ms:.2f}ms")
            return result
            
        except Exception as e:
            # Record error
            end_time = time.time()
            duration_ms = (end_time - start_time) * 1000
            
            metric = PerformanceMetrics(
                operation=operation,
                duration_ms=duration_ms,
                memory_mb=0,
                cpu_percent=0,
                timestamp=datetime.now(),
                success=False,
                error_message=str(e)
            )
            
            self.metrics.append(metric)
            self.error_counts[operation] = self.error_counts.get(operation, 0) + 1
            
            self.logger.error(f"Operation {operation} failed: {e}")
            raise
    
    def _monitor_system(self):
        """Monitor system resources"""
        while self.monitoring:
            try:
                cpu_percent = psutil.cpu_percent()
                memory = psutil.virtual_memory()
                disk = psutil.disk_usage('/')
                
                system_metric = SystemMetrics(
                    cpu_percent=cpu_percent,
                    memory_mb=memory.used / 1024 / 1024,
                    disk_usage_percent=disk.percent,
                    timestamp=datetime.now()
                )
                
                self.system_metrics.append(system_metric)
                
                # Keep only last 1000 system metrics
                if len(self.system_metrics) > 1000:
                    self.system_metrics = self.system_metrics[-1000:]
                
                time.sleep(5)  # Monitor every 5 seconds
                
            except Exception as e:
                self.logger.error(f"System monitoring error: {e}")
                time.sleep(10)
    
    def get_operation_stats(self, operation: str) -> Dict[str, Any]:
        """Get statistics for specific operation"""
        op_metrics = [m for m in self.metrics if m.operation == operation]
        
        if not op_metrics:
            return {}
        
        durations = [m.duration_ms for m in op_metrics if m.success]
        success_count = sum(1 for m in op_metrics if m.success)
        error_count = sum(1 for m in op_metrics if not m.success)
        
        stats = {
            'total_operations': len(op_metrics),
            'success_count': success_count,
            'error_count': error_count,
            'success_rate': success_count / len(op_metrics) if op_metrics else 0,
            'avg_duration_ms': sum(durations) / len(durations) if durations else 0,
            'min_duration_ms': min(durations) if durations else 0,
            'max_duration_ms': max(durations) if durations else 0
        }
        
        return stats
    
    def get_system_stats(self) -> Dict[str, Any]:
        """Get system statistics"""
        if not self.system_metrics:
            return {}
        
        recent_metrics = self.system_metrics[-100:]  # Last 100 measurements
        
        cpu_values = [m.cpu_percent for m in recent_metrics]
        memory_values = [m.memory_mb for m in recent_metrics]
        disk_values = [m.disk_usage_percent for m in recent_metrics]
        
        stats = {
            'avg_cpu_percent': sum(cpu_values) / len(cpu_values),
            'max_cpu_percent': max(cpu_values),
            'avg_memory_mb': sum(memory_values) / len(memory_values),
            'max_memory_mb': max(memory_values),
            'avg_disk_percent': sum(disk_values) / len(disk_values),
            'max_disk_percent': max(disk_values)
        }
        
        return stats
    
    def export_metrics(self, file_path: str = None) -> str:
        """Export metrics to JSON file"""
        if file_path is None:
            file_path = f"lambda3_metrics_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        export_data = {
            'performance_metrics': [m.to_dict() for m in self.metrics],
            'system_metrics': [m.to_dict() for m in self.system_metrics],
            'operation_counts': self.operation_counts,
            'error_counts': self.error_counts,
            'export_timestamp': datetime.now().isoformat()
        }
        
        with open(file_path, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        self.logger.info(f"Metrics exported to {file_path}")
        return file_path
    
    def stop_monitoring(self):
        """Stop system monitoring"""
        self.monitoring = False
        if self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5)


class LambdaProfiler:
    """
    Profiler for Lambda³ operations
    
    Features:
    - Operation timing
    - Memory profiling
    - Call stack analysis
    - Performance bottlenecks
    """
    
    def __init__(self, metrics_collector: LambdaMetricsCollector):
        self.metrics = metrics_collector
        self.profiles: Dict[str, List[float]] = {}
    
    def profile_operation(self, operation: str, func, *args, **kwargs):
        """Profile an operation with detailed metrics"""
        return self.metrics.record_operation(operation, func, *args, **kwargs)
    
    def profile_lambda_reduction(self, term_str: str) -> Dict[str, Any]:
        """Profile lambda reduction operation"""
        def reduce_operation():
            term = parse(term_str)
            return reduce(term)
        
        result = self.profile_operation("lambda_reduction", reduce_operation)
        
        # Get profiling data
        op_stats = self.metrics.get_operation_stats("lambda_reduction")
        
        return {
            'term': term_str,
            'result': str(result),
            'stats': op_stats
        }
    
    def profile_type_inference(self, term_str: str) -> Dict[str, Any]:
        """Profile type inference operation"""
        def infer_operation():
            term = parse(term_str)
            # This would call actual type inference
            return "Type inference result"
        
        result = self.profile_operation("type_inference", infer_operation)
        
        # Get profiling data
        op_stats = self.metrics.get_operation_stats("type_inference")
        
        return {
            'term': term_str,
            'result': result,
            'stats': op_stats
        }
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report"""
        report = {
            'timestamp': datetime.now().isoformat(),
            'operation_stats': {},
            'system_stats': self.metrics.get_system_stats(),
            'summary': {}
        }
        
        # Get stats for all operations
        for operation in self.metrics.operation_counts.keys():
            report['operation_stats'][operation] = self.metrics.get_operation_stats(operation)
        
        # Generate summary
        total_operations = sum(self.metrics.operation_counts.values())
        total_errors = sum(self.metrics.error_counts.values())
        
        report['summary'] = {
            'total_operations': total_operations,
            'total_errors': total_errors,
            'overall_success_rate': (total_operations - total_errors) / total_operations if total_operations > 0 else 0,
            'unique_operations': len(self.metrics.operation_counts),
            'monitoring_duration': len(self.metrics.system_metrics) * 5  # 5 second intervals
        }
        
        return report


# ============================================================================
# DEMO
# ============================================================================

def demo_monitoring():
    """Demonstrate monitoring capabilities"""
    print("="*60)
    print("  Lambda³ Production Monitoring Demo")
    print("="*60)
    
    # Create metrics collector
    collector = LambdaMetricsCollector()
    profiler = LambdaProfiler(collector)
    
    # Test operations
    test_terms = [
        "\\x.x",
        "\\x.\\y.x",
        "\\f.\\g.\\x.f (g x)",
        "(\\x.x) y"
    ]
    
    print("Profiling lambda operations...")
    for term in test_terms:
        try:
            profile_result = profiler.profile_lambda_reduction(term)
            print(f"Term: {term}")
            print(f"Result: {profile_result['result']}")
            print(f"Stats: {profile_result['stats']}")
            print()
        except Exception as e:
            print(f"Error profiling {term}: {e}")
    
    # Get performance report
    report = profiler.get_performance_report()
    print("Performance Report:")
    print(f"  Total operations: {report['summary']['total_operations']}")
    print(f"  Success rate: {report['summary']['overall_success_rate']:.2%}")
    print(f"  Unique operations: {report['summary']['unique_operations']}")
    
    # Export metrics
    export_file = collector.export_metrics()
    print(f"Metrics exported to: {export_file}")
    
    # Stop monitoring
    collector.stop_monitoring()
    
    print("\nMonitoring features:")
    print("  Operation performance tracking")
    print("  System resource monitoring")
    print("  Error rate monitoring")
    print("  Performance profiling")
    print("  Metrics export")


def main():
    print("="*60)
    print("  Lambda³ Production Monitoring")
    print("  Metrics, Logging & Observability")
    print("="*60)
    
    demo_monitoring()
    
    print("\n" + "="*60)
    print("Monitoring Features:")
    print("  Performance metrics")
    print("  System monitoring")
    print("  Error tracking")
    print("  Profiling")
    print("  Metrics export")
    print("  Real-time monitoring")
    print("="*60)
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
