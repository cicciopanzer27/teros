"""
Inter-Process Communication (IPC) for TEROS.

This module provides IPC mechanisms including message queues, shared memory,
and synchronization primitives for the TEROS system.
"""

from typing import Dict, List, Optional, Any, Union, Set
import time
import threading
from enum import Enum
from collections import deque
from ..core.trit import Trit
from ..core.tritarray import TritArray


class IPCMessageType(Enum):
    """IPC message types."""
    DATA = "data"
    CONTROL = "control"
    SIGNAL = "signal"
    REQUEST = "request"
    RESPONSE = "response"
    NOTIFICATION = "notification"


class IPCPriority(Enum):
    """IPC message priorities."""
    LOW = 0
    NORMAL = 1
    HIGH = 2
    CRITICAL = 3


class TernaryIPCMessage:
    """
    IPC message for TEROS.
    
    Represents a message that can be sent between processes
    using ternary data structures.
    """
    
    def __init__(self, sender_pid: int, receiver_pid: int, 
                 message_type: IPCMessageType, data: Any,
                 priority: IPCPriority = IPCPriority.NORMAL):
        """
        Initialize IPC message.
        
        Args:
            sender_pid: Sender process ID
            receiver_pid: Receiver process ID
            message_type: Type of message
            data: Message data
            priority: Message priority
        """
        self.sender_pid = sender_pid
        self.receiver_pid = receiver_pid
        self.message_type = message_type
        self.data = data
        self.priority = priority
        self.timestamp = time.time()
        self.message_id = id(self)
        self.delivered = False
        self.acknowledged = False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert message to dictionary."""
        return {
            'message_id': self.message_id,
            'sender_pid': self.sender_pid,
            'receiver_pid': self.receiver_pid,
            'message_type': self.message_type.value,
            'data': self.data,
            'priority': self.priority.value,
            'timestamp': self.timestamp,
            'delivered': self.delivered,
            'acknowledged': self.acknowledged
        }
    
    def __str__(self) -> str:
        """String representation."""
        return (f"IPCMessage(id={self.message_id}, sender={self.sender_pid}, "
                f"receiver={self.receiver_pid}, type={self.message_type.value})")
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryIPCMessage(id={self.message_id}, sender={self.sender_pid}, "
                f"receiver={self.receiver_pid}, type={self.message_type.value}, "
                f"priority={self.priority.value})")


class TernaryIPCQueue:
    """
    IPC message queue for TEROS.
    
    Provides a priority-based message queue for inter-process communication.
    """
    
    def __init__(self, queue_id: int, max_size: int = 1000):
        """
        Initialize IPC queue.
        
        Args:
            queue_id: Unique queue identifier
            max_size: Maximum queue size
        """
        self.queue_id = queue_id
        self.max_size = max_size
        self.messages = deque()
        self.lock = threading.Lock()
        self.condition = threading.Condition(self.lock)
        
        # Queue statistics
        self.stats = {
            'messages_sent': 0,
            'messages_received': 0,
            'messages_dropped': 0,
            'current_size': 0,
            'peak_size': 0
        }
    
    def send_message(self, message: TernaryIPCMessage) -> bool:
        """
        Send a message to the queue.
        
        Args:
            message: Message to send
            
        Returns:
            True if message sent successfully, False otherwise
        """
        with self.lock:
            if len(self.messages) >= self.max_size:
                self.stats['messages_dropped'] += 1
                return False
            
            # Insert message based on priority
            self._insert_by_priority(message)
            
            self.stats['messages_sent'] += 1
            self.stats['current_size'] = len(self.messages)
            if len(self.messages) > self.stats['peak_size']:
                self.stats['peak_size'] = len(self.messages)
            
            # Notify waiting receivers
            self.condition.notify_all()
            return True
    
    def receive_message(self, timeout: Optional[float] = None) -> Optional[TernaryIPCMessage]:
        """
        Receive a message from the queue.
        
        Args:
            timeout: Maximum time to wait for message
            
        Returns:
            Message if available, None if timeout
        """
        with self.condition:
            if timeout is None:
                # Wait indefinitely
                while not self.messages:
                    self.condition.wait()
            else:
                # Wait with timeout
                if not self.messages:
                    self.condition.wait(timeout)
            
            if self.messages:
                message = self.messages.popleft()
                message.delivered = True
                self.stats['messages_received'] += 1
                self.stats['current_size'] = len(self.messages)
                return message
            
            return None
    
    def peek_message(self) -> Optional[TernaryIPCMessage]:
        """
        Peek at the next message without removing it.
        
        Returns:
            Next message or None if queue is empty
        """
        with self.lock:
            if self.messages:
                return self.messages[0]
            return None
    
    def _insert_by_priority(self, message: TernaryIPCMessage) -> None:
        """Insert message based on priority."""
        # Higher priority messages go to the front
        inserted = False
        for i, existing_message in enumerate(self.messages):
            if message.priority.value > existing_message.priority.value:
                self.messages.insert(i, message)
                inserted = True
                break
        
        if not inserted:
            self.messages.append(message)
    
    def get_queue_stats(self) -> Dict[str, Any]:
        """Get queue statistics."""
        with self.lock:
            return self.stats.copy()
    
    def clear_queue(self) -> None:
        """Clear all messages from the queue."""
        with self.lock:
            self.messages.clear()
            self.stats['current_size'] = 0
    
    def __len__(self) -> int:
        """Get queue length."""
        with self.lock:
            return len(self.messages)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryIPCQueue(id={self.queue_id}, size={len(self.messages)})"


class TernaryIPCManager:
    """
    IPC Manager for TEROS.
    
    Manages inter-process communication including message queues,
    shared memory, and synchronization primitives.
    """
    
    def __init__(self):
        """Initialize IPC manager."""
        self.queues = {}
        self.shared_memory = {}
        self.semaphores = {}
        self.mutexes = {}
        self.next_queue_id = 1
        self.next_shared_mem_id = 1
        self.lock = threading.Lock()
        
        # IPC statistics
        self.stats = {
            'total_queues': 0,
            'total_shared_memory': 0,
            'total_messages': 0,
            'active_queues': 0,
            'active_shared_memory': 0
        }
    
    def create_queue(self, max_size: int = 1000) -> int:
        """
        Create a new IPC queue.
        
        Args:
            max_size: Maximum queue size
            
        Returns:
            Queue ID
        """
        with self.lock:
            queue_id = self.next_queue_id
            self.next_queue_id += 1
            
            queue = TernaryIPCQueue(queue_id, max_size)
            self.queues[queue_id] = queue
            
            self.stats['total_queues'] += 1
            self.stats['active_queues'] += 1
            
            return queue_id
    
    def destroy_queue(self, queue_id: int) -> bool:
        """
        Destroy an IPC queue.
        
        Args:
            queue_id: Queue ID to destroy
            
        Returns:
            True if destroyed successfully, False otherwise
        """
        with self.lock:
            if queue_id in self.queues:
                del self.queues[queue_id]
                self.stats['active_queues'] -= 1
                return True
            return False
    
    def send_message(self, queue_id: int, message: TernaryIPCMessage) -> bool:
        """
        Send a message to a queue.
        
        Args:
            queue_id: Target queue ID
            message: Message to send
            
        Returns:
            True if sent successfully, False otherwise
        """
        if queue_id in self.queues:
            success = self.queues[queue_id].send_message(message)
            if success:
                self.stats['total_messages'] += 1
            return success
        return False
    
    def receive_message(self, queue_id: int, timeout: Optional[float] = None) -> Optional[TernaryIPCMessage]:
        """
        Receive a message from a queue.
        
        Args:
            queue_id: Source queue ID
            timeout: Maximum time to wait
            
        Returns:
            Message if available, None otherwise
        """
        if queue_id in self.queues:
            return self.queues[queue_id].receive_message(timeout)
        return None
    
    def create_shared_memory(self, size: int, name: str = "") -> int:
        """
        Create shared memory segment.
        
        Args:
            size: Size of shared memory
            name: Optional name for the segment
            
        Returns:
            Shared memory ID
        """
        with self.lock:
            shm_id = self.next_shared_mem_id
            self.next_shared_mem_id += 1
            
            # Create shared memory segment
            shared_mem = {
                'id': shm_id,
                'size': size,
                'name': name,
                'data': bytearray(size),
                'attached_processes': set(),
                'created_time': time.time()
            }
            
            self.shared_memory[shm_id] = shared_mem
            
            self.stats['total_shared_memory'] += 1
            self.stats['active_shared_memory'] += 1
            
            return shm_id
    
    def attach_shared_memory(self, shm_id: int, process_id: int) -> bool:
        """
        Attach process to shared memory.
        
        Args:
            shm_id: Shared memory ID
            process_id: Process ID
            
        Returns:
            True if attached successfully, False otherwise
        """
        with self.lock:
            if shm_id in self.shared_memory:
                self.shared_memory[shm_id]['attached_processes'].add(process_id)
                return True
            return False
    
    def detach_shared_memory(self, shm_id: int, process_id: int) -> bool:
        """
        Detach process from shared memory.
        
        Args:
            shm_id: Shared memory ID
            process_id: Process ID
            
        Returns:
            True if detached successfully, False otherwise
        """
        with self.lock:
            if shm_id in self.shared_memory:
                self.shared_memory[shm_id]['attached_processes'].discard(process_id)
                return True
            return False
    
    def destroy_shared_memory(self, shm_id: int) -> bool:
        """
        Destroy shared memory segment.
        
        Args:
            shm_id: Shared memory ID
            
        Returns:
            True if destroyed successfully, False otherwise
        """
        with self.lock:
            if shm_id in self.shared_memory:
                del self.shared_memory[shm_id]
                self.stats['active_shared_memory'] -= 1
                return True
            return False
    
    def read_shared_memory(self, shm_id: int, offset: int, size: int) -> Optional[bytes]:
        """
        Read from shared memory.
        
        Args:
            shm_id: Shared memory ID
            offset: Offset to read from
            size: Number of bytes to read
            
        Returns:
            Data read or None if error
        """
        if shm_id in self.shared_memory:
            shm = self.shared_memory[shm_id]
            if offset + size <= shm['size']:
                return bytes(shm['data'][offset:offset + size])
        return None
    
    def write_shared_memory(self, shm_id: int, offset: int, data: bytes) -> bool:
        """
        Write to shared memory.
        
        Args:
            shm_id: Shared memory ID
            offset: Offset to write to
            data: Data to write
            
        Returns:
            True if written successfully, False otherwise
        """
        if shm_id in self.shared_memory:
            shm = self.shared_memory[shm_id]
            if offset + len(data) <= shm['size']:
                shm['data'][offset:offset + len(data)] = data
                return True
        return False
    
    def create_semaphore(self, initial_value: int = 1) -> int:
        """
        Create a semaphore.
        
        Args:
            initial_value: Initial semaphore value
            
        Returns:
            Semaphore ID
        """
        with self.lock:
            sem_id = len(self.semaphores) + 1
            self.semaphores[sem_id] = {
                'value': initial_value,
                'waiting_processes': deque(),
                'created_time': time.time()
            }
            return sem_id
    
    def semaphore_wait(self, sem_id: int, process_id: int) -> bool:
        """
        Wait on a semaphore.
        
        Args:
            sem_id: Semaphore ID
            process_id: Process ID
            
        Returns:
            True if acquired, False if error
        """
        if sem_id in self.semaphores:
            sem = self.semaphores[sem_id]
            if sem['value'] > 0:
                sem['value'] -= 1
                return True
            else:
                sem['waiting_processes'].append(process_id)
                return False
        return False
    
    def semaphore_signal(self, sem_id: int) -> bool:
        """
        Signal a semaphore.
        
        Args:
            sem_id: Semaphore ID
            
        Returns:
            True if signaled successfully, False otherwise
        """
        if sem_id in self.semaphores:
            sem = self.semaphores[sem_id]
            if sem['waiting_processes']:
                # Wake up a waiting process
                process_id = sem['waiting_processes'].popleft()
                return True
            else:
                sem['value'] += 1
                return True
        return False
    
    def get_ipc_stats(self) -> Dict[str, Any]:
        """Get IPC statistics."""
        with self.lock:
            return self.stats.copy()
    
    def get_queue_info(self, queue_id: int) -> Optional[Dict[str, Any]]:
        """Get information about a queue."""
        if queue_id in self.queues:
            return self.queues[queue_id].get_queue_stats()
        return None
    
    def get_shared_memory_info(self, shm_id: int) -> Optional[Dict[str, Any]]:
        """Get information about shared memory."""
        if shm_id in self.shared_memory:
            shm = self.shared_memory[shm_id]
            return {
                'id': shm['id'],
                'size': shm['size'],
                'name': shm['name'],
                'attached_processes': list(shm['attached_processes']),
                'created_time': shm['created_time']
            }
        return None
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryIPCManager(queues={len(self.queues)}, shared_mem={len(self.shared_memory)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryIPCManager(queues={len(self.queues)}, "
                f"shared_mem={len(self.shared_memory)}, "
                f"messages={self.stats['total_messages']})")
