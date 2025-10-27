"""
Ternary Device Manager implementation.

This module provides device management for TEROS,
including device registration, discovery, and coordination.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import threading
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class DeviceType(Enum):
    """Device types."""
    CONSOLE = "console"
    STORAGE = "storage"
    NETWORK = "network"
    AUDIO = "audio"
    VIDEO = "video"
    INPUT = "input"
    OUTPUT = "output"
    UNKNOWN = "unknown"


class DeviceStatus(Enum):
    """Device status."""
    ONLINE = "online"
    OFFLINE = "offline"
    ERROR = "error"
    BUSY = "busy"
    UNKNOWN = "unknown"


class TernaryDevice:
    """
    Ternary Device - Represents a device in the system.
    
    Contains device information, capabilities, and status.
    """
    
    def __init__(self, device_id: str, device_type: DeviceType, 
                 driver: Any, capabilities: List[str] = None):
        """
        Initialize device.
        
        Args:
            device_id: Device identifier
            device_type: Type of device
            driver: Device driver instance
            capabilities: List of device capabilities
        """
        self.device_id = device_id
        self.device_type = device_type
        self.driver = driver
        self.capabilities = capabilities or []
        
        # Device status
        self.status = DeviceStatus.ONLINE
        self.last_activity = time.time()
        self.error_count = 0
        
        # Device metadata
        self.metadata = {
            'manufacturer': 'Unknown',
            'model': 'Unknown',
            'version': '1.0',
            'serial_number': 'Unknown',
            'firmware_version': '1.0'
        }
        
        # Ternary-specific device features
        self.ternary_features = {
            'ternary_support': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False
        }
    
    def update_activity(self) -> None:
        """Update last activity time."""
        self.last_activity = time.time()
    
    def set_status(self, status: DeviceStatus) -> None:
        """Set device status."""
        self.status = status
        self.update_activity()
    
    def increment_error_count(self) -> None:
        """Increment error count."""
        self.error_count += 1
        self.update_activity()
    
    def reset_error_count(self) -> None:
        """Reset error count."""
        self.error_count = 0
        self.update_activity()
    
    def has_capability(self, capability: str) -> bool:
        """Check if device has capability."""
        return capability in self.capabilities
    
    def add_capability(self, capability: str) -> None:
        """Add capability to device."""
        if capability not in self.capabilities:
            self.capabilities.append(capability)
    
    def remove_capability(self, capability: str) -> None:
        """Remove capability from device."""
        if capability in self.capabilities:
            self.capabilities.remove(capability)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert device to dictionary."""
        return {
            'device_id': self.device_id,
            'device_type': self.device_type.value,
            'status': self.status.value,
            'capabilities': self.capabilities.copy(),
            'error_count': self.error_count,
            'last_activity': self.last_activity,
            'metadata': self.metadata.copy(),
            'ternary_features': self.ternary_features.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryDevice(id={self.device_id}, type={self.device_type.value}, status={self.status.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryDevice(id={self.device_id}, type={self.device_type.value}, "
                f"status={self.status.value}, capabilities={len(self.capabilities)})")


class TernaryDeviceManager:
    """
    Ternary Device Manager - Device management system.
    
    Manages device registration, discovery, and coordination
    for the TEROS system.
    """
    
    def __init__(self):
        """Initialize device manager."""
        self.devices = {}  # device_id -> TernaryDevice
        self.devices_by_type = {}  # device_type -> List[device_id]
        
        # Device management statistics
        self.stats = {
            'total_devices': 0,
            'online_devices': 0,
            'offline_devices': 0,
            'error_devices': 0,
            'device_discoveries': 0,
            'device_registrations': 0,
            'device_unregistrations': 0
        }
        
        # Threading
        self.lock = threading.Lock()
        self.monitor_thread = None
        self.running = False
        
        # Device discovery
        self.discovery_callbacks = []
        self.device_callbacks = {}
    
    def register_device(self, device_id: str, device_type: DeviceType, 
                       driver: Any, capabilities: List[str] = None) -> bool:
        """
        Register a device.
        
        Args:
            device_id: Device identifier
            device_type: Type of device
            driver: Device driver instance
            capabilities: List of device capabilities
            
        Returns:
            True if registered successfully, False otherwise
        """
        with self.lock:
            if device_id in self.devices:
                return False
            
            # Create device
            device = TernaryDevice(device_id, device_type, driver, capabilities)
            self.devices[device_id] = device
            
            # Add to type index
            if device_type not in self.devices_by_type:
                self.devices_by_type[device_type] = []
            self.devices_by_type[device_type].append(device_id)
            
            # Update statistics
            self.stats['total_devices'] += 1
            self.stats['online_devices'] += 1
            self.stats['device_registrations'] += 1
            
            # Call device callbacks
            if device_id in self.device_callbacks:
                for callback in self.device_callbacks[device_id]:
                    callback(device, 'registered')
            
            return True
    
    def unregister_device(self, device_id: str) -> bool:
        """
        Unregister a device.
        
        Args:
            device_id: Device identifier
            
        Returns:
            True if unregistered successfully, False otherwise
        """
        with self.lock:
            if device_id not in self.devices:
                return False
            
            device = self.devices[device_id]
            device_type = device.device_type
            
            # Remove from type index
            if device_type in self.devices_by_type:
                if device_id in self.devices_by_type[device_type]:
                    self.devices_by_type[device_type].remove(device_id)
            
            # Update statistics
            self.stats['total_devices'] -= 1
            if device.status == DeviceStatus.ONLINE:
                self.stats['online_devices'] -= 1
            elif device.status == DeviceStatus.OFFLINE:
                self.stats['offline_devices'] -= 1
            elif device.status == DeviceStatus.ERROR:
                self.stats['error_devices'] -= 1
            
            self.stats['device_unregistrations'] += 1
            
            # Call device callbacks
            if device_id in self.device_callbacks:
                for callback in self.device_callbacks[device_id]:
                    callback(device, 'unregistered')
            
            del self.devices[device_id]
            return True
    
    def get_device(self, device_id: str) -> Optional[TernaryDevice]:
        """
        Get device by ID.
        
        Args:
            device_id: Device identifier
            
        Returns:
            Device or None if not found
        """
        with self.lock:
            return self.devices.get(device_id)
    
    def get_devices_by_type(self, device_type: DeviceType) -> List[TernaryDevice]:
        """
        Get devices by type.
        
        Args:
            device_type: Device type
            
        Returns:
            List of devices of specified type
        """
        with self.lock:
            device_ids = self.devices_by_type.get(device_type, [])
            return [self.devices[device_id] for device_id in device_ids if device_id in self.devices]
    
    def get_all_devices(self) -> List[TernaryDevice]:
        """Get all devices."""
        with self.lock:
            return list(self.devices.values())
    
    def get_online_devices(self) -> List[TernaryDevice]:
        """Get online devices."""
        with self.lock:
            return [device for device in self.devices.values() if device.status == DeviceStatus.ONLINE]
    
    def get_offline_devices(self) -> List[TernaryDevice]:
        """Get offline devices."""
        with self.lock:
            return [device for device in self.devices.values() if device.status == DeviceStatus.OFFLINE]
    
    def get_error_devices(self) -> List[TernaryDevice]:
        """Get devices with errors."""
        with self.lock:
            return [device for device in self.devices.values() if device.status == DeviceStatus.ERROR]
    
    def set_device_status(self, device_id: str, status: DeviceStatus) -> bool:
        """
        Set device status.
        
        Args:
            device_id: Device identifier
            status: New status
            
        Returns:
            True if status set successfully, False otherwise
        """
        with self.lock:
            if device_id not in self.devices:
                return False
            
            device = self.devices[device_id]
            old_status = device.status
            
            device.set_status(status)
            
            # Update statistics
            if old_status == DeviceStatus.ONLINE and status != DeviceStatus.ONLINE:
                self.stats['online_devices'] -= 1
            elif old_status != DeviceStatus.ONLINE and status == DeviceStatus.ONLINE:
                self.stats['online_devices'] += 1
            
            if old_status == DeviceStatus.OFFLINE and status != DeviceStatus.OFFLINE:
                self.stats['offline_devices'] -= 1
            elif old_status != DeviceStatus.OFFLINE and status == DeviceStatus.OFFLINE:
                self.stats['offline_devices'] += 1
            
            if old_status == DeviceStatus.ERROR and status != DeviceStatus.ERROR:
                self.stats['error_devices'] -= 1
            elif old_status != DeviceStatus.ERROR and status == DeviceStatus.ERROR:
                self.stats['error_devices'] += 1
            
            return True
    
    def add_discovery_callback(self, callback: Callable[[TernaryDevice], None]) -> None:
        """
        Add device discovery callback.
        
        Args:
            callback: Function to call when device is discovered
        """
        with self.lock:
            self.discovery_callbacks.append(callback)
    
    def add_device_callback(self, device_id: str, callback: Callable[[TernaryDevice, str], None]) -> None:
        """
        Add device-specific callback.
        
        Args:
            device_id: Device identifier
            callback: Function to call on device events
        """
        with self.lock:
            if device_id not in self.device_callbacks:
                self.device_callbacks[device_id] = []
            self.device_callbacks[device_id].append(callback)
    
    def start_monitoring(self) -> None:
        """Start device monitoring."""
        if self.monitor_thread and self.monitor_thread.is_alive():
            return
        
        self.running = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
    
    def stop_monitoring(self) -> None:
        """Stop device monitoring."""
        with self.lock:
            self.running = False
        
        if self.monitor_thread:
            self.monitor_thread.join()
    
    def _monitor_loop(self) -> None:
        """Device monitoring loop."""
        while self.running:
            try:
                with self.lock:
                    # Check device health
                    for device in self.devices.values():
                        # Check if device is responsive
                        if device.status == DeviceStatus.ONLINE:
                            # In a real implementation, this would check device health
                            pass
                        
                        # Check for devices that haven't been active
                        if time.time() - device.last_activity > 300:  # 5 minutes
                            if device.status == DeviceStatus.ONLINE:
                                device.set_status(DeviceStatus.OFFLINE)
                
                time.sleep(10)  # Check every 10 seconds
                
            except Exception as e:
                print(f"Device monitoring error: {e}")
                break
    
    def get_device_stats(self) -> Dict[str, Any]:
        """Get device statistics."""
        with self.lock:
            return self.stats.copy()
    
    def get_device_info(self, device_id: str) -> Optional[Dict[str, Any]]:
        """
        Get device information.
        
        Args:
            device_id: Device identifier
            
        Returns:
            Device information or None if not found
        """
        device = self.get_device(device_id)
        if device:
            return device.to_dict()
        return None
    
    def discover_devices(self) -> List[TernaryDevice]:
        """
        Discover new devices.
        
        Returns:
            List of discovered devices
        """
        # In a real implementation, this would scan for new devices
        # For now, return empty list
        discovered = []
        
        with self.lock:
            self.stats['device_discoveries'] += 1
            
            # Call discovery callbacks
            for device in discovered:
                for callback in self.discovery_callbacks:
                    callback(device)
        
        return discovered
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryDeviceManager(devices={len(self.devices)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryDeviceManager(devices={len(self.devices)}, "
                f"online={self.stats['online_devices']})")
