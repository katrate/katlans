# 🚀 Katlans Programming Language

**Version:** 0.1.0  
**Extension:** `.kl`  
**Compilation:** Katlans → C (via codegen) → binary (via gcc/clang)  
**Implementation:** Python 3.10+

---

Katlans is a **dynamic, compiled programming language** that transpiles to C. Write `.kl` code, the compiler generates valid C, and then a C compiler (gcc/clang) turns it into a native binary.

---

## ⚡ Quick Start

```bash
# Emit C code from a .kl file
python3 katlans.py emit hello.kl

# Run all tests
python3 katlans.py test

# Run a specific test
python3 katlans.py test test_phase5.kl

# Compile and run (requires gcc)
python3 katlans.py run hello.kl

# Compile to binary (requires gcc)
python3 katlans.py build hello.kl -o hello

# Debug: see internal representations
python3 katlans.py tokens hello.kl    # token stream
python3 katlans.py ast hello.kl       # abstract syntax tree
```

---

## 📁 Project Structure

```
katlans/
├── katlans.py            ← CLI entry point
├── README.md             ← this file
├── pyproject.toml        ← pip install support
├── Makefile              ← build & test automation
├── src/
│   ├── __init__.py       ← package exports
│   ├── errors.py         ← LexerError, ParseError, CodeGenError
│   ├── lexer.py          ← tokeniser  (source → tokens)
│   ├── ast_nodes.py      ← all AST node classes
│   ├── parser.py         ← parser     (tokens → AST)
│   ├── codegen.py        ← C codegen  (AST → C source)
│   └── compiler.py       ← gcc wrapper (C source → binary)
├── runtime/
│   ├── katlans.h         ← Core runtime (KVal, math, I/O, lists, dicts)
│   ├── k_fileio.h        ← File I/O
│   ├── k_os.h            ← OS operations
│   ├── k_regex.h         ← Regular expressions
│   ├── k_formats.h       ← JSON, CSV, XML
│   ├── k_crypto.h        ← MD5, SHA, Base64, UUID
│   ├── k_cli.h           ← CLI tools, colours, progress bars
│   ├── k_datetime.h      ← Date & time
│   ├── k_network.h       ← HTTP, DNS, ping
│   ├── k_concurrency.h   ← Queues, channels, futures
│   ├── k_datastructs.h   ← Stacks, queues, sets, trees, graphs, heaps
│   ├── k_ml.h            ← ML module (tensors, models, losses)
│   ├── k_game.h          ← Game engine (ncurses sprites, collisions)
│   ├── k_ui.h            ← UI module (ncurses TUI widgets)
│   ├── k_vis.h           ← VIS module (computer vision simulation)
│   └── k_finance.h       ← Finance module (stocks, indicators, portfolio)
├── test_runner.py        ← Automated test suite
├── .gitignore
├── hello.kl              ← Hello World example
├── control.kl            ← Control flow example
├── functions.kl          ← Function definition example
├── test_all.kl           ← Phase 1 test suite
├── test_phase2.kl        ← Phase 2 test suite
├── test_phase3.kl        ← Phase 3 test suite
├── test_phase4.kl        ← Phase 4 test suite
└── test_phase5.kl        ← Phase 5 test suite
```

---

## 🏗 Compiler Architecture

```
Source (.kl)
    │
    ▼
┌─────────────┐
│   Lexer     │  src/lexer.py
│             │  Raw text → Token stream
└──────┬──────┘
       │ list[Token]
       ▼
┌─────────────┐
│   Parser    │  src/parser.py
│             │  Tokens → AST (Program)
└──────┬──────┘
       │ Program (AST)
       ▼
┌─────────────┐
│   CodeGen   │  src/codegen.py
│             │  AST → C source string
│             │  Maps built-ins via BUILTIN_MAP (~300 entries)
└──────┬──────┘
       │ C source
       ▼
┌─────────────┐
│  Compiler   │  src/compiler.py
│             │  gcc -o out file.c -lm
└──────┬──────┘
       │ native binary
       ▼
    ./output
```

---

## 📊 Progress

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Foundation (lexer, parser, codegen, runtime, CLI) | ✅ **100%** |
| 2 | Core completeness (strings, lists, dicts, tuples, classes, template strings, infix operators) | ✅ **100%** |
| 3 | Standard library (File I/O, OS, Regex, JSON/CSV/XML, Crypto, CLI tools, Testing) | ✅ **100%** |
| 4 | Extended modules (DateTime, Networking, Concurrency, DataStructures) | ✅ **100%** |
| 5 | Power modules (ML, Game, UI, VIS, Finance) | ✅ **100%** |
| **Total** | | **5/5 Phases Complete** |

## 🧪 Test Results

```
  ╔══════════════════════════════════════╗
  ║    Katlans Test Suite                ║
  ╚══════════════════════════════════════╝

  Testing hello.kl        ... ✓   16 lines of C generated
  Testing control.kl      ... ✓   30 lines of C generated
  Testing functions.kl    ... ✓   45 lines of C generated
  Testing test_all.kl     ... ✓   70 lines of C generated
  Testing test_phase2.kl  ... ✓  143 lines of C generated
  Testing test_phase3.kl  ... ✓   92 lines of C generated
  Testing test_phase4.kl  ... ✓  131 lines of C generated
  Testing test_phase5.kl  ... ✓  321 lines of C generated

  Passed: 8  Failed: 0  Total: 8
```

---

