"""
Ternary Security Manager implementation.

This module provides the main security management system for TEROS,
integrating access control, capabilities, and audit logging.
"""

from typing import Dict, List, Optional, Any, Union, Callable
import time
import threading
from .access_control import TernaryAccessControl, Permission, ResourceType, SecurityLevel
from .capabilities import TernaryCapabilities, CapabilityType, CapabilityScope
from .audit_logger import TernaryAuditLogger, AuditEventType, AuditLevel


class TernarySecurityManager:
    """
    Ternary Security Manager - Main security system.
    
    Integrates access control, capabilities, and audit logging
    for comprehensive security management in TEROS.
    """
    
    def __init__(self):
        """Initialize security manager."""
        # Security components
        self.access_control = TernaryAccessControl()
        self.capabilities = TernaryCapabilities()
        self.audit_logger = TernaryAuditLogger()
        
        # Security state
        self.security_enabled = True
        self.security_level = SecurityLevel.PUBLIC
        self.enforcement_mode = "strict"  # strict, permissive, disabled
        
        # Security statistics
        self.stats = {
            'total_security_checks': 0,
            'access_granted': 0,
            'access_denied': 0,
            'capability_granted': 0,
            'capability_denied': 0,
            'security_violations': 0,
            'audit_events': 0
        }
        
        # Ternary-specific security features
        self.ternary_features = {
            'ternary_authentication': True,
            'ternary_authorization': True,
            'ternary_encryption': False,
            'ternary_compression': False,
            'ternary_checksum': True,
            'ternary_audit': True
        }
        
        # Threading
        self.lock = threading.Lock()
        
        # Start audit cleanup
        self.audit_logger.start_cleanup()
    
    def enable_security(self) -> None:
        """Enable security system."""
        with self.lock:
            self.security_enabled = True
            self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                   AuditLevel.INFO, "Security system enabled")
    
    def disable_security(self) -> None:
        """Disable security system."""
        with self.lock:
            self.security_enabled = False
            self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                   AuditLevel.WARNING, "Security system disabled")
    
    def set_security_level(self, level: SecurityLevel) -> None:
        """
        Set security level.
        
        Args:
            level: Security level
        """
        with self.lock:
            self.security_level = level
            self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                   AuditLevel.INFO, f"Security level set to {level.value}")
    
    def set_enforcement_mode(self, mode: str) -> None:
        """
        Set enforcement mode.
        
        Args:
            mode: Enforcement mode (strict, permissive, disabled)
        """
        with self.lock:
            self.enforcement_mode = mode
            self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                   AuditLevel.INFO, f"Enforcement mode set to {mode}")
    
    def check_access(self, user_id: str, resource: str, permission: Permission,
                    process_id: str = None, capability_type: CapabilityType = None) -> bool:
        """
        Check if user has access to resource.
        
        Args:
            user_id: User identifier
            resource: Resource identifier
            permission: Permission to check
            process_id: Process identifier
            capability_type: Capability type
            
        Returns:
            True if access granted, False otherwise
        """
        if not self.security_enabled:
            return True
        
        with self.lock:
            self.stats['total_security_checks'] += 1
            
            # Check access control
            access_granted = self.access_control.check_access(user_id, resource, permission)
            
            # Check capabilities if specified
            if capability_type and process_id:
                capability_granted = self.capabilities.check_capability(
                    user_id, process_id, resource, capability_type, permission.value)
                
                if not capability_granted:
                    access_granted = False
            
            # Log access attempt
            if access_granted:
                self.stats['access_granted'] += 1
                self._log_security_event(AuditEventType.ACCESS_GRANTED, 
                                       AuditLevel.INFO, 
                                       f"Access granted to {resource} for {user_id}")
            else:
                self.stats['access_denied'] += 1
                self.stats['security_violations'] += 1
                self._log_security_event(AuditEventType.ACCESS_DENIED, 
                                       AuditLevel.WARNING, 
                                       f"Access denied to {resource} for {user_id}")
            
            return access_granted
    
    def grant_access(self, user_id: str, resource: str, permission: Permission) -> bool:
        """
        Grant access to user.
        
        Args:
            user_id: User identifier
            resource: Resource identifier
            permission: Permission to grant
            
        Returns:
            True if granted successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.grant_permission(user_id, resource, permission)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                          AuditLevel.INFO, 
                                          f"Access granted to {resource} for {user_id}")
            
            return success
    
    def revoke_access(self, user_id: str, resource: str, permission: Permission) -> bool:
        """
        Revoke access from user.
        
        Args:
            user_id: User identifier
            resource: Resource identifier
            permission: Permission to revoke
            
        Returns:
            True if revoked successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.revoke_permission(user_id, resource, permission)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Access revoked from {resource} for {user_id}")
            
            return success
    
    def grant_capability(self, user_id: str, process_id: str, resource: str,
                        capability_type: CapabilityType, permissions: set) -> bool:
        """
        Grant capability to user/process.
        
        Args:
            user_id: User identifier
            process_id: Process identifier
            resource: Resource identifier
            capability_type: Capability type
            permissions: Set of permissions
            
        Returns:
            True if granted successfully, False otherwise
        """
        with self.lock:
            # Create capability
            capability_id = f"{user_id}_{process_id}_{resource}_{capability_type.value}"
            success = self.capabilities.create_capability(
                capability_id, capability_type, CapabilityScope.PROCESS, permissions)
            
            if success:
                # Grant to user
                self.capabilities.grant_capability_to_user(user_id, capability_id)
                
                # Grant to process
                self.capabilities.grant_capability_to_process(process_id, capability_id)
                
                # Grant to resource
                self.capabilities.grant_capability_to_resource(resource, capability_id)
                
                self.stats['capability_granted'] += 1
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                       AuditLevel.INFO, 
                                       f"Capability granted to {user_id} for {resource}")
            
            return success
    
    def revoke_capability(self, user_id: str, process_id: str, resource: str,
                         capability_type: CapabilityType) -> bool:
        """
        Revoke capability from user/process.
        
        Args:
            user_id: User identifier
            process_id: Process identifier
            resource: Resource identifier
            capability_type: Capability type
            
        Returns:
            True if revoked successfully, False otherwise
        """
        with self.lock:
            capability_id = f"{user_id}_{process_id}_{resource}_{capability_type.value}"
            success = self.capabilities.delete_capability(capability_id)
            
            if success:
                self.stats['capability_denied'] += 1
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Capability revoked from {user_id} for {resource}")
            
            return success
    
    def create_user(self, user_id: str, username: str, password_hash: str,
                   security_level: SecurityLevel = SecurityLevel.PUBLIC) -> bool:
        """
        Create a new user.
        
        Args:
            user_id: User identifier
            username: Username
            password_hash: Password hash
            security_level: Security level
            
        Returns:
            True if created successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.create_user(user_id, username, password_hash, security_level)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"User created: {username}")
            
            return success
    
    def delete_user(self, user_id: str) -> bool:
        """
        Delete a user.
        
        Args:
            user_id: User identifier
            
        Returns:
            True if deleted successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.delete_user(user_id)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"User deleted: {user_id}")
            
            return success
    
    def create_group(self, group_id: str, group_name: str,
                    security_level: SecurityLevel = SecurityLevel.PUBLIC) -> bool:
        """
        Create a new group.
        
        Args:
            group_id: Group identifier
            group_name: Group name
            security_level: Security level
            
        Returns:
            True if created successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.create_group(group_id, group_name, security_level)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Group created: {group_name}")
            
            return success
    
    def delete_group(self, group_id: str) -> bool:
        """
        Delete a group.
        
        Args:
            group_id: Group identifier
            
        Returns:
            True if deleted successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.delete_group(group_id)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Group deleted: {group_id}")
            
            return success
    
    def create_role(self, role_id: str, role_name: str, permissions: set) -> bool:
        """
        Create a new role.
        
        Args:
            role_id: Role identifier
            role_name: Role name
            permissions: Set of permissions
            
        Returns:
            True if created successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.create_role(role_id, role_name, permissions)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Role created: {role_name}")
            
            return success
    
    def delete_role(self, role_id: str) -> bool:
        """
        Delete a role.
        
        Args:
            role_id: Role identifier
            
        Returns:
            True if deleted successfully, False otherwise
        """
        with self.lock:
            success = self.access_control.delete_role(role_id)
            
            if success:
                self._log_security_event(AuditEventType.CONFIGURATION_CHANGE, 
                                      AuditLevel.INFO, 
                                      f"Role deleted: {role_id}")
            
            return success
    
    def get_security_stats(self) -> Dict[str, Any]:
        """Get security statistics."""
        with self.lock:
            stats = self.stats.copy()
            stats.update({
                'access_control_stats': self.access_control.get_access_control_stats(),
                'capability_stats': self.capabilities.get_capability_stats(),
                'audit_stats': self.audit_logger.get_audit_stats()
            })
            return stats
    
    def get_audit_events(self, event_type: AuditEventType = None, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get audit events.
        
        Args:
            event_type: Event type filter
            limit: Maximum number of events
            
        Returns:
            List of audit events
        """
        if event_type:
            events = self.audit_logger.get_events_by_type(event_type, limit)
        else:
            events = self.audit_logger.get_recent_events(limit)
        
        return [event.to_dict() for event in events]
    
    def search_audit_events(self, query: str, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Search audit events.
        
        Args:
            query: Search query
            limit: Maximum number of events
            
        Returns:
            List of matching audit events
        """
        events = self.audit_logger.search_events(query, limit)
        return [event.to_dict() for event in events]
    
    def _log_security_event(self, event_type: AuditEventType, level: AuditLevel, message: str) -> None:
        """Log security event."""
        self.audit_logger.log_event(event_type, level, message)
        self.stats['audit_events'] += 1
    
    def set_ternary_feature(self, feature: str, enabled: bool) -> None:
        """
        Set ternary feature.
        
        Args:
            feature: Feature name
            enabled: Whether feature is enabled
        """
        with self.lock:
            self.ternary_features[feature] = enabled
            
            # Update component features
            self.access_control.set_ternary_feature(feature, enabled)
            self.capabilities.set_ternary_feature(feature, enabled)
            self.audit_logger.set_ternary_feature(feature, enabled)
    
    def get_ternary_features(self) -> Dict[str, bool]:
        """Get ternary features."""
        return self.ternary_features.copy()
    
    def cleanup_expired_capabilities(self) -> int:
        """Clean up expired capabilities."""
        return self.capabilities.cleanup_expired_capabilities()
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernarySecurityManager(enabled={self.security_enabled})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernarySecurityManager(enabled={self.security_enabled}, "
                f"level={self.security_level.value}, mode={self.enforcement_mode})")
