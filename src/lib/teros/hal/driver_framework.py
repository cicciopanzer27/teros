"""
Device Driver Framework for Hardware Abstraction Layer.

This module provides a framework for implementing ternary device drivers.
"""

from typing import Dict, List, Optional, Any, Callable, Union
import threading
import time
from enum import Enum
from abc import ABC, abstractmethod
from ..core.trit import Trit
from ..core.tritarray import TritArray


class DriverState(Enum):
    """Driver states."""
    UNINITIALIZED = "uninitialized"
    INITIALIZED = "initialized"
    RUNNING = "running"
    STOPPED = "stopped"
    ERROR = "error"


class DriverCapability(Enum):
    """Driver capabilities."""
    READ = "read"
    WRITE = "write"
    CONTROL = "control"
    STATUS = "status"
    CONFIGURATION = "configuration"
    INTERRUPT = "interrupt"


class TernaryDeviceDriver(ABC):
    """
    Abstract base class for ternary device drivers.
    
    All ternary device drivers must inherit from this class.
    """
    
    def __init__(self, device_id: str, device_type: str):
        """
        Initialize device driver.
        
        Args:
            device_id: Unique device identifier
            device_type: Type of device
        """
        self.device_id = device_id
        self.device_type = device_type
        self.state = DriverState.UNINITIALIZED
        self.capabilities = []
        self.interrupt_handlers = {}
        
        # Driver statistics
        self.stats = {
            'operations': 0,
            'errors': 0,
            'interrupts': 0,
            'bytes_transferred': 0
        }
        
        # Threading
        self.lock = threading.Lock()
    
    @abstractmethod
    def initialize(self) -> bool:
        """
        Initialize the device driver.
        
        Returns:
            True if initialization successful, False otherwise
        """
        pass
    
    @abstractmethod
    def cleanup(self) -> bool:
        """
        Cleanup the device driver.
        
        Returns:
            True if cleanup successful, False otherwise
        """
        pass
    
    @abstractmethod
    def read(self, address: int, size: int) -> List[Trit]:
        """
        Read data from device.
        
        Args:
            address: Device address
            size: Number of trits to read
            
        Returns:
            List of Trit objects
        """
        pass
    
    @abstractmethod
    def write(self, address: int, data: List[Trit]) -> bool:
        """
        Write data to device.
        
        Args:
            address: Device address
            data: List of Trit objects to write
            
        Returns:
            True if write successful, False otherwise
        """
        pass
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get device status.
        
        Returns:
            Device status dictionary
        """
        with self.lock:
            return {
                'device_id': self.device_id,
                'device_type': self.device_type,
                'state': self.state.value,
                'capabilities': [cap.value for cap in self.capabilities],
                **self.stats
            }
    
    def register_interrupt_handler(self, interrupt_type: str, handler: Callable) -> None:
        """
        Register interrupt handler.
        
        Args:
            interrupt_type: Type of interrupt
            handler: Handler function
        """
        self.interrupt_handlers[interrupt_type] = handler
    
    def handle_interrupt(self, interrupt_type: str, data: Any = None) -> bool:
        """
        Handle device interrupt.
        
        Args:
            interrupt_type: Type of interrupt
            data: Optional interrupt data
            
        Returns:
            True if interrupt handled successfully, False otherwise
        """
        if interrupt_type in self.interrupt_handlers:
            handler = self.interrupt_handlers[interrupt_type]
            try:
                result = handler(data)
                self.stats['interrupts'] += 1
                return result
            except Exception as e:
                print(f"Interrupt handler failed: {e}")
                return False
        return False
    
    def has_capability(self, capability: DriverCapability) -> bool:
        """Check if driver has specific capability."""
        return capability in self.capabilities
    
    def update_stats(self, operation: str, count: int = 1) -> None:
        """Update driver statistics."""
        with self.lock:
            if operation in self.stats:
                self.stats[operation] += count


class ConsoleDriver(TernaryDeviceDriver):
    """
    Ternary Console Driver - Handles console I/O operations.
    """
    
    def __init__(self, device_id: str = "console_0"):
        """Initialize console driver."""
        super().__init__(device_id, "console")
        self.capabilities = [
            DriverCapability.READ,
            DriverCapability.WRITE,
            DriverCapability.CONTROL
        ]
        self.buffer = []
        self.buffer_lock = threading.Lock()
    
    def initialize(self) -> bool:
        """Initialize console driver."""
        try:
            self.state = DriverState.INITIALIZED
            print(f"Console driver {self.device_id} initialized")
            return True
        except Exception as e:
            print(f"Failed to initialize console driver: {e}")
            return False
    
    def cleanup(self) -> bool:
        """Cleanup console driver."""
        try:
            self.state = DriverState.STOPPED
            print(f"Console driver {self.device_id} cleaned up")
            return True
        except Exception as e:
            print(f"Failed to cleanup console driver: {e}")
            return False
    
    def read(self, address: int, size: int) -> List[Trit]:
        """Read data from console."""
        with self.buffer_lock:
            if not self.buffer:
                return [Trit(0)] * size  # Return zeros if no data
            
            # Read available data
            data = self.buffer[:size]
            self.buffer = self.buffer[size:]
            
            self.update_stats('operations')
            return data
    
    def write(self, address: int, data: List[Trit]) -> bool:
        """Write data to console."""
        try:
            # Convert trits to characters and print
            chars = []
            for trit in data:
                if trit.value == 1:
                    chars.append('1')
                elif trit.value == -1:
                    chars.append('-')
                else:
                    chars.append('0')
            
            print(''.join(chars), end='')
            self.update_stats('operations')
            return True
            
        except Exception as e:
            print(f"Console write failed: {e}")
            return False
    
    def input_ternary(self, prompt: str = "") -> List[Trit]:
        """Get ternary input from user."""
        if prompt:
            print(prompt, end='')
        
        user_input = input()
        trits = []
        
        for char in user_input:
            if char == '1':
                trits.append(Trit(1))
            elif char == '-':
                trits.append(Trit(-1))
            else:
                trits.append(Trit(0))
        
        return trits


class StorageDriver(TernaryDeviceDriver):
    """
    Ternary Storage Driver - Handles storage I/O operations.
    """
    
    def __init__(self, device_id: str = "storage_0", capacity: int = 1024*1024):
        """Initialize storage driver."""
        super().__init__(device_id, "storage")
        self.capabilities = [
            DriverCapability.READ,
            DriverCapability.WRITE,
            DriverCapability.STATUS
        ]
        self.capacity = capacity
        self.storage = {}  # address -> data
        self.storage_lock = threading.Lock()
    
    def initialize(self) -> bool:
        """Initialize storage driver."""
        try:
            self.state = DriverState.INITIALIZED
            print(f"Storage driver {self.device_id} initialized (capacity: {self.capacity})")
            return True
        except Exception as e:
            print(f"Failed to initialize storage driver: {e}")
            return False
    
    def cleanup(self) -> bool:
        """Cleanup storage driver."""
        try:
            with self.storage_lock:
                self.storage.clear()
            self.state = DriverState.STOPPED
            print(f"Storage driver {self.device_id} cleaned up")
            return True
        except Exception as e:
            print(f"Failed to cleanup storage driver: {e}")
            return False
    
    def read(self, address: int, size: int) -> List[Trit]:
        """Read data from storage."""
        with self.storage_lock:
            if address in self.storage:
                data = self.storage[address]
                return data[:size] if len(data) >= size else data + [Trit(0)] * (size - len(data))
            else:
                return [Trit(0)] * size
    
    def write(self, address: int, data: List[Trit]) -> bool:
        """Write data to storage."""
        try:
            with self.storage_lock:
                self.storage[address] = data.copy()
                self.update_stats('operations')
                return True
        except Exception as e:
            print(f"Storage write failed: {e}")
            return False
    
    def get_capacity(self) -> int:
        """Get storage capacity."""
        return self.capacity
    
    def get_used_space(self) -> int:
        """Get used storage space."""
        with self.storage_lock:
            return sum(len(data) for data in self.storage.values())


class NetworkDriver(TernaryDeviceDriver):
    """
    Ternary Network Driver - Handles network I/O operations.
    """
    
    def __init__(self, device_id: str = "network_0"):
        """Initialize network driver."""
        super().__init__(device_id, "network")
        self.capabilities = [
            DriverCapability.READ,
            DriverCapability.WRITE,
            DriverCapability.CONTROL,
            DriverCapability.STATUS
        ]
        self.connections = {}
        self.connection_lock = threading.Lock()
    
    def initialize(self) -> bool:
        """Initialize network driver."""
        try:
            self.state = DriverState.INITIALIZED
            print(f"Network driver {self.device_id} initialized")
            return True
        except Exception as e:
            print(f"Failed to initialize network driver: {e}")
            return False
    
    def cleanup(self) -> bool:
        """Cleanup network driver."""
        try:
            with self.connection_lock:
                self.connections.clear()
            self.state = DriverState.STOPPED
            print(f"Network driver {self.device_id} cleaned up")
            return True
        except Exception as e:
            print(f"Failed to cleanup network driver: {e}")
            return False
    
    def read(self, address: int, size: int) -> List[Trit]:
        """Read data from network."""
        # Simulate network read
        return [Trit(0)] * size
    
    def write(self, address: int, data: List[Trit]) -> bool:
        """Write data to network."""
        # Simulate network write
        self.update_stats('operations')
        return True
    
    def connect(self, host: str, port: int) -> bool:
        """Connect to network host."""
        try:
            connection_id = f"{host}:{port}"
            with self.connection_lock:
                self.connections[connection_id] = {
                    'host': host,
                    'port': port,
                    'connected': True
                }
            print(f"Connected to {host}:{port}")
            return True
        except Exception as e:
            print(f"Failed to connect to {host}:{port}: {e}")
            return False
    
    def disconnect(self, host: str, port: int) -> bool:
        """Disconnect from network host."""
        try:
            connection_id = f"{host}:{port}"
            with self.connection_lock:
                if connection_id in self.connections:
                    del self.connections[connection_id]
            print(f"Disconnected from {host}:{port}")
            return True
        except Exception as e:
            print(f"Failed to disconnect from {host}:{port}: {e}")
            return False


class DriverManager:
    """
    Driver Manager - Manages ternary device drivers.
    
    Provides driver registration, loading, and management.
    """
    
    def __init__(self):
        """Initialize driver manager."""
        self.drivers = {}  # device_id -> driver_instance
        self.driver_types = {}  # driver_type -> List[device_id]
        
        # Threading
        self.lock = threading.Lock()
        
        # Statistics
        self.stats = {
            'drivers_loaded': 0,
            'drivers_unloaded': 0,
            'total_operations': 0,
            'total_errors': 0
        }
    
    def register_driver(self, driver: TernaryDeviceDriver) -> bool:
        """
        Register a device driver.
        
        Args:
            driver: Device driver instance
            
        Returns:
            True if registration successful, False otherwise
        """
        try:
            with self.lock:
                if driver.device_id in self.drivers:
                    print(f"Driver {driver.device_id} already registered")
                    return False
                
                # Initialize driver
                if not driver.initialize():
                    print(f"Failed to initialize driver {driver.device_id}")
                    return False
                
                # Register driver
                self.drivers[driver.device_id] = driver
                
                # Update driver types index
                if driver.device_type not in self.driver_types:
                    self.driver_types[driver.device_type] = []
                self.driver_types[driver.device_type].append(driver.device_id)
                
                self.stats['drivers_loaded'] += 1
                print(f"Driver {driver.device_id} registered successfully")
                return True
                
        except Exception as e:
            print(f"Failed to register driver {driver.device_id}: {e}")
            return False
    
    def unregister_driver(self, device_id: str) -> bool:
        """
        Unregister a device driver.
        
        Args:
            device_id: Device identifier
            
        Returns:
            True if unregistration successful, False otherwise
        """
        try:
            with self.lock:
                if device_id not in self.drivers:
                    print(f"Driver {device_id} not registered")
                    return False
                
                driver = self.drivers[device_id]
                
                # Cleanup driver
                if not driver.cleanup():
                    print(f"Failed to cleanup driver {device_id}")
                
                # Remove from driver types index
                if driver.device_type in self.driver_types:
                    self.driver_types[driver.device_type].remove(device_id)
                    if not self.driver_types[driver.device_type]:
                        del self.driver_types[driver.device_type]
                
                # Remove driver
                del self.drivers[device_id]
                
                self.stats['drivers_unloaded'] += 1
                print(f"Driver {device_id} unregistered successfully")
                return True
                
        except Exception as e:
            print(f"Failed to unregister driver {device_id}: {e}")
            return False
    
    def get_driver(self, device_id: str) -> Optional[TernaryDeviceDriver]:
        """Get driver by device ID."""
        with self.lock:
            return self.drivers.get(device_id)
    
    def get_drivers_by_type(self, driver_type: str) -> List[TernaryDeviceDriver]:
        """Get drivers by type."""
        with self.lock:
            if driver_type not in self.driver_types:
                return []
            
            device_ids = self.driver_types[driver_type]
            return [self.drivers[device_id] for device_id in device_ids 
                   if device_id in self.drivers]
    
    def get_all_drivers(self) -> List[TernaryDeviceDriver]:
        """Get all registered drivers."""
        with self.lock:
            return list(self.drivers.values())
    
    def get_stats(self) -> dict:
        """Get driver manager statistics."""
        with self.lock:
            return {
                'total_drivers': len(self.drivers),
                'driver_types': len(self.driver_types),
                **self.stats
            }
    
    def cleanup(self) -> None:
        """Cleanup driver manager."""
        with self.lock:
            # Unregister all drivers
            for device_id in list(self.drivers.keys()):
                self.unregister_driver(device_id)
            
            print("Driver manager cleaned up")
    
    def __del__(self):
        """Destructor."""
        self.cleanup()
