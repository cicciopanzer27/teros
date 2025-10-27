"""
Ternary System Monitor - System monitoring application for TEROS.

This module provides comprehensive system monitoring capabilities for the ternary operating system.
"""

from typing import List, Union, Optional, Dict, Any, Tuple
import time
import threading
import psutil
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..vm.tvm import TVM
from ..hal.device_manager import HALDeviceManager, DeviceType, DeviceStatus
from ..memory.memory_manager import TernaryMemoryManager
from ..process.scheduler import TernaryScheduler
from ..libs.libgraphics import TernaryGraphics, TernaryCanvas, TernaryColor


class MonitorType(Enum):
    """Monitor types."""
    CPU = "cpu"
    MEMORY = "memory"
    PROCESSES = "processes"
    DEVICES = "devices"
    NETWORK = "network"
    STORAGE = "storage"
    SYSTEM = "system"


class SystemMetric:
    """
    System Metric - Represents a system metric.
    
    Provides metric information and monitoring capabilities.
    """
    
    def __init__(self, name: str, value: float, unit: str = "", 
                 min_value: float = 0.0, max_value: float = 100.0):
        """
        Initialize system metric.
        
        Args:
            name: Metric name
            value: Current value
            unit: Value unit
            min_value: Minimum value
            max_value: Maximum value
        """
        self.name = name
        self.value = value
        self.unit = unit
        self.min_value = min_value
        self.max_value = max_value
        
        # History for trending
        self.history = []
        self.max_history = 100
        
        # Thresholds
        self.warning_threshold = 80.0
        self.critical_threshold = 95.0
    
    def update(self, new_value: float) -> None:
        """
        Update metric value.
        
        Args:
            new_value: New metric value
        """
        self.value = new_value
        
        # Add to history
        self.history.append(new_value)
        if len(self.history) > self.max_history:
            self.history.pop(0)
    
    def get_status(self) -> str:
        """
        Get metric status.
        
        Returns:
            Status string (normal, warning, critical)
        """
        if self.value >= self.critical_threshold:
            return "critical"
        elif self.value >= self.warning_threshold:
            return "warning"
        else:
            return "normal"
    
    def get_trend(self) -> str:
        """
        Get metric trend.
        
        Returns:
            Trend string (up, down, stable)
        """
        if len(self.history) < 2:
            return "stable"
        
        recent = self.history[-5:] if len(self.history) >= 5 else self.history
        if len(recent) < 2:
            return "stable"
        
        avg_recent = sum(recent) / len(recent)
        avg_older = sum(self.history[:-len(recent)]) / len(self.history[:-len(recent)]) if len(self.history) > len(recent) else avg_recent
        
        if avg_recent > avg_older * 1.05:
            return "up"
        elif avg_recent < avg_older * 0.95:
            return "down"
        else:
            return "stable"


class ProcessInfo:
    """
    Process Information - Represents process information.
    
    Provides process details and monitoring capabilities.
    """
    
    def __init__(self, pid: int, name: str, cpu_percent: float, 
                 memory_percent: float, status: str):
        """
        Initialize process information.
        
        Args:
            pid: Process ID
            name: Process name
            cpu_percent: CPU usage percentage
            memory_percent: Memory usage percentage
            status: Process status
        """
        self.pid = pid
        self.name = name
        self.cpu_percent = cpu_percent
        self.memory_percent = memory_percent
        self.status = status
        self.start_time = time.time()
        self.last_update = time.time()


