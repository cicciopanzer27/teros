"""
HAL Device Manager for Hardware Abstraction Layer.

This module provides device management and driver integration for ternary hardware.
"""

from typing import Dict, List, Optional, Any, Callable
import threading
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class DeviceType(Enum):
    """Device types."""
    CPU = "cpu"
    MEMORY = "memory"
    STORAGE = "storage"
    NETWORK = "network"
    INPUT = "input"
    OUTPUT = "output"
    AUDIO = "audio"
    VIDEO = "video"
    USB = "usb"
    PCI = "pci"


class DeviceStatus(Enum):
    """Device status."""
    ONLINE = "online"
    OFFLINE = "offline"
    ERROR = "error"
    BUSY = "busy"
    UNKNOWN = "unknown"


class HALDevice:
    """
    HAL Device - Represents a hardware device in the HAL.
    
    Provides device abstraction with ternary-specific features.
    """
    
    def __init__(self, device_id: str, device_type: DeviceType,
                 driver: Any = None, capabilities: List[str] = None):
        """
        Initialize HAL device.
        
        Args:
            device_id: Unique device identifier
            device_type: Type of device
            driver: Device driver instance
            capabilities: List of device capabilities
        """
        self.device_id = device_id
        self.device_type = device_type
        self.driver = driver
        self.capabilities = capabilities or []
        
        # Device state
        self.status = DeviceStatus.UNKNOWN
        self.last_activity = time.time()
        self.error_count = 0
        self.performance_stats = {
            'operations': 0,
            'errors': 0,
            'latency_avg': 0.0,
            'throughput': 0.0
        }
        
        # Ternary-specific features
        self.ternary_features = {
            'ternary_support': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_checksum': True
        }
        
        # Threading
        self.lock = threading.Lock()
    
    def update_activity(self) -> None:
        """Update last activity timestamp."""
        with self.lock:
            self.last_activity = time.time()
    
    def set_status(self, status: DeviceStatus) -> None:
        """Set device status."""
        with self.lock:
            self.status = status
            self.update_activity()
    
    def increment_error_count(self) -> None:
        """Increment error count."""
        with self.lock:
            self.error_count += 1
            self.performance_stats['errors'] += 1
            self.update_activity()
    
    def reset_error_count(self) -> None:
        """Reset error count."""
        with self.lock:
            self.error_count = 0
            self.performance_stats['errors'] = 0
            self.update_activity()
    
    def has_capability(self, capability: str) -> bool:
        """Check if device has specific capability."""
        return capability in self.capabilities
    
    def get_info(self) -> dict:
        """Get device information."""
        with self.lock:
            return {
                'device_id': self.device_id,
                'device_type': self.device_type.value,
                'status': self.status.value,
                'capabilities': self.capabilities.copy(),
                'ternary_features': self.ternary_features.copy(),
                'performance_stats': self.performance_stats.copy(),
                'last_activity': self.last_activity,
                'error_count': self.error_count
            }


