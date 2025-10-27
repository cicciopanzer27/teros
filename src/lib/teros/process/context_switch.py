"""
Context Switch Manager for TEROS.

This module provides context switching functionality for the TEROS system,
including register saving/restoration and process state management.
"""

from typing import Dict, Optional, Any, List
import time
from ..core.t3_pcb import T3_PCB
from ..core.trit import Trit
from ..core.tritarray import TritArray


class ContextSwitchManager:
    """
    Manages context switching between processes.
    
    Handles saving and restoring of process state including registers,
    memory mappings, and execution context.
    """
    
    def __init__(self):
        """Initialize the context switch manager."""
        self.current_process = None
        self.previous_process = None
        self.switch_count = 0
        self.total_switch_time = 0.0
        
        # Context switch statistics
        self.stats = {
            'total_switches': 0,
            'average_switch_time': 0.0,
            'fastest_switch': float('inf'),
            'slowest_switch': 0.0,
            'voluntary_switches': 0,
            'involuntary_switches': 0
        }
    
    def save_context(self, pcb: T3_PCB) -> Dict[str, Any]:
        """
        Save the current process context.
        
        Args:
            pcb: Process Control Block to save context for
            
        Returns:
            Saved context dictionary
        """
        context = {
            'pid': pcb.pid,
            'registers': {
                'pc': pcb.registers['pc'].copy() if pcb.registers['pc'] else None,
                'sp': pcb.registers['sp'].copy() if pcb.registers['sp'] else None,
                'fp': pcb.registers['fp'].copy() if pcb.registers['fp'] else None,
                'r0': pcb.registers['r0'].copy() if pcb.registers['r0'] else None,
                'r1': pcb.registers['r1'].copy() if pcb.registers['r1'] else None,
                'r2': pcb.registers['r2'].copy() if pcb.registers['r2'] else None,
                'r3': pcb.registers['r3'].copy() if pcb.registers['r3'] else None,
                'r4': pcb.registers['r4'].copy() if pcb.registers['r4'] else None,
                'r5': pcb.registers['r5'].copy() if pcb.registers['r5'] else None,
                'r6': pcb.registers['r6'].copy() if pcb.registers['r6'] else None,
                'r7': pcb.registers['r7'].copy() if pcb.registers['r7'] else None,
                'r8': pcb.registers['r8'].copy() if pcb.registers['r8'] else None,
                'r9': pcb.registers['r9'].copy() if pcb.registers['r9'] else None,
                'r10': pcb.registers['r10'].copy() if pcb.registers['r10'] else None,
                'r11': pcb.registers['r11'].copy() if pcb.registers['r11'] else None,
                'r12': pcb.registers['r12'].copy() if pcb.registers['r12'] else None,
                'r13': pcb.registers['r13'].copy() if pcb.registers['r13'] else None,
                'r14': pcb.registers['r14'].copy() if pcb.registers['r14'] else None,
                'r15': pcb.registers['r15'].copy() if pcb.registers['r15'] else None,
            },
            'flags': {
                'zero': pcb.flags['zero'],
                'negative': pcb.flags['negative'],
                'overflow': pcb.flags['overflow'],
                'carry': pcb.flags['carry']
            },
            'memory_mappings': pcb.memory_info.copy() if pcb.memory_info else {},
            'stack_pointer': pcb.stack_pointer,
            'frame_pointer': pcb.frame_pointer,
            'program_counter': pcb.program_counter,
            'saved_time': time.time()
        }
        
        return context
    
    def restore_context(self, pcb: T3_PCB, context: Dict[str, Any]) -> None:
        """
        Restore a process context.
        
        Args:
            pcb: Process Control Block to restore context to
            context: Context dictionary to restore from
        """
        # Restore registers
        if 'registers' in context:
            for reg_name, reg_value in context['registers'].items():
                if reg_value is not None:
                    pcb.registers[reg_name] = reg_value.copy() if hasattr(reg_value, 'copy') else reg_value
        
        # Restore flags
        if 'flags' in context:
            pcb.flags.update(context['flags'])
        
        # Restore memory mappings
        if 'memory_mappings' in context:
            pcb.memory_info = context['memory_mappings'].copy()
        
        # Restore stack and frame pointers
        if 'stack_pointer' in context:
            pcb.stack_pointer = context['stack_pointer']
        if 'frame_pointer' in context:
            pcb.frame_pointer = context['frame_pointer']
        if 'program_counter' in context:
            pcb.program_counter = context['program_counter']
    
    def switch_context(self, from_pcb: Optional[T3_PCB], to_pcb: T3_PCB, 
                      voluntary: bool = True) -> float:
        """
        Perform a context switch between processes.
        
        Args:
            from_pcb: Process to switch from (None if no current process)
            to_pcb: Process to switch to
            voluntary: Whether this is a voluntary context switch
            
        Returns:
            Time taken for context switch in seconds
        """
        start_time = time.time()
        
        # Save previous process context if exists
        if from_pcb:
            saved_context = self.save_context(from_pcb)
            from_pcb.saved_context = saved_context
        
        # Restore target process context
        if to_pcb.saved_context:
            self.restore_context(to_pcb, to_pcb.saved_context)
        
        # Update process states
        if from_pcb:
            from_pcb.state = 'ready'  # or 'blocked' depending on reason
        to_pcb.state = 'running'
        
        # Update current process tracking
        self.previous_process = self.current_process
        self.current_process = to_pcb.pid
        
        # Calculate switch time
        switch_time = time.time() - start_time
        
        # Update statistics
        self._update_switch_stats(switch_time, voluntary)
        
        return switch_time
    
    def _update_switch_stats(self, switch_time: float, voluntary: bool) -> None:
        """Update context switch statistics."""
        self.stats['total_switches'] += 1
        self.total_switch_time += switch_time
        self.stats['average_switch_time'] = self.total_switch_time / self.stats['total_switches']
        
        if switch_time < self.stats['fastest_switch']:
            self.stats['fastest_switch'] = switch_time
        if switch_time > self.stats['slowest_switch']:
            self.stats['slowest_switch'] = switch_time
        
        if voluntary:
            self.stats['voluntary_switches'] += 1
        else:
            self.stats['involuntary_switches'] += 1
    
    def get_context_switch_stats(self) -> Dict[str, Any]:
        """Get context switch statistics."""
        return self.stats.copy()
    
    def reset_stats(self) -> None:
        """Reset context switch statistics."""
        self.stats = {
            'total_switches': 0,
            'average_switch_time': 0.0,
            'fastest_switch': float('inf'),
            'slowest_switch': 0.0,
            'voluntary_switches': 0,
            'involuntary_switches': 0
        }
        self.total_switch_time = 0.0
    
    def optimize_context_switch(self) -> None:
        """Optimize context switching performance."""
        # This could include:
        # - Lazy loading of process state
        # - Register windowing
        # - Fast path for common switches
        # - Cache optimization
        pass
    
    def __str__(self) -> str:
        """String representation."""
        return f"ContextSwitchManager(switches={self.stats['total_switches']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"ContextSwitchManager(switches={self.stats['total_switches']}, "
                f"avg_time={self.stats['average_switch_time']:.6f}s)")