## 🔧 Built-in Functions (~300 total)

The language provides extensive built-in functions organized by prefix:

| Prefix | Module | Example Functions |
|--------|--------|-------------------|
| *(none)* | Math/Cast | `sqrt`, `abs`, `sin`, `cos`, `round`, `I`, `F`, `S`, `type` |
| *(none)* | Global | `len`, `sum`, `all`, `any`, `chr`, `ord`, `hex`, `bin`, `rand`, `gcd`, `fact` |
| `S` | String | `Scap`, `Slow`, `Slen`, `Schop`, `Sfind`, `Sjoin`, `Speeks`, `Sisalpha` |
| `L` | List | `Ladd`, `Llen`, `Lget`, `Lsort`, `Lflip`, `Lhas`, `Lslice`, `Lunique` |
| `A` | Dict/Adero | `Aget`, `Akeys`, `Avals`, `Ahas`, `Aset`, `Adrop`, `Aclear` |
| `Z` | Tuple/Zelo | `Zlen`, `Zhas`, `Zsum`, `Zmax`, `Zmin`, `Zindex` |
| `f` | File I/O | `fread`, `fwrite`, `fdel`, `fexists`, `flist`, `fmkdir` |
| `os` | OS | `ossys`, `osram`, `osuser`, `osrun`, `osget`, `ospwd` |
| `rx` | Regex | `rxmatch`, `rxfind`, `rxswap`, `rxchop`, `rxgroups` |
| `js`/`csv`/`xml` | Formats | `jsparse`, `jsread`, `jswrite`, `csvread`, `xmlparse` |
| `cr` | Crypto | `crmd5`, `crsha`, `crb64enc`, `crb64dec`, `cruuid`, `crtoken` |
| `cli` | CLI | `clired`, `cligreen`, `climenu`, `cliargs`, `cliprogress` |
| `dt` | DateTime | `dtnow`, `dtdate`, `dtformat`, `dtadd`, `dtdiff`, `dtstamp` |
| `net` | Networking | `netget`, `netpost`, `netdns`, `netping`, `netdl` |
| `cx` | Concurrency | `cxqueue`, `cxlock`, `cxchan`, `cxfuture`, `cxpool` |
| `ds` | Data Structs | `dsstack`, `dsqueue`, `dsset`, `dstree`, `dsheap`, `dsmat` |
| `ml` | ML | `mlzeros`, `mlones`, `mlt_matmul`, `mlt_relu`, `mlmodel_seq`, `mlloss_mse` |
| `gm` | Game | `gmsprite`, `gmsp_move`, `gmcol_rect`, `gmgroup`, `gmclock` |
| `ui` | UI | `uiwin`, `uilabel`, `uibtn`, `uientry`, `uicheck`, `uidialog_info` |
| `vis` | Vision | `viscam`, `visframe`, `vis_facedetect`, `vis_hands`, `vis_objects`, `visocr` |
| `fin` | Finance | `finticker`, `finprice`, `finind_sma`, `finport_create`, `finback_run` |

---

## 🖊️ Language Syntax (Cheat Sheet)

```python
~ Single-line comment
~~ Multi-line
    comment ~~

~ Statement terminator
display "Hello" ::           ← Every statement ends with ::

~ Block delimiters
if x > 5 |>                   ← |> opens a block
    display "big" ::
<|::                          ← <|:: closes a block

~ Variables (dynamic typing by default)
name = "Alice" ::
int age = 30 ::               ← explicit type (optional)
flt pi = 3.14 ::
flag = true ::
lt nums = 1, 2, 3 ::          ← list
ad person = {name,"Bob"} ::   ← dict
zl coords = 10, 20 ::         ← tuple/zelo

~ Template strings
display "Hello {name}, you are {age}!" ::

~ Control flow
if x > 5 |> ... <|::
elif x == 5 |> ... <|::
else |> ... <|::

deta i in range(1, 10, 2) |> ... <|::     ← for range
deta i belongs in list |> ... <|::          ← for each
denta cond |> ... <|::                      ← while

~ Functions
fxn dec <add> <a, b> |>
    return a + b ::
<|::

fxn dec <divmod> <a, b> <q, r> |>          ← multiple returns
    q = a // b ::
    r = a /// b ::
<|::

~ Classes
cls dec <Counter> |>
    int count = 0 ::
    fxn dec <inc> |>
        self.count = self.count + 1 ::
    <|::
<|::
Counter c = cls Counter <> ::
fxn c.inc ::

~ Built-in calls (two syntaxes)
display len(list) ::                         ← expression: ()
fwrite <"file.txt", "content"> ::            ← statement: <>

~ Infix operators
x Speeks "World" ::      ← string contains
nums Lhas 5 ::           ← list has value
"hello" Sfront "hel" ::  ← starts with
```

---

## 🔧 Requirements

- **Python 3.10+** (compiler)
- **gcc** or **clang** (to compile generated C to binary)
- **ncurses** library (for Game and UI modules — typically pre-installed on Linux/macOS; use `pdcurses` on Windows)
- **libcurl** (for networking module)
- **libssl** or **OpenSSL** (for crypto)

## 📦 Install

```bash
# From source
pip install -e .

# Or just run directly
python3 katlans.py --help
```

## 🧹 Development

```bash
make test          # Run all tests
make emit          # Generate C code for all tests
make compile       # Compile to binaries (if gcc available)
make clean         # Clean build artifacts
make install       # pip install -e .
```
