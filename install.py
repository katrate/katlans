#!/usr/bin/env python3
"""
Katlans Installer  —  One-command setup for the Katlans language.

Usage:
    python install.py            # Install + verify
    python install.py --check    # Just check prerequisites
    python install.py --quick    # Skip GCC check (just pip install)

What this does:
  1. Installs the katlans package via pip
  2. Checks for a C compiler (gcc/clang/MinGW) — required to run .kl files
  3. Verifies the installation works by emitting a test file
  4. Shows you how to start writing .kl code
"""

import sys
import os
import subprocess
import shutil
import platform
import glob as glob_mod

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
PACKAGE_DIR = os.path.join(PROJECT_ROOT, "files")


def _run(cmd, desc="", timeout=60):
    """Run a command and return (success, output)."""
    try:
        r = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout, shell=isinstance(cmd, str)
        )
        ok = r.returncode == 0
        out = (r.stdout or "") + (r.stderr or "")
        return ok, out.strip()
    except FileNotFoundError:
        return False, "Command not found"
    except subprocess.TimeoutExpired:
        return False, "Timed out"


def _print_step(n, total, msg):
    print(f"\n  [{n}/{total}] {msg}")


def _ok(msg):
    print(f"    [OK] {msg}")


def _fail(msg):
    print(f"    [FAIL] {msg}")


def _warn(msg):
    print(f"    [WARN] {msg}")


def find_gcc():
    """Find a working C compiler on the system."""
    candidates = ["gcc", "clang", "x86_64-w64-mingw32-gcc", "cc"]
    for c in candidates:
        ok, _ = _run([c, "--version"])
        if ok:
            return c

    # Check common Windows paths
    win_paths = [
        os.path.expanduser(
            "~/AppData/Local/Microsoft/WinGet/Packages/"
            "*/mingw64/bin/gcc.exe"
        ),
        "/mingw64/bin/gcc.exe",
        "C:/msys64/mingw64/bin/gcc.exe",
        "C:/MinGW/bin/gcc.exe",
        "C:/TDM-GCC-64/bin/gcc.exe",
    ]
    for pattern in win_paths:
        for match in glob_mod.glob(pattern):
            if os.path.exists(match):
                return match

    return None


