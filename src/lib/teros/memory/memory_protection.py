"""
Ternary Memory Protection - Memory protection for TEROS.

This module provides memory protection functionality for the ternary memory system,
including access control and memory isolation.
"""

from typing import Dict, List, Optional, Any, Union, Set
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class ProtectionLevel(Enum):
    """Memory protection levels."""
    NO_ACCESS = "no_access"
    READ_ONLY = "read_only"
    READ_WRITE = "read_write"
    EXECUTE = "execute"
    READ_WRITE_EXECUTE = "read_write_execute"


class SecurityLevel(Enum):
    """Security levels for memory protection."""
    USER = "user"
    KERNEL = "kernel"
    SUPERVISOR = "supervisor"


class TernaryMemoryProtection:
    """
    Memory protection system for TEROS.
    
    Provides access control and memory isolation for the ternary memory system,
    ensuring security and preventing unauthorized access.
    """
    
    def __init__(self):
        """Initialize the memory protection system."""
        self.protection_map = {}
        self.security_levels = {}
        self.access_log = []
        self.violations = []
        
        # Protection statistics
        self.stats = {
            'total_checks': 0,
            'access_granted': 0,
            'access_denied': 0,
            'violations': 0,
            'protection_updates': 0
        }
    
    def set_protection(self, address: int, size: int, protection: Union[str, ProtectionLevel],
                      security_level: Union[str, SecurityLevel] = SecurityLevel.USER) -> bool:
        """
        Set memory protection for a region.
        
        Args:
            address: Starting address
            size: Size of the region
            protection: Protection level
            security_level: Security level
            
        Returns:
            True if protection set successfully, False otherwise
        """
        if isinstance(protection, str):
            protection = ProtectionLevel(protection)
        if isinstance(security_level, str):
            security_level = SecurityLevel(security_level)
        
        # Validate parameters
        if size <= 0:
            return False
        
        # Set protection for the region
        for i in range(size):
            self.protection_map[address + i] = {
                'protection': protection,
                'security_level': security_level,
                'timestamp': time.time()
            }
        
        self.stats['protection_updates'] += 1
        return True
    
    def remove_protection(self, address: int, size: int) -> bool:
        """
        Remove memory protection from a region.
        
        Args:
            address: Starting address
            size: Size of the region
            
        Returns:
            True if protection removed successfully, False otherwise
        """
        if size <= 0:
            return False
        
        # Remove protection for the region
        for i in range(size):
            if address + i in self.protection_map:
                del self.protection_map[address + i]
        
        return True
    
    def check_access(self, address: int, size: int, access_type: str, 
                    security_level: Union[str, SecurityLevel] = SecurityLevel.USER) -> bool:
        """
        Check if access to memory region is allowed.
        
        Args:
            address: Starting address
            size: Size of the region
            access_type: Type of access ('read', 'write', 'execute')
            security_level: Security level of the requester
            
        Returns:
            True if access is allowed, False otherwise
        """
        if isinstance(security_level, str):
            security_level = SecurityLevel(security_level)
        
        self.stats['total_checks'] += 1
        
        # Check each address in the region
        for i in range(size):
            if not self._check_single_access(address + i, access_type, security_level):
                self.stats['access_denied'] += 1
                self._log_violation(address + i, access_type, security_level)
                return False
        
        self.stats['access_granted'] += 1
        self._log_access(address, size, access_type, security_level, True)
        return True
    
    def _check_single_access(self, address: int, access_type: str, 
                            security_level: SecurityLevel) -> bool:
        """Check access to a single address."""
        if address not in self.protection_map:
            # No protection set, allow access
            return True
        
        protection_info = self.protection_map[address]
        protection = protection_info['protection']
        required_security = protection_info['security_level']
        
        # Check security level
        if not self._check_security_level(security_level, required_security):
            return False
        
        # Check protection level
        if access_type == 'read':
            return protection in [ProtectionLevel.READ_ONLY, ProtectionLevel.READ_WRITE, 
                               ProtectionLevel.READ_WRITE_EXECUTE]
        elif access_type == 'write':
            return protection in [ProtectionLevel.READ_WRITE, ProtectionLevel.READ_WRITE_EXECUTE]
        elif access_type == 'execute':
            return protection in [ProtectionLevel.EXECUTE, ProtectionLevel.READ_WRITE_EXECUTE]
        else:
            return False
    
    def _check_security_level(self, requester_level: SecurityLevel, 
                             required_level: SecurityLevel) -> bool:
        """Check if requester security level is sufficient."""
        security_hierarchy = {
            SecurityLevel.USER: 0,
            SecurityLevel.KERNEL: 1,
            SecurityLevel.SUPERVISOR: 2
        }
        
        requester_rank = security_hierarchy.get(requester_level, 0)
        required_rank = security_hierarchy.get(required_level, 0)
        
        return requester_rank >= required_rank
    
    def _log_access(self, address: int, size: int, access_type: str, 
                   security_level: SecurityLevel, granted: bool) -> None:
        """Log memory access."""
        self.access_log.append({
            'address': address,
            'size': size,
            'access_type': access_type,
            'security_level': security_level.value,
            'granted': granted,
            'timestamp': time.time()
        })
        
        # Keep only last 1000 entries
        if len(self.access_log) > 1000:
            self.access_log = self.access_log[-1000:]
    
    def _log_violation(self, address: int, access_type: str, security_level: SecurityLevel) -> None:
        """Log security violation."""
        self.violations.append({
            'address': address,
            'access_type': access_type,
            'security_level': security_level.value,
            'timestamp': time.time()
        })
        
        self.stats['violations'] += 1
        
        # Keep only last 1000 violations
        if len(self.violations) > 1000:
            self.violations = self.violations[-1000:]
    
    def get_protection_info(self, address: int) -> Optional[Dict[str, Any]]:
        """Get protection information for an address."""
        return self.protection_map.get(address)
    
    def get_protected_regions(self) -> List[Dict[str, Any]]:
        """Get all protected memory regions."""
        regions = []
        current_region = None
        
        for address in sorted(self.protection_map.keys()):
            protection_info = self.protection_map[address]
            
            if (current_region is None or 
                current_region['protection'] != protection_info['protection'] or
                current_region['security_level'] != protection_info['security_level'] or
                current_region['end'] != address - 1):
                
                # Start new region
                if current_region is not None:
                    regions.append(current_region)
                
                current_region = {
                    'start': address,
                    'end': address,
                    'protection': protection_info['protection'],
                    'security_level': protection_info['security_level']
                }
            else:
                # Extend current region
                current_region['end'] = address
        
        if current_region is not None:
            regions.append(current_region)
        
        return regions
    
    def get_access_log(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get recent access log entries."""
        return self.access_log[-limit:] if limit > 0 else self.access_log.copy()
    
    def get_violations(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get recent security violations."""
        return self.violations[-limit:] if limit > 0 else self.violations.copy()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get protection statistics."""
        return self.stats.copy()
    
    def get_security_summary(self) -> Dict[str, Any]:
        """Get security summary."""
        total_checks = self.stats['total_checks']
        access_granted = self.stats['access_granted']
        access_denied = self.stats['access_denied']
        
        return {
            'total_checks': total_checks,
            'access_granted': access_granted,
            'access_denied': access_denied,
            'grant_rate': access_granted / total_checks if total_checks > 0 else 0.0,
            'deny_rate': access_denied / total_checks if total_checks > 0 else 0.0,
            'violations': self.stats['violations'],
            'protected_addresses': len(self.protection_map),
            'recent_violations': len(self.violations[-100:]) if len(self.violations) > 100 else len(self.violations)
        }
    
    def clear_protection(self) -> None:
        """Clear all memory protection."""
        self.protection_map.clear()
        self.security_levels.clear()
        self.access_log.clear()
        self.violations.clear()
        
        # Reset statistics
        self.stats = {
            'total_checks': 0,
            'access_granted': 0,
            'access_denied': 0,
            'violations': 0,
            'protection_updates': 0
        }
    
    def export_protection_map(self) -> Dict[str, Any]:
        """Export protection map for backup/restore."""
        return {
            'protection_map': self.protection_map.copy(),
            'stats': self.stats.copy(),
            'timestamp': time.time()
        }
    
    def import_protection_map(self, data: Dict[str, Any]) -> bool:
        """Import protection map from backup."""
        try:
            self.protection_map = data['protection_map']
            self.stats = data['stats']
            return True
        except KeyError:
            return False
    
    def __str__(self) -> str:
        """String representation."""
        return f"TernaryMemoryProtection(protected={len(self.protection_map)}, violations={self.stats['violations']})"
    
    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"TernaryMemoryProtection(protected={len(self.protection_map)}, "
                f"violations={self.stats['violations']}, "
                f"checks={self.stats['total_checks']})")
