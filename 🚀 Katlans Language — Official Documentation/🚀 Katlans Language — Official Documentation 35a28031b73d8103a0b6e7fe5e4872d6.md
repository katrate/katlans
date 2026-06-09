# 🚀 Katlans Language — Official Documentation

Katlans (`.kl`) is a modern, feature-packed, imperative programming language that compiles to C (via gcc). It features inferred types, unique syntax, and everything built-in — no imports needed.

> **Philosophy:** Clean syntax. Powerful built-ins. Zero modules needed. Everything is inside the language.
> 

> ⚠️ **Block Syntax Correction:** Block open = `|>` (pipe then greater-than). Block close = `<|::` (less-than then pipe then double-colon). Example: `if x > 0 |> ... <|::`. The table below may render these incorrectly due to Notion's pipe character handling.
> 

---

## 📚 Extended Module Library

> Click any module to open its full reference page.
> 

| Module | Prefix | Description |
| --- | --- | --- |
| [🪟 Katlans UI Module — Complete Window & Widget Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%AA%9F%20Katlans%20UI%20Module%20%E2%80%94%20Complete%20Window%20&%20Widget%20Ref%2035a28031b73d8152ba1dd1a89fad49b0.md) | `ui` | Complete windowed UI — all tkinter functionality |
| [📷 Katlans VIS Module — Complete Computer Vision Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%93%B7%20Katlans%20VIS%20Module%20%E2%80%94%20Complete%20Computer%20Vision%20Re%2035b28031b73d813791bcc117161535bf.md) | `vis` | Camera, face, hands, body, OCR, motion detection |
| [🧠 Katlans ML Module — Machine Learning Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%A7%A0%20Katlans%20ML%20Module%20%E2%80%94%20Machine%20Learning%20Reference%2035b28031b73d81c6a9d4e641de638e22.md) | `ml` | PyTorch + TensorFlow style machine learning |
| [🎮 Katlans Game Module — Complete Game Engine Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%8E%AE%20Katlans%20Game%20Module%20%E2%80%94%20Complete%20Game%20Engine%20Refer%2035b28031b73d8111b4a1db82f0c1b82e.md) | `gm` | Full game engine — Pygame style |
| [📈 Katlans Finance Module — Stocks, Crypto & Markets Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%93%88%20Katlans%20Finance%20Module%20%E2%80%94%20Stocks,%20Crypto%20&%20Market%2035b28031b73d81fca3ebc32e1e3a5392.md) | `fin` | Stocks, crypto, forex, charts, backtesting |

