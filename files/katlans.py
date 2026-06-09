#!/usr/bin/env python3
"""
katlans  —  The Katlans Language CLI

Usage:
  katlans run   <file.kl>               compile & run
  katlans build <file.kl> [-o out]      compile to binary
  katlans emit  <file.kl>               print generated C
  katlans test  [file.kl]               run test suite
  katlans tokens <file.kl>              print token stream (debug)
  katlans ast    <file.kl>              print AST (debug)
"""

import sys
import os
import argparse
import tempfile
import subprocess
import urllib.request
import zipfile
import shutil


# ── Path setup ──────────────────────────────────────────────────────────────
ROOT        = os.path.dirname(os.path.abspath(__file__))
SRC_DIR     = os.path.join(ROOT, "src")
RUNTIME_DIR = os.path.join(ROOT, "runtime")
RUNTIME_PATH = os.path.join(RUNTIME_DIR, "katlans.h")

sys.path.insert(0, ROOT)
sys.path.insert(0, SRC_DIR)
# pyrefly: ignore [missing-import]
from src import Lexer, Parser, CodeGen, Compiler, KatlansError


# ── Terminal colours (ASCII only for Windows cp1252 compat) ─────────────────
def _col(text: str, code: str) -> str:
    return f"\033[{code}m{text}\033[0m" if sys.stdout.isatty() else text

def _red(s):    return _col(s, "31")
def _green(s):  return _col(s, "32")
def _yellow(s): return _col(s, "33")
def _blue(s):   return _col(s, "36")
def _bold(s):   return _col(s, "1")


# ── Helpers ─────────────────────────────────────────────────────────────────
def _read(path: str) -> str:
    if not os.path.exists(path):
        print(f"{_red('[Error]')} File not found: {path}")
        sys.exit(1)
    if not path.endswith(".kl"):
        print(f"{_yellow('[Warning]')} File does not have .kl extension: {path}")
    with open(path, "r", encoding="utf-8") as f:
        return f.read()


def _pipeline(source: str, filename: str) -> str:
    """source -> C source string"""
    lexer  = Lexer(source, filename)
    tokens = lexer.tokenise()
    parser = Parser(tokens, filename)
    ast    = parser.parse()
    gen    = CodeGen(RUNTIME_PATH, filename)
    c_src  = gen.generate(ast)
    return c_src


def _find_compiler() -> str:
    """Locate a C compiler on the system (checks PATH and known locations)."""
    candidates = ["gcc", "clang", "x86_64-w64-mingw32-gcc", "cc"]
    for c in candidates:
        try:
            subprocess.run([c, "--version"], capture_output=True, text=True, timeout=5)
            return c
        except (FileNotFoundError, subprocess.TimeoutExpired):
            continue
    # Check WinLibs cached location
    cached = os.path.join(os.path.expanduser("~"), ".katlans", "mingw64", "bin", "gcc.exe")
    if os.path.exists(cached):
        os.environ["PATH"] = os.path.dirname(cached) + os.pathsep + os.environ.get("PATH", "")
        return cached
    return None


