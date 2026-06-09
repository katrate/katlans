#!/usr/bin/env python3
"""
katlans  —  The Katlans Language CLI

Usage:
  katlans run  <file.kl>          compile & run
  katlans build <file.kl> [-o out] compile to binary
  katlans emit  <file.kl>          print generated C
  katlans tokens <file.kl>         print token stream (debug)
  katlans ast    <file.kl>         print AST (debug)
"""

import sys
import os
import argparse
import tempfile

# Resolve paths
ROOT         = os.path.dirname(os.path.abspath(__file__))
RUNTIME_DIR  = os.path.join(ROOT, "runtime")
RUNTIME_PATH = os.path.join(RUNTIME_DIR, "katlans.h")

sys.path.insert(0, ROOT)
from src import Lexer, Parser, CodeGen, Compiler, KatlansError


def _read(path: str) -> str:
    if not os.path.exists(path):
        print(f"\033[31m[Error]\033[0m File not found: {path}")
        sys.exit(1)
    if not path.endswith(".kl"):
        print(f"\033[33m[Warning]\033[0m File does not have .kl extension: {path}")
    with open(path, "r", encoding="utf-8") as f:
        return f.read()


def _pipeline(source: str, filename: str) -> str:
    """source → C source string"""
    lexer  = Lexer(source, filename)
    tokens = lexer.tokenise()

    parser = Parser(tokens, filename)
    ast    = parser.parse()

    gen    = CodeGen(RUNTIME_PATH, filename)
    c_src  = gen.generate(ast)
    return c_src


def cmd_run(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)

    try:
        c_src = _pipeline(source, filename)
    except KatlansError as e:
        print(e); sys.exit(1)

    comp = Compiler(RUNTIME_DIR)

    with tempfile.NamedTemporaryFile(suffix="", delete=False) as f:
        binary = f.name

    ok, err = comp.compile(c_src, binary)
    if not ok:
        print(f"\033[31m[Compile Error]\033[0m\n{err}")
        sys.exit(1)

    os.chmod(binary, 0o755)
    code = comp.run(binary)
    os.unlink(binary)
    sys.exit(code)


def cmd_build(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    out      = args.output or filename.replace(".kl", "")

    try:
        c_src = _pipeline(source, filename)
    except KatlansError as e:
        print(e); sys.exit(1)

    comp = Compiler(RUNTIME_DIR)
    ok, err = comp.compile(c_src, out)
    if not ok:
        print(f"\033[31m[Compile Error]\033[0m\n{err}")
        sys.exit(1)

    print(f"\033[32m✓ Built:\033[0m {out}")


def cmd_emit(args):
    source   = _read(args.file)
    filename = os.path.basename(args.file)
    try:
        c_src = _pipeline(source, filename)
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


def main():
    p = argparse.ArgumentParser(
        prog="katlans",
        description="The Katlans Programming Language Compiler"
    )
    sub = p.add_subparsers(dest="cmd")

    r = sub.add_parser("run",    help="Compile and run a .kl file")
    r.add_argument("file")

    b = sub.add_parser("build",  help="Compile a .kl file to a binary")
    b.add_argument("file")
    b.add_argument("-o", "--output", default=None)

    e = sub.add_parser("emit",   help="Print generated C code")
    e.add_argument("file")

    t = sub.add_parser("tokens", help="Print token stream (debug)")
    t.add_argument("file")

    a = sub.add_parser("ast",    help="Print AST (debug)")
    a.add_argument("file")

    args = p.parse_args()

    if   args.cmd == "run":    cmd_run(args)
    elif args.cmd == "build":  cmd_build(args)
    elif args.cmd == "emit":   cmd_emit(args)
    elif args.cmd == "tokens": cmd_tokens(args)
    elif args.cmd == "ast":    cmd_ast(args)
    else:
        # No command — show banner
        print("""
  ██╗  ██╗ █████╗ ████████╗██╗      █████╗ ███╗   ██╗███████╗
  ██║ ██╔╝██╔══██╗╚══██╔══╝██║     ██╔══██╗████╗  ██║██╔════╝
  █████╔╝ ███████║   ██║   ██║     ███████║██╔██╗ ██║███████╗
  ██╔═██╗ ██╔══██║   ██║   ██║     ██╔══██║██║╚██╗██║╚════██║
  ██║  ██╗██║  ██║   ██║   ███████╗██║  ██║██║ ╚████║███████║
  ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝
  
  The Katlans Programming Language  v0.1.0
  
  Usage:
    katlans run   <file.kl>          run a program
    katlans build <file.kl> [-o out] compile to binary
    katlans emit  <file.kl>          print generated C
    katlans tokens <file.kl>         debug: token stream
    katlans ast    <file.kl>         debug: AST
""")


if __name__ == "__main__":
    main()
