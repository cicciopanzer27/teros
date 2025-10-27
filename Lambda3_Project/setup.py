#!/usr/bin/env python3
"""
Lambda³ Project Setup
"""

from setuptools import setup, find_packages
import os

def read_requirements():
    """Read requirements from requirements.txt"""
    requirements_path = os.path.join(os.path.dirname(__file__), 'requirements.txt')
    if os.path.exists(requirements_path):
        with open(requirements_path, 'r', encoding='utf-8') as f:
            return [
                line.strip() 
                for line in f 
                if line.strip() and not line.startswith('#') and not line.startswith('python')
            ]
    return []

def read_readme():
    """Read README.md"""
    readme_path = os.path.join(os.path.dirname(__file__), 'README.md')
    if os.path.exists(readme_path):
        with open(readme_path, 'r', encoding='utf-8') as f:
            return f.read()
    return "Lambda³ - Hybrid Neural-Symbolic AI on Ternary Substrate"

setup(
    name="lambda3",
    version="0.1.0",
    author="Lambda³ Project Team",
    author_email="team@lambda3.ai",
    description="First Hybrid Neural-Symbolic AI on Ternary Substrate",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    url="https://github.com/lambda3-project/lambda3",
    project_urls={
        "Bug Reports": "https://github.com/lambda3-project/lambda3/issues",
        "Source": "https://github.com/lambda3-project/lambda3",
        "Documentation": "https://lambda3.ai/docs",
    },
    packages=find_packages(exclude=["tests", "tests.*", "examples", "examples.*"]),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Compilers",
    ],
    python_requires=">=3.11",
    install_requires=read_requirements(),
    extras_require={
        "dev": [
            "pytest>=7.4.0",
            "pytest-cov>=4.1.0",
            "black>=23.7.0",
            "flake8>=6.0.0",
            "mypy>=1.5.0",
        ],
        "docs": [
            "sphinx>=7.0.0",
            "sphinx-rtd-theme>=1.2.0",
            "mkdocs>=1.5.0",
            "mkdocs-material>=9.0.0",
        ],
        "gpu": [
            "torch[cuda]>=2.0.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "lambda3=lambda3.cli:main",
            "lambda3-repl=lambda3.repl:main",
            "lambda3-prove=lambda3.proof.cli:main",
        ],
    },
    include_package_data=True,
    zip_safe=False,
)

