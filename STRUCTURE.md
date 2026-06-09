# Katlans Programming Language — Project Structure

> **Author:** katrate (github.com/katrate)
> **Version:** 0.1.0
> **Repository:** https://github.com/katrate/katlans
> **Pipeline:** `.kl` → Lexer → Parser → AST → CodeGen → C → `gcc` → Binary

---

## Table of Contents

- [1. Overview](#1-overview)
- [2. Directory Structure](#2-directory-structure)
- [3. Compiler Pipeline](#3-compiler-pipeline)
- [4. Language Syntax & Keywords](#4-language-syntax--keywords)
- [5. Data Types](#5-data-types)
- [6. Built-in Functions (~350+)](#6-built-in-functions)
- [7. Runtime Library](#7-runtime-library)
- [8. CLI Commands](#8-cli-commands)
- [9. Build System](#9-build-system)
- [10. CI/CD (GitHub Actions)](#10-cicd-github-actions)
- [11. Test Suite](#11-test-suite)
- [12. Packaging & Distribution](#12-packaging--distribution)
- [13. Phase Summary](#13-phase-summary)
- [14. Development Guide](#14-development-guide)

---

## 1. Overview

Katlans is a compiled programming language that transpiles to C. It provides a modern, expressive syntax with built-in support for:

- **5 Phases** of language features (foundation → core → stdlib → extended → power modules)
- **~350 built-in functions** across 20+ categories (math, strings, lists, dicts, file I/O, networking, crypto, ML, finance, game dev, UI, computer vision, datetime, regex, data structures, concurrency, serialization, compression, etc.)
- **Zero external dependencies** for the language itself — only a C compiler (gcc/clang) is needed to run compiled programs
- **Standalone executable** distribution via PyInstaller (8 MB, no Python required)

### Key Design Decisions

| Decision | Choice |
|----------|--------|
| Compilation target | C (portable, fast, universal) |
| Runtime types | Dynamic (KVal union struct with type tags) |
| Memory model | Heap-allocated, simple malloc/free |
| Concurrency model | Fork-based (POSIX), stubbed on Windows |
| Distribution | PyInstaller single-file .exe |
| Package manager | None (single-file compiler) |

---

## 2. Directory Structure

```
katlans/
├── .github/
│   └── workflows/
│       └── build.yml              # GitHub Actions CI/CD
├── files/
│   ├── katlans.py                  # CLI entry point (argparse, commands)
│   ├── test_runner.py             # Test suite runner
│   ├── src/                       # Compiler source
│   │   ├── __init__.py            # Re-exports: Lexer, Parser, CodeGen, Compiler
│   │   ├── lexer.py               # Tokeniser ~60 token types
│   │   ├── parser.py              # Recursive-descent parser → AST
│   │   ├── codegen.py             # AST → C source generator
│   │   ├── compiler.py            # gcc invocation wrapper
│   │   ├── ast_nodes.py           # 30+ AST node dataclasses
│   │   └── errors.py              # KatlansError, LexerError, ParseError, CodeGenError
│   ├── runtime/                   # C runtime library (17 headers)
│   │   ├── katlans.h              # Core runtime (types, math, I/O, lists, dicts)
│   │   ├── k_cli.h                # CLI tools & testing framework
│   │   ├── k_concurrency.h        # Concurrency (fork, channels, atomic, futures)
│   │   ├── k_crypto.h             # Cryptography (MD5, SHA-256, Base64, UUID, XOR)
│   │   ├── k_datastructs.h        # Data structures (stack, queue, set, LL, tree, graph, heap, deque, matrix)
│   │   ├── k_datetime.h           # Date & time operations
│   │   ├── k_fileio.h             # File I/O (read, write, list, mkdir, etc.)
│   │   ├── k_formats.h            # Data formats (JSON, CSV, XML)
│   │   ├── k_network.h            # Networking (HTTP client, DNS, ping)
│   │   ├── k_os.h                 # OS operations (env, process, system info)
│   │   ├── k_regex.h              # Regex (POSIX extended, Win32 fallback)
│   │   ├── k_ml.h                 # Machine learning (tensors, neural networks)
│   │   ├── k_finance.h            # Finance (stocks, indicators, portfolios, backtesting)
│   │   ├── k_game.h               # Game engine (sprites, collision, sound, ncurses)
│   │   ├── k_ui.h                 # TUI widgets (windows, labels, buttons, dialogs)
│   │   ├── k_vis.h                # Computer vision (camera, face detection, OCR, QR)
│   │   ├── k_zp.h                 # Zip compression
│   │   └── k_ser.h                # Binary serialization
│   ├── hello.kl                   # Basic "Hello World" example
│   ├── control.kl                 # Control flow examples (if, for, while)
│   ├── functions.kl               # Function examples (basic, recursive)
│   ├── test_all.kl                # Foundation test (types, arithmetic, control flow)
│   ├── test_phase2.kl             # Phase 2 test (strings, lists, dicts, classes)
│   ├── test_phase3.kl             # Phase 3 test (file I/O, regex, JSON, crypto, CLI, testing)
│   ├── test_phase4.kl             # Phase 4 test (datetime, networking, concurrency, data structures)
│   ├── test_phase5.kl             # Phase 5 test (ML, finance, game, UI, VIS)
│   └── mnt/user-data/outputs/     # Legacy v0.1 examples
├── Makefile                       # Build automation
├── install.py                     # One-command setup script
├── STRUCTURE.md                   # This file
├── .gitignore                     # Git ignore rules
└── 🚀 Katlans Language — Official Documentation/  # Legacy docs (8 markdown files)
```

---

## 3. Compiler Pipeline

```
.kl source file
       │
       ▼
  ┌──────────┐
  │  Lexer    │  Tokenises source into ~60 token types
  │ lexer.py  │  Handles strings, numbers, identifiers, operators
  └─────┬────┘
        │ tokens[]
        ▼
  ┌──────────┐
  │  Parser   │  Recursive-descent parser
  │ parser.py │  Produces typed AST nodes
  └─────┬────┘
        │ Program(body=[...])
        ▼
  ┌──────────┐
  │ CodeGen  │  AST → C source code
  │ codegen.py│  Emits #include "katlans.h" + forward decls + main()
  └─────┬────┘
        │ C source string
        ▼
  ┌──────────┐
  │ Compiler │  Invokes gcc/clang with {c_source} -I{runtime} -lm -O2
  │ compiler.│  Returns (success_bool, error_string)
  └─────┬────┘
        │ binary (or .c file for emit)
        ▼
    executable output
```

### 3.1 Lexer (`files/src/lexer.py`)

- **Token types:** ~60 token types via `enum.Enum` (TT class)
- **Keywords:** `int, flt, lt, zl, ad, if, elif, else, deta, denta, fxn, dec, return, cls, self, try, catch, test, display, ask, async, from, borrow, halt, continue, belongs, range, in, true, false, void`
- **Operators:** `+, -, *, /, // (int div), /// (mod), ^ (pow), % (pct)`, `==, =\, =>, =<, =, <, >`, `&&, &+, !&`, `++ (string concat)`
- **Comments:** `~ line comment`, `~~ block comment ~~`
- **Block delimiters:** `|> ... <|::`
- **Statement terminator:** `::` (double colon)

### 3.2 Parser (`files/src/parser.py`)

- Recursive-descent parser with one-token lookahead
- **Expression precedence** (lowest to highest):

| Level | Operators | Associativity |
|-------|-----------|---------------|
| 6 | `Speeks`, `Lhas`, `Zhas`, `Sfront`, `Sback`, `Ahas` (infix) | Left |
| 5 | `&+` (or) | Left |
| 4 | `&&` (and) | Left |
| 3 | `!&` (not) | Right (unary) |
| 3 | `==`, `=\`, `=>`, `=<`, `<`, `>` | Left |
| 2 | `++` (concat) | Left |
| 2 | `+`, `-` | Left |
| 1 | `*`, `/`, `//`, `///`, `%` | Left |
| 0 | `^` (power) | Right |
| 0 | unary `-`, postfix `.`, `[]` | Right |

### 3.3 Code Generator (`files/src/codegen.py`)

- **~550 lines** — maps AST nodes to C code
- Every variable is a `KVal*` (dynamic type)
- **Built-in function map:** ~350 entries in `BUILTIN_MAP` dict
- **VOID_BUILTINS set:** ~150 functions that modify in-place (no return value)
- Template strings (`"Hello {name}!"`) compile to `k_concat()` chains
- Classes compile to dict-based objects with constructor functions

### 3.4 Compiler (`files/src/compiler.py`)

- Auto-detects `gcc` on PATH + common WinLibs install locations
- Compiles C → binary with: `-lm -O2 -Wall -Wno-unused-variable -Wno-unused-value -Wno-implicit-function-declaration`
- Windows: adds `-lws2_32` for Winsock networking

### 3.5 AST Nodes (`files/src/ast_nodes.py`)

30+ node types, all Python `@dataclass`:

| Category | Nodes |
|----------|-------|
| Program | `Program` |
| Literals | `IntLiteral`, `FltLiteral`, `StrLiteral`, `BoolLiteral`, `VoidLiteral`, `ListLiteral`, `TupleLiteral`, `DictLiteral` |
| Template | `TemplateString` |
| Identifiers | `Identifier`, `MemberAccess`, `IndexAccess` |
| Declarations | `VarDecl`, `FuncDecl`, `ClassDecl` |
| Statements | `DisplayStmt`, `IfStmt`, `ForRangeStmt`, `ForEachStmt`, `WhileStmt`, `HaltStmt`, `ContinueStmt`, `ReturnStmt`, `Assignment`, `FuncCall`, `BuiltinCall`, `ClassInstantiate`, `TryCatch`, `TestDecl` |
| Expressions | `BinOp`, `UnaryOp`, `AskExpr` |

---

## 4. Language Syntax & Keywords

### 4.1 Variable Declarations

```katlans
int x = 42 ::          ~ Integer (64-bit signed)
flt pi = 3.14159 ::    ~ Float (double)
str name = "Katlans" :: ~ String (implicit via bare assignment)
name = "Katlans" ::    ~ Typeless (inferred as str)
bool flag = true ::    ~ Boolean
void nothing ::        ~ Void

lt nums = 1, 2, 3 ::   ~ List (dynamic array)
zl coords = 10, 20 ::  ~ Zelo/Tuple (list-based)
ad person = {name,"Alice"}; {age,30} ::  ~ Adero/Dict
```

### 4.2 Control Flow

```katlans
~ If / elif / else
if score => 90 |>
    display "A" ::
<|::
elif score => 80 |>
    display "B" ::
<|::
else |>
    display "F" ::
<|::

~ For range (start; end; step)
deta i belongs range(1; 10; 1) |>
    display i ::
<|::

~ For each
deta fruit belongs in fruits |>
    display fruit ::
<|::

~ While
denta n > 0 |>
    display n ::
    n = n - 1 ::
<|::

~ Break / Continue
halt ::        ~ break
continue ::    ~ continue
```

### 4.3 Functions

```katlans
~ Declaration
fxn dec <add> <a, b> |>
    return a + b ::
<|::

~ Call (as statement)
fxn add <10, 32> ::

~ Call (as expression)
int result = fxn add <10, 32> ::

~ Async
fxn async <fetchData> <url> |>
    ...
<|::
```

### 4.4 Classes

```katlans
~ Declaration
cls dec <Counter> |>
    int count = 0 ::
    fxn dec <inc> |>
        self.count = self.count + 1 ::
    <|::
    fxn dec <get> |>
        return self.count ::
    <|::
<|::

~ Instantiation
Counter c = cls Counter <> ::

~ Method call
fxn c.inc ::
fxn c.get ::
```

### 4.5 Error Handling

```katlans
try |>
    display "safe code" ::
<|::
catch <err> |>
    display err ::
<|::
```

### 4.6 Testing

```katlans
test dec <mathTest> |>
    testcheck <2 + 2 == 4> ::
    testcheck <10 - 3 == 7> ::
<|::

testrun() ::    ~ Run all tests and print summary
```

### 4.7 Template Strings

```katlans
name = "Alice" ::
int age = 30 ::
display "Hello {name}, you are {age}!" ::    ~ "Hello Alice, you are 30!"
display "2 + 2 = {2 + 2}" ::                 ~ "2 + 2 = 4"
```

### 4.8 Operators

| Category | Operators |
|----------|-----------|
| Arithmetic | `+` `-` `*` `/` `//` (int div) `///` (mod) `^` (pow) `%` (pct) |
| String | `++` (concat) |
| Comparison | `==` `=\` (neq) `<` `>` `=<` (lte) `=>` (gte) |
| Logical | `&&` (and) `&+` (or) `!&` (not) |
| Infix | `Speeks` (str contains) `Lhas` (list contains) `Zhas` (tuple contains) `Sfront` (starts with) `Sback` (ends with) `Ahas` (dict has key) |
| Indexing | `x[0]` `x[0;5]` (slice) `x[0;5;2]` (slice with step) |
| Access | `obj.field` (member access) |

---

## 5. Data Types

| Katlans | C Runtime | Description |
|---------|-----------|-------------|
| `int` | `KVal*` (KT_INT) | 64-bit signed integer |
| `flt` | `KVal*` (KT_FLT) | Double-precision float |
| `str` | `KVal*` (KT_STR) | `strdup`-ed C string |
| `bool` | `KVal*` (KT_BOOL) | `true` / `false` |
| `void` | `KVal*` (KT_VOID) | Unit type |
| `lt` | `KVal*` (KT_LIST) | Dynamic array `KList` |
| `zl` | `KVal*` (KT_LIST) | Tuple (immutable list) |
| `ad` | `KVal*` (KT_DICT) | Dict `KDict` (string → KVal*) |

The C runtime `KVal` is a tagged union:

```c
struct KVal {
    KType type;  // KT_INT | KT_FLT | KT_STR | KT_BOOL | KT_VOID | KT_LIST | KT_DICT
    union { long long i; double f; char *s; bool b; KList *list; KDict *dict; };
};
```

---

## 6. Built-in Functions (~350+)

### 6.1 Math (16)

`sqrt`, `abs`, `floor`, `ceil`, `log`, `ln`, `exp`, `sin`, `cos`, `tan`, `arcsin`, `arccos`, `arctan`, `sum`, `len`, `round`, `PI`, `E`, `min`, `max`

### 6.2 String — S prefix (30+)

`Scap`, `Slow`, `Slen`, `Snip`, `Sfind`, `Scount`, `Speeks`, `Sfront`, `Sback`, `Sswap`, `Schop`, `Srchop`, `Schoplines`, `Stitle`, `Scapfirst`, `Sswapcase`, `Scasefold`, `Sremprefix`, `Sremsuffix`, `Szfill`, `Srfind`, `Sindex`, `Srindex`, `Sjoin`, `Sisalpha`, `Sisdigit`, `Sisalnum`, `Sisspace`, `Sislower`, `Sisupper`, `Sistitle`, `Sisascii`

### 6.3 List — L prefix (20+)

`Ladd`, `Llen`, `Lmax`, `Lmin`, `Lsum`, `Lget`, `Lsort`, `Lflip`, `Ldrop`, `Lpop`, `Lindex`, `Lcount`, `Lhas`, `Lcopy`, `Lclear`, `Linsert`, `Lextend`, `Lunique`, `Lslice`, `Lreversed`, `Lsorted`

### 6.4 Dict — A prefix (10+)

`Alen`, `Akeys`, `Avals`, `Aitems`, `Aget`, `Ahas`, `Aset`, `Adrop`, `Aclear`, `Acopy`

### 6.5 Tuple — Z prefix (7)

`Zlen`, `Zmax`, `Zmin`, `Zsum`, `Zhas`, `Zindex`, `Zcount`

### 6.6 Global (20)

`type`, `chr`, `ord`, `hex`, `oct`, `bin`, `divmod`, `all`, `any`, `gcd`, `lcm`, `fact`, `rand`, `enumerate`, `zip`, `I` (to int), `F` (to float), `S` (to string)

### 6.7 File I/O — f prefix (20)

`fread`, `fwrite`, `fadd`, `fdel`, `fexists`, `flist`, `fmkdir`, `frmdir`, `frename`, `fcopy`, `fsize`, `fisdir`, `fisfile`, `fext`, `fbase`, `fdir`, `fjoin`, `freadlines`, `fwritelines`

### 6.8 OS — os prefix (20+)

`ossys`, `osram`, `osuptime`, `osboot`, `osuser`, `osrun`, `osget`, `onset`, `ospwd`, `oscd`, `osjoin`, `osbase`, `osdir`, `osext`, `ospath`, `osopen`, `osclipget`, `osclipset`, `osnoti`, `oshalt`

### 6.9 Regex — rx prefix (7)

`rxmatch`, `rxfind`, `rxswap`, `rxchop`, `rxgroups`, `rxfull`, `rxcount`

### 6.10 Data Formats (11)

`jsparse`, `jsstring`, `jsread`, `jswrite`, `csvread`, `csvwrite`, `csvheads`, `csvparse`, `xmlparse`, `xmlget`, `xmlattr`, `xmlstring`

### 6.11 Cryptography — cr prefix (12)

`crmd5`, `crsha`, `crhash`, `crb64enc`, `crb64dec`, `cruuid`, `crtoken`, `crenc`, `crdec`, `crcomp`

### 6.12 CLI — cli prefix (11)

`clired`, `cligreen`, `cliyellow`, `clibold`, `cliblue`, `climagenta`, `clicyan`, `clidim`, `cliunder`, `climenu`, `cliargs`, `cliflag`, `cliopt`, `cliprogress`, `clipbset`, `clipbdone`

### 6.13 Date/Time — dt prefix (20+)

`dtnow`, `dtdate`, `dttime`, `dtformat`, `dtparse`, `dtstamp`, `dtfromstamp`, `dtadd`, `dtsub`, `dtdiff`, `dtconv`, `dtwait`, `dtleap`, `dtbefore`, `dtafter`, `dtequal`, `dtzone`

### 6.14 Networking — net prefix (12)

`netget`, `netpost`, `netput`, `netpatch`, `netdel`, `netjson`, `netdns`, `netping`, `netdl`

### 6.15 Concurrency — cx prefix (20+)

`cxqueue`, `cxpush`, `cxpop`, `cxatom`, `cxinc`, `cxdec`, `cxlock`, `cxacquire`, `cxrelease`, `cxfuture`, `cxresolve`, `cxchan`, `cxsend`, `cxrecv`, `cxwait`, `cxsem`, `cxretry`, `cxtimeout`, `cxpool`, `cxsubmit`, `cxcancel`, `cxabort`

### 6.16 Data Structures — ds prefix (50+)

**Stack:** `dsstack`, `dspush`, `dspop`, `dspeek`, `dssize`
**Queue:** `dsqueue`, `dsenqueue`, `dsdequeue`
**Set:** `dsset`, `dsadd`, `dsrem`, `dshas`, `dsunion`, `dsinter`, `dsdiff`
**Linked List:** `dsll`, `dsllpush`, `dsllpop`, `dsllget`
**Tree:** `dstree`, `dstadd`, `dstchildren`, `dstparent`
**Graph:** `dsgraph`, `dsgedge`, `dsgnbr`, `dsgpath`
**Heap:** `dsheap`, `dshpush`, `dshpop`
**Deque:** `dsdeque`, `dqdpushl`, `dqdpushr`, `dqdpopl`, `dqdpopr`
**Matrix:** `dsmat`, `dsmatset`, `dsmatget`, `dsmatmul`, `dsmattrans`, `dsmatprint`

### 6.17 ML Module — ml prefix (40+)

**Tensors:** `mlzeros`, `mlones`, `mlrand`, `mlrandn`, `mltensor_from_list`, `mltensor_shape`, `mlt_sum`, `mlt_mean`, `mlt_max`, `mlt_min`, `mlt_add`, `mlt_sub`, `mlt_mul`, `mlt_matmul`, `mlt_relu`, `mlt_sigmoid`, `mlt_softmax`, `mlt_tanh`, `mlt_flatten`, `mlt_transpose`, `mlt_tolist`, `mlt_get`, `mlt_set`, `mlt_print`
**Models:** `mlmodel_seq`, `mlmodel_add_linear`, `mlmodel_add_relu`, `mlmodel_add_sigmoid`, `mlmodel_add_softmax`, `mlmodel_forward`, `mlmodel_predict`, `mlmodel_predict_class`, `mlmodel_save`
**Loss:** `mlloss_mse`, `mlloss_bce`, `mlloss_crossentropy`, `mlloss_mae`
**Metrics:** `mlmetric_accuracy`, `mlmetric_r2`
**GPU:** `ml_gpu_available`, `mlt_gpu`, `mlt_cpu`

### 6.18 Finance — fin prefix (40+)

**Data:** `finticker`, `finprice`, `finhist`, `fininfo`
**Indicators:** `finind_sma`, `finind_ema`, `finind_rsi`, `finind_macd`, `finind_bollinger`, `finind_atr`, `finind_obv`, `finind_vwap`
**Charts:** `finchart_line`, `finchart_candle`, `finchart_portfolio`, `finchart_show`, `finchart_save`, `finchart_compare`
**Portfolio:** `finport_create`, `finport_add`, `finport_value`, `finport_cost`, `finport_pnl`, `finport_pnl_pct`, `finport_save`, `finport_load`
**Backtesting:** `finback_create`, `finback_cash`, `finback_commission`, `finback_buy`, `finback_sell`, `finback_run`
**Market:** `finmarket_overview`, `finforex`, `fincrypto_top`
**Patterns:** `finpat_doji`, `finpat_hammer`

### 6.19 Game Engine — gm prefix (60+)

**Initialization:** `gm_init`, `gmwin`, `gmwin_clear`, `gmwin_flip`, `gmwin_caption`, `gmwin_fps`, `gm_running`, `gm_stop`, `gm_quit`, `gm_events`
**Input:** `gminput_key_pressed`, `gminput_mouse_pos`
**Drawing:** `gmdraw_rect`, `gmdraw_circle`, `gmdraw_line`, `gmdraw_text`, `gmdraw_point`
**Sprites:** `gmsprite`, `gmsp_pos`, `gmsp_x`, `gmsp_y`, `gmsp_vel`, `gmsp_move`, `gmsp_moveto`, `gmsp_visible`, `gmsp_accel`, `gmsp_update`, `gmsp_draw`, `gmsp_kill`, `gmsp_rect`
**Groups:** `gmgroup`, `gmgroup_add`, `gmgroup_remove`, `gmgroup_update`, `gmgroup_draw`, `gmgroup_clear`, `gmgroup_count`
**Collision:** `gmcol_rect`, `gmcol_rect_point`, `gmcol_circle`, `gmcol_group`, `gmcol_dist`
**Audio:** `gmsound`, `gmsound_play`, `gmsound_stop`, `gmsound_vol`, `gmmusic_play`, `gmmusic_stop`
**Font/Text:** `gmfont`, `gmfont_render`, `gmtext_draw`
**Camera:** `gmcam`, `gmcam_follow`, `gmcam_pos`, `gmcam_zoom`
**State:** `gmstate_save`, `gmstate_load`, `gmstate_exists`
**Events:** `gm_event_post`

### 6.20 UI Module — ui prefix (40+)

**Window:** `uiwin`, `uiwin_run`, `uiwin_close`, `uiwin_title`, `uiwin_flip`, `uiwin_center`, `uirun`
**Widgets:** `uilabel`, `uilabel_text`, `uibtn`, `uientry`, `uientry_get`, `uientry_set`, `uientry_clear`, `uitext`, `uitext_insert`, `uitext_get`, `uitext_clear`, `uicheck`, `uicheck_get`, `uicheck_set`, `uicheck_toggle`, `uiprog`, `uiprog_set`, `uislider`, `uislider_get`, `uislider_set`
**Layout:** `uipack`, `uigrid`, `uiplace`
**Dialogs:** `uidialog_info`, `uidialog_ask`, `uidialog_input`
**Clipboard:** `uiclip_set`, `uiclip_get`
**Variables:** `uivar_str`, `uivar_int`, `uivar_bool`, `uivar_get`, `uivar_set`
**Screen:** `uiinfo_screenwidth`, `uiinfo_screenheight`

### 6.21 VIS Module — vis prefix (60+)

**Camera:** `viscam`, `viscam_list`, `viscam_open`, `viscam_close`, `viscam_width`, `viscam_height`
**Frames:** `visframe`, `visframe_blank`, `visload`, `vissave`, `visframe_show`, `visframe_size`, `visframe_gray`, `visframe_rgb`, `visframe_blur`, `visframe_threshold`, `visframe_contours`, `visframe_equalize`, `visframe_flip`, `visframe_rotate`, `visframe_resize`, `visframe_crop`, `visframe_histogram`, `visframe_edges_canny`
**Face:** `vis_facedetect`, `vis_facemesh`, `visface_add`, `visface_remove`, `visface_load`, `visface_save`, `vis_facerecog`, `vis_landmarks`
**Hands:** `vis_hands`
**Pose:** `vis_pose`
**Objects:** `vis_objects`, `vis_segment`, `vis_eyes`, `vis_emotion`, `vis_agegender`
**Motion:** `vis_motion`, `vis_flow`, `vis_color`, `vis_qr`, `visocr`, `vis_plate`
**Features:** `vistemplate`, `visfeature_detect`
**Drawing:** `visdraw_box`, `visdraw_circle`, `visdraw_line`, `visdraw_text`
**Recording:** `visrec_start`, `visrec_frame`, `visrec_stop`, `visshot`
**Video:** `visvid`, `visvid_fps`, `visvid_framecount`, `visvid_duration`, `visvid_read`, `visvid_close`, `visvid_seek`, `visvid_writer`, `visvid_write`, `visvid_close_writer`

### 6.22 Other (6)

`testcheck`, `testrun_summary`, `zppack`, `zpunpack`, `zplist`, `zpadd`, `zprem`, `serencode`, `serdecode`, `serbytes`, `serfrombytes`

---

## 7. Runtime Library

The runtime is 17 C header files in `files/runtime/`. They are:

### 7.1 `katlans.h` (Core)

- **Type system:** `KType` enum, `KVal` struct (tagged union)
- **Constructors:** `kv_int()`, `kv_flt()`, `kv_str()`, `kv_bool()`, `kv_void()`, `kv_list()`, `kv_dict()`
- **List:** `KList` struct with dynamic array, `klist_new()`, `klist_push()`
- **Dict:** `KDict` struct with open-addressing, `kdict_new()`, `kdict_set()`, `kdict_get()`
- **Display:** `k_display()` — prints any KVal with formatting
- **Arithmetic:** `k_add()`, `k_sub()`, `k_mul()`, `k_div()`, `k_idiv()`, `k_mod()`, `k_pow()`, `k_pct()`, `k_concat()`
- **Comparison:** `k_eq()`, `k_neq()`, `k_lt()`, `k_gt()`, `k_lte()`, `k_gte()`
- **Logic:** `k_truthy()`, `k_and()`, `k_or()`, `k_not()`
- **Math:** `k_sqrt()`, `k_abs()`, `k_floor()`, `k_ceil()`, `k_round()`, `k_log()`, `k_ln()`, `k_exp()`, `k_sin()`, `k_cos()`, `k_tan()`, `k_asin()`, `k_acos()`, `k_atan()`, `k_min2()`, `k_max2()`, `k_sum()`, `k_len()`
- **String (20+):** `k_Scap()` through `k_Sisascii()`
- **List (20+):** `k_Ladd()` through `k_Lsorted()`
- **Dict (10):** `k_Alen()` through `k_Acopy()`
- **Tuple (7):** `k_Zlen()` through `k_Zcount()`
- **Global (20+):** `k_type()` through `k_ltzip()`
- **I/O:** `k_ask()` for console input
- **Cast:** `k_I()`, `k_F()`, `k_S()`
- **Constants:** `K_PI`, `K_E`

### 7.2 `k_cli.h` (CLI Tools + Testing)

- ANSI color printing: `k_clired()`, `k_cligreen()`, `k_cliyellow()`, `k_clibold()`, `k_cliblue()`, `k_climagenta()`, `k_clicyan()`, `k_clidim()`, `k_cliunder()`
- `k_climenu()` — interactive text menu
- `k_cliprogress()` / `k_clipbset()` / `k_clipbdone()` — progress bar
- `k_cliargs()` / `k_cliflag()` / `k_cliopt()` — CLI argument parsing
- `k_testcheck()` / `k_testrun_summary()` — test assertion framework

### 7.3 `k_concurrency.h`

- Fork-based threading: `k_cxrun_bg()`, `k_cxjoin()`, `k_cxstop()`
- `k_cxqueue()` / `k_cxpush()` / `k_cxpop()` — thread queue
- `k_cxatom()` / `k_cxinc()` / `k_cxdec()` — atomic counter
- `k_cxlock()` / `k_cxacquire()` / `k_cxrelease()` — mutex
- `k_cxchan()` / `k_cxsend()` / `k_cxrecv()` — pipe-based channel
- `k_cxwait_secs()` — sleep
- `k_cxsem()` — counting semaphore
- `k_cxfuture()` / `k_cxresolve()` — future value

### 7.4 `k_crypto.h`

- MD5: `k_crmd5()` — pure C implementation
- SHA-256: `k_crsha256()`, `k_crsha()`, `k_crhash()`
- Base64: `k_crb64enc()`, `k_crb64dec()`
- UUIDv4: `k_cruuid()`
- Secure tokens: `k_crtoken()`
- XOR encryption: `k_crenc()`, `k_crdec()`
- Constant-time compare: `k_crcomp()`

### 7.5 `k_datastructs.h`

- **Stack** (list-based): `k_dsstack()`, `k_dspush()`, `k_dspop()`, `k_dspeek()`, `k_dssize()`
- **Queue** (list-based): `k_dsqueue()`, `k_dsenqueue()`, `k_dsdequeue()`
- **Set** (list-based, unique): `k_dsset()`, `k_dsadd()`, `k_dsrem()`, `k_dshas()`, `k_dsunion()`, `k_dsinter()`, `k_dsdiff()`
- **Linked List** (doubly-linked): `k_dsll()`, `k_dsllpush()`, `k_dsllpop()`, `k_dsllget()`
- **Tree** (dict-based): `k_dstree()`, `k_dstadd()`, `k_dstchildren()`, `k_dstparent()`
- **Graph** (adjacency list): `k_dsgraph()`, `k_dsgedge()`, `k_dsgnbr()`, `k_dsgpath()`
- **Heap** (binary heap): `k_dsheap()`, `k_dshpush()`, `k_dshpop()`
- **Deque** (list-based): `k_dsdeque()`, `k_dqdpushl()`, `k_dqdpushr()`, `k_dqdpopl()`, `k_dqdpopr()`
- **Matrix** (flat array): `k_dsmat()`, `k_dsmatset()`, `k_dsmatget()`, `k_dsmatmul()`, `k_dsmattrans()`, `k_dsmatprint()`

### 7.6 `k_datetime.h`

- `k_dtnow()` — current date/time as dict with all fields
- `k_dtdate()` / `k_dttime()` — ISO date/time strings
- `k_dtformat()` — custom format (DD-MM-YYYY, etc.)
- `k_dtparse()` — parse string to dt dict
- `k_dtstamp()` / `k_dtfromstamp()` — Unix timestamp conversion
- `k_dtadd()` / `k_dtsub()` — add/subtract time units
- `k_dtdiff()` — difference between two dates
- `k_dtconv()` — convert between time units
- `k_dtwait()` — sleep/hold
- `k_dtleap()` — leap year check
- `k_dtbefore()` / `k_dtafter()` / `k_dtequal()` — comparison
- `k_dtzone()` — timezone conversion (simplified)

### 7.7 `k_fileio.h`

- `k_fread()` / `k_fwrite()` / `k_fadd()` — read/write/append files
- `k_fdel()` / `k_frename()` / `k_fcopy()` — file management
- `k_fexists()` / `k_fsize()` / `k_fisdir()` / `k_fisfile()` — file info
- `k_flist()` — list directory (cross-platform: FindFirstFile on Windows, opendir on POSIX)
- `k_fmkdir()` / `k_frmdir()` — directory operations
- `k_fext()` / `k_fbase()` / `k_fdir()` / `k_fjoin()` — path manipulation
- `k_freadlines()` / `k_fwritelines()` — line-based I/O

### 7.8 `k_formats.h`

- **JSON:** Full recursive parser and serializer (`k_jsparse()`, `k_jsstring()`, `k_jsread()`, `k_jswrite()`)
- **CSV:** `k_csvread()`, `k_csvheads()`, `k_csvwrite()`, `k_csvparse()`
- **XML:** `k_xmlget()`, `k_xmlattr()`, `k_xmlparse()`, `k_xmlstring()`

### 7.9 `k_network.h`

- TCP socket HTTP/1.1 client: `k_netget()`, `k_netpost()`, `k_netput()`, `k_netpatch()`, `k_netdel()`
- `k_netjson()` — GET + parse JSON
- `k_netdns()` — DNS lookup (getaddrinfo)
- `k_netping()` — TCP ping
- `k_netdl()` — file download
- Cross-platform: Winsock2 on Windows, BSD sockets on POSIX

### 7.10 `k_os.h`

- `k_ossys()` — system info (OS, arch, hostname, RAM, username)
- `k_osram()` — memory usage (total, used, free, swap)
- `k_osuptime()` / `k_osboot()` — uptime info
- `k_osuser()` — user info (name, id, home, shell)
- `k_osrun()` — run shell command (captures stdout/stderr)
- `k_osget()` / `k_onset()` — environment variables
- `k_ospwd()` / `k_oscd()` — working directory
- `k_osopen()` — open file/URL with system handler
- `k_osclipget()` / `k_osclipset()` — clipboard
- `k_osnoti()` — desktop notification
- `k_oshalt()` — exit process

### 7.11 `k_regex.h`

- Uses POSIX extended regex on POSIX; substring-based fallback on Windows
- `k_rxmatch()`, `k_rxfull()`, `k_rxfind()`, `k_rxswap()`, `k_rxchop()`, `k_rxgroups()`, `k_rxcount()`

### 7.12 `k_ml.h` (Phase 5)

- Pure C tensor implementation and neural network
- **~1000 lines** of dense math code
- Forward pass through sequential models with linear, ReLU, sigmoid, softmax layers
- MSE, BCE, cross-entropy, MAE loss functions

### 7.13 `k_finance.h` (Phase 5)

- Stock data simulation with realistic price movements (geometric Brownian motion)
- Technical indicators: SMA, EMA, RSI, MACD, Bollinger Bands, ATR, OBV, VWAP
- Portfolio management and backtesting engine
- ASCII chart rendering
- Candlestick pattern detection (doji, hammer)

### 7.14 `k_game.h` (Phase 5)

- ncurses-based terminal game engine (with full Windows stubs)
- Sprite system with physics (position, velocity, acceleration)
- Collision detection (AABB, circle, point, group)
- Sprite groups for batch operations
- Sound beeps, font rendering, camera follow, state save/load

### 7.15 `k_ui.h` (Phase 5)

- ncurses-based TUI widget system (with full Windows stubs)
- Windows, labels, buttons, entries, text boxes, checkboxes, progress bars, sliders
- Grid and absolute positioning (`uigrid`, `uiplace`)
- Dialogs: info, confirm, input
- Clipboard and variable binding

### 7.16 `k_vis.h` (Phase 5)

- Simulated computer vision module
- Camera, frame, face detection, face mesh (468 landmarks), face recognition
- Hand tracking (21 landmarks), pose detection (33 landmarks)
- Object detection, segmentation, eye tracking, emotion, age/gender
- Optical flow, motion detection, color detection, QR, OCR, license plate
- Feature detection (ORB), template matching
- Drawing tools, video recording/playback

### 7.17 `k_zp.h` + `k_ser.h`

- Zip: system tar/powershell-based compression
- Serialization: custom binary format for KVal types (int, flt, str, bool, void, list, dict)

---

## 8. CLI Commands

The Katlans CLI (`files/katlans.py` or `katlans.exe`) provides:

| Command | Description | Example |
|---------|-------------|---------|
| `run` | Compile .kl → C → binary → execute | `katlans run hello.kl` |
| `build` | Compile .kl → standalone binary | `katlans build hello.kl -o hello` |
| `emit` | Print generated C source | `katlans emit hello.kl` |
| `test` | Run test suite (all or specific file) | `katlans test` or `katlans test hello.kl` |
| `tokens` | Debug: print token stream | `katlans tokens hello.kl` |
| `ast` | Debug: print AST | `katlans ast hello.kl` |
| `setup` | One-time setup (check Python, download compiler, add to PATH) | `katlans setup` |
| `update` | Self-update from GitHub releases | `katlans update` |

### 8.1 Auto-download Compiler

When no C compiler is found, `katlans run`/`build`/`setup` will:
1. Prompt user to download MinGW-w64 (~100 MB)
2. Download from WinLibs GitHub releases
3. Extract to `~/.katlans/mingw64/`
4. Add to PATH (cached permanently)
5. Compile the program

Supports both `.zip` (Python stdlib extraction) and `.7z` (requires 7-Zip) formats.

### 8.2 Self-Update

`katlans update` fetches the latest release from `github.com/katlans/katlans`, downloads the new `.exe`, and replaces the running binary (safe rename on Windows).

---

## 9. Build System

### 9.1 Makefile

```
make          # check-cc → emit C → compile binaries
make test     # Run test suite (Python)
make clean    # Remove build artifacts
make run-%    # Run specific .kl file (e.g., make run-hello)
```

### 9.2 PyInstaller Build

The standalone `katlans.exe` (8 MB) is built with:

```bash
pip install pyinstaller
python -m PyInstaller --onefile --name katlans \
  --add-data "files/src;src" \
  --add-data "files/runtime;runtime" \
  --add-data "files/test_runner.py;." \
  --add-data "files/hello.kl;." \
  ... (all 8 .kl files) \
  --hidden-import src.lexer \
  --hidden-import src.parser \
  --hidden-import src.codegen \
  --hidden-import src.compiler \
  --hidden-import src.errors \
  --hidden-import src.ast_nodes \
  --distpath . files/katlans.py
```

**Bundled in .exe:**
- `src/` — 7 compiler modules
- `runtime/` — 17 C headers
- `test_runner.py` — test suite
- 8 × `.kl` test files

### 9.3 Install Script

`install.py` — one-command setup:
- Checks/installs Python (3.10+)
- Installs PyInstaller
- Builds katlans.exe
- Sets up PATH

---

## 10. CI/CD (GitHub Actions)

`.github/workflows/build.yml` has 3 jobs:

### 10.1 Test (push + PR)

| OS | Python Versions |
|----|----------------|
| Ubuntu | 3.10, 3.12, 3.13 |
| macOS | 3.10, 3.12 |
| Windows | 3.10, 3.12 |

Runs: `python files/test_runner.py`

### 10.2 Build (push only, after tests)

- **Windows only** (`windows-latest`)
- Installs PyInstaller
- Builds `katlans.exe`
- Smoke tests: `emit hello.kl` + `test suite`
- Uploads artifact: `katlans-${{ github.sha }}`

### 10.3 Release (tag push `v*`)

- Downloads artifact from build job
- Publishes GitHub Release with auto-generated release notes

---

## 11. Test Suite

The test runner (`files/test_runner.py`) runs all `.kl` files through the `emit` pipeline (lex → parse → codegen).

### Test Files

| Test | Phase | What it Tests | Lines of C Generated |
|------|-------|---------------|---------------------|
| `hello.kl` | Foundation | Variables, string concat, math | ~16 |
| `control.kl` | Foundation | if/elif/else, for, while, for-each | ~30 |
| `functions.kl` | Foundation | Basic functions, recursion (factorial, square, cube) | ~40 |
| `test_all.kl` | Foundation | All basic types, operators, control flow, functions, try/catch, fib | ~80 |
| `test_phase2.kl` | Core | Template strings, string methods, list/dict/tuple methods, classes, global builtins | ~100 |
| `test_phase3.kl` | Std Lib | File I/O, OS, regex, JSON, crypto, CLI tools, testing framework | ~120 |
| `test_phase4.kl` | Extended | DateTime, networking, concurrency, data structures (stack, queue, set, heap, deque, matrix, tree, graph) | ~150 |
| `test_phase5.kl` | Power | ML tensors/models, finance tickers/indicators/portfolio, game sprites/collision, UI widgets, VIS camera/face/objects | ~200 |

**Validation checks:**
- `#include "katlans.h"` exists in output
- `int main(void)` present
- `return 0;` present
- Non-empty C output

---

## 12. Packaging & Distribution

### 12.1 Standalone .exe

```bash
# Build
python -m PyInstaller --onefile --name katlans ... files/katlans.py

# Distribute: just katlans.exe (8 MB)
# User needs: C compiler (auto-downloaded if missing)
```

### 12.2 Source Distribution

```bash
git clone https://github.com/katrate/katlans
cd katlans
python files/katlans.py test    # Run tests
python files/katlans.py setup   # One-time setup
```

### 12.3 Environment Variables

| Variable | Default | Purpose |
|----------|---------|---------|
| `KATLANS_REPO` | `katlans/katlans` | GitHub repo for self-update |
| `KATLANS_VERSION` | `0.1.0` | Current version |

---

## 13. Phase Summary

### Phase 1 — Foundation ✅
Hello World, variables, math operators, string concat, if/elif/else, for/while loops, functions, recursion, try/catch, basic I/O

### Phase 2 — Core ✅
Template strings, 30+ string methods, 20+ list methods, 10+ dict methods, tuple methods, classes with self, global builtins (chr, ord, hex, bin, etc.)

### Phase 3 — Standard Library ✅
File I/O (read/write/list/mkdir), OS operations (system info, RAM, user, env), regex (match/find/replace/split), JSON parse/stringify, CSV read/write, XML parse, MD5/SHA-256/Base64/UUID/XOR crypto, CLI colors/menus, test framework (testcheck/testrun)

### Phase 4 — Extended ✅
DateTime (now, format, parse, add, diff, timezone), networking (HTTP GET/POST/PUT/DELETE, DNS, ping, download), concurrency (channels, futures, atomic, lock), data structures (stack, queue, set, linked list, tree, graph, heap, deque, matrix)

### Phase 5 — Power Modules ✅
- **ML Module:** Tensor operations, sequential neural network, forward pass, loss functions
- **Finance Module:** Stock data, technical indicators, portfolios, backtesting, candlestick charts
- **Game Engine:** ncurses sprites, physics, collision detection, sprite groups, sound
- **UI Module:** ncurses windows, labels, buttons, entries, checkboxes, sliders, dialogs
- **VIS Module:** Camera, face detection/recognition, hand tracking, pose, objects, OCR, QR, video

---

## 14. Development Guide

### Quick Start

```bash
# From source
python files/katlans.py emit files/hello.kl    # See generated C
python files/katlans.py test                     # Run all tests

# Build standalone .exe
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

# Install script
python install.py
```

### Adding a Built-in Function

1. Add C implementation in the appropriate `files/runtime/k_*.h` header
2. Add mapping in `files/src/codegen.py` `BUILTIN_MAP` dict
3. If void-returning, add to `VOID_BUILTINS` set
4. Write a `.kl` test file or add to existing test
5. Run `python files/test_runner.py` to verify

### Adding a New Phase

1. Create test file: `files/test_phaseN.kl`
2. Update `files/src/codegen.py` for any new syntax
3. Update `files/test_runner.py` phase summary
4. Add `--add-data` to PyInstaller build command
5. Update `files/katlans.py` banner
6. Regenerate `.exe`

### Adding to CI

Edit `.github/workflows/build.yml`:
- Add new test matrix entries
- Add `--add-data` entries for new test files
- Update smoke test commands