| [📈 Finance Module](https://www.notion.so/35b28031b73d81fca3ebc32e1e3a5392) | `fin` | Stocks, crypto, forex, charts, backtesting |
| --- | --- | --- |
| [🧰 Built-ins & Datatype Methods](https://www.notion.so/36228031b73d8171a317fe68d7a336e6) | various | All Python built-in functions mapped to Katlans |

---

| Rule | Symbol | Example |
| --- | --- | --- |
| Statement end | `::` | `int x = 10 ::` |
| Block open | `PIPE>` — type as `\ | >` |
| Block close | `<PIPE::` — type as `<\ | ::` |
| Single comment | `~` | `~ this is a comment` |
| Multi-line comment | `~~ ... ~~` | `~~ hello world ~~` |
| Function define | `fxn dec <name> <params>` | `fxn dec <add> <a,b>` |
| Function call | `fxn <name> <args>` | `fxn add <5,10>` |
| Built-in call | `builtinName <args>` | `fread <"file.txt">` |

> **Block syntax:** open block with `|>` (pipe + greater-than), close block with `<|::` (less-than + pipe + double-colon)
> 

| Rule | Symbol | Example |  |  |
| --- | --- | --- | --- | --- |
| Statement end | `::` | `int x = 10 ::` |  |  |
| Block open | `\ | >` | `if x > 0 \ | >` |
| Block close | `<\ | ::` | `<\ | ::` |
| Single comment | `~` | `~ this is a comment` |  |  |
| Multi-line comment | `~~ ... ~~` | `~~ hello world ~~` |  |  |
| Function define | `fxn dec <name> <params>` | `fxn dec <add> <a,b>` |  |  |
| Function call | `fxn <name> <args>` | `fxn add <5,10>` |  |  |
| Built-in call | `builtinName <args>` | `fread <"file.txt">` |  |  |

| Rule | Symbol | Example |
| --- | --- | --- |
| Statement end | `::` | `int x = 10 ::` |
| Block open | `\ | >` |
| Block close | `<\ | ::` |
| Single comment | `~` | `~ this is a comment` |
| Multi-line comment | `~~ ... ~~` | `~~ hello world ~~` |
| Function define | `fxn dec <name> <params>` | `fxn dec <add> <a,b>` |
| Function call | `fxn <name> <args>` | `fxn add <5,10>` |
| Built-in call | `builtinName <args>` | `fread <"file.txt">` |

---

## 📦 Variables & Types

Katlans uses **inferred typing** — the compiler figures out types. You only declare a type keyword when it's not a string.

```
~ String (default — no keyword needed)
x = "hello" ::

~ Integer
int x = 10 ::

~ Float
flt x = 10.23 ::

~ List (mutable)
lt x = "help", 10, 10.3 ::

~ Zelo / Tuple (immutable)
zl x = "help", 10, 10.3 ::

~ Adero / Dict (mutable)
ad x = {name,"John"}; {age,25} ::

~ Boolean
x = true ::
x = false ::

~ Empty / Null
x = void ::
```

### Mutability Rules

| Type | Mutable? |
| --- | --- |
| string | ❌ immutable |
| int / flt | ❌ immutable |
| zl (tuple) | ❌ immutable |
| lt (list) | ✅ mutable |
| ad (adero/dict) | ✅ mutable |

---

## 🔢 Operators

### Arithmetic

| Operator | Meaning | Example | Output |
| --- | --- | --- | --- |
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `5 - 3` | `2` |
| `*` | Multiply | `5 * 3` | `15` |
| `/` | Divide | `10 / 2` | `5.0` |
| `//` | Integer divide | `10 // 3` | `3` |
| `///` | Modulo | `10 /// 3` | `1` |
| `^` | Power | `2 ^ 8` | `256` |
| `%` | Percentage | `20 % 100` | `20` |

### Comparison

| Operator | Meaning |
| --- | --- |
| `==` | Equal |
| `=\` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `=<` | Less or equal |
| `=>` | Greater or equal |

### Logical

| Operator | Meaning | Example |
| --- | --- | --- |
| `&&` | AND | `x > 0 && x < 10` |
| `&+` | OR | `x == 0 &+ x == 1` |
| `!&` | NOT | `!& x == 5` |

---

## 🔀 Control Flow

### Conditionals

```
if x > 10 |>
    display "big" ::
<|::
elif x == 5 |>
    display "five" ::
<|::
else |>
    display "small" ::
<|::
```

### For Loop

```
deta i belongs range(1;10;1) |>
    display i ::
<|::
```

### For-Each Loop

```
lt fruits = "apple", "banana", "mango" ::
deta i belongs in fruits |>
    display i ::
<|::
```

### While Loop

```
int x = 0 ::
denta x < 5 |>
    display x ::
    x = x + 1 ::
<|::
```

### Loop Control

```
halt ::       ~ break
continue ::   ~ skip to next iteration
```

---

## 🔧 Functions

```
~ Define
fxn dec <add> <a, b> |>
    return a + b ::
<|::

~ Call
fxn add <5, 10> ::

~ With return value
int result = fxn add <5, 10> ::

~ Multiple return values
fxn dec <minmax> <lt> <a, b> |>
    return min(lt), max(lt) ::
<|::

lt nums = 3, 1, 7, 2 ::
fxn minmax <nums> ::
display a ::   ~ 1
display b ::   ~ 7
```

---

## 🏛️ Classes & OOP

```
cls dec <Animal> |>
    name = void ::
    int age = 0 ::

    fxn dec <init> <name, age> |>
        self.name = name ::
        self.age = age ::
    <|::

    fxn dec <speak> |>
        display self.name ::
    <|::
<|::

~ Create instance
Animal dog = cls Animal <"Rex", 5> ::
fxn dog.speak ::

~ Inheritance
cls dec <Dog> from <Animal> borrow * |>
    ~ borrows everything from Animal
<|::

cls dec <Cat> from <Animal> borrow speak |>
    ~ borrows only speak
<|::
```

---

## 🔤 String Operations (capital S prefix)

```
x = "Hello World" ::

Slen(x)                          ~ 11
Scap(x)                          ~ "HELLO WORLD"
Slow(x)                          ~ "hello world"
Snip("  hi  ")                   ~ "hi"
Schop(x, " ")                    ~ ["Hello", "World"]
Sswap(x, "World", "Katlans")     ~ "Hello Katlans"
x Speeks "World"                 ~ true
x Sfront "Hello"                 ~ true
x Sback "World"                  ~ true

~ Concatenation
result = "Hello" ++ " World" ::

~ Slicing
result = x[0;5] ::               ~ "Hello"

~ Template strings
name = "John" ::
display "Hello {name}" ::        ~ "Hello John"
```

---

## 🔄 Type Casting

```
I("10")    ~ string to integer  →  10
S(10)      ~ any to string      →  "10"
F("3.5")   ~ string to float   →  3.5
```

---

## 📐 Math Functions

```
sqrt(x)  ln(x)  log(x)  exp(x)
ceil(x)  floor(x)  round(x, 2)
min(a,b)  max(a,b)  fact(x)
gcd(a,b)  lcm(a,b)  |x|
rand(1;100)  sum(myList)

~ Trig
sin cos tan sec csc cot
arcsin arccos arctan

~ Constants
PI    E
```

---

## 📁 File I/O (f prefix)

```
x = fread <"file.txt"> ::
fwrite <"file.txt"> <"content"> ::
fadd <"file.txt"> <"append"> ::
fdel <"file.txt"> ::
fexists <"file.txt"> ::
lt files = flist <"./folder"> ::
fmkdir <"newfolder"> ::
frmdir <"folder"> ::
```

---

## 🌐 Networking (net prefix)

```
x = netget <"url"> ::
x = netpost <"url"> <{data,"val"}> ::
x = netput / netpatch / netdel ...

~ Options
netget <"url"> headers <{auth,"token"}> ::
netget <"url"> timeout <5> ::
netget <"url"> auth <"Bearer token"> ::
netget <"url"> query <{page,"1"}> ::
netget <"url"> retry <3> ::
netget <"url"> proxy <"http://proxy:8080"> ::
netget <"url"> redirect <false> ::

~ Response
x.body  x.status  x.headers  x.url  x.ok

~ JSON auto-parse
ad data = netjson <"url"> ::

~ Other
netdl <"url"> <"file.zip"> ::
netup <"url"> <"file.txt"> ::
netdns <"domain.com"> ::
netping <"domain.com"> ::
netmail <"to@x.com"> <"subj"> <"body"> auth <"smtp://..."> ::
netsock / netsend / netrecv  ~ WebSocket
```

---

## ⏰ Date & Time (dt prefix)

```
x = dtnow() :: dtdate() :: dttime() ::
x.year / x.month / x.week / x.day / x.hour / x.min / x.sec / x.ms

dtformat <x, "DD-MM-YYYY"> ::
x = dtparse <"08-05-2026", "DD-MM-YYYY"> ::

dtadd <dtnow(), 7, "day"> ::
dtsub <dtnow(), 1, "month"> ::
dtdiff <date1, date2, "day"> ::
dtconv <5, "min", "sec"> ::     ~ 300

dtstamp <dtnow()> ::
dtfromstamp <1746700000> ::
dtzone <dtnow(), "UTC", "IST"> ::
dtallzones() ::

dtwait <5> ::
dtwait <500, "ms"> ::
dtbench <fxn myFxn> ::

dtleap <2024> ::
dtbefore / dtafter / dtequal <date1, date2> ::
```

---

## 🔄 Concurrency (cx prefix)

```
fxn dec async <fetchData> <url> |> ... <|::
cxwait / cxrun / cxthread / cxstart / cxjoin / cxstop
cxall / cxgather
cxlock / cxacquire / cxrelease
cxqueue / cxpush / cxpop
cxchan / cxsend / cxrecv
cxon / cxemit
cxsem / cxfuture / cxresolve
cxrace / cxretry / cxpool / cxsubmit
cxcancel / cxabort / cxtimeout
cxatom / cxinc / cxdec

~ Parallel loop
cxdeta i belongs range(1;10;1) |> ... <|::
```

---

## 🗃️ Data Structures (ds prefix)

```
~ Stack:        dsstack / dspush / dspop / dspeek / dssize
~ Queue:        dsqueue / dsenqueue / dsdequeue
~ Set:          dsset / dsadd / dsrem / dshas / dsunion / dsinter / dsdiff
~ Linked List:  dsll / dsllpush / dsllpop / dsllget
~ Tree:         dstree / dstadd / dstchildren / dstparent
~ Graph:        dsgraph / dsgedge / dsgnbr / dsgpath
~ Heap:         dsheap / dshpush / dshpop
~ Deque:        dsdeque / dqdpushl / dqdpushr / dqdpopl / dqdpopr
~ Matrix:       dsmat / dsmatset / dsmatget / dsmatmul / dsmattrans
```

---

## 🔍 Regex (rx prefix)

```
rxmatch <"hello123", "[a-z]+"> ::
rxfind <"hello world", "\w+"> ::
rxswap <"hello123", "[0-9]", ""> ::
rxchop <"a1b2c3", "[0-9]"> ::
rxgroups <"2026-05-08", "(\d+)-(\d+)-(\d+)"> ::
rxfull <"abc123", "[a-z]+[0-9]+"> ::
```

---

## 💻 OS Operations (os prefix)

```
~ System: ossys / oscpu / osram / osdisk / osgpu / osnet / osbat
~ Processes: osprocs / osfind / oskill
~ Uptime: osuptime / osboot
~ Screen: osscreen
~ Clipboard: osclipget / osclipset
~ Notifications: osnoti
~ Open: osopen
~ Env: osget / onset
~ Shell: osrun
~ Paths: osjoin / osbase / osdir / osext / ospath
~ User: osuser
~ Exit: oshalt
```

---

## 📊 Data Formats

```
~ JSON:  jsparse / jsstring / jswrite / jsread
~ CSV:   csvread / csvwrite / csvheads
~ XML:   xmlparse / xmlget / xmlstring
```

---

## 🔐 Cryptography (cr prefix)

```
crmd5 / crsha / crhash
crenc / crdec
crb64enc / crb64dec
cruuid / crtoken / crcomp
```

---

## 🧪 Testing (test prefix)

```
test dec <myTest> |>
    testcheck <x == 5> ::
<|::
testrun() ::
```

---

## 🗜️ Compression (zp prefix)

```
zppack / zpunpack / zplist / zpadd / zprem
```

---

## 📡 Serialization (ser prefix)

```
serencode / serdecode / serbytes / serfrombytes
```

---

## 🖥️ CLI Tools (cli prefix)

```
cliargs / cliflag / cliopt
clired / cligreen / cliyellow / clibold
cliset / clidone / climenu
```

---

## ⚠️ Error Handling

```
try |>
    x = fread <"missing.txt"> ::
<|::
catch <error> |>
    display error ::
<|::
```

---

## 📝 Full Hello World Example

```
~ Katlans Hello World
display "Hello, World!" ::

name = ask <"Enter your name: "> ::
display "Hello {name}" ::

fxn dec <greet> <name> |>
    display "Welcome, {name}!" ::
<|::

fxn greet <name> ::

deta i belongs range(1;5;1) |>
    display i ::
<|::

cls dec <Person> |>
    fxn dec <init> <name, age> |>
        self.name = name ::
        self.age = age ::
    <|::
<|::

Person p = cls Person <"Alice", 25> ::
display p.name ::
```

---

> 📌 **Katlans is currently under development. This document reflects the full language specification as designed.**
> 

[🪟 Katlans UI Module — Complete Window & Widget Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%AA%9F%20Katlans%20UI%20Module%20%E2%80%94%20Complete%20Window%20&%20Widget%20Ref%2035a28031b73d8152ba1dd1a89fad49b0.md)

[📷 Katlans VIS Module — Complete Computer Vision Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%93%B7%20Katlans%20VIS%20Module%20%E2%80%94%20Complete%20Computer%20Vision%20Re%2035b28031b73d813791bcc117161535bf.md)

[🧠 Katlans ML Module — Machine Learning Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%A7%A0%20Katlans%20ML%20Module%20%E2%80%94%20Machine%20Learning%20Reference%2035b28031b73d81c6a9d4e641de638e22.md)

[📈 Katlans Finance Module — Stocks, Crypto & Markets Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%93%88%20Katlans%20Finance%20Module%20%E2%80%94%20Stocks,%20Crypto%20&%20Market%2035b28031b73d81fca3ebc32e1e3a5392.md)

[🧱 Katlans Data Types — Complete Built-in Methods Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%A7%B1%20Katlans%20Data%20Types%20%E2%80%94%20Complete%20Built-in%20Methods%20R%2035c28031b73d81748ef5d3c0fc27a5a6.md)

[🧰 Katlans Built-ins & Complete Datatype Methods](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%A7%B0%20Katlans%20Built-ins%20&%20Complete%20Datatype%20Methods%2035c28031b73d8164ae55e07052258a93.md)

[🎮 Katlans Game Module — Complete Game Engine Reference](%F0%9F%9A%80%20Katlans%20Language%20%E2%80%94%20Official%20Documentation/%F0%9F%8E%AE%20Katlans%20Game%20Module%20%E2%80%94%20Complete%20Game%20Engine%20Refer%2035b28031b73d8111b4a1db82f0c1b82e.md)