def _download_progress(count, block, total):
    """Progress callback for urllib."""
    if total > 0:
        pct = min(100, count * block * 100 // total)
        bar = '#' * (pct // 5) + '-' * (20 - pct // 5)
        sys.stdout.write(f"\r    [{bar}] {pct}%")
        sys.stdout.flush()


def _find_7z() -> str:
    """Find 7-Zip executable on the system."""
    candidates = ["7z", "7za"]
    for c in candidates:
        exe = shutil.which(c)
        if exe:
            return exe
    # Check common install paths on Windows
    win_paths = [
        "C:/Program Files/7-Zip/7z.exe",
        os.path.expanduser("~/AppData/Local/7-Zip/7z.exe"),
        os.path.expanduser("~/AppData/Local/Programs/7-Zip/7z.exe"),
    ]
    for p in win_paths:
        if os.path.exists(p):
            return p
    return None


def _download_mingw() -> str:
    """Download MinGW-w64 (WinLibs UCRT) to ~/.katlans/mingw64/ and set it up.
    Returns the path to gcc.exe or None on failure."""
    cache_root = os.path.join(os.path.expanduser("~"), ".katlans")
    extract_to = cache_root
    gcc_exe = os.path.join(cache_root, "mingw64", "bin", "gcc.exe")

    # Check if already cached
    if os.path.exists(gcc_exe):
        os.environ["PATH"] = os.path.dirname(gcc_exe) + os.pathsep + os.environ.get("PATH", "")
        return gcc_exe

    os.makedirs(cache_root, exist_ok=True)

    # Find 7z first (for .7z extraction)
    seven_z = _find_7z()

    # Determine which URL format to try (prefer .zip, fall back to .7z)
    base_url = ("https://github.com/brechtsanders/winlibs_mingw/releases/download/"
                "16.1.0posix-14.0.0-ucrt-r2/"
                "winlibs-x86_64-posix-seh-gcc-16.1.0-mingw-w64ucrt-14.0.0-r2")

    # Prompt user before downloading ~100 MB (skip prompt if non-interactive)
    if sys.stdin.isatty():
        sys.stdout.write(f"  No C compiler found. Download MinGW-w64 (~100 MB)? [Y/n] ")
        sys.stdout.flush()
        reply = sys.stdin.readline().strip().lower()
        if reply and reply != 'y' and reply != 'yes' and reply != '':
            print(f"  {_yellow('[i]')} Skipped. Install manually: winget install BrechtSanders.WinLibs.MCF.UCRT")
            return None

    print(f"  {_yellow('[i]')} Downloading MinGW-w64 (one-time, cached at {cache_root})...")

    # Try .zip first (extractable with Python stdlib)
    zip_path = os.path.join(cache_root, "mingw_download.zip")
    try:
        print(f"  Trying .zip...")
        urllib.request.urlretrieve(base_url + ".zip", zip_path, _download_progress)
        print()
        ext = ".zip"
    except Exception as e:
        # Clean up partial download
        if os.path.exists(zip_path):
            os.remove(zip_path)
        print(f"\n    .zip not available: {e}")
        if not seven_z:
            print(f"  {_red('[Error]')} MinGW only available as .7z, but 7-Zip not found.")
            print(f"  Install 7-Zip from https://7-zip.org/ or install MinGW via winget.")
            return None
        # Fall back to .7z
        zip_path = os.path.join(cache_root, "mingw_download.7z")
        print(f"  Trying .7z...")
        try:
            urllib.request.urlretrieve(base_url + ".7z", zip_path, _download_progress)
            print()
            ext = ".7z"
        except Exception as e2:
            if os.path.exists(zip_path):
                os.remove(zip_path)
            print(f"\n  {_red('[Error]')} Could not download MinGW: {e2}")
            return None

    # Extract
    print(f"  Extracting...", end="", flush=True)
    try:
        if ext == ".zip":
            with zipfile.ZipFile(zip_path, 'r') as zf:
                zf.extractall(extract_to)
        else:  # .7z
            subprocess.run([seven_z, "x", zip_path, f"-o{extract_to}", "-y"],
                          capture_output=True, timeout=120)
        print(" OK")
    except Exception as e:
        print(f"\n  {_red('[Error]')} Extraction failed: {e}")
        return None
    finally:
        if os.path.exists(zip_path):
            os.remove(zip_path)

    # Verify
    if os.path.exists(gcc_exe):
        os.environ["PATH"] = os.path.dirname(gcc_exe) + os.pathsep + os.environ.get("PATH", "")
        try:
            r = subprocess.run([gcc_exe, "--version"], capture_output=True, text=True, timeout=5)
            ver = r.stdout.split('\n')[0] if r.stdout else ""
            print(f"  {_green('[OK]')} MinGW-w64 installed: {ver[:60]}")
        except:
            print(f"  {_green('[OK]')} MinGW-w64 installed at {os.path.dirname(gcc_exe)}")
        return gcc_exe

    # Fallback: walk and find gcc
    for root, dirs, files in os.walk(cache_root):
        for f in files:
            if f in ("gcc.exe", "gcc"):
                full = os.path.join(root, f)
                os.environ["PATH"] = root + os.pathsep + os.environ.get("PATH", "")
                return full

    print(f"  {_red('[Error]')} gcc.exe not found after extraction.")
    return None


def _ensure_compiler() -> str:
    """Find or auto-download a C compiler.
    Returns the path/name of a working C compiler, or None if impossible."""
    # 1. Check if one is already available
    cc = _find_compiler()
    if cc:
        return cc

    # 2. On Windows, try to download MinGW automatically
    if sys.platform == "win32" or os.name == "nt":
        cc = _download_mingw()
        if cc:
            return cc
        print(f"  {_yellow('[i]')} Could not auto-install MinGW.")
        print(f"  Install manually: winget install BrechtSanders.WinLibs.MCF.UCRT")
        return None

    # 3. On macOS / Linux, give instructions
    if sys.platform == "darwin":
        print(f"  {_yellow('[i]')} Install GCC: xcode-select --install")
    else:
        print(f"  {_yellow('[i]')} Install GCC: sudo apt install gcc  (or your distro's equivalent)")
    return None


# ── Commands ─────────────────────────────────────────────────────────────────
def cmd_run(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)

    try:
        c_src = _pipeline(source, filename)
    except KatlansError as e:
        print(e); sys.exit(1)

    # Auto-detect or download C compiler
    cc_path = _ensure_compiler()
    if not cc_path:
        sys.exit(1)

    comp = Compiler(RUNTIME_DIR)
    suffix = ".exe" if os.name == "nt" else ""
    with tempfile.NamedTemporaryFile(suffix=suffix, delete=False) as f:
        binary = f.name

    ok, err = comp.compile(c_src, binary)
    if not ok:
        print(f"{_red('[Compile Error]')}\n{err}")
        sys.exit(1)

    os.chmod(binary, 0o755)
    code = comp.run(binary)
    os.unlink(binary)
    sys.exit(code)


def cmd_build(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    out      = args.output or filename.replace(".kl", ".exe" if os.name == "nt" else "")

    try:
        c_src = _pipeline(source, filename)
    except KatlansError as e:
        print(e); sys.exit(1)

    cc_path = _ensure_compiler()
    if not cc_path:
        sys.exit(1)

    comp = Compiler(RUNTIME_DIR)
    ok, err = comp.compile(c_src, out)
    if not ok:
        print(f"{_red('[Compile Error]')}\n{err}")
        sys.exit(1)

    print(f"{_green('OK Built:')} {out}")


def cmd_emit(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    try:
        c_src = _pipeline(source, filename)
        # Always try stdout first - works for pipes, subprocess capture, etc.
        # On Windows cp1252 terminals, also write .c file as fallback
        if sys.stdout.isatty() and sys.stdout.encoding and sys.stdout.encoding.lower() in ("cp1252", "cp1254"):
            c_file = args.file.replace(".kl", ".c")
            with open(c_file, "w", encoding="utf-8") as f:
                f.write(c_src)
            print(f"{_green('OK Emitted:')} {c_file}")
        else:
            print(c_src)
    except KatlansError as e:
        print(e); sys.exit(1)


def cmd_tokens(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    try:
        lexer  = Lexer(source, filename)
        tokens = lexer.tokenise()
        for tok in tokens:
            print(tok)
    except KatlansError as e:
        print(e); sys.exit(1)


def cmd_ast(args):
    import pprint
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    try:
        lexer  = Lexer(source, filename)
        tokens = lexer.tokenise()
        parser = Parser(tokens, filename)
        ast    = parser.parse()
        pprint.pprint(ast)
    except KatlansError as e:
        print(e); sys.exit(1)


def cmd_test(args):
    """Run the test suite."""
    runner = os.path.join(ROOT, "test_runner.py")
    if not os.path.exists(runner):
        print(f"{_red('[Error]')} Test runner not found at {runner}")
        sys.exit(1)

    # In frozen (PyInstaller) builds, sys.executable is the .exe, not python.
    # Import and call the test runner directly instead of using subprocess.
    if getattr(sys, 'frozen', False):
        import importlib.util
        spec = importlib.util.spec_from_file_location("test_runner", runner)
        tr = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(tr)
        # Override sys.argv so test_runner's own argparse works correctly
        test_argv = [runner]
        if args.emit:
            test_argv.append("--emit")
        if args.list_only:
            test_argv.append("--list")
        if args.file:
            test_argv.append(args.file)
        sys.argv = test_argv
        try:
            tr.main()
        except SystemExit as e:
            sys.exit(e.code)
    else:
        cmd = [sys.executable, runner]
        if args.file:
            cmd.append(args.file)
        if args.emit:
            cmd.append("--emit")
        if args.list_only:
            cmd.append("--list")
        result = subprocess.run(cmd)
        sys.exit(result.returncode)


# ── Version & update URLs ────────────────────────────────────────────────
KATLANS_VERSION = "1.0"

# GitHub repo for updates (override with env var KATLANS_REPO)
GITHUB_REPO = os.environ.get("KATLANS_REPO", "katlans/katlans")
GITHUB_API = f"https://api.github.com/repos/{GITHUB_REPO}/releases/latest"


def _get_exe_dir() -> str:
    """Get the directory containing this executable (for PATH setup)."""
    if getattr(sys, 'frozen', False):
        return os.path.dirname(os.path.abspath(sys.executable))
    return os.path.dirname(os.path.abspath(__file__))


def _add_to_path_permanent() -> bool:
    """Add the exe directory to the user's permanent PATH.
    Returns True on success."""
    exe_dir = _get_exe_dir()

    # Check if already in PATH
    current_path = os.environ.get("PATH", "")
    if exe_dir.lower() in [p.strip().lower() for p in current_path.split(os.pathsep)]:
        return True  # already there

    try:
        if sys.platform == "win32" or os.name == "nt":
            # Use setx to add to user PATH permanently
            new_path = f"{current_path}{os.pathsep}{exe_dir}" if current_path else exe_dir
            subprocess.run(["setx", "PATH", new_path], capture_output=True, timeout=10)
            return True
        else:
            # Linux/macOS: add to shell profile (check for duplicates)
            profile = os.path.expanduser("~/.bashrc")
            if os.path.exists(os.path.expanduser("~/.zshrc")):
                profile = os.path.expanduser("~/.zshrc")
            export_line = f'export PATH="{exe_dir}:$PATH"'
            if os.path.exists(profile):
                with open(profile) as f:
                    if exe_dir in f.read():
                        return True  # already present
            with open(profile, "a") as f:
                f.write(f"\n# Added by Katlans setup\n{export_line}\n")
            return True
    except Exception:
        return False


def cmd_update(args):
    """Self-update: download latest katlans.exe from GitHub releases."""
    del args  # unused

    if not getattr(sys, 'frozen', False):
        print(f"  {_red('[Error]')} Self-update only works with the katlans.exe binary.")
        print(f"  Update manually: git pull && python -m PyInstaller ...")
        sys.exit(1)

    print(f"\n  {_bold('Katlans Update')}")
    print(f"  {'='*40}\n")

    print(f"  Current version: v{KATLANS_VERSION}")
    print(f"  Checking {GITHUB_REPO} for updates...")

    try:
        # Fetch latest release info from GitHub API
        req = urllib.request.Request(GITHUB_API, headers={"User-Agent": "katlans"})
        resp = urllib.request.urlopen(req, timeout=15)
        data = resp.read().decode("utf-8")

        # Parse JSON manually (avoid needing json module in frozen builds... actually json is fine)
        import json
        release = json.loads(data)
        latest_tag = release.get("tag_name", "") or release.get("name", "unknown")
        print(f"  Latest version:  {latest_tag}")

        # Check if already up-to-date
        current_tag = f"v{KATLANS_VERSION}"
        if latest_tag == current_tag or latest_tag == KATLANS_VERSION:
            print(f"  {_green('[OK]')} Already up-to-date!")
            return

        # Find the katlans.exe asset
        assets = release.get("assets", [])
        download_url = None
        for asset in assets:
            name = asset.get("name", "").lower()
            if "katlans" in name and name.endswith(".exe"):
                download_url = asset.get("browser_download_url")
                break

        if not download_url:
            print(f"  {_red('[Error]')} No katlans.exe found in latest release.")
            print(f"  Visit: https://github.com/{GITHUB_REPO}/releases/latest")
            sys.exit(1)

        print(f"  Downloading {asset.get('name', 'katlans.exe')}...")

        exe_path = os.path.abspath(sys.executable)
        exe_dir = os.path.dirname(exe_path)
        backup_path = exe_path + ".old"
        new_path = os.path.join(exe_dir, "katlans.exe.new")

        # Download to .new file
        urllib.request.urlretrieve(download_url, new_path, _download_progress)
        print()

        # Windows: rename running exe, then move new into place
        if os.name == "nt":
            # On Windows, we can rename the running exe
            os.replace(exe_path, backup_path)
            os.replace(new_path, exe_path)
            # Clean up the old backup
            try:
                os.remove(backup_path)
            except:
                pass  # will be cleaned on next reboot
            print(f"  {_green('[OK]')} Updated to {latest_tag}!")
            print(f"  Restart your terminal or run: {exe_path} --help")
        else:
            # Linux/macOS: same approach works
            os.replace(exe_path, backup_path)
            os.replace(new_path, exe_path)
            os.remove(backup_path)
            print(f"  {_green('[OK]')} Updated to {latest_tag}!")

    except Exception as e:
        print(f"\n  {_red('[Error]')} Update failed: {e}")
        print(f"  Manual update: https://github.com/{GITHUB_REPO}/releases/latest")
        sys.exit(1)


def cmd_setup(args):
    """One-time setup: install compiler, add to PATH, verify."""
    del args  # unused
    print(f"\n  {_bold('Katlans Setup')} v{KATLANS_VERSION}")
    print(f"  {'='*40}\n")

    # Step 1: Check Python
    print(f"  [1/4] Checking Python...")
    v = sys.version_info
    if v.major >= 3 and v.minor >= 10:
        print(f"    {_green('OK')} Python {v.major}.{v.minor}.{v.micro}")
    else:
        print(f"    {_red('FAIL')} Python 3.10+ required (have {v.major}.{v.minor})")
        sys.exit(1)

    # Step 2: Download/check C compiler
    print(f"  [2/4] Setting up C compiler...")
    cc = _ensure_compiler()
    if cc:
        try:
            r = subprocess.run([cc, "--version"], capture_output=True, text=True, timeout=5)
            ver = r.stdout.split('\n')[0][:60] if r.stdout else str(cc)
            print(f"    {_green('OK')} {ver}")
        except:
            print(f"    {_green('OK')} {cc}")
    else:
        print(f"    {_yellow('WARN')} No C compiler (use 'katlans emit' for C output only)")

    # Step 3: Add to PATH (so `katlans` works without `.exe`)
    print(f"  [3/4] Adding katlans to PATH...")
    exe_dir = _get_exe_dir()
    if _add_to_path_permanent():
        print(f"    {_green('OK')} {exe_dir} added to PATH")
        print(f"    {_yellow('[i]')} Open a NEW terminal, then try: katlans")
        if exe_dir not in os.environ.get("PATH", ""):
            os.environ["PATH"] = exe_dir + os.pathsep + os.environ.get("PATH", "")
    else:
        print(f"    {_yellow('WARN')} Could not auto-add to PATH")
        print(f"    Add this directory to PATH manually: {exe_dir}")

    # Step 4: Run tests  (use a dummy args object)
    print(f"  [4/4] Verifying installation...")
    class FakeArgs:
        file = None
        emit = False
        list_only = False
    cmd_test(FakeArgs())


# ── Banner (ASCII only - cp1252 safe) ───────────────────────────────────────
BANNER = f"""
  {_bold('Katlans')} Programming Language  v{KATLANS_VERSION}

  [v] All 5 phases implemented
  [i] ~300 built-in functions
  [i] Compilation: .kl -> C -> binary
"""


def main():
    p = argparse.ArgumentParser(
        prog="katlans",
        description="The Katlans Programming Language Compiler",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  katlans run   hello.kl              Compile and run
  katlans emit  hello.kl              Show generated C code
  katlans setup                       One-time setup (compiler + PATH)
  katlans update                      Self-update from GitHub
  katlans test                        Run all tests
        """
    )
    sub = p.add_subparsers(dest="cmd")

    r = sub.add_parser("run",    help="Compile and run a .kl file")
    r.add_argument("file")

    b = sub.add_parser("build",  help="Compile a .kl file to a binary")
    b.add_argument("file")
    b.add_argument("-o", "--output", default=None)

    e = sub.add_parser("emit",   help="Print generated C code")
    e.add_argument("file")

    t = sub.add_parser("test",   help="Run test suite")
    t.add_argument("file", nargs="?", default=None)
    t.add_argument("--emit", action="store_true", help="Print generated C")
    t.add_argument("--list", dest="list_only", action="store_true", help="List tests")

    s = sub.add_parser("setup",  help="One-time setup (install compiler, add to PATH)")
    u = sub.add_parser("update", help="Self-update from GitHub releases")

    tk = sub.add_parser("tokens", help="Print token stream (debug)")
    tk.add_argument("file")

    a = sub.add_parser("ast",    help="Print AST (debug)")
    a.add_argument("file")

    args = p.parse_args()

    commands = {
        "run":    cmd_run,
        "build":  cmd_build,
        "emit":   cmd_emit,
        "test":   cmd_test,
        "tokens": cmd_tokens,
        "ast":    cmd_ast,
        "setup":  cmd_setup,
        "update": cmd_update,
    }

    if args.cmd in commands:
        commands[args.cmd](args)
    else:
        print(BANNER)
        p.print_help()


if __name__ == "__main__":
    main()
