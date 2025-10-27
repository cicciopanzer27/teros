"""
T3_PCB - Process Control Block for ternary processes.

This module defines the Process Control Block structure for managing
ternary processes in the TEROS operating system.
"""

from typing import Dict, List, Optional, Any, Union
from datetime import datetime
from .trit import Trit
from .tritarray import TritArray


class T3_PCB:
    """
    Process Control Block for ternary processes.
    
    The PCB contains all the information needed to manage a process,
    including its state, registers, memory information, and runtime statistics.
    """
    
    # Process states
    STATE_BLOCKED = "blocked"
    STATE_READY = "ready"
    STATE_RUNNING = "running"
    STATE_ZOMBIE = "zombie"
    STATE_TERMINATED = "terminated"
    
    # Process priorities
    PRIORITY_LOW = 0
    PRIORITY_NORMAL = 1
    PRIORITY_HIGH = 2
    PRIORITY_CRITICAL = 3
    
    # Security levels
    SECURITY_USER = "user"
    SECURITY_KERNEL = "kernel"
    SECURITY_SUPERVISOR = "supervisor"
    
    def __init__(self, pid: int, name: str = "", priority: int = PRIORITY_NORMAL):
        """
        Initialize a Process Control Block.
        
        Args:
            pid: Process ID
            name: Process name
            priority: Process priority
        """
        # Basic process information
        self.pid = pid
        self.name = name
        self.parent_pid = None
        self.children_pids = []
        
        # Process state
        self.state = self.STATE_READY
        self.priority = priority
        self.security_level = self.SECURITY_USER
        
        # Registers (8 general purpose + special registers)
        self.registers = {
            'R0': TritArray(0),
            'R1': TritArray(0),
            'R2': TritArray(0),
            'R3': TritArray(0),
            'R4': TritArray(0),
            'R5': TritArray(0),
            'R6': TritArray(0),
            'R7': TritArray(0),
            'PC': TritArray(0),  # Program Counter
            'SP': TritArray(0),  # Stack Pointer
            'FP': TritArray(0),  # Frame Pointer
            'FLAGS': TritArray(0)  # Status Flags
        }
        
        # Memory information
        self.memory_info = {
            'code_start': 0,
            'code_size': 0,
            'data_start': 0,
            'data_size': 0,
            'stack_start': 0,
            'stack_size': 0,
            'heap_start': 0,
            'heap_size': 0,
            'allocated_pages': set(),
            'memory_limit': 0
        }
        
        # File descriptors
        self.file_descriptors = {}
        self.max_fd = 0
        
        # Process statistics
        self.stats = {
            'creation_time': datetime.now(),
            'start_time': None,
            'end_time': None,
            'cpu_time': 0,
            'memory_usage': 0,
            'io_operations': 0,
            'context_switches': 0,
            'page_faults': 0,
            'system_calls': 0
        }
        
        # Security and permissions
        self.permissions = {
            'can_read': True,
            'can_write': True,
            'can_execute': True,
            'can_allocate_memory': True,
            'can_create_processes': False,
            'can_access_network': False,
            'can_access_files': True
        }
        
        # Signals and interrupts
        self.pending_signals = []
        self.signal_handlers = {}
        self.interrupt_mask = 0
        
        # Process resources
        self.resources = {
            'cpu_quota': 1000,  # CPU time quota
            'memory_quota': 10000,  # Memory quota
            'io_quota': 1000,  # I/O quota
            'file_quota': 100  # File descriptor quota
        }
        
        # Process environment
        self.environment = {}
        self.working_directory = "/"
        self.umask = 0o022
        
        # Process arguments and environment
        self.argv = []
        self.envp = {}
        
        # Process capabilities
        self.capabilities = set()
        
        # Process limits
        self.limits = {
            'max_memory': 1000000,
            'max_files': 100,
            'max_processes': 10,
            'max_cpu_time': 3600
        }
    
    def get_register(self, name: str) -> TritArray:
        """Get a register value."""
        if name not in self.registers:
            raise ValueError(f"Invalid register: {name}")
        return self.registers[name]
    
    def set_register(self, name: str, value: Union[TritArray, int]) -> None:
        """Set a register value."""
        if name not in self.registers:
            raise ValueError(f"Invalid register: {name}")
        
        if isinstance(value, int):
            self.registers[name] = TritArray(value)
        else:
            self.registers[name] = value
    
    def get_all_registers(self) -> Dict[str, TritArray]:
        """Get all register values."""
        return self.registers.copy()
    
    def set_all_registers(self, registers: Dict[str, TritArray]) -> None:
        """Set all register values."""
        for name, value in registers.items():
            if name in self.registers:
                self.registers[name] = value
    
    def save_context(self) -> Dict[str, Any]:
        """Save the current process context."""
        return {
            'registers': self.registers.copy(),
            'state': self.state,
            'priority': self.priority,
            'memory_info': self.memory_info.copy(),
            'stats': self.stats.copy()
        }
    
    def restore_context(self, context: Dict[str, Any]) -> None:
        """Restore a saved process context."""
        self.registers = context['registers'].copy()
        self.state = context['state']
        self.priority = context['priority']
        self.memory_info = context['memory_info'].copy()
        self.stats = context['stats'].copy()
    
    def allocate_memory(self, size: int) -> int:
        """Allocate memory for the process."""
        if self.memory_info['memory_usage'] + size > self.limits['max_memory']:
            raise MemoryError("Memory allocation would exceed process limit")
        
        # Simple allocation (in real implementation, this would be more complex)
        start_address = self.memory_info['heap_start'] + self.memory_info['memory_usage']
        self.memory_info['memory_usage'] += size
        return start_address
    
    def deallocate_memory(self, address: int, size: int) -> None:
        """Deallocate memory for the process."""
        # Simple deallocation (in real implementation, this would be more complex)
        self.memory_info['memory_usage'] -= size
    
    def open_file(self, filename: str, mode: str) -> int:
        """Open a file and return file descriptor."""
        if len(self.file_descriptors) >= self.limits['max_files']:
            raise OSError("Too many open files")
        
        fd = self.max_fd
        self.max_fd += 1
        
        self.file_descriptors[fd] = {
            'filename': filename,
            'mode': mode,
            'position': 0,
            'flags': 0
        }
        
        return fd
    
    def close_file(self, fd: int) -> None:
        """Close a file descriptor."""
        if fd in self.file_descriptors:
            del self.file_descriptors[fd]
    
    def get_file_info(self, fd: int) -> Dict[str, Any]:
        """Get file descriptor information."""
        if fd not in self.file_descriptors:
            raise ValueError(f"Invalid file descriptor: {fd}")
        return self.file_descriptors[fd]
    
    def send_signal(self, signal: int) -> None:
        """Send a signal to the process."""
        self.pending_signals.append(signal)
    
    def handle_signal(self, signal: int) -> None:
        """Handle a pending signal."""
        if signal in self.pending_signals:
            self.pending_signals.remove(signal)
            
            if signal in self.signal_handlers:
                handler = self.signal_handlers[signal]
                handler(signal)
    
    def set_signal_handler(self, signal: int, handler) -> None:
        """Set a signal handler."""
        self.signal_handlers[signal] = handler
    
    def update_stats(self, stat_type: str, value: int = 1) -> None:
        """Update process statistics."""
        if stat_type in self.stats:
            self.stats[stat_type] += value
    
    def get_stats(self) -> Dict[str, Any]:
        """Get process statistics."""
        return self.stats.copy()
    
    def set_priority(self, priority: int) -> None:
        """Set process priority."""
        if priority < 0 or priority > 3:
            raise ValueError(f"Invalid priority: {priority}")
        self.priority = priority
    
    def set_security_level(self, level: str) -> None:
        """Set process security level."""
        if level not in [self.SECURITY_USER, self.SECURITY_KERNEL, self.SECURITY_SUPERVISOR]:
            raise ValueError(f"Invalid security level: {level}")
        self.security_level = level
    
    def has_permission(self, permission: str) -> bool:
        """Check if process has a specific permission."""
        return self.permissions.get(permission, False)
    
    def set_permission(self, permission: str, value: bool) -> None:
        """Set a process permission."""
        self.permissions[permission] = value
    
    def add_capability(self, capability: str) -> None:
        """Add a capability to the process."""
        self.capabilities.add(capability)
    
    def remove_capability(self, capability: str) -> None:
        """Remove a capability from the process."""
        self.capabilities.discard(capability)
    
    def has_capability(self, capability: str) -> bool:
        """Check if process has a specific capability."""
        return capability in self.capabilities
    
    def set_environment_variable(self, name: str, value: str) -> None:
        """Set an environment variable."""
        self.environment[name] = value
    
    def get_environment_variable(self, name: str) -> Optional[str]:
        """Get an environment variable."""
        return self.environment.get(name)
    
    def set_working_directory(self, path: str) -> None:
        """Set the working directory."""
        self.working_directory = path
    
    def get_working_directory(self) -> str:
        """Get the working directory."""
        return self.working_directory
    
    def set_umask(self, umask: int) -> None:
        """Set the umask."""
        self.umask = umask
    
    def get_umask(self) -> int:
        """Get the umask."""
        return self.umask
    
    def set_limits(self, limits: Dict[str, int]) -> None:
        """Set process limits."""
        self.limits.update(limits)
    
    def get_limits(self) -> Dict[str, int]:
        """Get process limits."""
        return self.limits.copy()
    
    def is_alive(self) -> bool:
        """Check if process is alive."""
        return self.state not in [self.STATE_TERMINATED, self.STATE_ZOMBIE]
    
    def is_runnable(self) -> bool:
        """Check if process can be scheduled."""
        return self.state == self.STATE_READY
    
    def is_blocked(self) -> bool:
        """Check if process is blocked."""
        return self.state == self.STATE_BLOCKED
    
    def is_running(self) -> bool:
        """Check if process is running."""
        return self.state == self.STATE_RUNNING
    
    def terminate(self) -> None:
        """Terminate the process."""
        self.state = self.STATE_TERMINATED
        self.stats['end_time'] = datetime.now()
    
    def block(self) -> None:
        """Block the process."""
        self.state = self.STATE_BLOCKED
    
    def unblock(self) -> None:
        """Unblock the process."""
        self.state = self.STATE_READY
    
    def run(self) -> None:
        """Mark process as running."""
        self.state = self.STATE_RUNNING
        if self.stats['start_time'] is None:
            self.stats['start_time'] = datetime.now()
    
    def get_runtime(self) -> float:
        """Get process runtime in seconds."""
        if self.stats['start_time'] is None:
            return 0.0
        
        end_time = self.stats['end_time'] or datetime.now()
        return (end_time - self.stats['start_time']).total_seconds()
    
    def get_memory_usage(self) -> int:
        """Get current memory usage."""
        return self.memory_info['memory_usage']
    
    def get_cpu_usage(self) -> float:
        """Get CPU usage percentage."""
        if self.stats['cpu_time'] == 0:
            return 0.0
        
        runtime = self.get_runtime()
        if runtime == 0:
            return 0.0
        
        return (self.stats['cpu_time'] / runtime) * 100
    
    def __str__(self) -> str:
        """String representation."""
        return f"T3_PCB(pid={self.pid}, name='{self.name}', state={self.state})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"T3_PCB(pid={self.pid}, name='{self.name}', state={self.state}, "
                f"priority={self.priority}, memory={self.get_memory_usage()})")