class TernarySystemMonitor:
    """
    Ternary System Monitor - Main system monitoring application.
    
    Provides comprehensive system monitoring including CPU, memory, processes,
    devices, network, and storage monitoring.
    """
    
    def __init__(self):
        """Initialize system monitor."""
        self.metrics = {}  # metric_name -> SystemMetric
        self.processes = {}  # pid -> ProcessInfo
        self.devices = []
        
        # Monitor state
        self.is_monitoring = False
        self.monitor_thread = None
        self.update_interval = 1.0  # seconds
        
        # Graphics for visualization
        self.graphics = TernaryGraphics()
        self.canvas = None
        
        # Initialize metrics
        self._initialize_metrics()
        
        # Initialize devices
        self._initialize_devices()
    
    def _initialize_metrics(self) -> None:
        """Initialize system metrics."""
        # CPU metrics
        self.metrics['cpu_usage'] = SystemMetric("CPU Usage", 0.0, "%", 0.0, 100.0)
        self.metrics['cpu_cores'] = SystemMetric("CPU Cores", 0.0, "cores", 1.0, 64.0)
        self.metrics['cpu_frequency'] = SystemMetric("CPU Frequency", 0.0, "MHz", 100.0, 5000.0)
        
        # Memory metrics
        self.metrics['memory_usage'] = SystemMetric("Memory Usage", 0.0, "%", 0.0, 100.0)
        self.metrics['memory_total'] = SystemMetric("Total Memory", 0.0, "GB", 0.0, 1000.0)
        self.metrics['memory_available'] = SystemMetric("Available Memory", 0.0, "GB", 0.0, 1000.0)
        self.metrics['memory_used'] = SystemMetric("Used Memory", 0.0, "GB", 0.0, 1000.0)
        
        # Process metrics
        self.metrics['process_count'] = SystemMetric("Process Count", 0.0, "processes", 0.0, 10000.0)
        self.metrics['thread_count'] = SystemMetric("Thread Count", 0.0, "threads", 0.0, 100000.0)
        
        # System metrics
        self.metrics['uptime'] = SystemMetric("System Uptime", 0.0, "seconds", 0.0, 86400.0)
        self.metrics['load_average'] = SystemMetric("Load Average", 0.0, "", 0.0, 100.0)
        
        # Network metrics
        self.metrics['network_sent'] = SystemMetric("Network Sent", 0.0, "MB", 0.0, 10000.0)
        self.metrics['network_received'] = SystemMetric("Network Received", 0.0, "MB", 0.0, 10000.0)
        
        # Storage metrics
        self.metrics['disk_usage'] = SystemMetric("Disk Usage", 0.0, "%", 0.0, 100.0)
        self.metrics['disk_total'] = SystemMetric("Total Disk", 0.0, "GB", 0.0, 10000.0)
        self.metrics['disk_free'] = SystemMetric("Free Disk", 0.0, "GB", 0.0, 10000.0)
    
    def _initialize_devices(self) -> None:
        """Initialize device monitoring."""
        try:
            self.device_manager = HALDeviceManager()
            self.devices = self.device_manager.get_all_devices()
        except Exception as e:
            print(f"Failed to initialize device manager: {e}")
            self.devices = []
    
    def start_monitoring(self) -> bool:
        """
        Start system monitoring.
        
        Returns:
            True if monitoring started successfully, False otherwise
        """
        if self.is_monitoring:
            return True
        
        try:
            self.is_monitoring = True
            self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
            self.monitor_thread.start()
            
            print("System monitoring started")
            return True
        except Exception as e:
            print(f"Failed to start monitoring: {e}")
            self.is_monitoring = False
            return False
    
    def stop_monitoring(self) -> None:
        """Stop system monitoring."""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=2.0)
        print("System monitoring stopped")
    
    def _monitor_loop(self) -> None:
        """Main monitoring loop."""
        while self.is_monitoring:
            try:
                self._update_metrics()
                self._update_processes()
                self._update_devices()
                time.sleep(self.update_interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.update_interval)
    
    def _update_metrics(self) -> None:
        """Update system metrics."""
        try:
            # CPU metrics
            cpu_percent = psutil.cpu_percent(interval=None)
            cpu_count = psutil.cpu_count()
            cpu_freq = psutil.cpu_freq()
            
            self.metrics['cpu_usage'].update(cpu_percent)
            self.metrics['cpu_cores'].update(cpu_count)
            self.metrics['cpu_frequency'].update(cpu_freq.current if cpu_freq else 0.0)
            
            # Memory metrics
            memory = psutil.virtual_memory()
            self.metrics['memory_usage'].update(memory.percent)
            self.metrics['memory_total'].update(memory.total / (1024**3))  # Convert to GB
            self.metrics['memory_available'].update(memory.available / (1024**3))
            self.metrics['memory_used'].update(memory.used / (1024**3))
            
            # Process metrics
            process_count = len(psutil.pids())
            thread_count = psutil.Process().num_threads()
            
            self.metrics['process_count'].update(process_count)
            self.metrics['thread_count'].update(thread_count)
            
            # System metrics
            boot_time = psutil.boot_time()
            uptime = time.time() - boot_time
            load_avg = psutil.getloadavg()[0] if hasattr(psutil, 'getloadavg') else 0.0
            
            self.metrics['uptime'].update(uptime)
            self.metrics['load_average'].update(load_avg)
            
            # Network metrics
            network = psutil.net_io_counters()
            if network:
                self.metrics['network_sent'].update(network.bytes_sent / (1024**2))  # Convert to MB
                self.metrics['network_received'].update(network.bytes_recv / (1024**2))
            
            # Storage metrics
            disk = psutil.disk_usage('/')
            self.metrics['disk_usage'].update(disk.percent)
            self.metrics['disk_total'].update(disk.total / (1024**3))  # Convert to GB
            self.metrics['disk_free'].update(disk.free / (1024**3))
            
        except Exception as e:
            print(f"Failed to update metrics: {e}")
    
    def _update_processes(self) -> None:
        """Update process information."""
        try:
            current_pids = set()
            
            for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent', 'status']):
                try:
                    info = proc.info
                    pid = info['pid']
                    current_pids.add(pid)
                    
                    if pid not in self.processes:
                        self.processes[pid] = ProcessInfo(
                            pid, info['name'], info['cpu_percent'], 
                            info['memory_percent'], info['status']
                        )
                    else:
                        proc_info = self.processes[pid]
                        proc_info.cpu_percent = info['cpu_percent']
                        proc_info.memory_percent = info['memory_percent']
                        proc_info.status = info['status']
                        proc_info.last_update = time.time()
                
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    continue
            
            # Remove processes that no longer exist
            dead_pids = set(self.processes.keys()) - current_pids
            for pid in dead_pids:
                del self.processes[pid]
                
        except Exception as e:
            print(f"Failed to update processes: {e}")
    
    def _update_devices(self) -> None:
        """Update device information."""
        try:
            if hasattr(self, 'device_manager'):
                self.devices = self.device_manager.get_all_devices()
        except Exception as e:
            print(f"Failed to update devices: {e}")
    
    def get_metric(self, name: str) -> Optional[SystemMetric]:
        """
        Get metric by name.
        
        Args:
            name: Metric name
            
        Returns:
            SystemMetric instance or None if not found
        """
        return self.metrics.get(name)
    
    def get_all_metrics(self) -> Dict[str, SystemMetric]:
        """Get all metrics."""
        return self.metrics.copy()
    
    def get_processes(self, limit: int = 10) -> List[ProcessInfo]:
        """
        Get top processes by CPU usage.
        
        Args:
            limit: Maximum number of processes to return
            
        Returns:
            List of ProcessInfo instances
        """
        sorted_processes = sorted(
            self.processes.values(),
            key=lambda p: p.cpu_percent,
            reverse=True
        )
        return sorted_processes[:limit]
    
    def get_devices(self) -> List:
        """Get all devices."""
        return self.devices.copy()
    
    def create_visualization(self, width: int = 80, height: int = 24) -> TernaryCanvas:
        """
        Create system visualization.
        
        Args:
            width: Canvas width
            height: Canvas height
            
        Returns:
            TernaryCanvas with system visualization
        """
        self.canvas = self.graphics.create_canvas(width, height)
        
        # Clear canvas
        self.canvas.clear(TernaryColor.BLACK)
        
        # Draw system information
        self._draw_system_info()
        self._draw_cpu_usage()
        self._draw_memory_usage()
        self._draw_processes()
        self._draw_devices()
        
        return self.canvas
    
    def _draw_system_info(self) -> None:
        """Draw system information."""
        if not self.canvas:
            return
        
        # System title
        self.graphics.draw_text("TEROS System Monitor", 2, 1, TernaryColor.WHITE)
        
        # System metrics
        uptime = self.metrics['uptime'].value
        uptime_str = f"Uptime: {int(uptime//3600)}h {int((uptime%3600)//60)}m"
        self.graphics.draw_text(uptime_str, 2, 3, TernaryColor.GRAY)
        
        load_avg = self.metrics['load_average'].value
        load_str = f"Load: {load_avg:.2f}"
        self.graphics.draw_text(load_str, 2, 4, TernaryColor.GRAY)
    
    def _draw_cpu_usage(self) -> None:
        """Draw CPU usage visualization."""
        if not self.canvas:
            return
        
        cpu_usage = self.metrics['cpu_usage'].value
        cpu_cores = int(self.metrics['cpu_cores'].value)
        
        # CPU title
        self.graphics.draw_text("CPU", 2, 6, TernaryColor.WHITE)
        
        # CPU usage bar
        bar_width = 20
        bar_height = 3
        bar_x = 2
        bar_y = 8
        
        # Draw background
        self.graphics.draw_rectangle(bar_x, bar_y, bar_width, bar_height, filled=True)
        
        # Draw usage
        usage_width = int((cpu_usage / 100.0) * bar_width)
        if usage_width > 0:
            color = TernaryColor.WHITE if cpu_usage < 80 else TernaryColor.BLACK
            self.graphics.draw_rectangle(bar_x, bar_y, usage_width, bar_height, filled=True)
        
        # CPU info
        cpu_str = f"{cpu_usage:.1f}% ({cpu_cores} cores)"
        self.graphics.draw_text(cpu_str, 2, 12, TernaryColor.GRAY)
    
    def _draw_memory_usage(self) -> None:
        """Draw memory usage visualization."""
        if not self.canvas:
            return
        
        memory_usage = self.metrics['memory_usage'].value
        memory_total = self.metrics['memory_total'].value
        memory_used = self.metrics['memory_used'].value
        
        # Memory title
        self.graphics.draw_text("Memory", 2, 14, TernaryColor.WHITE)
        
        # Memory usage bar
        bar_width = 20
        bar_height = 3
        bar_x = 2
        bar_y = 16
        
        # Draw background
        self.graphics.draw_rectangle(bar_x, bar_y, bar_width, bar_height, filled=True)
        
        # Draw usage
        usage_width = int((memory_usage / 100.0) * bar_width)
        if usage_width > 0:
            color = TernaryColor.WHITE if memory_usage < 80 else TernaryColor.BLACK
            self.graphics.draw_rectangle(bar_x, bar_y, usage_width, bar_height, filled=True)
        
        # Memory info
        memory_str = f"{memory_usage:.1f}% ({memory_used:.1f}/{memory_total:.1f} GB)"
        self.graphics.draw_text(memory_str, 2, 20, TernaryColor.GRAY)
    
    def _draw_processes(self) -> None:
        """Draw top processes."""
        if not self.canvas:
            return
        
        # Processes title
        self.graphics.draw_text("Top Processes", 30, 6, TernaryColor.WHITE)
        
        # Get top processes
        top_processes = self.get_processes(5)
        
        y_pos = 8
        for i, proc in enumerate(top_processes):
            if y_pos >= self.canvas.height - 2:
                break
            
            # Process info
            proc_str = f"{proc.name[:15]:<15} {proc.cpu_percent:5.1f}% {proc.memory_percent:5.1f}%"
            self.graphics.draw_text(proc_str, 30, y_pos, TernaryColor.GRAY)
            y_pos += 2
    
    def _draw_devices(self) -> None:
        """Draw device information."""
        if not self.canvas:
            return
        
        # Devices title
        self.graphics.draw_text("Devices", 30, 18, TernaryColor.WHITE)
        
        # Device count
        device_count = len(self.devices)
        device_str = f"Total: {device_count}"
        self.graphics.draw_text(device_str, 30, 20, TernaryColor.GRAY)
        
        # Show first few devices
        y_pos = 22
        for i, device in enumerate(self.devices[:3]):
            if y_pos >= self.canvas.height - 1:
                break
            
            device_str = f"{device.device_id[:20]:<20} {device.device_type.value}"
            self.graphics.draw_text(device_str, 30, y_pos, TernaryColor.GRAY)
            y_pos += 1
    
    def generate_report(self) -> str:
        """Generate system monitoring report."""
        report = []
        report.append("=== TEROS System Monitor Report ===")
        report.append("")
        
        # System information
        report.append("System Information:")
        uptime = self.metrics['uptime'].value
        report.append(f"  Uptime: {int(uptime//3600)}h {int((uptime%3600)//60)}m {int(uptime%60)}s")
        report.append(f"  Load Average: {self.metrics['load_average'].value:.2f}")
        report.append("")
        
        # CPU information
        report.append("CPU Information:")
        report.append(f"  Usage: {self.metrics['cpu_usage'].value:.1f}%")
        report.append(f"  Cores: {int(self.metrics['cpu_cores'].value)}")
        report.append(f"  Frequency: {self.metrics['cpu_frequency'].value:.0f} MHz")
        report.append("")
        
        # Memory information
        report.append("Memory Information:")
        report.append(f"  Usage: {self.metrics['memory_usage'].value:.1f}%")
        report.append(f"  Total: {self.metrics['memory_total'].value:.1f} GB")
        report.append(f"  Used: {self.metrics['memory_used'].value:.1f} GB")
        report.append(f"  Available: {self.metrics['memory_available'].value:.1f} GB")
        report.append("")
        
        # Process information
        report.append("Process Information:")
        report.append(f"  Total Processes: {int(self.metrics['process_count'].value)}")
        report.append(f"  Total Threads: {int(self.metrics['thread_count'].value)}")
        report.append("")
        
        # Top processes
        report.append("Top Processes by CPU:")
        top_processes = self.get_processes(10)
        for i, proc in enumerate(top_processes):
            report.append(f"  {i+1:2d}. {proc.name:<20} {proc.cpu_percent:5.1f}% {proc.memory_percent:5.1f}%")
        report.append("")
        
        # Device information
        report.append("Device Information:")
        report.append(f"  Total Devices: {len(self.devices)}")
        for device in self.devices[:5]:
            report.append(f"  {device.device_id}: {device.device_type.value} ({device.status.value})")
        report.append("")
        
        # Network information
        report.append("Network Information:")
        report.append(f"  Bytes Sent: {self.metrics['network_sent'].value:.1f} MB")
        report.append(f"  Bytes Received: {self.metrics['network_received'].value:.1f} MB")
        report.append("")
        
        # Storage information
        report.append("Storage Information:")
        report.append(f"  Usage: {self.metrics['disk_usage'].value:.1f}%")
        report.append(f"  Total: {self.metrics['disk_total'].value:.1f} GB")
        report.append(f"  Free: {self.metrics['disk_free'].value:.1f} GB")
        report.append("")
        
        return "\n".join(report)
    
    def save_report(self, filename: str) -> bool:
        """
        Save monitoring report to file.
        
        Args:
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        try:
            report = self.generate_report()
            with open(filename, 'w') as f:
                f.write(report)
            print(f"System monitor report saved to {filename}")
            return True
        except Exception as e:
            print(f"Failed to save report: {e}")
            return False
    
    def run_interactive(self) -> None:
        """Run interactive system monitor."""
        print("=== TEROS System Monitor ===")
        print("Starting interactive system monitor...")
        print("Press Ctrl+C to exit")
        print()
        
        try:
            # Start monitoring
            if not self.start_monitoring():
                print("Failed to start monitoring")
                return
            
            # Main loop
            while True:
                # Clear screen (simple approach)
                print("\033[2J\033[H", end="")
                
                # Generate and display visualization
                canvas = self.create_visualization(80, 24)
                
                # Convert canvas to text representation
                for y in range(canvas.height):
                    row = ""
                    for x in range(canvas.width):
                        pixel = canvas.get_pixel(x, y)
                        if pixel:
                            if pixel.value == 1:
                                row += "█"
                            elif pixel.value == 0:
                                row += "░"
                            else:
                                row += " "
                        else:
                            row += " "
                    print(row)
                
                # Wait for next update
                time.sleep(2.0)
                
        except KeyboardInterrupt:
            print("\nStopping system monitor...")
        finally:
            self.stop_monitoring()
    
    def cleanup(self) -> None:
        """Cleanup monitor resources."""
        self.stop_monitoring()
        if hasattr(self, 'device_manager'):
            self.device_manager.cleanup()
    
    def __del__(self):
        """Destructor."""
        self.cleanup()


if __name__ == "__main__":
    # Demo system monitor
    print("=== TEROS System Monitor Demo ===")
    
    monitor = TernarySystemMonitor()
    
    try:
        # Run interactive monitor
        monitor.run_interactive()
    except Exception as e:
        print(f"Monitor error: {e}")
    finally:
        monitor.cleanup()
