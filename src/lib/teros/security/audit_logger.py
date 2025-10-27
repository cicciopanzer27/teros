"""
Ternary Audit Logger implementation.

This module provides audit logging for TEROS,
including security events and system activities.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import threading
import json
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class AuditEventType(Enum):
    """Audit event types."""
    LOGIN = "login"
    LOGOUT = "logout"
    ACCESS_GRANTED = "access_granted"
    ACCESS_DENIED = "access_denied"
    FILE_ACCESS = "file_access"
    PROCESS_CREATE = "process_create"
    PROCESS_TERMINATE = "process_terminate"
    SYSTEM_CALL = "system_call"
    SECURITY_VIOLATION = "security_violation"
    CONFIGURATION_CHANGE = "configuration_change"


class AuditLevel(Enum):
    """Audit levels."""
    DEBUG = 0
    INFO = 1
    WARNING = 2
    ERROR = 3
    CRITICAL = 4


class TernaryAuditEvent:
    """
    Ternary Audit Event - Represents an audit event.
    
    Contains event information including type, level,
    and metadata.
    """
    
    def __init__(self, event_id: str, event_type: AuditEventType,
                 level: AuditLevel, message: str, user_id: str = None,
                 process_id: str = None, resource: str = None):
        """
        Initialize audit event.
        
        Args:
            event_id: Event identifier
            event_type: Type of event
            level: Event level
            message: Event message
            user_id: User identifier
            process_id: Process identifier
            resource: Resource identifier
        """
        self.event_id = event_id
        self.event_type = event_type
        self.level = level
        self.message = message
        self.user_id = user_id
        self.process_id = process_id
        self.resource = resource
        
        # Event metadata
        self.timestamp = time.time()
        self.source_ip = None
        self.session_id = None
        self.correlation_id = None
        
        # Ternary-specific event features
        self.ternary_features = {
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_checksum': True
        }
    
    def set_source_ip(self, ip: str) -> None:
        """Set source IP address."""
        self.source_ip = ip
    
    def set_session_id(self, session_id: str) -> None:
        """Set session identifier."""
        self.session_id = session_id
    
    def set_correlation_id(self, correlation_id: str) -> None:
        """Set correlation identifier."""
        self.correlation_id = correlation_id
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event to dictionary."""
        return {
            'event_id': self.event_id,
            'event_type': self.event_type.value,
            'level': self.level.value,
            'message': self.message,
            'user_id': self.user_id,
            'process_id': self.process_id,
            'resource': self.resource,
            'timestamp': self.timestamp,
            'source_ip': self.source_ip,
            'session_id': self.session_id,
            'correlation_id': self.correlation_id,
            'ternary_features': self.ternary_features.copy()
        }
    
    def to_json(self) -> str:
        """Convert event to JSON string."""
        return json.dumps(self.to_dict(), indent=2)
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryAuditEvent(id={self.event_id}, type={self.event_type.value}, level={self.level.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryAuditEvent(id={self.event_id}, type={self.event_type.value}, "
                f"level={self.level.value}, user={self.user_id})")


class TernaryAuditLogger:
    """
    Ternary Audit Logger - Audit logging system.
    
    Provides audit logging including event creation,
    storage, and retrieval for the TEROS system.
    """
    
    def __init__(self, max_events: int = 100000):
        """
        Initialize audit logger.
        
        Args:
            max_events: Maximum number of events to store
        """
        self.max_events = max_events
        self.events = []  # List of audit events
        self.event_index = {}  # event_id -> event
        self.next_event_id = 1
        
        # Audit statistics
        self.stats = {
            'total_events': 0,
            'events_by_type': {},
            'events_by_level': {},
            'events_by_user': {},
            'events_by_process': {},
            'events_by_resource': {},
            'events_today': 0,
            'events_this_hour': 0
        }
        
        # Ternary-specific audit features
        self.ternary_features = {
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_checksum': True,
            'ternary_aggregation': True
        }
        
        # Threading
        self.lock = threading.Lock()
        self.cleanup_thread = None
        self.running = False
        
        # Event callbacks
        self.event_callbacks = []
    
    def log_event(self, event_type: AuditEventType, level: AuditLevel,
                  message: str, user_id: str = None, process_id: str = None,
                  resource: str = None, source_ip: str = None,
                  session_id: str = None, correlation_id: str = None) -> str:
        """
        Log an audit event.
        
        Args:
            event_type: Type of event
            level: Event level
            message: Event message
            user_id: User identifier
            process_id: Process identifier
            resource: Resource identifier
            source_ip: Source IP address
            session_id: Session identifier
            correlation_id: Correlation identifier
            
        Returns:
            Event ID
        """
        with self.lock:
            event_id = str(self.next_event_id)
            self.next_event_id += 1
            
            # Create audit event
            event = TernaryAuditEvent(event_id, event_type, level, message,
                                    user_id, process_id, resource)
            
            if source_ip:
                event.set_source_ip(source_ip)
            if session_id:
                event.set_session_id(session_id)
            if correlation_id:
                event.set_correlation_id(correlation_id)
            
            # Store event
            self.events.append(event)
            self.event_index[event_id] = event
            
            # Update statistics
            self._update_stats(event)
            
            # Call event callbacks
            for callback in self.event_callbacks:
                callback(event)
            
            # Cleanup if necessary
            if len(self.events) > self.max_events:
                self._cleanup_old_events()
            
            return event_id
    
    def get_event(self, event_id: str) -> Optional[TernaryAuditEvent]:
        """
        Get audit event by ID.
        
        Args:
            event_id: Event identifier
            
        Returns:
            Audit event or None if not found
        """
        with self.lock:
            return self.event_index.get(event_id)
    
    def get_events_by_type(self, event_type: AuditEventType, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get events by type.
        
        Args:
            event_type: Event type
            limit: Maximum number of events to return
            
        Returns:
            List of audit events
        """
        with self.lock:
            events = [event for event in self.events if event.event_type == event_type]
            return events[-limit:] if limit > 0 else events
    
    def get_events_by_level(self, level: AuditLevel, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get events by level.
        
        Args:
            level: Event level
            limit: Maximum number of events to return
            
        Returns:
            List of audit events
        """
        with self.lock:
            events = [event for event in self.events if event.level == level]
            return events[-limit:] if limit > 0 else events
    
    def get_events_by_user(self, user_id: str, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get events by user.
        
        Args:
            user_id: User identifier
            limit: Maximum number of events to return
            
        Returns:
            List of audit events
        """
        with self.lock:
            events = [event for event in self.events if event.user_id == user_id]
            return events[-limit:] if limit > 0 else events
    
    def get_events_by_process(self, process_id: str, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get events by process.
        
        Args:
            process_id: Process identifier
            limit: Maximum number of events to return
            
        Returns:
            List of audit events
        """
        with self.lock:
            events = [event for event in self.events if event.process_id == process_id]
            return events[-limit:] if limit > 0 else events
    
    def get_events_by_resource(self, resource: str, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get events by resource.
        
        Args:
            resource: Resource identifier
            limit: Maximum number of events to return
            
        Returns:
            List of audit events
        """
        with self.lock:
            events = [event for event in self.events if event.resource == resource]
            return events[-limit:] if limit > 0 else events
    
    def get_recent_events(self, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Get recent events.
        
        Args:
            limit: Maximum number of events to return
            
        Returns:
            List of recent audit events
        """
        with self.lock:
            return self.events[-limit:] if limit > 0 else self.events
    
    def search_events(self, query: str, limit: int = 100) -> List[TernaryAuditEvent]:
        """
        Search events by query.
        
        Args:
            query: Search query
            limit: Maximum number of events to return
            
        Returns:
            List of matching audit events
        """
        with self.lock:
            matching_events = []
            for event in self.events:
                if (query.lower() in event.message.lower() or
                    query.lower() in event.event_type.value.lower() or
                    (event.user_id and query.lower() in event.user_id.lower()) or
                    (event.process_id and query.lower() in event.process_id.lower()) or
                    (event.resource and query.lower() in event.resource.lower())):
                    matching_events.append(event)
            
            return matching_events[-limit:] if limit > 0 else matching_events
    
    def get_audit_stats(self) -> Dict[str, Any]:
        """Get audit statistics."""
        with self.lock:
            return self.stats.copy()
    
    def add_event_callback(self, callback: Callable[[TernaryAuditEvent], None]) -> None:
        """
        Add event callback.
        
        Args:
            callback: Function to call on new events
        """
        with self.lock:
            self.event_callbacks.append(callback)
    
    def start_cleanup(self) -> None:
        """Start cleanup thread."""
        if self.cleanup_thread and self.cleanup_thread.is_alive():
            return
        
        self.running = True
        self.cleanup_thread = threading.Thread(target=self._cleanup_loop, daemon=True)
        self.cleanup_thread.start()
    
    def stop_cleanup(self) -> None:
        """Stop cleanup thread."""
        with self.lock:
            self.running = False
        
        if self.cleanup_thread:
            self.cleanup_thread.join()
    
    def _update_stats(self, event: TernaryAuditEvent) -> None:
        """Update audit statistics."""
        self.stats['total_events'] += 1
        
        # Update by type
        event_type = event.event_type.value
        if event_type not in self.stats['events_by_type']:
            self.stats['events_by_type'][event_type] = 0
        self.stats['events_by_type'][event_type] += 1
        
        # Update by level
        level = event.level.value
        if level not in self.stats['events_by_level']:
            self.stats['events_by_level'][level] = 0
        self.stats['events_by_level'][level] += 1
        
        # Update by user
        if event.user_id:
            if event.user_id not in self.stats['events_by_user']:
                self.stats['events_by_user'][event.user_id] = 0
            self.stats['events_by_user'][event.user_id] += 1
        
        # Update by process
        if event.process_id:
            if event.process_id not in self.stats['events_by_process']:
                self.stats['events_by_process'][event.process_id] = 0
            self.stats['events_by_process'][event.process_id] += 1
        
        # Update by resource
        if event.resource:
            if event.resource not in self.stats['events_by_resource']:
                self.stats['events_by_resource'][event.resource] = 0
            self.stats['events_by_resource'][event.resource] += 1
        
        # Update time-based stats
        current_time = time.time()
        today_start = time.mktime(time.localtime(current_time)[:3] + (0, 0, 0, 0, 0, 0))
        this_hour_start = time.mktime(time.localtime(current_time)[:4] + (0, 0, 0, 0, 0))
        
        if event.timestamp >= today_start:
            self.stats['events_today'] += 1
        if event.timestamp >= this_hour_start:
            self.stats['events_this_hour'] += 1
    
    def _cleanup_old_events(self) -> None:
        """Clean up old events."""
        if len(self.events) > self.max_events:
            # Remove oldest events
            events_to_remove = len(self.events) - self.max_events
            for i in range(events_to_remove):
                event = self.events.pop(0)
                if event.event_id in self.event_index:
                    del self.event_index[event.event_id]
    
    def _cleanup_loop(self) -> None:
        """Cleanup loop."""
        while self.running:
            try:
                time.sleep(3600)  # Cleanup every hour
                self._cleanup_old_events()
            except Exception as e:
                print(f"Audit cleanup error: {e}")
                break
    
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
        return f"TernaryAuditLogger(events={len(self.events)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryAuditLogger(events={len(self.events)}, "
                f"total={self.stats['total_events']})")
