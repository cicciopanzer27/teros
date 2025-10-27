"""
System Initialization for TEROS.

This module provides system initialization and service startup.
"""

from typing import Dict, List, Optional, Any, Callable
import threading
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..hal.device_manager import HALDeviceManager, DeviceType
from ..hal.driver_framework import DriverManager, ConsoleDriver, StorageDriver, NetworkDriver


class ServiceState(Enum):
    """Service states."""
    STOPPED = "stopped"
    STARTING = "starting"
    RUNNING = "running"
    STOPPING = "stopping"
    ERROR = "error"


class ServicePriority(Enum):
    """Service priorities."""
    CRITICAL = "critical"
    HIGH = "high"
    MEDIUM = "medium"
    LOW = "low"


class SystemService:
    """
    System Service - Represents a system service.
    
    Provides service lifecycle management and monitoring.
    """
    
    def __init__(self, name: str, priority: ServicePriority, 
                 startup_func: Callable = None, shutdown_func: Callable = None):
        """
        Initialize system service.
        
        Args:
            name: Service name
            priority: Service priority
            startup_func: Startup function
            shutdown_func: Shutdown function
        """
        self.name = name
        self.priority = priority
        self.state = ServiceState.STOPPED
        self.startup_func = startup_func
        self.shutdown_func = shutdown_func
        
        # Service statistics
        self.stats = {
            'start_time': 0,
            'stop_time': 0,
            'restart_count': 0,
            'error_count': 0
        }
        
        # Threading
        self.lock = threading.Lock()
    
    def start(self) -> bool:
        """Start the service."""
        try:
            with self.lock:
                if self.state != ServiceState.STOPPED:
                    return False
                
                self.state = ServiceState.STARTING
                self.stats['start_time'] = time.time()
                
                if self.startup_func:
                    result = self.startup_func()
                    if not result:
                        self.state = ServiceState.ERROR
                        return False
                
                self.state = ServiceState.RUNNING
                print(f"Service {self.name} started")
                return True
                
        except Exception as e:
            with self.lock:
                self.state = ServiceState.ERROR
                self.stats['error_count'] += 1
            print(f"Failed to start service {self.name}: {e}")
            return False
    
    def stop(self) -> bool:
        """Stop the service."""
        try:
            with self.lock:
                if self.state != ServiceState.RUNNING:
                    return False
                
                self.state = ServiceState.STOPPING
                
                if self.shutdown_func:
                    result = self.shutdown_func()
                    if not result:
                        self.state = ServiceState.ERROR
                        return False
                
                self.state = ServiceState.STOPPED
                self.stats['stop_time'] = time.time()
                print(f"Service {self.name} stopped")
                return True
                
        except Exception as e:
            with self.lock:
                self.state = ServiceState.ERROR
                self.stats['error_count'] += 1
            print(f"Failed to stop service {self.name}: {e}")
            return False
    
    def restart(self) -> bool:
        """Restart the service."""
        try:
            if self.stop():
                time.sleep(0.1)  # Brief pause
                if self.start():
                    self.stats['restart_count'] += 1
                    return True
            return False
        except Exception as e:
            print(f"Failed to restart service {self.name}: {e}")
            return False
    
    def get_status(self) -> Dict[str, Any]:
        """Get service status."""
        with self.lock:
            return {
                'name': self.name,
                'priority': self.priority.value,
                'state': self.state.value,
                **self.stats
            }


