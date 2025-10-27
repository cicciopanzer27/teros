"""
Ternary System Call Handlers implementation.

This module provides system call handler implementations for TEROS,
including process, memory, file, network, and system operations.
"""

from typing import Dict, List, Optional, Any, Union
import time
from ..core.trit import Trit
from ..core.tritarray import TritArray
from ..process.scheduler import TernaryScheduler
from ..memory.memory_manager import TernaryMemoryManager
from ..fs.tfs import TernaryFileSystem
from ..io.io_manager import TernaryIOManager
from ..security.security_manager import TernarySecurityManager
from .syscall_manager import SyscallType


class TernarySyscallHandlers:
    """
    Ternary System Call Handlers - System call handler implementations.
    
    Provides handler implementations for various system calls
    in the TEROS system.
    """
    
    def __init__(self, scheduler: TernaryScheduler = None,
                 memory_manager: TernaryMemoryManager = None,
                 file_system: TernaryFileSystem = None,
                 io_manager: TernaryIOManager = None,
                 security_manager: TernarySecurityManager = None):
        """
        Initialize syscall handlers.
        
        Args:
            scheduler: Process scheduler
            memory_manager: Memory manager
            file_system: File system
            io_manager: I/O manager
            security_manager: Security manager
        """
        self.scheduler = scheduler
        self.memory_manager = memory_manager
        self.file_system = file_system
        self.io_manager = io_manager
        self.security_manager = security_manager
        
        # Handler statistics
        self.stats = {
            'handlers_registered': 0,
            'handlers_executed': 0,
            'handlers_failed': 0,
            'total_execution_time': 0.0,
            'average_execution_time': 0.0
        }
        
        # Ternary-specific handler features
        self.ternary_features = {
            'ternary_handlers': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_optimization': True
        }
    
    def register_handlers(self, syscall_manager) -> None:
        """
        Register all syscall handlers.
        
        Args:
            syscall_manager: System call manager
        """
        # Process management handlers
        syscall_manager.register_syscall('create_process', SyscallType.PROCESS, self._handle_create_process)
        syscall_manager.register_syscall('terminate_process', SyscallType.PROCESS, self._handle_terminate_process)
        syscall_manager.register_syscall('get_process_info', SyscallType.PROCESS, self._handle_get_process_info)
        syscall_manager.register_syscall('list_processes', SyscallType.PROCESS, self._handle_list_processes)
        
        # Memory management handlers
        syscall_manager.register_syscall('allocate_memory', SyscallType.MEMORY, self._handle_allocate_memory)
        syscall_manager.register_syscall('deallocate_memory', SyscallType.MEMORY, self._handle_deallocate_memory)
        syscall_manager.register_syscall('get_memory_info', SyscallType.MEMORY, self._handle_get_memory_info)
        
        # File system handlers
        syscall_manager.register_syscall('open_file', SyscallType.FILE, self._handle_open_file)
        syscall_manager.register_syscall('close_file', SyscallType.FILE, self._handle_close_file)
        syscall_manager.register_syscall('read_file', SyscallType.FILE, self._handle_read_file)
        syscall_manager.register_syscall('write_file', SyscallType.FILE, self._handle_write_file)
        syscall_manager.register_syscall('create_file', SyscallType.FILE, self._handle_create_file)
        syscall_manager.register_syscall('delete_file', SyscallType.FILE, self._handle_delete_file)
        syscall_manager.register_syscall('list_directory', SyscallType.FILE, self._handle_list_directory)
        
        # Network handlers
        syscall_manager.register_syscall('create_socket', SyscallType.NETWORK, self._handle_create_socket)
        syscall_manager.register_syscall('connect_socket', SyscallType.NETWORK, self._handle_connect_socket)
        syscall_manager.register_syscall('send_data', SyscallType.NETWORK, self._handle_send_data)
        syscall_manager.register_syscall('receive_data', SyscallType.NETWORK, self._handle_receive_data)
        
        # System handlers
        syscall_manager.register_syscall('get_system_info', SyscallType.SYSTEM, self._handle_get_system_info)
        syscall_manager.register_syscall('get_time', SyscallType.SYSTEM, self._handle_get_time)
        syscall_manager.register_syscall('sleep', SyscallType.SYSTEM, self._handle_sleep)
        
        self.stats['handlers_registered'] += 1
    
    # Process management handlers
    
    def _handle_create_process(self, parameters: Dict[str, Any]) -> int:
        """Handle create_process syscall."""
        if not self.scheduler:
            raise RuntimeError("Process scheduler not available")
        
        program_path = parameters.get('program_path', '')
        arguments = parameters.get('arguments', [])
        
        # Create process
        pid = self.scheduler.create_process(program_path, priority=1)
        
        self.stats['handlers_executed'] += 1
        return pid
    
    def _handle_terminate_process(self, parameters: Dict[str, Any]) -> bool:
        """Handle terminate_process syscall."""
        if not self.scheduler:
            raise RuntimeError("Process scheduler not available")
        
        process_id = parameters.get('process_id')
        if process_id is None:
            raise ValueError("process_id parameter required")
        
        success = self.scheduler.terminate_process(process_id)
        
        self.stats['handlers_executed'] += 1
        return success
    
    def _handle_get_process_info(self, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Handle get_process_info syscall."""
        if not self.scheduler:
            raise RuntimeError("Process scheduler not available")
        
        process_id = parameters.get('process_id')
        if process_id is None:
            # Get current process info
            process_id = self.scheduler.get_running_process()
        
        if process_id is None:
            raise ValueError("No process ID provided")
        
        info = self.scheduler.get_process_info(process_id)
        if not info:
            raise ValueError(f"Process {process_id} not found")
        
        self.stats['handlers_executed'] += 1
        return info
    
    def _handle_list_processes(self, parameters: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Handle list_processes syscall."""
        if not self.scheduler:
            raise RuntimeError("Process scheduler not available")
        
        processes = self.scheduler.get_all_processes()
        
        self.stats['handlers_executed'] += 1
        return processes
    
    # Memory management handlers
    
    def _handle_allocate_memory(self, parameters: Dict[str, Any]) -> int:
        """Handle allocate_memory syscall."""
        if not self.memory_manager:
            raise RuntimeError("Memory manager not available")
        
        size = parameters.get('size', 0)
        if size <= 0:
            raise ValueError("Invalid memory size")
        
        # Allocate memory
        address = self.memory_manager.allocate_memory(size)
        if address is None:
            raise RuntimeError("Failed to allocate memory")
        
        self.stats['handlers_executed'] += 1
        return address
    
    def _handle_deallocate_memory(self, parameters: Dict[str, Any]) -> bool:
        """Handle deallocate_memory syscall."""
        if not self.memory_manager:
            raise RuntimeError("Memory manager not available")
        
        address = parameters.get('address')
        if address is None:
            raise ValueError("address parameter required")
        
        success = self.memory_manager.deallocate_memory(address)
        
        self.stats['handlers_executed'] += 1
        return success
    
    def _handle_get_memory_info(self, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Handle get_memory_info syscall."""
        if not self.memory_manager:
            raise RuntimeError("Memory manager not available")
        
        info = self.memory_manager.get_memory_info()
        
        self.stats['handlers_executed'] += 1
        return info
    
    # File system handlers
    
    def _handle_open_file(self, parameters: Dict[str, Any]) -> int:
        """Handle open_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_path = parameters.get('file_path', '')
        mode = parameters.get('mode', 'r')
        
        file_id = self.file_system.open_file(file_path, mode)
        if file_id is None:
            raise RuntimeError(f"Failed to open file: {file_path}")
        
        self.stats['handlers_executed'] += 1
        return file_id
    
    def _handle_close_file(self, parameters: Dict[str, Any]) -> bool:
        """Handle close_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_descriptor = parameters.get('file_descriptor')
        if file_descriptor is None:
            raise ValueError("file_descriptor parameter required")
        
        success = self.file_system.close_file(file_descriptor)
        
        self.stats['handlers_executed'] += 1
        return success
    
    def _handle_read_file(self, parameters: Dict[str, Any]) -> bytes:
        """Handle read_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_descriptor = parameters.get('file_descriptor')
        size = parameters.get('size', -1)
        
        if file_descriptor is None:
            raise ValueError("file_descriptor parameter required")
        
        data = self.file_system.read_file(file_descriptor, size)
        
        self.stats['handlers_executed'] += 1
        return data
    
    def _handle_write_file(self, parameters: Dict[str, Any]) -> int:
        """Handle write_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_descriptor = parameters.get('file_descriptor')
        data = parameters.get('data', b'')
        
        if file_descriptor is None:
            raise ValueError("file_descriptor parameter required")
        
        bytes_written = self.file_system.write_file(file_descriptor, data)
        
        self.stats['handlers_executed'] += 1
        return bytes_written
    
    def _handle_create_file(self, parameters: Dict[str, Any]) -> bool:
        """Handle create_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_path = parameters.get('file_path', '')
        permissions = parameters.get('permissions', 0o644)
        
        success = self.file_system.create_file(file_path, permissions=permissions)
        
        self.stats['handlers_executed'] += 1
        return success
    
    def _handle_delete_file(self, parameters: Dict[str, Any]) -> bool:
        """Handle delete_file syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        file_path = parameters.get('file_path', '')
        
        success = self.file_system.delete_file(file_path)
        
        self.stats['handlers_executed'] += 1
        return success
    
    def _handle_list_directory(self, parameters: Dict[str, Any]) -> List[str]:
        """Handle list_directory syscall."""
        if not self.file_system:
            raise RuntimeError("File system not available")
        
        directory_path = parameters.get('directory_path', '/')
        
        entries = self.file_system.list_directory(directory_path)
        
        self.stats['handlers_executed'] += 1
        return entries
    
    # Network handlers
    
    def _handle_create_socket(self, parameters: Dict[str, Any]) -> int:
        """Handle create_socket syscall."""
        if not self.io_manager:
            raise RuntimeError("I/O manager not available")
        
        protocol = parameters.get('protocol', 'tcp')
        
        # Create socket through I/O manager
        socket_id = self.io_manager.submit_request('create_socket', 'network', {'protocol': protocol})
        
        self.stats['handlers_executed'] += 1
        return socket_id
    
    def _handle_connect_socket(self, parameters: Dict[str, Any]) -> bool:
        """Handle connect_socket syscall."""
        if not self.io_manager:
            raise RuntimeError("I/O manager not available")
        
        socket_descriptor = parameters.get('socket_descriptor')
        address = parameters.get('address', '')
        port = parameters.get('port', 0)
        
        if socket_descriptor is None:
            raise ValueError("socket_descriptor parameter required")
        
        # Connect socket through I/O manager
        result = self.io_manager.submit_request('connect_socket', 'network', {
            'socket_descriptor': socket_descriptor,
            'address': address,
            'port': port
        })
        
        self.stats['handlers_executed'] += 1
        return result
    
    def _handle_send_data(self, parameters: Dict[str, Any]) -> int:
        """Handle send_data syscall."""
        if not self.io_manager:
            raise RuntimeError("I/O manager not available")
        
        socket_descriptor = parameters.get('socket_descriptor')
        data = parameters.get('data', b'')
        
        if socket_descriptor is None:
            raise ValueError("socket_descriptor parameter required")
        
        # Send data through I/O manager
        result = self.io_manager.submit_request('send_data', 'network', {
            'socket_descriptor': socket_descriptor,
            'data': data
        })
        
        self.stats['handlers_executed'] += 1
        return result
    
    def _handle_receive_data(self, parameters: Dict[str, Any]) -> bytes:
        """Handle receive_data syscall."""
        if not self.io_manager:
            raise RuntimeError("I/O manager not available")
        
        socket_descriptor = parameters.get('socket_descriptor')
        size = parameters.get('size', 4096)
        
        if socket_descriptor is None:
            raise ValueError("socket_descriptor parameter required")
        
        # Receive data through I/O manager
        result = self.io_manager.submit_request('receive_data', 'network', {
            'socket_descriptor': socket_descriptor,
            'size': size
        })
        
        self.stats['handlers_executed'] += 1
        return result
    
    # System handlers
    
    def _handle_get_system_info(self, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Handle get_system_info syscall."""
        info = {
            'system_name': 'TEROS',
            'version': '0.1.0',
            'architecture': 'ternary',
            'uptime': time.time(),
            'memory_total': 0,
            'memory_free': 0,
            'processes': 0,
            'load_average': [0.0, 0.0, 0.0]
        }
        
        # Add component-specific info
        if self.scheduler:
            info['processes'] = len(self.scheduler.get_all_processes())
        
        if self.memory_manager:
            memory_info = self.memory_manager.get_memory_info()
            info['memory_total'] = memory_info.get('total_memory', 0)
            info['memory_free'] = memory_info.get('free_memory', 0)
        
        self.stats['handlers_executed'] += 1
        return info
    
    def _handle_get_time(self, parameters: Dict[str, Any]) -> float:
        """Handle get_time syscall."""
        self.stats['handlers_executed'] += 1
        return time.time()
    
    def _handle_sleep(self, parameters: Dict[str, Any]) -> None:
        """Handle sleep syscall."""
        duration = parameters.get('duration', 0.0)
        if duration > 0:
            time.sleep(duration)
        
        self.stats['handlers_executed'] += 1
    
    def get_handler_stats(self) -> Dict[str, Any]:
        """Get handler statistics."""
        return self.stats.copy()
    
    def set_ternary_feature(self, feature: str, enabled: bool) -> None:
        """
        Set ternary feature.
        
        Args:
            feature: Feature name
            enabled: Whether feature is enabled
        """
        self.ternary_features[feature] = enabled
    
    def get_ternary_features(self) -> Dict[str, bool]:
        """Get ternary features."""
        return self.ternary_features.copy()
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernarySyscallHandlers(registered={self.stats['handlers_registered']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernarySyscallHandlers(registered={self.stats['handlers_registered']}, "
                f"executed={self.stats['handlers_executed']})")