class HALDeviceManager:
    """
    HAL Device Manager - Manages hardware devices in the HAL.
    
    Provides device discovery, registration, and management for ternary hardware.
    """
    
    def __init__(self):
        """Initialize HAL device manager."""
        self.devices = {}  # device_id -> HALDevice
        self.device_types = {}  # device_type -> List[device_id]
        self.drivers = {}  # driver_name -> driver_instance
        
        # Device discovery
        self.discovery_callbacks = []
        self.auto_discovery = True
        
        # Threading
        self.lock = threading.Lock()
        self.discovery_thread = None
        self.running = False
        
        # Statistics
        self.stats = {
            'devices_registered': 0,
            'devices_discovered': 0,
            'devices_removed': 0,
            'driver_loads': 0,
            'driver_unloads': 0
        }
    
    def register_device(self, device_id: str, device_type: DeviceType,
                       driver: Any = None, capabilities: List[str] = None) -> bool:
        """
        Register a device with the HAL.
        
        Args:
            device_id: Unique device identifier
            device_type: Type of device
            driver: Device driver instance
            capabilities: List of device capabilities
            
        Returns:
            True if registration successful, False otherwise
        """
        try:
            with self.lock:
                if device_id in self.devices:
                    print(f"Device {device_id} already registered")
                    return False
                
                # Create device
                device = HALDevice(device_id, device_type, driver, capabilities)
                self.devices[device_id] = device
                
                # Update device types index
                if device_type not in self.device_types:
                    self.device_types[device_type] = []
                self.device_types[device_type].append(device_id)
                
                # Load driver if provided
                if driver:
                    self._load_driver(device_id, driver)
                
                self.stats['devices_registered'] += 1
                print(f"Device {device_id} registered successfully")
                return True
                
        except Exception as e:
            print(f"Failed to register device {device_id}: {e}")
            return False
    
    def unregister_device(self, device_id: str) -> bool:
        """
        Unregister a device from the HAL.
        
        Args:
            device_id: Device identifier to unregister
            
        Returns:
            True if unregistration successful, False otherwise
        """
        try:
            with self.lock:
                if device_id not in self.devices:
                    print(f"Device {device_id} not registered")
                    return False
                
                device = self.devices[device_id]
                
                # Unload driver if loaded
                if device.driver:
                    self._unload_driver(device_id)
                
                # Remove from device types index
                device_type = device.device_type
                if device_type in self.device_types:
                    self.device_types[device_type].remove(device_id)
                    if not self.device_types[device_type]:
                        del self.device_types[device_type]
                
                # Remove device
                del self.devices[device_id]
                
                self.stats['devices_removed'] += 1
                print(f"Device {device_id} unregistered successfully")
                return True
                
        except Exception as e:
            print(f"Failed to unregister device {device_id}: {e}")
            return False
    
    def get_device(self, device_id: str) -> Optional[HALDevice]:
        """Get device by ID."""
        with self.lock:
            return self.devices.get(device_id)
    
    def get_devices_by_type(self, device_type: DeviceType) -> List[HALDevice]:
        """Get devices by type."""
        with self.lock:
            if device_type not in self.device_types:
                return []
            
            device_ids = self.device_types[device_type]
            return [self.devices[device_id] for device_id in device_ids 
                   if device_id in self.devices]
    
    def get_all_devices(self) -> List[HALDevice]:
        """Get all registered devices."""
        with self.lock:
            return list(self.devices.values())
    
    def _load_driver(self, device_id: str, driver: Any) -> bool:
        """Load device driver."""
        try:
            # Initialize driver
            if hasattr(driver, 'initialize'):
                driver.initialize()
            
            # Store driver reference
            self.drivers[device_id] = driver
            
            self.stats['driver_loads'] += 1
            print(f"Driver loaded for device {device_id}")
            return True
            
        except Exception as e:
            print(f"Failed to load driver for device {device_id}: {e}")
            return False
    
    def _unload_driver(self, device_id: str) -> bool:
        """Unload device driver."""
        try:
            if device_id in self.drivers:
                driver = self.drivers[device_id]
                
                # Cleanup driver
                if hasattr(driver, 'cleanup'):
                    driver.cleanup()
                
                del self.drivers[device_id]
                
                self.stats['driver_unloads'] += 1
                print(f"Driver unloaded for device {device_id}")
                return True
            
            return False
            
        except Exception as e:
            print(f"Failed to unload driver for device {device_id}: {e}")
            return False
    
    def start_auto_discovery(self) -> None:
        """Start automatic device discovery."""
        if self.discovery_thread and self.discovery_thread.is_alive():
            return
        
        self.running = True
        self.discovery_thread = threading.Thread(target=self._discovery_loop)
        self.discovery_thread.daemon = True
        self.discovery_thread.start()
        
        print("Auto discovery started")
    
    def stop_auto_discovery(self) -> None:
        """Stop automatic device discovery."""
        self.running = False
        if self.discovery_thread:
            self.discovery_thread.join(timeout=1.0)
        
        print("Auto discovery stopped")
    
    def _discovery_loop(self) -> None:
        """Device discovery loop."""
        while self.running:
            try:
                # Perform device discovery
                self._discover_devices()
                
                # Sleep between discovery cycles
                time.sleep(1.0)
                
            except Exception as e:
                print(f"Error in discovery loop: {e}")
                time.sleep(5.0)
    
    def _discover_devices(self) -> None:
        """Discover new devices."""
        # This is a placeholder - in real implementation,
        # this would scan for actual hardware devices
        
        # Example: Discover CPU device
        if 'cpu_0' not in self.devices:
            self.register_device('cpu_0', DeviceType.CPU, 
                               capabilities=['ternary_arithmetic', 'ternary_logic'])
        
        # Example: Discover memory device
        if 'memory_0' not in self.devices:
            self.register_device('memory_0', DeviceType.MEMORY,
                               capabilities=['ternary_storage', 'ternary_retrieval'])
        
        # Example: Discover storage device
        if 'storage_0' not in self.devices:
            self.register_device('storage_0', DeviceType.STORAGE,
                               capabilities=['ternary_io', 'ternary_persistence'])
    
    def register_discovery_callback(self, callback: Callable) -> None:
        """Register device discovery callback."""
        self.discovery_callbacks.append(callback)
    
    def get_stats(self) -> dict:
        """Get device manager statistics."""
        with self.lock:
            return {
                'total_devices': len(self.devices),
                'device_types': len(self.device_types),
                'loaded_drivers': len(self.drivers),
                'auto_discovery': self.running,
                **self.stats
            }
    
    def cleanup(self) -> None:
        """Cleanup device manager."""
        # Stop auto discovery
        self.stop_auto_discovery()
        
        # Unload all drivers
        with self.lock:
            for device_id in list(self.drivers.keys()):
                self._unload_driver(device_id)
            
            # Clear devices
            self.devices.clear()
            self.device_types.clear()
        
        print("HAL device manager cleaned up")
    
    def __del__(self):
        """Destructor."""
        self.cleanup()
