"""
Ternary System Call Manager implementation.

This module provides the main system call management system for TEROS,
including syscall registration, dispatch, and execution.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import threading
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class SyscallType(Enum):
    """System call types."""
    PROCESS = "process"
    MEMORY = "memory"
    FILE = "file"
    NETWORK = "network"
    DEVICE = "device"
    SECURITY = "security"
    SYSTEM = "system"
    IO = "io"


class SyscallStatus(Enum):
    """System call status."""
    PENDING = "pending"
    EXECUTING = "executing"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


class TernarySyscall:
    """
    Ternary System Call - Represents a system call.
    
    Contains syscall information including type, parameters,
    and execution status.
    """
    
    def __init__(self, syscall_id: int, syscall_type: SyscallType,
                 syscall_name: str, parameters: Dict[str, Any] = None):
        """
        Initialize system call.
        
        Args:
            syscall_id: System call identifier
            syscall_type: Type of system call
            syscall_name: System call name
            parameters: System call parameters
        """
        self.syscall_id = syscall_id
        self.syscall_type = syscall_type
        self.syscall_name = syscall_name
        self.parameters = parameters or {}
        
        # Syscall metadata
        self.status = SyscallStatus.PENDING
        self.created_time = time.time()
        self.started_time = None
        self.completed_time = None
        self.process_id = None
        self.user_id = None
        
        # Result data
        self.return_value = None
        self.error_code = None
        self.error_message = None
        
        # Ternary-specific syscall features
        self.ternary_features = {
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_checksum': True
        }
    
    def start(self) -> None:
        """Mark syscall as started."""
        self.status = SyscallStatus.EXECUTING
        self.started_time = time.time()
    
    def complete(self, return_value: Any = None) -> None:
        """
        Mark syscall as completed.
        
        Args:
            return_value: Return value
        """
        self.status = SyscallStatus.COMPLETED
        self.completed_time = time.time()
        self.return_value = return_value
    
    def fail(self, error_code: int, error_message: str) -> None:
        """
        Mark syscall as failed.
        
        Args:
            error_code: Error code
            error_message: Error message
        """
        self.status = SyscallStatus.FAILED
        self.completed_time = time.time()
        self.error_code = error_code
        self.error_message = error_message
    
    def cancel(self) -> None:
        """Cancel the syscall."""
        self.status = SyscallStatus.CANCELLED
        self.completed_time = time.time()
    
    def get_duration(self) -> Optional[float]:
        """Get syscall duration in seconds."""
        if self.started_time and self.completed_time:
            return self.completed_time - self.started_time
        return None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert syscall to dictionary."""
        return {
            'syscall_id': self.syscall_id,
            'syscall_type': self.syscall_type.value,
            'syscall_name': self.syscall_name,
            'parameters': self.parameters.copy(),
            'status': self.status.value,
            'created_time': self.created_time,
            'started_time': self.started_time,
            'completed_time': self.completed_time,
            'process_id': self.process_id,
            'user_id': self.user_id,
            'return_value': self.return_value,
            'error_code': self.error_code,
            'error_message': self.error_message,
            'ternary_features': self.ternary_features.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernarySyscall(id={self.syscall_id}, name={self.syscall_name}, status={self.status.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernarySyscall(id={self.syscall_id}, name={self.syscall_name}, "
                f"type={self.syscall_type.value}, status={self.status.value})")


class TernarySyscallManager:
    """
    Ternary System Call Manager - Main syscall management system.
    
    Manages system call registration, dispatch, and execution
    for the TEROS system.
    """
    
    def __init__(self):
        """Initialize syscall manager."""
        self.syscalls = {}  # syscall_id -> TernarySyscall
        self.syscall_handlers = {}  # syscall_name -> handler_function
        self.syscall_queue = []  # Pending syscalls
        self.completed_syscalls = []  # Completed syscalls
        
        # Syscall statistics
        self.stats = {
            'total_syscalls': 0,
            'completed_syscalls': 0,
            'failed_syscalls': 0,
            'cancelled_syscalls': 0,
            'active_syscalls': 0,
            'average_execution_time': 0.0,
            'syscalls_by_type': {},
            'syscalls_by_process': {}
        }
        
        # Ternary-specific syscall features
        self.ternary_features = {
            'ternary_syscalls': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_optimization': True
        }
        
        # Threading
        self.lock = threading.Lock()
        self.worker_thread = None
        self.running = False
        
        # Syscall ID counter
        self.next_syscall_id = 1
    
    def register_syscall(self, syscall_name: str, syscall_type: SyscallType,
                        handler: Callable[[Dict[str, Any]], Any]) -> bool:
        """
        Register a system call handler.
        
        Args:
            syscall_name: System call name
            syscall_type: Type of system call
            handler: Handler function
            
        Returns:
            True if registered successfully, False otherwise
        """
        with self.lock:
            if syscall_name in self.syscall_handlers:
                return False
            
            self.syscall_handlers[syscall_name] = {
                'type': syscall_type,
                'handler': handler,
                'registered_time': time.time()
            }
            
            return True
    
    def unregister_syscall(self, syscall_name: str) -> bool:
        """
        Unregister a system call handler.
        
        Args:
            syscall_name: System call name
            
        Returns:
            True if unregistered successfully, False otherwise
        """
        with self.lock:
            if syscall_name not in self.syscall_handlers:
                return False
            
            del self.syscall_handlers[syscall_name]
            return True
    
    def submit_syscall(self, syscall_name: str, parameters: Dict[str, Any] = None,
                       process_id: str = None, user_id: str = None) -> int:
        """
        Submit a system call.
        
        Args:
            syscall_name: System call name
            parameters: System call parameters
            process_id: Process identifier
            user_id: User identifier
            
        Returns:
            System call ID
        """
        with self.lock:
            if syscall_name not in self.syscall_handlers:
                raise ValueError(f"Unknown syscall: {syscall_name}")
            
            syscall_id = self.next_syscall_id
            self.next_syscall_id += 1
            
            # Get syscall type
            syscall_type = self.syscall_handlers[syscall_name]['type']
            
            # Create syscall
            syscall = TernarySyscall(syscall_id, syscall_type, syscall_name, parameters)
            syscall.process_id = process_id
            syscall.user_id = user_id
            
            self.syscalls[syscall_id] = syscall
            self.syscall_queue.append(syscall)
            
            # Update statistics
            self.stats['total_syscalls'] += 1
            self.stats['active_syscalls'] += 1
            
            # Update by type
            syscall_type_str = syscall_type.value
            if syscall_type_str not in self.stats['syscalls_by_type']:
                self.stats['syscalls_by_type'][syscall_type_str] = 0
            self.stats['syscalls_by_type'][syscall_type_str] += 1
            
            # Update by process
            if process_id:
                if process_id not in self.stats['syscalls_by_process']:
                    self.stats['syscalls_by_process'][process_id] = 0
                self.stats['syscalls_by_process'][process_id] += 1
            
            return syscall_id
    
    def execute_syscall(self, syscall_id: int) -> Any:
        """
        Execute a system call.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            System call result
        """
        if syscall_id not in self.syscalls:
            raise ValueError(f"Unknown syscall ID: {syscall_id}")
        
        syscall = self.syscalls[syscall_id]
        
        if syscall.status != SyscallStatus.PENDING:
            raise ValueError(f"Syscall {syscall_id} is not pending")
        
        try:
            # Start syscall
            syscall.start()
            
            # Get handler
            handler_info = self.syscall_handlers[syscall.syscall_name]
            handler = handler_info['handler']
            
            # Execute handler
            result = handler(syscall.parameters)
            
            # Complete syscall
            syscall.complete(result)
            
            # Update statistics
            self.stats['completed_syscalls'] += 1
            self.stats['active_syscalls'] -= 1
            
            # Update average execution time
            if syscall.get_duration():
                total_time = self.stats['average_execution_time'] * (self.stats['completed_syscalls'] - 1)
                total_time += syscall.get_duration()
                self.stats['average_execution_time'] = total_time / self.stats['completed_syscalls']
            
            # Move to completed
            self.completed_syscalls.append(syscall)
            
            return result
            
        except Exception as e:
            # Fail syscall
            syscall.fail(1, str(e))
            
            # Update statistics
            self.stats['failed_syscalls'] += 1
            self.stats['active_syscalls'] -= 1
            
            # Move to completed
            self.completed_syscalls.append(syscall)
            
            raise
    
    def get_syscall_status(self, syscall_id: int) -> Optional[SyscallStatus]:
        """
        Get syscall status.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            System call status or None if not found
        """
        if syscall_id in self.syscalls:
            return self.syscalls[syscall_id].status
        return None
    
    def get_syscall_result(self, syscall_id: int) -> Optional[Any]:
        """
        Get syscall result.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            System call result or None if not found or not completed
        """
        if syscall_id in self.syscalls:
            syscall = self.syscalls[syscall_id]
            if syscall.status == SyscallStatus.COMPLETED:
                return syscall.return_value
        return None
    
    def cancel_syscall(self, syscall_id: int) -> bool:
        """
        Cancel a syscall.
        
        Args:
            syscall_id: System call identifier
            
        Returns:
            True if cancelled successfully, False otherwise
        """
        if syscall_id not in self.syscalls:
            return False
        
        syscall = self.syscalls[syscall_id]
        
        if syscall.status in [SyscallStatus.PENDING, SyscallStatus.EXECUTING]:
            syscall.cancel()
            self.stats['cancelled_syscalls'] += 1
            self.stats['active_syscalls'] -= 1
            return True
        
        return False
    
    def start_worker(self) -> None:
        """Start the syscall worker thread."""
        if self.worker_thread and self.worker_thread.is_alive():
            return
        
        self.running = True
        self.worker_thread = threading.Thread(target=self._worker_loop, daemon=True)
        self.worker_thread.start()
    
    def stop_worker(self) -> None:
        """Stop the syscall worker thread."""
        with self.lock:
            self.running = False
        
        if self.worker_thread:
            self.worker_thread.join()
    
    def _worker_loop(self) -> None:
        """Main worker loop for processing syscalls."""
        while self.running:
            with self.lock:
                if self.syscall_queue:
                    syscall = self.syscall_queue.pop(0)
                    self._process_syscall(syscall)
                else:
                    time.sleep(0.01)  # Small delay to prevent busy waiting
    
    def _process_syscall(self, syscall: TernarySyscall) -> None:
        """Process a single syscall."""
        try:
            self.execute_syscall(syscall.syscall_id)
        except Exception as e:
            print(f"Syscall processing error: {e}")
    
    def get_syscall_stats(self) -> Dict[str, Any]:
        """Get syscall statistics."""
        with self.lock:
            return self.stats.copy()
    
    def get_active_syscalls(self) -> List[Dict[str, Any]]:
        """Get active syscalls."""
        with self.lock:
            active = []
            for syscall in self.syscalls.values():
                if syscall.status in [SyscallStatus.PENDING, SyscallStatus.EXECUTING]:
                    active.append(syscall.to_dict())
            return active
    
    def get_completed_syscalls(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get completed syscalls."""
        with self.lock:
            return [syscall.to_dict() for syscall in self.completed_syscalls[-limit:]]
    
    def clear_completed_syscalls(self) -> None:
        """Clear completed syscalls."""
        with self.lock:
            self.completed_syscalls.clear()
    
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
        return f"TernarySyscallManager(syscalls={len(self.syscalls)}, handlers={len(self.syscall_handlers)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernarySyscallManager(syscalls={len(self.syscalls)}, "
                f"handlers={len(self.syscall_handlers)}, active={self.stats['active_syscalls']})")
