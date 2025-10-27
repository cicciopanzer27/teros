"""
Ternary Scheduler - Process scheduling for TEROS.

This module provides process scheduling functionality for the TEROS system,
including various scheduling policies and process management.
"""

from typing import Dict, List, Optional, Any, Union, Set
import time
import heapq
from enum import Enum
from ..core.t3_pcb import T3_PCB


class SchedulingPolicy(Enum):
    """Scheduling policies."""
    ROUND_ROBIN = "round_robin"
    PRIORITY = "priority"
    MULTILEVEL = "multilevel"
    SHORTEST_JOB_FIRST = "shortest_job_first"
    FIRST_COME_FIRST_SERVED = "first_come_first_served"


class ProcessState(Enum):
    """Process states."""
    BLOCKED = "blocked"
    READY = "ready"
    RUNNING = "running"
    ZOMBIE = "zombie"
    TERMINATED = "terminated"


class TernaryScheduler:
    """
    Process scheduler for TEROS.
    
    Manages process scheduling using various policies and provides
    efficient process switching and resource management.
    """
    
    def __init__(self, policy: SchedulingPolicy = SchedulingPolicy.ROUND_ROBIN):
        """
        Initialize the scheduler.
        
        Args:
            policy: Scheduling policy to use
        """
        self.policy = policy
        self.processes = {}
        self.ready_queue = []
        self.blocked_queue = []
        self.running_process = None
        self.next_pid = 1
        
        # Scheduling parameters
        self.time_quantum = 100  # Time quantum in milliseconds
        self.current_quantum = 0
        self.last_switch_time = 0
        
        # Statistics
        self.stats = {
            'total_processes': 0,
            'active_processes': 0,
            'context_switches': 0,
            'total_cpu_time': 0,
            'average_wait_time': 0,
            'average_turnaround_time': 0,
            'scheduling_decisions': 0
        }
        
        # Process queues by priority
        self.priority_queues = {}
        for priority in range(4):  # 0-3 priority levels
            self.priority_queues[priority] = []
    
    def create_process(self, name: str, priority: int = 1, 
                      memory_size: int = 1000) -> int:
        """
        Create a new process.
        
        Args:
            name: Process name
            priority: Process priority (0-3)
            memory_size: Memory size for the process
            
        Returns:
            Process ID
        """
        pid = self.next_pid
        self.next_pid += 1
        
        # Create process control block
        pcb = T3_PCB(pid, name, priority)
        pcb.set_priority(priority)
        pcb.state = ProcessState.READY.value
        
        # Add to process list
        self.processes[pid] = pcb
        
        # Add to ready queue
        self._add_to_ready_queue(pid)
        
        # Update statistics
        self.stats['total_processes'] += 1
        self.stats['active_processes'] += 1
        
        return pid
    
    def terminate_process(self, pid: int) -> bool:
        """
        Terminate a process.
        
        Args:
            pid: Process ID
            
        Returns:
            True if termination successful, False otherwise
        """
        if pid not in self.processes:
            return False
        
        pcb = self.processes[pid]
        pcb.state = ProcessState.TERMINATED.value
        pcb.terminate()
        
        # Remove from queues
        self._remove_from_queues(pid)
        
        # If this was the running process, schedule next
        if self.running_process == pid:
            self.running_process = None
            self._schedule_next()
        
        # Update statistics
        self.stats['active_processes'] -= 1
        
        return True
    
    def block_process(self, pid: int, reason: str = "I/O") -> bool:
        """
        Block a process.
        
        Args:
            pid: Process ID
            reason: Reason for blocking
            
        Returns:
            True if blocking successful, False otherwise
        """
        if pid not in self.processes:
            return False
        
        pcb = self.processes[pid]
        pcb.state = ProcessState.BLOCKED.value
        pcb.block()
        
        # Remove from ready queue
        self._remove_from_ready_queue(pid)
        
        # Add to blocked queue
        self.blocked_queue.append(pid)
        
        # If this was the running process, schedule next
        if self.running_process == pid:
            self.running_process = None
            self._schedule_next()
        
        return True
    
    def unblock_process(self, pid: int) -> bool:
        """
        Unblock a process.
        
        Args:
            pid: Process ID
            
        Returns:
            True if unblocking successful, False otherwise
        """
        if pid not in self.processes:
            return False
        
        pcb = self.processes[pid]
        pcb.state = ProcessState.READY.value
        pcb.unblock()
        
        # Remove from blocked queue
        if pid in self.blocked_queue:
            self.blocked_queue.remove(pid)
        
        # Add to ready queue
        self._add_to_ready_queue(pid)
        
        return True
    
    def schedule(self) -> Optional[int]:
        """
        Schedule the next process to run.
        
        Returns:
            Process ID to run, or None if no process available
        """
        if not self.ready_queue:
            return None
        
        # Select process based on scheduling policy
        if self.policy == SchedulingPolicy.ROUND_ROBIN:
            next_pid = self._round_robin_schedule()
        elif self.policy == SchedulingPolicy.PRIORITY:
            next_pid = self._priority_schedule()
        elif self.policy == SchedulingPolicy.MULTILEVEL:
            next_pid = self._multilevel_schedule()
        elif self.policy == SchedulingPolicy.SHORTEST_JOB_FIRST:
            next_pid = self._shortest_job_first_schedule()
        elif self.policy == SchedulingPolicy.FIRST_COME_FIRST_SERVED:
            next_pid = self._first_come_first_served_schedule()
        else:
            next_pid = self._round_robin_schedule()
        
        if next_pid:
            self._switch_to_process(next_pid)
            self.stats['scheduling_decisions'] += 1
        
        return next_pid
    
    def _round_robin_schedule(self) -> Optional[int]:
        """Round-robin scheduling."""
        if not self.ready_queue:
            return None
        
        # Get next process from ready queue
        next_pid = self.ready_queue.pop(0)
        self.ready_queue.append(next_pid)  # Move to end of queue
        
        return next_pid
    
    def _priority_schedule(self) -> Optional[int]:
        """Priority-based scheduling."""
        if not self.ready_queue:
            return None
        
        # Find highest priority process
        highest_priority = -1
        selected_pid = None
        
        for pid in self.ready_queue:
            pcb = self.processes[pid]
            if pcb.priority > highest_priority:
                highest_priority = pcb.priority
                selected_pid = pid
        
        if selected_pid:
            self.ready_queue.remove(selected_pid)
            self.ready_queue.append(selected_pid)
        
        return selected_pid
    
    def _multilevel_schedule(self) -> Optional[int]:
        """Multilevel scheduling."""
        # Check priority queues in order
        for priority in range(3, -1, -1):  # 3 to 0
            if priority in self.priority_queues and self.priority_queues[priority]:
                next_pid = self.priority_queues[priority].pop(0)
                return next_pid
        
        return None
    
    def _shortest_job_first_schedule(self) -> Optional[int]:
        """Shortest job first scheduling."""
        if not self.ready_queue:
            return None
        
        # Find process with shortest estimated runtime
        shortest_runtime = float('inf')
        selected_pid = None
        
        for pid in self.ready_queue:
            pcb = self.processes[pid]
            # Use priority as a proxy for job length (lower priority = longer job)
            if pcb.priority < shortest_runtime:
                shortest_runtime = pcb.priority
                selected_pid = pid
        
        if selected_pid:
            self.ready_queue.remove(selected_pid)
            self.ready_queue.append(selected_pid)
        
        return selected_pid
    
    def _first_come_first_served_schedule(self) -> Optional[int]:
        """First come first served scheduling."""
        if not self.ready_queue:
            return None
        
        # Get first process from ready queue
        next_pid = self.ready_queue.pop(0)
        self.ready_queue.append(next_pid)
        
        return next_pid
    
    def _add_to_ready_queue(self, pid: int) -> None:
        """Add process to ready queue."""
        if pid not in self.ready_queue:
            self.ready_queue.append(pid)
        
        # Also add to priority queue if using multilevel scheduling
        if self.policy == SchedulingPolicy.MULTILEVEL:
            pcb = self.processes[pid]
            priority = pcb.priority
            if priority not in self.priority_queues:
                self.priority_queues[priority] = []
            if pid not in self.priority_queues[priority]:
                self.priority_queues[priority].append(pid)
    
    def _remove_from_ready_queue(self, pid: int) -> None:
        """Remove process from ready queue."""
        if pid in self.ready_queue:
            self.ready_queue.remove(pid)
        
        # Also remove from priority queues
        for priority, queue in self.priority_queues.items():
            if pid in queue:
                queue.remove(pid)
    
    def _remove_from_queues(self, pid: int) -> None:
        """Remove process from all queues."""
        self._remove_from_ready_queue(pid)
        if pid in self.blocked_queue:
            self.blocked_queue.remove(pid)
    
    def _switch_to_process(self, pid: int) -> None:
        """Switch to a process."""
        if self.running_process:
            # Save current process state
            self._save_process_state(self.running_process)
        
        # Set new running process
        self.running_process = pid
        pcb = self.processes[pid]
        pcb.state = ProcessState.RUNNING.value
        pcb.run()
        
        # Update scheduling statistics
        self.stats['context_switches'] += 1
        self.last_switch_time = time.time()
        self.current_quantum = 0
    
    def _save_process_state(self, pid: int) -> None:
        """Save process state."""
        if pid in self.processes:
            pcb = self.processes[pid]
            # Update process statistics
            current_time = time.time()
            if pcb.stats['start_time']:
                pcb.stats['cpu_time'] += current_time - pcb.stats['start_time']
    
    def _schedule_next(self) -> None:
        """Schedule the next process."""
        next_pid = self.schedule()
        if next_pid:
            self._switch_to_process(next_pid)
    
    def tick(self) -> None:
        """Handle scheduler tick."""
        if self.running_process:
            self.current_quantum += 1
            
            # Check if time quantum expired
            if self.current_quantum >= self.time_quantum:
                # Preempt current process
                pcb = self.processes[self.running_process]
                pcb.state = ProcessState.READY.value
                self._add_to_ready_queue(self.running_process)
                self.running_process = None
                
                # Schedule next process
                self._schedule_next()
    
    def get_running_process(self) -> Optional[int]:
        """Get currently running process ID."""
        return self.running_process
    
    def get_process_info(self, pid: int) -> Optional[Dict[str, Any]]:
        """Get information about a process."""
        if pid not in self.processes:
            return None
        
        pcb = self.processes[pid]
        return {
            'pid': pid,
            'name': pcb.name,
            'state': pcb.state,
            'priority': pcb.priority,
            'cpu_time': pcb.stats['cpu_time'],
            'memory_usage': pcb.get_memory_usage(),
            'creation_time': pcb.stats['creation_time'],
            'start_time': pcb.stats['start_time']
        }
    
    def get_all_processes(self) -> List[Dict[str, Any]]:
        """Get information about all processes."""
        processes = []
        for pid in self.processes:
            info = self.get_process_info(pid)
            if info:
                processes.append(info)
        return processes
    
    def get_scheduler_stats(self) -> Dict[str, Any]:
        """Get scheduler statistics."""
        return self.stats.copy()
    
    def get_queue_lengths(self) -> Dict[str, int]:
        """Get queue lengths."""
        return {
            'ready_queue': len(self.ready_queue),
            'blocked_queue': len(self.blocked_queue),
            'priority_queues': {str(p): len(q) for p, q in self.priority_queues.items()}
        }
    
    def set_scheduling_policy(self, policy: SchedulingPolicy) -> None:
        """Set scheduling policy."""
        self.policy = policy
        
        # Reorganize queues if necessary
        if policy == SchedulingPolicy.MULTILEVEL:
            # Rebuild priority queues
            self.priority_queues = {}
            for priority in range(4):
                self.priority_queues[priority] = []
            
            for pid in self.ready_queue:
                pcb = self.processes[pid]
                priority = pcb.priority
                if priority not in self.priority_queues:
                    self.priority_queues[priority] = []
                self.priority_queues[priority].append(pid)
    
    def set_time_quantum(self, quantum: int) -> None:
        """Set time quantum."""
        self.time_quantum = quantum
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryScheduler(policy={self.policy.value}, processes={len(self.processes)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryScheduler(policy={self.policy.value}, processes={len(self.processes)}, "
                f"running={self.running_process})")
