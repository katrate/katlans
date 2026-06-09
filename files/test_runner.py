#!/usr/bin/env python3
"""
Katlans Test Runner  —  Runs all .kl test files through the compiler pipeline.

Usage:
    python test_runner.py                 # run all tests
    python test_runner.py hello.kl        # run specific test
    python test_runner.py --emit          # show generated C code
    python test_runner.py --list          # list test files
"""

import sys
import os
import subprocess

ROOT = os.path.dirname(os.path.abspath(__file__))
# In frozen (PyInstaller) builds, katlans.py becomes the .exe; the script itself is the compiler.
if getattr(sys, 'frozen', False):
    KATLANS = None  # not needed — use sys.executable directly
else:
    KATLANS = os.path.join(ROOT, "katlans.py")
FILES_DIR = ROOT  # tests are in files/ relative to project root

def color(text, code):
    return f"\033[{code}m{text}\033[0m" if sys.stdout.isatty() else text

def green(text): return color(text, "32")
def red(text):   return color(text, "31")
def yellow(text):return color(text, "33")
def blue(text):  return color(text, "36")

def find_test_files(path=None):
    """Find all .kl test files."""
    search_dir = path or ROOT
    files = []
    for f in sorted(os.listdir(search_dir)):
        if f.endswith(".kl") and f.startswith("test_"):
            files.append(os.path.join(search_dir, f))
    # Also include the basic examples
    for f in ["hello.kl", "control.kl", "functions.kl", "test_all.kl"]:
        p = os.path.join(search_dir, f)
        if os.path.exists(p):
            files.append(p)
    return sorted(set(files))

def run_test(kl_file, emit_only=False):
    """Test a .kl file through the emit pipeline."""
    name = os.path.basename(kl_file)
    if getattr(sys, 'frozen', False):
        # In frozen build, sys.executable IS katlans.exe — call it directly
        cmd = [sys.executable, "emit", kl_file]
    else:
        cmd = [sys.executable, KATLANS, "emit", kl_file]
    
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
    
    if result.returncode != 0:
        return False, result.stderr.strip()
    
    c_code = result.stdout.strip()
    if not c_code:
        return False, "Empty C output"
    
    # Check key structural elements
    checks = []
    checks.append(("Preamble (katlans.h include)", '#include' in c_code))
    checks.append(("Main function", 'int main(void)' in c_code))
    checks.append(("Return statement", 'return 0;' in c_code))
    
    # Count lines
    lines = c_code.count('\n') + 1
    
    failed_checks = [name for name, ok in checks if not ok]
    if failed_checks:
        return False, f"Missing: {', '.join(failed_checks)}"
    
    if emit_only:
        print(c_code)
        return True, ""
    
    return True, f"{lines} lines of C generated"

def main():
    import argparse
    
    p = argparse.ArgumentParser(description="Katlans Test Runner")
    p.add_argument("file", nargs="?", help="Specific test file to run")
    p.add_argument("--emit", action="store_true", help="Print generated C code")
    p.add_argument("--list", action="store_true", help="List test files")
    args = p.parse_args()
    
    if args.list:
        print("Katlans Test Files:")
        for f in find_test_files():
            print(f"  {os.path.relpath(f, ROOT)}")
        return
    
    if args.file:
        test_path = args.file if os.path.isabs(args.file) else os.path.join(ROOT, args.file)
        if not os.path.exists(test_path):
            test_path = args.file  # maybe it's a relative path from CWD
        test_files = [test_path]
    else:
        test_files = find_test_files()
    
    if not test_files:
        print(red("No test files found!"))
        sys.exit(1)
    
    passed = 0
    failed = 0
    cache = {}
    
    print(blue("\n  ========================================"))
    print(blue("  =   Katlans Test Suite                ="))
    print(blue("  ========================================\n"))
    
    for tf in test_files:
        name = os.path.basename(tf)
        print(f"  Testing {name} ... ", end="", flush=True)
        
        try:
            ok, msg = run_test(tf, emit_only=args.emit)
            cache[tf] = (ok, msg)
            if ok:
                print(green("OK"))
                print(f"    {msg}")
                passed += 1
            else:
                print(red("FAIL"))
                print(f"    {red(msg)}")
                failed += 1
        except subprocess.TimeoutExpired:
            print(red("TIMEOUT"))
            cache[tf] = (False, "Timeout")
            failed += 1
        except Exception as e:
            print(red("ERROR"))
            print(f"    {red(str(e))}")
            cache[tf] = (False, str(e))
            failed += 1
    
    print()
    print(f"  {green('Passed:')} {passed}  {red('Failed:')} {failed}  Total: {passed + failed}")
    
    # Detailed phases summary (use cached results)
    print()
    print(blue("  Phase Summary:"))
    phases = {
        "1 - Foundation":    ["hello.kl", "control.kl", "functions.kl", "test_all.kl"],
        "2 - Core":          ["test_phase2.kl"],
        "3 - Standard Lib":  ["test_phase3.kl"],
        "4 - Extended":      ["test_phase4.kl"],
        "5 - Power Modules": ["test_phase5.kl"],
    }
    for phase, file_list in phases.items():
        results = []
        for f in file_list:
            path = os.path.join(ROOT, f)
            if path in cache:
                ok, _ = cache[path]
                results.append(green("OK") if ok else red("FAIL"))
            elif os.path.exists(path):
                results.append(yellow("?"))
            else:
                results.append(yellow("-"))
        print(f"    Phase {phase}: {' '.join(results)}")
    
    sys.exit(0 if failed == 0 else 1)

if __name__ == "__main__":
    main()
