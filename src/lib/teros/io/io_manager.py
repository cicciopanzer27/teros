"""
Ternary I/O Manager implementation.

This module provides the main I/O management system for TEROS,
coordinating all I/O operations and device drivers.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import threading
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class IOOperation(Enum):
    """I/O operation types."""
    READ = "read"
    WRITE = "write"
    SEEK = "seek"
    FLUSH = "flush"
    CLOSE = "close"
    OPEN = "open"


class IOStatus(Enum):
    """I/O operation status."""
    PENDING = "pending"
    IN_PROGRESS = "in_progress"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"


class TernaryIORequest:
    """
    Ternary I/O Request.
    
    Represents an I/O operation request with ternary-specific
    metadata and status tracking.
    """
    
    def __init__(self, request_id: int, operation: IOOperation, 
                 device_id: str, data: Any = None, callback: Optional[Callable] = None):
        """
        Initialize I/O request.
        
        Args:
            request_id: Unique request identifier
            operation: I/O operation type
            device_id: Target device identifier
            data: Request data
            callback: Completion callback
        """
        self.request_id = request_id
        self.operation = operation
        self.device_id = device_id
        self.data = data
        self.callback = callback
        
        # Request metadata
        self.status = IOStatus.PENDING
        self.priority = 1  # Default priority
        self.created_time = time.time()
        self.started_time = None
        self.completed_time = None
        
        # Ternary-specific metadata
        self.ternary_metadata = {
            'encoding': 'utf-8',
            'ternary_compression': False,
            'ternary_checksum': None,
            'ternary_encryption': False
        }
        
        # Result data
        self.result_data = None
        self.error_message = None
        self.bytes_processed = 0
    
    def start(self) -> None:
        """Mark request as started."""
        self.status = IOStatus.IN_PROGRESS
        self.started_time = time.time()
    
    def complete(self, result_data: Any = None, bytes_processed: int = 0) -> None:
        """
        Mark request as completed.
        
        Args:
            result_data: Result data
            bytes_processed: Number of bytes processed
        """
        self.status = IOStatus.COMPLETED
        self.completed_time = time.time()
        self.result_data = result_data
        self.bytes_processed = bytes_processed
        
        if self.callback:
            self.callback(self)
    
    def fail(self, error_message: str) -> None:
        """
        Mark request as failed.
        
        Args:
            error_message: Error message
        """
        self.status = IOStatus.FAILED
        self.completed_time = time.time()
        self.error_message = error_message
        
        if self.callback:
            self.callback(self)
    
    def cancel(self) -> None:
        """Cancel the request."""
        self.status = IOStatus.CANCELLED
        self.completed_time = time.time()
    
    def get_duration(self) -> Optional[float]:
        """Get request duration in seconds."""
        if self.started_time and self.completed_time:
            return self.completed_time - self.started_time
        return None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert request to dictionary."""
        return {
            'request_id': self.request_id,
            'operation': self.operation.value,
            'device_id': self.device_id,
            'status': self.status.value,
            'priority': self.priority,
            'created_time': self.created_time,
            'started_time': self.started_time,
            'completed_time': self.completed_time,
            'bytes_processed': self.bytes_processed,
            'error_message': self.error_message,
            'ternary_metadata': self.ternary_metadata.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryIORequest(id={self.request_id}, op={self.operation.value}, device={self.device_id})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryIORequest(id={self.request_id}, op={self.operation.value}, "
                f"device={self.device_id}, status={self.status.value})")


class TernaryIOManager:
    """
    Ternary I/O Manager - Main I/O coordination system.
    
    Manages I/O operations, device drivers, and request queuing
    for the TEROS system.
    """
    
    def __init__(self):
        """Initialize I/O manager."""
        self.devices = {}  # device_id -> device_driver
        self.requests = {}  # request_id -> TernaryIORequest
        self.request_queue = []  # Pending requests
        self.completed_requests = []  # Completed requests
        
        # I/O statistics
        self.stats = {
            'total_requests': 0,
            'completed_requests': 0,
            'failed_requests': 0,
            'cancelled_requests': 0,
            'total_bytes_read': 0,
            'total_bytes_written': 0,
            'average_request_time': 0.0,
            'active_requests': 0
        }
        
        # Threading
        self.lock = threading.Lock()
        self.condition = threading.Condition(self.lock)
        self.worker_thread = None
        self.running = False
        
        # Request ID counter
        self.next_request_id = 1
    
    def register_device(self, device_id: str, device_driver: Any) -> bool:
        """
        Register a device driver.
        
        Args:
            device_id: Device identifier
            device_driver: Device driver instance
            
        Returns:
            True if registered successfully, False otherwise
        """
        with self.lock:
            if device_id in self.devices:
                return False
            
            self.devices[device_id] = device_driver
            return True
    
    def unregister_device(self, device_id: str) -> bool:
        """
        Unregister a device driver.
        
        Args:
            device_id: Device identifier
            
        Returns:
            True if unregistered successfully, False otherwise
        """
        with self.lock:
            if device_id not in self.devices:
                return False
            
            del self.devices[device_id]
            return True
    
    def submit_request(self, operation: IOOperation, device_id: str, 
                      data: Any = None, priority: int = 1, 
                      callback: Optional[Callable] = None) -> int:
        """
        Submit an I/O request.
        
        Args:
            operation: I/O operation type
            device_id: Target device identifier
            data: Request data
            priority: Request priority
            callback: Completion callback
            
        Returns:
            Request ID
        """
        with self.lock:
            request_id = self.next_request_id
            self.next_request_id += 1
            
            request = TernaryIORequest(request_id, operation, device_id, data, callback)
            request.priority = priority
            
            self.requests[request_id] = request
            self.request_queue.append(request)
            
            # Sort queue by priority
            self.request_queue.sort(key=lambda r: r.priority, reverse=True)
            
            self.stats['total_requests'] += 1
            self.stats['active_requests'] += 1
            
            # Notify worker thread
            self.condition.notify()
            
            return request_id
    
    def get_request_status(self, request_id: int) -> Optional[IOStatus]:
        """
        Get request status.
        
        Args:
            request_id: Request identifier
            
        Returns:
            Request status or None if not found
        """
        with self.lock:
            if request_id in self.requests:
                return self.requests[request_id].status
            return None
    
    def cancel_request(self, request_id: int) -> bool:
        """
        Cancel a request.
        
        Args:
            request_id: Request identifier
            
        Returns:
            True if cancelled successfully, False otherwise
        """
        with self.lock:
            if request_id in self.requests:
                request = self.requests[request_id]
                if request.status == IOStatus.PENDING:
                    request.cancel()
                    self.request_queue.remove(request)
                    self.stats['cancelled_requests'] += 1
                    self.stats['active_requests'] -= 1
                    return True
            return False
    
    def get_request_result(self, request_id: int) -> Optional[Any]:
        """
        Get request result.
        
        Args:
            request_id: Request identifier
            
        Returns:
            Request result or None if not found or not completed
        """
        with self.lock:
            if request_id in self.requests:
                request = self.requests[request_id]
                if request.status == IOStatus.COMPLETED:
                    return request.result_data
            return None
    
    def start_worker(self) -> None:
        """Start the I/O worker thread."""
        if self.worker_thread and self.worker_thread.is_alive():
            return
        
        self.running = True
        self.worker_thread = threading.Thread(target=self._worker_loop, daemon=True)
        self.worker_thread.start()
    
    def stop_worker(self) -> None:
        """Stop the I/O worker thread."""
        with self.lock:
            self.running = False
            self.condition.notify_all()
        
        if self.worker_thread:
            self.worker_thread.join()
    
    def _worker_loop(self) -> None:
        """Main worker loop for processing I/O requests."""
        while self.running:
            with self.condition:
                while not self.request_queue and self.running:
                    self.condition.wait()
                
                if not self.running:
                    break
                
                # Get next request
                if self.request_queue:
                    request = self.request_queue.pop(0)
                    self._process_request(request)
    
    def _process_request(self, request: TernaryIORequest) -> None:
        """Process a single I/O request."""
        try:
            request.start()
            
            # Get device driver
            device = self.devices.get(request.device_id)
            if not device:
                request.fail(f"Device {request.device_id} not found")
                return
            
            # Execute operation based on type
            if request.operation == IOOperation.READ:
                result = device.read(request.data)
            elif request.operation == IOOperation.WRITE:
                result = device.write(request.data)
            elif request.operation == IOOperation.SEEK:
                result = device.seek(request.data)
            elif request.operation == IOOperation.FLUSH:
                result = device.flush()
            elif request.operation == IOOperation.CLOSE:
                result = device.close()
            elif request.operation == IOOperation.OPEN:
                result = device.open(request.data)
            else:
                request.fail(f"Unknown operation: {request.operation}")
                return
            
            # Handle result
            if isinstance(result, tuple) and len(result) == 2:
                data, bytes_processed = result
                request.complete(data, bytes_processed)
            else:
                request.complete(result, 0)
            
        except Exception as e:
            request.fail(f"Error processing request: {str(e)}")
        
        finally:
            # Update statistics
            with self.lock:
                self.stats['active_requests'] -= 1
                
                if request.status == IOStatus.COMPLETED:
                    self.stats['completed_requests'] += 1
                    if request.bytes_processed:
                        if request.operation == IOOperation.READ:
                            self.stats['total_bytes_read'] += request.bytes_processed
                        elif request.operation == IOOperation.WRITE:
                            self.stats['total_bytes_written'] += request.bytes_processed
                elif request.status == IOStatus.FAILED:
                    self.stats['failed_requests'] += 1
                
                # Move to completed requests
                self.completed_requests.append(request)
                
                # Update average request time
                if request.get_duration():
                    total_time = self.stats['average_request_time'] * (self.stats['completed_requests'] - 1)
                    total_time += request.get_duration()
                    self.stats['average_request_time'] = total_time / self.stats['completed_requests']
    
    def get_io_stats(self) -> Dict[str, Any]:
        """Get I/O statistics."""
        with self.lock:
            return self.stats.copy()
    
    def get_device_list(self) -> List[str]:
        """Get list of registered devices."""
        with self.lock:
            return list(self.devices.keys())
    
    def get_active_requests(self) -> List[Dict[str, Any]]:
        """Get active requests."""
        with self.lock:
            active = []
            for request in self.requests.values():
                if request.status in [IOStatus.PENDING, IOStatus.IN_PROGRESS]:
                    active.append(request.to_dict())
            return active
    
    def get_completed_requests(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get completed requests."""
        with self.lock:
            return [request.to_dict() for request in self.completed_requests[-limit:]]
    
    def clear_completed_requests(self) -> None:
        """Clear completed requests."""
        with self.lock:
            self.completed_requests.clear()
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryIOManager(devices={len(self.devices)}, requests={len(self.requests)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryIOManager(devices={len(self.devices)}, "
                f"requests={len(self.requests)}, active={self.stats['active_requests']})")
