"""Compile and run the base-index regression suite (GNU C++20)."""
from pathlib import Path
import os, shutil, subprocess, tempfile
root = Path(__file__).resolve().parent
compiler = os.environ.get('CXX') or shutil.which('g++')
if not compiler:
    raise SystemExit('Set CXX to a GNU C++20 compiler or add g++ to PATH.')
with tempfile.TemporaryDirectory(prefix='.base-index-', dir=root) as temp:
    exe = Path(temp) / ('base_index.exe' if os.name == 'nt' else 'base_index')
    for source in ['base_index.cpp', 'lifecycle.cpp']:
        subprocess.run([compiler, '-std=gnu++20', '-O2', str(root/source), '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)