def try_install_gcc():
    """Attempt to install GCC on various platforms."""
    system = platform.system()

    if system == "Linux":
        # Try apt (Debian/Ubuntu)
        ok, _ = _run(["which", "apt-get"])
        if ok:
            print("    Installing gcc via apt...")
            ok, out = _run(["sudo", "apt-get", "install", "-y", "gcc"], timeout=120)
            if ok:
                return True
            _warn(f"apt install failed: {out[:200]}")

        # Try dnf (Fedora)
        ok, _ = _run(["which", "dnf"])
        if ok:
            print("    Installing gcc via dnf...")
            ok, out = _run(["sudo", "dnf", "install", "-y", "gcc"], timeout=120)
            if ok:
                return True

    elif system == "Darwin":
        # macOS - try installing command line tools
        ok, _ = _run(["xcode-select", "--install"])
        if ok:
            return True
        # Check if already installed
        ok, _ = _run(["xcode-select", "-p"])
        if ok:
            return True

    elif system == "Windows":
        # Try winget (Windows 10+)
        ok, _ = _run(["where", "winget"])
        if ok:
            print("    Installing MinGW via winget...")
            ok, out = _run(
                ["winget", "install", "BrechtSanders.WinLibs.MCF.UCRT",
                 "--accept-source-agreements", "--accept-package-agreements"],
                timeout=300
            )
            if ok:
                return True
            _warn(f"winget install: {out[:200]}")

        # Try choco
        ok, _ = _run(["where", "choco"])
        if ok:
            print("    Installing MinGW via choco...")
            ok, out = _run(["choco", "install", "mingw", "-y", "--no-progress"], timeout=300)
            if ok:
                return True

    return False


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    flags = set(a for a in sys.argv[1:] if a.startswith("-"))

    quick = "--quick" in flags
    check_only = "--check" in flags

    total_steps = 4 if not quick else 3

    print("")
    print("  =========================================")
    print("     Katlans Language Installer  v0.1.0")
    print("  =========================================")
    print(f"  Platform: {platform.system()} {platform.release()}")
    print(f"  Python:   {platform.python_version()}")
    print("")

    # ── Step 1: Check Python version ────────────────────────────────────
    _print_step(1, total_steps, "Checking Python version...")
    v = sys.version_info
    if v.major >= 3 and v.minor >= 10:
        _ok(f"Python {v.major}.{v.minor}.{v.micro}")
    else:
        _fail(f"Python 3.10+ required (have {v.major}.{v.minor})")
        sys.exit(1)

    if check_only:
        return

    # ── Step 2: pip install ─────────────────────────────────────────────
    _print_step(2, total_steps, "Installing katlans package...")
    if not os.path.exists(os.path.join(PACKAGE_DIR, "pyproject.toml")):
        _fail(f"Package not found at {PACKAGE_DIR}")
        sys.exit(1)

    ok, out = _run(
        [sys.executable, "-m", "pip", "install", "-e", PACKAGE_DIR],
        timeout=60
    )
    if ok:
        _ok("katlans installed successfully!")
    else:
        _warn(f"pip install had issues: {out[:300]}")
        _warn("You can still use it directly via: python files/katlans.py")
        # Find the last 'Successfully installed' line
        for line in out.split("\n"):
            if "Successfully installed" in line:
                _ok(line.strip())

    # ── Step 3: Check / install C compiler ──────────────────────────────
    if not quick:
        _print_step(3, total_steps, "Checking for C compiler (gcc/clang)...")
        gcc = find_gcc()
        if gcc:
            ok, out = _run([gcc, "--version"])
            version = out.split("\n")[0] if out else "unknown"
            _ok(f"Found: {gcc} ({version})")
        else:
            _warn("No C compiler found.")
            print("    Attempting to install one...")
            if try_install_gcc():
                gcc = find_gcc()
                if gcc:
                    _ok(f"Installed: {gcc}")
                else:
                    _warn("Install seemed to work but gcc not in PATH yet")
                    _warn("Try restarting your terminal, or add it to PATH manually")
            else:
                _warn("Could not auto-install a C compiler.")
                print("    To compile .kl files, install gcc manually:")
                print("      Linux:   sudo apt install gcc")
                print("      macOS:   xcode-select --install")
                print("      Windows: winget install BrechtSanders.WinLibs.MCF.UCRT")
                print("    (You can still use 'katlans emit' to see generated C code)")

    # ── Step 4: Verify ──────────────────────────────────────────────────
    _print_step(4 if not quick else 3, total_steps, "Verifying installation...")

    # Try the pip-installed command first
    katlans_cmd = None
    ok, out = _run(["katlans", "--help"])
    if ok:
        katlans_cmd = "katlans"
        _ok("'katlans' command available in PATH")
    else:
        # Try direct path
        py_path = os.path.join(PACKAGE_DIR, "katlans.py")
        if os.path.exists(py_path):
            katlans_cmd = f'python "{py_path}"'
            _ok("Using direct path to katlans.py")

    if katlans_cmd:
        # Test emit on hello.kl
        hello_path = os.path.join(PACKAGE_DIR, "hello.kl")
        if os.path.exists(hello_path):
            ok, out = _run(f'{katlans_cmd} emit "{hello_path}"')
            if ok:
                lines = len(out.strip().split("\n"))
                _ok(f"Test passed: hello.kl → {lines} lines of C")
            else:
                _warn(f"Test emit failed: {out[:200]}")

        # Run full test suite
        ok, out = _run(f'{katlans_cmd} test', timeout=90)
        if ok:
            for line in out.split("\n"):
                if "Passed:" in line or "Failed:" in line:
                    _ok(line.strip())
    else:
        _warn("Could not find katlans command. Run manually:")
        print(f'    python "{PACKAGE_DIR}/katlans.py" --help')

    # ── Done ────────────────────────────────────────────────────────────
    print("")
    print("  =========================================")
    print("     Installation complete!")
    print("  =========================================")
    print("")
    print("  To write Katlans code:")
    print("    1. Create a file ending in .kl (e.g., myprog.kl)")
    print("    2. Write Katlans code in ANY text editor (VS Code, Notepad, Vim...)")
    print("    3. Run it:")
    print('       katlans run myprog.kl        # Compile and run')
    print('       katlans emit myprog.kl       # See generated C code')
    print('       katlans build myprog.kl      # Build standalone binary')
    print("")
    print("  Examples are in the files/ directory - try:")
    print("       katlans run files/hello.kl")
    print("       katlans run files/functions.kl")
    print("")
    print("  Full docs: files/README.md")
    print("")


if __name__ == "__main__":
    main()
