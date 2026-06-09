"""
Katlans Compiler  —  C source → binary via gcc
"""
import os
import subprocess
import tempfile
from .errors import KatlansError


class Compiler:
    def __init__(self, runtime_dir: str):
        self.runtime_dir = runtime_dir

    def compile(self, c_source: str, out_path: str) -> tuple[bool, str]:
        """
        Write C source to a temp file, compile with gcc.
        Returns (success, error_message).
        """
        with tempfile.NamedTemporaryFile(suffix=".c", delete=False, mode="w") as f:
            f.write(c_source)
            c_file = f.name

        try:
            result = subprocess.run(
                [
                    "gcc",
                    c_file,
                    "-o", out_path,
                    f"-I{self.runtime_dir}",
                    "-lm",
                    "-O2",
                    "-Wall",
                    "-Wno-unused-variable",
                    "-Wno-unused-value",
                    "-Wno-unused-function",
                    "-Wno-implicit-function-declaration",
                    "-Wno-misleading-indentation",
                    "-Wno-unused-result",
                ],
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
