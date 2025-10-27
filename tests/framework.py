"""
TEROS Test Framework
Provides utilities for testing kernel components and QEMU integration.
"""

import subprocess
import sys
import time
import os
from pathlib import Path
from typing import Optional, List, Tuple

class TerosTestFramework:
    """Test framework for TEROS kernel testing."""
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.bin_dir = self.project_root / "bin"
        self.build_dir = self.project_root / "build"
        
    def run_in_qemu(self, kernel_image: str, timeout: int = 10, 
                   expected_output: Optional[str] = None) -> Tuple[bool, str]:
        """
        Run kernel in QEMU and capture output.
        
        Args:
            kernel_image: Path to kernel binary
            timeout: Maximum execution time in seconds
            expected_output: String to search for in output
            
        Returns:
            Tuple of (success, output)
        """
        kernel_path = self.bin_dir / kernel_image
        
        if not kernel_path.exists():
            return False, f"Kernel image not found: {kernel_path}"
        
        try:
            # Run QEMU with serial output
            cmd = [
                'qemu-system-x86_64',
                '-kernel', str(kernel_path),
                '-serial', 'stdio',
                '-display', 'none'
            ]
            
            process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                cwd=str(self.project_root)
            )
            
            # Wait for output or timeout
            try:
                stdout, _ = process.communicate(timeout=timeout)
            except subprocess.TimeoutExpired:
                process.kill()
                stdout, _ = process.communicate()
            
            # Check for expected output
            success = True
            if expected_output:
                success = expected_output in stdout
            
            return success, stdout
            
        except FileNotFoundError:
            return False, "QEMU not found. Please install qemu-system-x86."
        except Exception as e:
            return False, f"Error running QEMU: {str(e)}"
    
    def assert_boot_message(self, output: str, expected: str) -> bool:
        """
        Verify boot message in QEMU output.
        
        Args:
            output: QEMU output
            expected: Expected boot message
            
        Returns:
            True if message found, False otherwise
        """
        return expected in output
    
    def run_unit_test(self, test_module: str) -> Tuple[bool, str]:
        """
        Run a Python unit test module.
        
        Args:
            test_module: Module path (e.g., 'tests.unit.test_trit')
            
        Returns:
            Tuple of (success, output)
        """
        try:
            result = subprocess.run(
                [sys.executable, '-m', 'pytest', f'--tb=short', test_module],
                capture_output=True,
                text=True,
                cwd=str(self.project_root)
            )
            return result.returncode == 0, result.stdout + result.stderr
        except Exception as e:
            return False, f"Error running test: {str(e)}"
    
    def build_kernel(self) -> Tuple[bool, str]:
        """
        Build the TEROS kernel.
        
        Returns:
            Tuple of (success, output)
        """
        try:
            result = subprocess.run(
                ['make', 'all'],
                capture_output=True,
                text=True,
                cwd=str(self.project_root)
            )
            return result.returncode == 0, result.stdout + result.stderr
        except Exception as e:
            return False, f"Error building kernel: {str(e)}"


def main():
    """Run test framework CLI."""
    framework = TerosTestFramework()
    
    import argparse
    parser = argparse.ArgumentParser(description='TEROS Test Framework')
    parser.add_argument('--test', help='Test module to run')
    parser.add_argument('--qemu', help='Run kernel in QEMU')
    parser.add_argument('--build', action='store_true', help='Build kernel')
    
    args = parser.parse_args()
    
    if args.test:
        success, output = framework.run_unit_test(args.test)
        print(output)
        sys.exit(0 if success else 1)
    elif args.qemu:
        success, output = framework.run_in_qemu(args.qemu)
        print(output)
        sys.exit(0 if success else 1)
    elif args.build:
        success, output = framework.build_kernel()
        print(output)
        sys.exit(0 if success else 1)
    else:
        parser.print_help()


if __name__ == '__main__':
    main()

