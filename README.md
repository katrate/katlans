# Katlans Programming Language

> **Author:** katrate (github.com/katrate)
> **Version:** 1.0
> **Extension:** `.kl`
> **Pipeline:** `.kl` → Lexer → Parser → AST → CodeGen → C → `gcc` → Binary

Katlans is a **compiled programming language** that transpiles to C. Write `.kl` code, the compiler generates valid C, and a C compiler (gcc/clang) turns it into a native binary. It has **~500 built-in functions** across 20+ modules — including ML tensors, finance indicators, a game engine, TUI widgets, and computer vision — all implemented in pure C headers with zero external dependencies.

---

## Quick Start

```bash
# From source (Python 3.10+ required)
python files/katlans.py emit hello.kl      # See generated C
python files/katlans.py test                # Run all tests
python files/katlans.py run hello.kl        # Compile + run (needs gcc)

# Or use the standalone .exe (no Python needed)
katlans.exe run hello.kl
katlans.exe test
katlans.exe setup                           # One-time setup (auto-downloads gcc)
katlans.exe update                          # Self-update from GitHub
```

---

## Table of Contents

- [1. Project Structure](#1-project-structure)
- [2. Compiler Pipeline](#2-compiler-pipeline)
- [3. Language Syntax](#3-language-syntax)
- [4. Data Types](#4-data-types)
- [5. Built-in Functions](#5-built-in-functions)
- [6. CLI Commands](#6-cli-commands)
- [7. Build System](#7-build-system)
- [8. CI/CD](#8-cicd)
- [9. Test Suite](#9-test-suite)
- [10. Runtime Library](#10-runtime-library)
- [11. Phase Summary](#11-phase-summary)
- [12. Development Guide](#12-development-guide)

---

## 1. Project Structure

```
katlans/
├── .github/workflows/build.yml     # CI/CD: test → build .exe → release
├── files/
│   ├── katlans.py                   # CLI entry point (argparse, all commands)
│   ├── test_runner.py              # Test suite runner
│   ├── src/                        # Compiler source (7 files)
│   │   ├── __init__.py             # Package exports
│   │   ├── lexer.py                # Tokeniser — ~60 token types
│   │   ├── parser.py               # Recursive-descent parser → AST
│   │   ├── codegen.py              # AST → C source (~550 lines)
│   │   ├── compiler.py             # gcc invocation wrapper
│   │   ├── ast_nodes.py            # 30+ AST node dataclasses
│   │   └── errors.py               # KatlansError hierarchy
│   ├── runtime/                    # C runtime library (17 headers)
│   │   ├── katlans.h               # Core (types, math, I/O, lists, dicts)
│   │   ├── k_cli.h                 # CLI tools & testing framework
│   │   ├── k_concurrency.h         # Fork-based concurrency
│   │   ├── k_crypto.h              # MD5, SHA-256, Base64, UUID, XOR
│   │   ├── k_datastructs.h         # Stack, queue, set, tree, graph, heap, matrix
│   │   ├── k_datetime.h            # Date & time operations
│   │   ├── k_fileio.h              # File I/O (cross-platform)
│   │   ├── k_formats.h             # JSON, CSV, XML
│   │   ├── k_network.h             # HTTP/1.1 client, DNS, ping
│   │   ├── k_os.h                  # OS operations (env, process, system info)
│   │   ├── k_regex.h               # POSIX regex + Windows fallback
│   │   ├── k_ml.h                  # ML tensors, neural networks
│   │   ├── k_finance.h             # Stocks, indicators, portfolios, backtesting
│   │   ├── k_game.h                # ncurses game engine (sprites, physics, collision)
│   │   ├── k_ui.h                  # ncurses TUI widgets
│   │   ├── k_vis.h                 # Computer vision (simulated)
│   │   ├── k_zp.h                  # Zip compression
│   │   └── k_ser.h                 # Binary serialization
│   ├── hello.kl                    # Hello World example
│   ├── control.kl                  # Control flow example
│   ├── functions.kl                # Function definition example
│   ├── test_all.kl                 # Phase 1 — Foundation tests
│   ├── test_phase2.kl              # Phase 2 — Core tests
│   ├── test_phase3.kl              # Phase 3 — Standard Lib tests
│   ├── test_phase4.kl              # Phase 4 — Extended tests
│   └── test_phase5.kl              # Phase 5 — Power Modules tests
├── Makefile                        # Build automation
├── install.py                      # One-command setup script
├── README.md                       # This file
└── .gitignore
```

---

## 2. Compiler Pipeline

```
.kl source file  ──►  Lexer  ──►  Parser  ──►  CodeGen  ──►  C source  ──►  gcc  ──►  binary
    (tokens)          (AST)           (C code)            (-lm -O2)
```

### 2.1 Lexer
- **~60 token types** via `enum.Enum`
- Handles strings (with escape sequences), numbers (int/flt), identifiers, operators, comments (`~` and `~~...~~`)
- Custom operators: `::` (statement end), `|>` `<|` (block delimiters), `//` `///` `^` `%` `++` `&&` `&+` `!&` `==` `=\` `=>` `=<`

### 2.2 Parser
- Recursive-descent parser with one-token lookahead
- **Expression precedence:** `or` < `and` < `not` < `comparison` < `concat` < `add` < `mul` < `power` < `unary`
- Parses all language constructs: variables, if/elif/else, for/while, functions, classes, try/catch, tests, template strings

### 2.3 Code Generator
- Maps each AST node to C code using `KVal*` (dynamic type)
- `BUILTIN_MAP` — ~500 built-in function mappings to C runtime calls
- `VOID_BUILTINS` — ~150 functions that modify in-place (no return value)
- Template strings compile to `k_concat()` chains
- Classes compile to dict-based objects with constructor functions

### 2.4 Compiler
- Auto-detects `gcc`/`clang` on PATH + common WinLibs install locations
- Flags: `-lm -O2 -Wall -Wno-unused-variable -Wno-implicit-function-declaration`
- Windows: adds `-lws2_32` for Winsock

---

## 3. Language Syntax

### 3.1 Variables

```katlans
int x = 42 ::           ~ Integer (64-bit signed)
flt pi = 3.14159 ::     ~ Float (double)
name = "Katlans" ::     ~ String (inferred)
bool flag = true ::     ~ Boolean
lt nums = 1, 2, 3 ::    ~ List (dynamic array)
zl coords = 10, 20 ::   ~ Zelo/Tuple
ad person = {name,"Alice"}; {age,30} ::   ~ Adero/Dict
```

### 3.2 Control Flow

```katlans
if score => 90 |>   ... <|::      ~ If / elif / else
elif score => 80 |> ... <|::
else |>             ... <|::

deta i belongs range(1; 10; 1) |> ... <|::  ~ For range
deta fruit belongs in fruits |>   ... <|::  ~ For each
denta n > 0 |>                    ... <|::  ~ While

halt ::        ~ break
continue ::    ~ continue
```

### 3.3 Functions

```katlans
fxn dec <add> <a, b> |>
    return a + b ::
<|::

fxn add <10, 32> ::                    ~ Call as statement
int result = fxn add <10, 32> ::       ~ Call as expression
```

### 3.4 Classes

```katlans
cls dec <Counter> |>
    int count = 0 ::
    fxn dec <inc> |> self.count = self.count + 1 :: <|::
<|::

Counter c = cls Counter <> ::
fxn c.inc ::
```

### 3.5 Error Handling & Testing

```katlans
try |> ... <|:: catch <err> |> ... <|::

test dec <mathTest> |>
    testcheck <2 + 2 == 4> ::
<|::
testrun() ::
```

### 3.6 Template Strings

```katlans
display "Hello {name}, you are {age}!" ::
```

### 3.7 Operators

| Category | Operators |
|----------|-----------|
| Arithmetic | `+` `-` `*` `/` `//` (int div) `///` (mod) `^` (pow) `%` (pct) |
| String | `++` (concat) |
| Comparison | `==` `=\` (neq) `<` `>` `=<` (lte) `=>` (gte) |
| Logical | `&&` (and) `&+` (or) `!&` (not) |
| Infix | `Speeks` `Lhas` `Zhas` `Sfront` `Sback` `Ahas` |
| Indexing | `x[0]` `x[0;5]` `x[0;5;2]` |
| Access | `obj.field` |

---

## 4. Data Types

| Katlans | C Runtime | Description |
|---------|-----------|-------------|
| `int` | `KVal*` (KT_INT) | 64-bit signed integer |
| `flt` | `KVal*` (KT_FLT) | Double-precision float |
| `str` | `KVal*` (KT_STR) | Heap-allocated C string |
| `bool` | `KVal*` (KT_BOOL) | `true` / `false` |
| `void` | `KVal*` (KT_VOID) | Unit type |
| `lt` | `KVal*` (KT_LIST) | Dynamic array (`KList`) |
| `zl` | `KVal*` (KT_LIST) | Tuple (immutable list) |
| `ad` | `KVal*` (KT_DICT) | Dict (`KDict`, string → KVal*) |

All values use the `KVal` tagged union at runtime:

```c
struct KVal {
    KType type;  // KT_INT | KT_FLT | KT_STR | KT_BOOL | KT_VOID | KT_LIST | KT_DICT
    union { long long i; double f; char *s; bool b; KList *list; KDict *dict; };
};
```

---

## 5. Built-in Functions (~500 total)

### 5.1 Math (16)
`sqrt`, `abs`, `floor`, `ceil`, `log`, `ln`, `exp`, `sin`, `cos`, `tan`, `arcsin`, `arccos`, `arctan`, `sum`, `len`, `round`, `PI`, `E`, `min`, `max`

### 5.2 String — S prefix (30+)
`Scap`, `Slow`, `Slen`, `Snip`, `Sfind`, `Scount`, `Speeks`, `Sfront`, `Sback`, `Sswap`, `Schop`, `Srchop`, `Schoplines`, `Stitle`, `Scapfirst`, `Sswapcase`, `Scasefold`, `Sremprefix`, `Sremsuffix`, `Szfill`, `Srfind`, `Sindex`, `Srindex`, `Sjoin`, `Sisalpha`, `Sisdigit`, `Sisalnum`, `Sisspace`, `Sislower`, `Sisupper`, `Sistitle`, `Sisascii`

### 5.3 List — L prefix (20+)
`Ladd`, `Llen`, `Lmax`, `Lmin`, `Lsum`, `Lget`, `Lsort`, `Lflip`, `Ldrop`, `Lpop`, `Lindex`, `Lcount`, `Lhas`, `Lcopy`, `Lclear`, `Linsert`, `Lextend`, `Lunique`, `Lslice`, `Lreversed`, `Lsorted`

### 5.4 Dict — A prefix (10)
`Alen`, `Akeys`, `Avals`, `Aitems`, `Aget`, `Ahas`, `Aset`, `Adrop`, `Aclear`, `Acopy`

### 5.5 Tuple — Z prefix (7)
`Zlen`, `Zmax`, `Zmin`, `Zsum`, `Zhas`, `Zindex`, `Zcount`

### 5.6 Global (20)
`type`, `chr`, `ord`, `hex`, `oct`, `bin`, `divmod`, `all`, `any`, `gcd`, `lcm`, `fact`, `rand`, `enumerate`, `zip`, `I` (to int), `F` (to float), `S` (to string)

### 5.7 File I/O — f prefix (20)
`fread`, `fwrite`, `fadd`, `fdel`, `fexists`, `flist`, `fmkdir`, `frmdir`, `frename`, `fcopy`, `fsize`, `fisdir`, `fisfile`, `fext`, `fbase`, `fdir`, `fjoin`, `freadlines`, `fwritelines`

### 5.8 OS — os prefix (20+)
`ossys`, `osram`, `osuptime`, `osboot`, `osuser`, `osrun`, `osget`, `onset`, `ospwd`, `oscd`, `osjoin`, `osbase`, `osdir`, `osext`, `ospath`, `osopen`, `osclipget`, `osclipset`, `osnoti`, `oshalt`

### 5.9 Regex — rx prefix (7)
`rxmatch`, `rxfind`, `rxswap`, `rxchop`, `rxgroups`, `rxfull`, `rxcount`

### 5.10 Data Formats (11)
`jsparse`, `jsstring`, `jsread`, `jswrite`, `csvread`, `csvwrite`, `csvheads`, `csvparse`, `xmlparse`, `xmlget`, `xmlattr`, `xmlstring`

### 5.11 Cryptography — cr prefix (12)
`crmd5`, `crsha`, `crhash`, `crb64enc`, `crb64dec`, `cruuid`, `crtoken`, `crenc`, `crdec`, `crcomp`

### 5.12 CLI (11)
`clired`, `cligreen`, `cliyellow`, `clibold`, `cliblue`, `climagenta`, `clicyan`, `clidim`, `cliunder`, `climenu`, `cliargs`, `cliflag`, `cliopt`, `cliprogress`, `clipbset`, `clipbdone`

### 5.13 Date/Time — dt prefix (20+)
`dtnow`, `dtdate`, `dttime`, `dtformat`, `dtparse`, `dtstamp`, `dtfromstamp`, `dtadd`, `dtsub`, `dtdiff`, `dtconv`, `dtwait`, `dtleap`, `dtbefore`, `dtafter`, `dtequal`, `dtzone`

### 5.14 Networking — net prefix (12)
`netget`, `netpost`, `netput`, `netpatch`, `netdel`, `netjson`, `netdns`, `netping`, `netdl`

### 5.15 Concurrency — cx prefix (20+)
`cxqueue`, `cxpush`, `cxpop`, `cxatom`, `cxinc`, `cxdec`, `cxlock`, `cxacquire`, `cxrelease`, `cxfuture`, `cxresolve`, `cxchan`, `cxsend`, `cxrecv`, `cxwait`, `cxsem`, `cxretry`, `cxtimeout`, `cxpool`, `cxsubmit`, `cxcancel`, `cxabort`

### 5.16 Data Structures — ds prefix (50+)
**Stack:** `dsstack`, `dspush`, `dspop`, `dspeek`, `dssize`
**Queue:** `dsqueue`, `dsenqueue`, `dsdequeue`
**Set:** `dsset`, `dsadd`, `dsrem`, `dshas`, `dsunion`, `dsinter`, `dsdiff`
**Linked List:** `dsll`, `dsllpush`, `dsllpop`, `dsllget`
**Tree:** `dstree`, `dstadd`, `dstchildren`, `dstparent`
**Graph:** `dsgraph`, `dsgedge`, `dsgnbr`, `dsgpath`
**Heap:** `dsheap`, `dshpush`, `dshpop`
**Deque:** `dsdeque`, `dqdpushl`, `dqdpushr`, `dqdpopl`, `dqdpopr`
**Matrix:** `dsmat`, `dsmatset`, `dsmatget`, `dsmatmul`, `dsmattrans`, `dsmatprint`

### 5.17 ML Module — ml prefix (40+)
`mlzeros`, `mlones`, `mlrand`, `mlrandn`, `mltensor_from_list`, `mltensor_shape`, `mlt_sum`, `mlt_mean`, `mlt_max`, `mlt_min`, `mlt_add`, `mlt_sub`, `mlt_mul`, `mlt_matmul`, `mlt_relu`, `mlt_sigmoid`, `mlt_softmax`, `mlt_tanh`, `mlt_flatten`, `mlt_transpose`, `mlt_tolist`, `mlt_get`, `mlt_set`, `mlt_print`, `mlmodel_seq`, `mlmodel_add_linear`, `mlmodel_add_relu`, `mlmodel_add_sigmoid`, `mlmodel_add_softmax`, `mlmodel_forward`, `mlmodel_predict`, `mlmodel_predict_class`, `mlmodel_save`, `mlloss_mse`, `mlloss_bce`, `mlloss_crossentropy`, `mlloss_mae`, `mlmetric_accuracy`, `mlmetric_r2`, `ml_gpu_available`, `mlt_gpu`, `mlt_cpu`

### 5.18 Finance — fin prefix (40+)
`finticker`, `finprice`, `finhist`, `fininfo`, `finind_sma`, `finind_ema`, `finind_rsi`, `finind_macd`, `finind_bollinger`, `finind_atr`, `finind_obv`, `finind_vwap`, `finchart_line`, `finchart_candle`, `finchart_portfolio`, `finchart_show`, `finchart_save`, `finchart_compare`, `finport_create`, `finport_add`, `finport_value`, `finport_cost`, `finport_pnl`, `finport_pnl_pct`, `finport_save`, `finport_load`, `finback_create`, `finback_cash`, `finback_commission`, `finback_buy`, `finback_sell`, `finback_run`, `finmarket_overview`, `finforex`, `fincrypto_top`, `finpat_doji`, `finpat_hammer`

### 5.19 Game Engine — gm prefix (60+)
`gm_init`, `gmwin`, `gmwin_clear`, `gmwin_flip`, `gmwin_caption`, `gmwin_fps`, `gm_running`, `gm_stop`, `gm_quit`, `gm_events`, `gminput_key_pressed`, `gminput_mouse_pos`, `gmdraw_rect`, `gmdraw_circle`, `gmdraw_line`, `gmdraw_text`, `gmdraw_point`, `gmsprite`, `gmsp_pos`, `gmsp_x`, `gmsp_y`, `gmsp_vel`, `gmsp_move`, `gmsp_moveto`, `gmsp_visible`, `gmsp_accel`, `gmsp_update`, `gmsp_draw`, `gmsp_kill`, `gmsp_rect`, `gmgroup`, `gmgroup_add`, `gmgroup_remove`, `gmgroup_update`, `gmgroup_draw`, `gmgroup_clear`, `gmgroup_count`, `gmclock`, `gmclock_tick`, `gmcol_rect`, `gmcol_rect_point`, `gmcol_circle`, `gmcol_group`, `gmcol_dist`, `gmsound`, `gmsound_play`, `gmsound_stop`, `gmsound_vol`, `gmmusic_play`, `gmmusic_stop`, `gmfont`, `gmfont_render`, `gmtext_draw`, `gmcam`, `gmcam_follow`, `gmcam_pos`, `gmcam_zoom`, `gmstate_save`, `gmstate_load`, `gmstate_exists`, `gm_event_post`

### 5.20 UI Module — ui prefix (40+)
`uiwin`, `uiwin_run`, `uiwin_close`, `uiwin_title`, `uiwin_flip`, `uiwin_center`, `uirun`, `uilabel`, `uilabel_text`, `uibtn`, `uientry`, `uientry_get`, `uientry_set`, `uientry_clear`, `uitext`, `uitext_insert`, `uitext_get`, `uitext_clear`, `uicheck`, `uicheck_get`, `uicheck_set`, `uicheck_toggle`, `uiprog`, `uiprog_set`, `uislider`, `uislider_get`, `uislider_set`, `uipack`, `uigrid`, `uiplace`, `uidialog_info`, `uidialog_ask`, `uidialog_input`, `uiclip_set`, `uiclip_get`, `uivar_str`, `uivar_int`, `uivar_bool`, `uivar_get`, `uivar_set`, `uiinfo_screenwidth`, `uiinfo_screenheight`

### 5.21 VIS Module — vis prefix (60+)
`viscam`, `viscam_list`, `viscam_open`, `viscam_close`, `viscam_width`, `viscam_height`, `visframe`, `visframe_blank`, `visload`, `vissave`, `visframe_show`, `visframe_size`, `visframe_gray`, `visframe_rgb`, `visframe_blur`, `visframe_threshold`, `visframe_contours`, `visframe_equalize`, `visframe_flip`, `visframe_rotate`, `visframe_resize`, `visframe_crop`, `visframe_histogram`, `visframe_edges_canny`, `vis_facedetect`, `vis_facemesh`, `visface_add`, `visface_remove`, `visface_load`, `visface_save`, `vis_facerecog`, `vis_landmarks`, `vis_hands`, `vis_pose`, `vis_objects`, `vis_segment`, `vis_eyes`, `vis_emotion`, `vis_agegender`, `vis_motion`, `vis_flow`, `vis_color`, `vis_qr`, `visocr`, `vis_plate`, `vistemplate`, `visfeature_detect`, `visdraw_box`, `visdraw_circle`, `visdraw_line`, `visdraw_text`, `visrec_start`, `visrec_frame`, `visrec_stop`, `visshot`, `visvid`, `visvid_fps`, `visvid_framecount`, `visvid_duration`, `visvid_read`, `visvid_close`, `visvid_seek`, `visvid_writer`, `visvid_write`, `visvid_close_writer`

### 5.22 Other
`testcheck`, `testrun_summary`, `zppack`, `zpunpack`, `zplist`, `serencode`, `serdecode`, `serbytes`, `serfrombytes`

---

## 6. CLI Commands

| Command | Description |
|---------|-------------|
| `katlans run hello.kl` | Compile .kl → C → binary → execute |
| `katlans build hello.kl -o hello` | Compile .kl → standalone binary |
| `katlans emit hello.kl` | Print generated C source |
| `katlans test` | Run all 8 test files |
| `katlans test hello.kl` | Run a specific test |
| `katlans tokens hello.kl` | Debug: print token stream |
| `katlans ast hello.kl` | Debug: print AST |
| `katlans setup` | One-time setup (check Python, download compiler, add to PATH) |
| `katlans update` | Self-update from GitHub releases |

### Auto-download Compiler
If no C compiler is found, `katlans run`/`build`/`setup` automatically downloads MinGW-w64 (~100 MB) from WinLibs, extracts it to `~/.katlans/mingw64/`, and adds it to PATH — one-time download, cached forever.

### Self-Update
`katlans update` fetches the latest `.exe` from GitHub releases and replaces the running binary.

---

## 7. Build System

### 7.1 Makefile

```bash
make          # Emit C → compile binaries
make test     # Run test suite
make clean    # Remove build artifacts
make run-%    # Run specific .kl (e.g., make run-hello)
```

### 7.2 Standalone .exe (PyInstaller)

Build the 8 MB standalone executable:

```bash
pip install pyinstaller
python -m PyInstaller --onefile --name katlans \
  --add-data "files/src;src" \
  --add-data "files/runtime;runtime" \
  --add-data "files/test_runner.py;." \
  --add-data "files/hello.kl;." \
  --add-data "files/control.kl;." \
  --add-data "files/functions.kl;." \
  --add-data "files/test_all.kl;." \
  --add-data "files/test_phase2.kl;." \
  --add-data "files/test_phase3.kl;." \
  --add-data "files/test_phase4.kl;." \
  --add-data "files/test_phase5.kl;." \
  --hidden-import src.lexer \
  --hidden-import src.parser \
  --hidden-import src.codegen \
  --hidden-import src.compiler \
  --hidden-import src.errors \
  --hidden-import src.ast_nodes \
  --distpath . files/katlans.py
```

### 7.3 Install Script

`python install.py` — automated setup that installs Python requirements and builds the .exe.

---

## 8. CI/CD

`.github/workflows/build.yml` has 3 jobs:

| Job | Runs On | When |
|-----|---------|------|
| `test` | Ubuntu/macOS/Windows × Python 3.10/3.12/3.13 | Every push + PR |
| `build` | Windows | Every push (after tests) — builds `katlans.exe`, smoke tests, uploads artifact |
| `release` | Ubuntu | Tags `v*` — downloads artifact, publishes GitHub Release with release notes |

---

## 9. Test Suite

The test runner (`files/test_runner.py`) runs all 8 `.kl` files through the emit pipeline:

| Test File | Phase | What it Tests |
|-----------|-------|---------------|
| `hello.kl` | 1 — Foundation | Variables, string concat, math |
| `control.kl` | 1 — Foundation | if/elif/else, for, while, for-each |
| `functions.kl` | 1 — Foundation | Basic functions, recursion |
| `test_all.kl` | 1 — Foundation | All basic types, operators, control flow, recursion |
| `test_phase2.kl` | 2 — Core | Template strings, string/list/dict methods, classes, builtins |
| `test_phase3.kl` | 3 — Standard Lib | File I/O, OS, regex, JSON, crypto, CLI, testing framework |
| `test_phase4.kl` | 4 — Extended | DateTime, networking, concurrency, data structures |
| `test_phase5.kl` | 5 — Power Modules | ML tensors/models, finance indicators/portfolio, game sprites, UI widgets, VIS camera/face |

**Validation:** Each test checks that the generated C includes `#include "katlans.h"`, `int main(void)`, and `return 0;`.

---

## 10. Runtime Library

17 C headers in `files/runtime/`:

| Header | Module | Key Functions |
|--------|--------|---------------|
| `katlans.h` | Core | KVal type system, constructors, arithmetic, comparison, logic, display, math, string/list/dict/tuple built-ins, I/O, casting |
| `k_cli.h` | CLI | ANSI colors, menu, progress bar, CLI args, test framework |
| `k_concurrency.h` | Concurrency | Fork/join, channels, futures, atomic, lock, semaphore |
| `k_crypto.h` | Crypto | MD5, SHA-256, Base64, UUID, XOR encryption |
| `k_datastructs.h` | Data Structs | Stack, queue, set, linked list, tree, graph, heap, deque, matrix |
| `k_datetime.h` | DateTime | Now, format, parse, add, diff, timezone, wait |
| `k_fileio.h` | File I/O | Read, write, append, delete, list dir, mkdir, path ops |
| `k_formats.h` | Formats | JSON parse/stringify, CSV read/write, XML parse |
| `k_network.h` | Network | HTTP/1.1 client, DNS, ping, download |
| `k_os.h` | OS | System info, RAM, uptime, user, env, clipboard, notifications |
| `k_regex.h` | Regex | Match, find, replace, split, groups (POSIX + Win32 fallback) |
| `k_ml.h` | ML | Tensors, sequential models, forward pass, loss functions |
| `k_finance.h` | Finance | Stock data, technical indicators, portfolio, backtesting, patterns |
| `k_game.h` | Game | ncurses sprites, physics, collision detection, groups, state |
| `k_ui.h` | UI | ncurses windows, labels, buttons, entries, dialogs |
| `k_vis.h` | VIS | Camera, face detection, hands, pose, objects, OCR, QR |
| `k_zp.h`, `k_ser.h` | Misc | Zip compression, binary serialization |

---

## 11. Phase Summary

| Phase | Description | Status |
|-------|-------------|--------|
| **1 — Foundation** | Hello World, variables, math, if/elif/else, for/while loops, functions, recursion, try/catch, I/O | ✅ |
| **2 — Core** | Template strings, 30+ string methods, 20+ list methods, dict methods, tuples, classes with `self`, global builtins | ✅ |
| **3 — Standard Library** | File I/O, OS operations, regex, JSON/CSV/XML, MD5/SHA/Base64/UUID crypto, CLI colors/menus, test framework | ✅ |
| **4 — Extended** | DateTime (format, parse, diff, timezone), networking (HTTP, DNS, ping), concurrency (channels, futures, atomic), data structures (stack, queue, set, tree, graph, heap, matrix) | ✅ |
| **5 — Power Modules** | ML tensors & neural networks, finance indicators & backtesting, ncurses game engine, TUI widgets, computer vision | ✅ |

---

## 12. Development Guide

### Adding a Built-in Function

1. Add C implementation in `files/runtime/k_*.h`
2. Add mapping in `files/src/codegen.py` `BUILTIN_MAP`
3. If void-returning, add to `VOID_BUILTINS`
4. Write test in a `.kl` file, run `python files/test_runner.py`

### Adding to CI Build

Update `.github/workflows/build.yml`:
- Add new `--add-data` entries for any new test files
- Update smoke test commands

### Requirements

- **Python 3.10+** (to run the compiler from source)
- **gcc/clang** (to compile generated C to binary — auto-downloaded on Windows)
- No external Python packages needed for the compiler itself

### Distribution

Share just `katlans.exe` (8 MB, built via PyInstaller). The user needs:
1. `katlans.exe` (bundles compiler + runtime + all headers)
2. A C compiler (`katlans setup` auto-downloads it)
3. Any text editor

Or run `python install.py` for a guided setup.
