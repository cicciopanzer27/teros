"""
Ternary Capabilities implementation.

This module provides capability-based security for TEROS,
including capability management and enforcement.
"""

from typing import Dict, List, Optional, Any, Union, Set
import time
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class CapabilityType(Enum):
    """Capability types."""
    READ = "read"
    WRITE = "write"
    EXECUTE = "execute"
    DELETE = "delete"
    MODIFY = "modify"
    ADMIN = "admin"
    NETWORK = "network"
    DEVICE = "device"
    MEMORY = "memory"
    SYSTEM = "system"


class CapabilityScope(Enum):
    """Capability scopes."""
    GLOBAL = "global"
    LOCAL = "local"
    PROCESS = "process"
    THREAD = "thread"
    SESSION = "session"


class TernaryCapability:
    """
    Ternary Capability - Represents a capability.
    
    Contains capability information including type, scope,
    and permissions.
    """
    
    def __init__(self, capability_id: str, capability_type: CapabilityType,
                 scope: CapabilityScope, permissions: Set[str] = None):
        """
        Initialize capability.
        
        Args:
            capability_id: Capability identifier
            capability_type: Type of capability
            scope: Capability scope
            permissions: Set of permissions
        """
        self.capability_id = capability_id
        self.capability_type = capability_type
        self.scope = scope
        self.permissions = permissions or set()
        
        # Capability metadata
        self.created_time = time.time()
        self.expires_time = None
        self.owner = None
        self.description = ""
        
        # Ternary-specific capability features
        self.ternary_features = {
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False,
            'ternary_checksum': True
        }
    
    def set_expiration(self, expires_time: float) -> None:
        """
        Set capability expiration time.
        
        Args:
            expires_time: Expiration timestamp
        """
        self.expires_time = expires_time
    
    def is_expired(self) -> bool:
        """Check if capability is expired."""
        if self.expires_time is None:
            return False
        return time.time() > self.expires_time
    
    def add_permission(self, permission: str) -> None:
        """
        Add permission to capability.
        
        Args:
            permission: Permission to add
        """
        self.permissions.add(permission)
    
    def remove_permission(self, permission: str) -> None:
        """
        Remove permission from capability.
        
        Args:
            permission: Permission to remove
        """
        self.permissions.discard(permission)
    
    def has_permission(self, permission: str) -> bool:
        """
        Check if capability has permission.
        
        Args:
            permission: Permission to check
            
        Returns:
            True if has permission, False otherwise
        """
        return permission in self.permissions
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert capability to dictionary."""
        return {
            'capability_id': self.capability_id,
            'capability_type': self.capability_type.value,
            'scope': self.scope.value,
            'permissions': list(self.permissions),
            'created_time': self.created_time,
            'expires_time': self.expires_time,
            'owner': self.owner,
            'description': self.description,
            'ternary_features': self.ternary_features.copy()
        }
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryCapability(id={self.capability_id}, type={self.capability_type.value})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryCapability(id={self.capability_id}, "
                f"type={self.capability_type.value}, scope={self.scope.value})")


class TernaryCapabilities:
    """
    Ternary Capabilities - Capability management system.
    
    Provides capability-based security including capability
    creation, management, and enforcement.
    """
    
    def __init__(self):
        """Initialize capabilities system."""
        self.capabilities = {}  # capability_id -> TernaryCapability
        self.user_capabilities = {}  # user_id -> Set[capability_id]
        self.process_capabilities = {}  # process_id -> Set[capability_id]
        self.resource_capabilities = {}  # resource -> Set[capability_id]
        
        # Capability statistics
        self.stats = {
            'total_capabilities': 0,
            'active_capabilities': 0,
            'expired_capabilities': 0,
            'capability_checks': 0,
            'capability_granted': 0,
            'capability_denied': 0
        }
        
        # Ternary-specific capability features
        self.ternary_features = {
            'ternary_capabilities': True,
            'ternary_encoding': True,
            'ternary_compression': False,
            'ternary_encryption': False
        }
    
    def create_capability(self, capability_id: str, capability_type: CapabilityType,
                         scope: CapabilityScope, permissions: Set[str] = None,
                         owner: str = None, description: str = "") -> bool:
        """
        Create a new capability.
        
        Args:
            capability_id: Capability identifier
            capability_type: Type of capability
            scope: Capability scope
            permissions: Set of permissions
            owner: Capability owner
            description: Capability description
            
        Returns:
            True if created successfully, False otherwise
        """
        if capability_id in self.capabilities:
            return False
        
        capability = TernaryCapability(capability_id, capability_type, scope, permissions)
        capability.owner = owner
        capability.description = description
        
        self.capabilities[capability_id] = capability
        self.stats['total_capabilities'] += 1
        self.stats['active_capabilities'] += 1
        
        return True
    
    def delete_capability(self, capability_id: str) -> bool:
        """
        Delete a capability.
        
        Args:
            capability_id: Capability identifier
            
        Returns:
            True if deleted successfully, False otherwise
        """
        if capability_id not in self.capabilities:
            return False
        
        # Remove from all mappings
        for user_id, caps in self.user_capabilities.items():
            caps.discard(capability_id)
        
        for process_id, caps in self.process_capabilities.items():
            caps.discard(capability_id)
        
        for resource, caps in self.resource_capabilities.items():
            caps.discard(capability_id)
        
        del self.capabilities[capability_id]
        self.stats['total_capabilities'] -= 1
        self.stats['active_capabilities'] -= 1
        
        return True
    
    def grant_capability_to_user(self, user_id: str, capability_id: str) -> bool:
        """
        Grant capability to user.
        
        Args:
            user_id: User identifier
            capability_id: Capability identifier
            
        Returns:
            True if granted successfully, False otherwise
        """
        if capability_id not in self.capabilities:
            return False
        
        if user_id not in self.user_capabilities:
            self.user_capabilities[user_id] = set()
        
        self.user_capabilities[user_id].add(capability_id)
        
        return True
    
    def revoke_capability_from_user(self, user_id: str, capability_id: str) -> bool:
        """
        Revoke capability from user.
        
        Args:
            user_id: User identifier
            capability_id: Capability identifier
            
        Returns:
            True if revoked successfully, False otherwise
        """
        if user_id not in self.user_capabilities:
            return False
        
        self.user_capabilities[user_id].discard(capability_id)
        
        return True
    
    def grant_capability_to_process(self, process_id: str, capability_id: str) -> bool:
        """
        Grant capability to process.
        
        Args:
            process_id: Process identifier
            capability_id: Capability identifier
            
        Returns:
            True if granted successfully, False otherwise
        """
        if capability_id not in self.capabilities:
            return False
        
        if process_id not in self.process_capabilities:
            self.process_capabilities[process_id] = set()
        
        self.process_capabilities[process_id].add(capability_id)
        
        return True
    
    def revoke_capability_from_process(self, process_id: str, capability_id: str) -> bool:
        """
        Revoke capability from process.
        
        Args:
            process_id: Process identifier
            capability_id: Capability identifier
            
        Returns:
            True if revoked successfully, False otherwise
        """
        if process_id not in self.process_capabilities:
            return False
        
        self.process_capabilities[process_id].discard(capability_id)
        
        return True
    
    def grant_capability_to_resource(self, resource: str, capability_id: str) -> bool:
        """
        Grant capability to resource.
        
        Args:
            resource: Resource identifier
            capability_id: Capability identifier
            
        Returns:
            True if granted successfully, False otherwise
        """
        if capability_id not in self.capabilities:
            return False
        
        if resource not in self.resource_capabilities:
            self.resource_capabilities[resource] = set()
        
        self.resource_capabilities[resource].add(capability_id)
        
        return True
    
    def revoke_capability_from_resource(self, resource: str, capability_id: str) -> bool:
        """
        Revoke capability from resource.
        
        Args:
            resource: Resource identifier
            capability_id: Capability identifier
            
        Returns:
            True if revoked successfully, False otherwise
        """
        if resource not in self.resource_capabilities:
            return False
        
        self.resource_capabilities[resource].discard(capability_id)
        
        return True
    
    def check_capability(self, user_id: str, process_id: str, resource: str,
                        capability_type: CapabilityType, permission: str) -> bool:
        """
        Check if user/process has capability for resource.
        
        Args:
            user_id: User identifier
            process_id: Process identifier
            resource: Resource identifier
            capability_type: Type of capability
            permission: Permission to check
            
        Returns:
            True if capability granted, False otherwise
        """
        self.stats['capability_checks'] += 1
        
        # Check user capabilities
        if user_id in self.user_capabilities:
            for capability_id in self.user_capabilities[user_id]:
                if self._check_capability_access(capability_id, capability_type, permission):
                    self.stats['capability_granted'] += 1
                    return True
        
        # Check process capabilities
        if process_id in self.process_capabilities:
            for capability_id in self.process_capabilities[process_id]:
                if self._check_capability_access(capability_id, capability_type, permission):
                    self.stats['capability_granted'] += 1
                    return True
        
        # Check resource capabilities
        if resource in self.resource_capabilities:
            for capability_id in self.resource_capabilities[resource]:
                if self._check_capability_access(capability_id, capability_type, permission):
                    self.stats['capability_granted'] += 1
                    return True
        
        self.stats['capability_denied'] += 1
        return False
    
    def _check_capability_access(self, capability_id: str, capability_type: CapabilityType,
                                permission: str) -> bool:
        """Check if capability grants access."""
        if capability_id not in self.capabilities:
            return False
        
        capability = self.capabilities[capability_id]
        
        # Check if capability is expired
        if capability.is_expired():
            self.stats['expired_capabilities'] += 1
            return False
        
        # Check capability type
        if capability.capability_type != capability_type:
            return False
        
        # Check permission
        return capability.has_permission(permission)
    
    def get_user_capabilities(self, user_id: str) -> List[Dict[str, Any]]:
        """
        Get user capabilities.
        
        Args:
            user_id: User identifier
            
        Returns:
            List of user capabilities
        """
        if user_id not in self.user_capabilities:
            return []
        
        capabilities = []
        for capability_id in self.user_capabilities[user_id]:
            if capability_id in self.capabilities:
                capability = self.capabilities[capability_id]
                if not capability.is_expired():
                    capabilities.append(capability.to_dict())
        
        return capabilities
    
    def get_process_capabilities(self, process_id: str) -> List[Dict[str, Any]]:
        """
        Get process capabilities.
        
        Args:
            process_id: Process identifier
            
        Returns:
            List of process capabilities
        """
        if process_id not in self.process_capabilities:
            return []
        
        capabilities = []
        for capability_id in self.process_capabilities[process_id]:
            if capability_id in self.capabilities:
                capability = self.capabilities[capability_id]
                if not capability.is_expired():
                    capabilities.append(capability.to_dict())
        
        return capabilities
    
    def get_resource_capabilities(self, resource: str) -> List[Dict[str, Any]]:
        """
        Get resource capabilities.
        
        Args:
            resource: Resource identifier
            
        Returns:
            List of resource capabilities
        """
        if resource not in self.resource_capabilities:
            return []
        
        capabilities = []
        for capability_id in self.resource_capabilities[resource]:
            if capability_id in self.capabilities:
                capability = self.capabilities[capability_id]
                if not capability.is_expired():
                    capabilities.append(capability.to_dict())
        
        return capabilities
    
    def cleanup_expired_capabilities(self) -> int:
        """
        Clean up expired capabilities.
        
        Returns:
            Number of capabilities cleaned up
        """
        cleaned = 0
        expired_capabilities = []
        
        for capability_id, capability in self.capabilities.items():
            if capability.is_expired():
                expired_capabilities.append(capability_id)
        
        for capability_id in expired_capabilities:
            if self.delete_capability(capability_id):
                cleaned += 1
        
        return cleaned
    
    def get_capability_stats(self) -> Dict[str, Any]:
        """Get capability statistics."""
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
        return f"TernaryCapabilities(capabilities={len(self.capabilities)})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryCapabilities(capabilities={len(self.capabilities)}, "
                f"active={self.stats['active_capabilities']})")
