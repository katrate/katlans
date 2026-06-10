"""
Katlans Compiler  —  C source → binary via gcc
Auto-detects GCC on Linux, macOS, and Windows (WinLibs/MinGW).
"""
import os
import shutil
import subprocess
import tempfile
from .errors import KatlansError


def _find_gcc() -> str:
    """Locate gcc on the system, checking common locations."""
    # 1. Check PATH first
    gcc = shutil.which("gcc")
    if gcc:
        return gcc
    
    # 2. Check common WinLibs/MinGW installation paths
    winlibs_paths = [
        os.path.expanduser("~/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.MCF.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe/mingw64/bin/gcc.exe"),
        os.path.expanduser("~/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe/mingw64/bin/gcc.exe"),
        os.path.expanduser("~/AppData/Local/Microsoft/WinGet/Packages/*/mingw64/bin/gcc.exe"),
    ]
    for pattern in winlibs_paths:
        import glob
        matches = glob.glob(pattern)
        if matches:
            return matches[0]
    
    # 3. Check MinGW/MSYS2 standard paths
    mingw_paths = [
        "/mingw64/bin/gcc.exe",
        "/mingw32/bin/gcc.exe",
        "C:/msys64/mingw64/bin/gcc.exe",
        "C:/MinGW/bin/gcc.exe",
        "C:/TDM-GCC-64/bin/gcc.exe",
    ]
    for path in mingw_paths:
        if os.path.exists(path):
            return path
    
    return None


class Compiler:
    def __init__(self, runtime_dir: str):
        self.runtime_dir = runtime_dir
        self.gcc_path = _find_gcc()

    def compile(self, c_source: str, out_path: str) -> tuple[bool, str]:
        """
        Write C source to a temp file, compile with gcc.
        Returns (success, error_message).
        """
        if not self.gcc_path:
            return False, (
                "No C compiler found. Install MinGW (winget install BrechtSanders.WinLibs.MCF.UCRT)\n"
                "  or add gcc to your PATH."
            )

        with tempfile.NamedTemporaryFile(suffix=".c", delete=False, mode="w") as f:
            f.write(c_source)
            c_file = f.name

        try:
            result = subprocess.run(
                [
                    self.gcc_path,
                    c_file,
                    "-o", out_path,
                    f"-I{self.runtime_dir}",
                    "-lm",
                    "-O2",
                    "-Wall",
                    "-Wno-unused-variable",
                    "-Wno-unused-value",
                    "-Wno-implicit-function-declaration",
                ] + (["-lws2_32", "-lcomctl32", "-lgdi32"] if os.name == "nt" else []),
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                return False, result.stderr
            return True, ""
        finally:
            os.unlink(c_file)

    def run(self, binary: str) -> int:
        """Run a compiled binary, return exit code."""
        result = subprocess.run([binary])
        return result.returncode



