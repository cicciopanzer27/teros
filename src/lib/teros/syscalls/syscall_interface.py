"""
Ternary System Call Interface implementation.

This module provides the user-space interface for T-SYSCALLs,
including syscall invocation and result handling.
"""

from typing import Dict, List, Optional, Any, Union
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray
from .syscall_manager import TernarySyscallManager, SyscallType


class TernarySyscallInterface:
    """
    Ternary System Call Interface - User-space syscall interface.
    
    Provides a high-level interface for invoking system calls
    from user space in the TEROS system.
    """
    
    def __init__(self, syscall_manager: TernarySyscallManager):
        """
        Initialize syscall interface.
        
        Args:
            syscall_manager: System call manager instance
        """
        self.syscall_manager = syscall_manager
        self.current_process_id = None
        self.current_user_id = None
        
        # Interface statistics
        self.stats = {
            'syscalls_invoked': 0,
            'syscalls_completed': 0,
            'syscalls_failed': 0,
            'total_execution_time': 0.0,
            'average_execution_time': 0.0
        }
        
        # Ternary-specific interface features
        self.ternary_features = {
            'ternary_interface': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_optimization': True
        }
    
    def set_context(self, process_id: str, user_id: str) -> None:
        """
        Set current context.
        
        Args:
            process_id: Process identifier
            user_id: User identifier
        """
        self.current_process_id = process_id
        self.current_user_id = user_id
    
    def invoke_syscall(self, syscall_name: str, parameters: Dict[str, Any] = None,
                      timeout: float = None) -> Any:
        """
        Invoke a system call.
        
        Args:
            syscall_name: System call name
            parameters: System call parameters
            timeout: Timeout in seconds
            
        Returns:
            System call result
        """
        start_time = time.time()
        
        try:
            # Submit syscall
            syscall_id = self.syscall_manager.submit_syscall(
                syscall_name, parameters, self.current_process_id, self.current_user_id)
            
            # Execute syscall
            result = self.syscall_manager.execute_syscall(syscall_id)
            
            # Update statistics
            execution_time = time.time() - start_time
            self.stats['syscalls_invoked'] += 1
            self.stats['syscalls_completed'] += 1
            self.stats['total_execution_time'] += execution_time
            self.stats['average_execution_time'] = (
                self.stats['total_execution_time'] / self.stats['syscalls_completed'])
            
            return result
            
        except Exception as e:
            # Update statistics
            self.stats['syscalls_failed'] += 1
            
            raise
    
    def invoke_syscall_async(self, syscall_name: str, parameters: Dict[str, Any] = None) -> int:
        """
        Invoke a system call asynchronously.
        
        Args:
            syscall_name: System call name
            parameters: System call parameters
            
        Returns:
            System call ID
        """
        syscall_id = self.syscall_manager.submit_syscall(
            syscall_name, parameters, self.current_process_id, self.current_user_id)
        
        self.stats['syscalls_invoked'] += 1
        
        return syscall_id
    
    def wait_for_syscall(self, syscall_id: int, timeout: float = None) -> Any:
        """
        Wait for syscall completion.
        
        Args:
            syscall_id: System call identifier
            timeout: Timeout in seconds
            
        Returns:
            System call result
        """
        start_time = time.time()
        
        while True:
            status = self.syscall_manager.get_syscall_status(syscall_id)
            
            if status is None:
                raise ValueError(f"Unknown syscall ID: {syscall_id}")
            
            if status.value == 'completed':
                result = self.syscall_manager.get_syscall_result(syscall_id)
                self.stats['syscalls_completed'] += 1
                return result
            
            elif status.value == 'failed':
                raise RuntimeError(f"Syscall {syscall_id} failed")
            
            elif status.value == 'cancelled':
                raise RuntimeError(f"Syscall {syscall_id} was cancelled")
            
            # Check timeout
            if timeout and (time.time() - start_time) > timeout:
                raise TimeoutError(f"Syscall {syscall_id} timed out")
            
            time.sleep(0.01)  # Small delay to prevent busy waiting
    
    def cancel_syscall(self, syscall_id: int) -> bool:
        """
        Cancel a syscall.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            True if cancelled successfully, False otherwise
        """
        return self.syscall_manager.cancel_syscall(syscall_id)
    
    def get_syscall_status(self, syscall_id: int) -> Optional[str]:
        """
        Get syscall status.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            System call status or None if not found
        """
        status = self.syscall_manager.get_syscall_status(syscall_id)
        return status.value if status else None
    
    def get_syscall_result(self, syscall_id: int) -> Optional[Any]:
        """
        Get syscall result.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            System call result or None if not found or not completed
        """
        return self.syscall_manager.get_syscall_result(syscall_id)
    
    # Convenience methods for common syscalls
    
    def create_process(self, program_path: str, arguments: List[str] = None) -> int:
        """
        Create a new process.
        
        Args:
            program_path: Path to program
            arguments: Program arguments
            
        Returns:
            Process ID
        """
        parameters = {
            'program_path': program_path,
            'arguments': arguments or []
        }
        return self.invoke_syscall('create_process', parameters)
    
    def terminate_process(self, process_id: int) -> bool:
        """
        Terminate a process.
        
        Args:
            process_id: Process identifier
            
        Returns:
            True if terminated successfully, False otherwise
        """
        parameters = {'process_id': process_id}
        return self.invoke_syscall('terminate_process', parameters)
    
    def allocate_memory(self, size: int) -> int:
        """
        Allocate memory.
        
        Args:
            size: Memory size in bytes
            
        Returns:
            Memory address
        """
        parameters = {'size': size}
        return self.invoke_syscall('allocate_memory', parameters)
    
    def deallocate_memory(self, address: int) -> bool:
        """
        Deallocate memory.
        
        Args:
            address: Memory address
            
        Returns:
            True if deallocated successfully, False otherwise
        """
        parameters = {'address': address}
        return self.invoke_syscall('deallocate_memory', parameters)
    
    def open_file(self, file_path: str, mode: str = 'r') -> int:
        """
        Open a file.
        
        Args:
            file_path: File path
            mode: Open mode
            
        Returns:
            File descriptor
        """
        parameters = {
            'file_path': file_path,
            'mode': mode
        }
        return self.invoke_syscall('open_file', parameters)
    
    def close_file(self, file_descriptor: int) -> bool:
        """
        Close a file.
        
        Args:
            file_descriptor: File descriptor
            
        Returns:
            True if closed successfully, False otherwise
        """
        parameters = {'file_descriptor': file_descriptor}
        return self.invoke_syscall('close_file', parameters)
    
    def read_file(self, file_descriptor: int, size: int = -1) -> bytes:
        """
        Read from file.
        
        Args:
            file_descriptor: File descriptor
            size: Number of bytes to read
            
        Returns:
            Data read from file
        """
        parameters = {
            'file_descriptor': file_descriptor,
            'size': size
        }
        return self.invoke_syscall('read_file', parameters)
    
    def write_file(self, file_descriptor: int, data: bytes) -> int:
        """
        Write to file.
        
        Args:
            file_descriptor: File descriptor
            data: Data to write
            
        Returns:
            Number of bytes written
        """
        parameters = {
            'file_descriptor': file_descriptor,
            'data': data
        }
        return self.invoke_syscall('write_file', parameters)
    
    def create_socket(self, protocol: str = 'tcp') -> int:
        """
        Create a network socket.
        
        Args:
            protocol: Network protocol
            
        Returns:
            Socket descriptor
        """
        parameters = {'protocol': protocol}
        return self.invoke_syscall('create_socket', parameters)
    
    def connect_socket(self, socket_descriptor: int, address: str, port: int) -> bool:
        """
        Connect socket to address.
        
        Args:
            socket_descriptor: Socket descriptor
            address: Remote address
            port: Remote port
            
        Returns:
            True if connected successfully, False otherwise
        """
        parameters = {
            'socket_descriptor': socket_descriptor,
            'address': address,
            'port': port
        }
        return self.invoke_syscall('connect_socket', parameters)
    
    def send_data(self, socket_descriptor: int, data: bytes) -> int:
        """
        Send data through socket.
        
        Args:
            socket_descriptor: Socket descriptor
            data: Data to send
            
        Returns:
            Number of bytes sent
        """
        parameters = {
            'socket_descriptor': socket_descriptor,
            'data': data
        }
        return self.invoke_syscall('send_data', parameters)
    
    def receive_data(self, socket_descriptor: int, size: int = 4096) -> bytes:
        """
        Receive data from socket.
        
        Args:
            socket_descriptor: Socket descriptor
            size: Buffer size
            
        Returns:
            Data received
        """
        parameters = {
            'socket_descriptor': socket_descriptor,
            'size': size
        }
        return self.invoke_syscall('receive_data', parameters)
    
    def get_system_info(self) -> Dict[str, Any]:
        """
        Get system information.
        
        Returns:
            System information dictionary
        """
        return self.invoke_syscall('get_system_info')
    
    def get_process_info(self, process_id: int = None) -> Dict[str, Any]:
        """
        Get process information.
        
        Args:
            process_id: Process identifier (None for current process)
            
        Returns:
            Process information dictionary
        """
        parameters = {'process_id': process_id}
        return self.invoke_syscall('get_process_info', parameters)
    
    def get_memory_info(self) -> Dict[str, Any]:
        """
        Get memory information.
        
        Returns:
            Memory information dictionary
        """
        return self.invoke_syscall('get_memory_info')
    
    def get_interface_stats(self) -> Dict[str, Any]:
        """Get interface statistics."""
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
        return f"TernarySyscallInterface(process={self.current_process_id}, user={self.current_user_id})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernarySyscallInterface(process={self.current_process_id}, "
                f"user={self.current_user_id}, stats={self.stats})")