class SystemInitializer:
    """
    System Initializer - Initializes the ternary operating system.
    
    Provides system initialization, service management, and startup.
    """
    
    def __init__(self):
        """Initialize system initializer."""
        self.services = {}  # service_name -> SystemService
        self.service_order = []  # Ordered list of services by priority
        
        # System components
        self.device_manager = HALDeviceManager()
        self.driver_manager = DriverManager()
        
        # Initialization state
        self.initialization_complete = False
        self.initialization_error = None
        
        # Statistics
        self.stats = {
            'initialization_start_time': 0,
            'initialization_end_time': 0,
            'services_started': 0,
            'services_failed': 0,
            'drivers_loaded': 0
        }
    
    def initialize_system(self) -> bool:
        """
        Initialize the entire system.
        
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            import time
            self.stats['initialization_start_time'] = time.time()
            
            print("=== TEROS System Initialization ===")
            print("Initializing ternary operating system...")
            
            # Step 1: Initialize core components
            if not self._initialize_core_components():
                return False
            
            # Step 2: Initialize hardware
            if not self._initialize_hardware():
                return False
            
            # Step 3: Initialize drivers
            if not self._initialize_drivers():
                return False
            
            # Step 4: Initialize services
            if not self._initialize_services():
                return False
            
            # Step 5: Start services
            if not self._start_services():
                return False
            
            # Initialization complete
            self.initialization_complete = True
            self.stats['initialization_end_time'] = time.time()
            
            init_time = self.stats['initialization_end_time'] - self.stats['initialization_start_time']
            print(f"=== System Initialization Complete ===")
            print(f"Initialization time: {init_time:.2f} seconds")
            print(f"Services started: {self.stats['services_started']}")
            print(f"Drivers loaded: {self.stats['drivers_loaded']}")
            
            return True
            
        except Exception as e:
            self.initialization_error = str(e)
            print(f"System initialization failed: {e}")
            return False
    
    def _initialize_core_components(self) -> bool:
        """Initialize core system components."""
        try:
            print("Initializing core components...")
            
            # Initialize device manager
            print("  Initializing device manager...")
            # Device manager is already initialized in __init__
            
            # Initialize driver manager
            print("  Initializing driver manager...")
            # Driver manager is already initialized in __init__
            
            print("Core components initialized")
            return True
            
        except Exception as e:
            print(f"Core component initialization failed: {e}")
            return False
    
    def _initialize_hardware(self) -> bool:
        """Initialize hardware components."""
        try:
            print("Initializing hardware...")
            
            # Start device discovery
            self.device_manager.start_auto_discovery()
            
            # Wait for device discovery
            time.sleep(0.5)
            
            # Get detected devices
            devices = self.device_manager.get_all_devices()
            print(f"  Detected {len(devices)} hardware devices")
            
            for device in devices:
                print(f"    {device.device_type.value}: {device.device_id}")
            
            print("Hardware initialization complete")
            return True
            
        except Exception as e:
            print(f"Hardware initialization failed: {e}")
            return False
    
    def _initialize_drivers(self) -> bool:
        """Initialize device drivers."""
        try:
            print("Initializing drivers...")
            
            # Create and register drivers
            drivers = [
                ConsoleDriver("console_0"),
                StorageDriver("storage_0"),
                NetworkDriver("network_0")
            ]
            
            for driver in drivers:
                if self.driver_manager.register_driver(driver):
                    self.stats['drivers_loaded'] += 1
                    print(f"  Driver {driver.device_id} loaded")
                else:
                    print(f"  Failed to load driver {driver.device_id}")
            
            print(f"Drivers initialized: {self.stats['drivers_loaded']} loaded")
            return True
            
        except Exception as e:
            print(f"Driver initialization failed: {e}")
            return False
    
    def _initialize_services(self) -> bool:
        """Initialize system services."""
        try:
            print("Initializing services...")
            
            # Define system services
            services = [
                ('Memory Manager', ServicePriority.CRITICAL, self._start_memory_manager, self._stop_memory_manager),
                ('Process Manager', ServicePriority.CRITICAL, self._start_process_manager, self._stop_process_manager),
                ('File System', ServicePriority.HIGH, self._start_file_system, self._stop_file_system),
                ('I/O Manager', ServicePriority.HIGH, self._start_io_manager, self._stop_io_manager),
                ('Security Manager', ServicePriority.MEDIUM, self._start_security_manager, self._stop_security_manager),
                ('Network Manager', ServicePriority.MEDIUM, self._start_network_manager, self._stop_network_manager)
            ]
            
            # Create services
            for name, priority, startup_func, shutdown_func in services:
                service = SystemService(name, priority, startup_func, shutdown_func)
                self.services[name] = service
                self.service_order.append(name)
            
            print(f"Services initialized: {len(self.services)} services")
            return True
            
        except Exception as e:
            print(f"Service initialization failed: {e}")
            return False
    
    def _start_services(self) -> bool:
        """Start system services."""
        try:
            print("Starting services...")
            
            # Start services in priority order
            for service_name in self.service_order:
                service = self.services[service_name]
                print(f"  Starting {service_name}...")
                
                if service.start():
                    self.stats['services_started'] += 1
                else:
                    self.stats['services_failed'] += 1
                    print(f"  Failed to start {service_name}")
            
            print(f"Services started: {self.stats['services_started']}/{len(self.services)}")
            return self.stats['services_failed'] == 0
            
        except Exception as e:
            print(f"Service startup failed: {e}")
            return False
    
    # Service startup/shutdown functions
    def _start_memory_manager(self) -> bool:
        """Start memory manager service."""
        print("    Memory Manager: Initializing ternary memory management...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_memory_manager(self) -> bool:
        """Stop memory manager service."""
        print("    Memory Manager: Shutting down...")
        return True
    
    def _start_process_manager(self) -> bool:
        """Start process manager service."""
        print("    Process Manager: Initializing process scheduling...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_process_manager(self) -> bool:
        """Stop process manager service."""
        print("    Process Manager: Shutting down...")
        return True
    
    def _start_file_system(self) -> bool:
        """Start file system service."""
        print("    File System: Initializing ternary file system...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_file_system(self) -> bool:
        """Stop file system service."""
        print("    File System: Shutting down...")
        return True
    
    def _start_io_manager(self) -> bool:
        """Start I/O manager service."""
        print("    I/O Manager: Initializing I/O operations...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_io_manager(self) -> bool:
        """Stop I/O manager service."""
        print("    I/O Manager: Shutting down...")
        return True
    
    def _start_security_manager(self) -> bool:
        """Start security manager service."""
        print("    Security Manager: Initializing security controls...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_security_manager(self) -> bool:
        """Stop security manager service."""
        print("    Security Manager: Shutting down...")
        return True
    
    def _start_network_manager(self) -> bool:
        """Start network manager service."""
        print("    Network Manager: Initializing network stack...")
        time.sleep(0.1)  # Simulate initialization
        return True
    
    def _stop_network_manager(self) -> bool:
        """Stop network manager service."""
        print("    Network Manager: Shutting down...")
        return True
    
    def get_system_status(self) -> Dict[str, Any]:
        """Get system status."""
        return {
            'initialization_complete': self.initialization_complete,
            'initialization_error': self.initialization_error,
            'services': {name: service.get_status() for name, service in self.services.items()},
            'device_manager': self.device_manager.get_stats(),
            'driver_manager': self.driver_manager.get_stats(),
            **self.stats
        }
    
    def shutdown_system(self) -> bool:
        """Shutdown the system."""
        try:
            print("=== TEROS System Shutdown ===")
            print("Shutting down ternary operating system...")
            
            # Stop services in reverse order
            for service_name in reversed(self.service_order):
                service = self.services[service_name]
                print(f"  Stopping {service_name}...")
                service.stop()
            
            # Stop device discovery
            self.device_manager.stop_auto_discovery()
            
            # Cleanup managers
            self.device_manager.cleanup()
            self.driver_manager.cleanup()
            
            print("System shutdown complete")
            return True
            
        except Exception as e:
            print(f"System shutdown failed: {e}")
            return False
    
    def cleanup(self) -> None:
        """Cleanup system initializer."""
        if self.initialization_complete:
            self.shutdown_system()
        
        print("System initializer cleaned up")
