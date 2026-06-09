# 🧰 Katlans Built-ins & Complete Datatype Methods

> Every Python built-in function and datatype method, mapped to Katlans syntax.
> 

---

## 🔧 Built-in Functions

### I/O & Conversion (already in Katlans)

```
display x ::          ~ print(x)
x = ask <"prompt"> :: ~ input()
I(x)                  ~ int(x)
S(x)                  ~ str(x)
F(x)                  ~ float(x)
```

### Type & Identity

```
x = typeof <val> ::         ~ type(x)        → "int"/"str"/"lt" etc.
true/false = istype <val, "int"> ::  ~ isinstance(x, int)
true/false = isparent <Dog, Animal> :: ~ issubclass(Dog, Animal)
x = idof <val> ::           ~ id(x)          → unique memory id
x = hashof <val> ::         ~ hash(x)
x = repr <val> ::           ~ repr(x)        → printable representation
x = dirof <val> ::          ~ dir(x)         → list of attributes
x = varsof <val> ::         ~ vars(x)        → __dict__ of object
true/false = iscall <val> :: ~ callable(x)
```

### Number Conversion

```
x = tobin <255> ::          ~ bin(255)        → "0b11111111"
x = tooct <8> ::            ~ oct(8)          → "0o10"
x = tohex <255> ::          ~ hex(255)        → "0xff"
x = tochr <65> ::           ~ chr(65)         → "A"
x = tord <"A"> ::           ~ ord("A")        → 65
int a, int b = divmod <10, 3> ::  ~ divmod(10,3) → 3, 1
x = fmt <3.14159, ".2f"> :: ~ format(3.14159, ".2f") → "3.14"
x = complex <3, 4> ::       ~ complex(3, 4)   → 3+4j
```

### Iteration & Functional

```
lt pairs = ltenum <lt> ::          ~ enumerate(lt)  → [(0,"a"),(1,"b")]
lt result = ltmap <lt, fxn fn> ::  ~ map(fn, lt)
lt result = ltfilter <lt, fxn fn> :: ~ filter(fn, lt)
lt result = ltzip <lt1, lt2> ::    ~ zip(lt1, lt2)
lt result = ltsort <lt> ::         ~ sorted(lt)
lt result = ltsort <lt, reverse<true>> :: ~ sorted(lt, reverse=True)
lt result = ltsort <lt, fxn key> :: ~ sorted(lt, key=fn)
lt result = ltrev <lt> ::          ~ reversed(lt)
true/false = ltall <lt> ::         ~ all(lt)
true/false = ltany <lt> ::         ~ any(lt)

~ Iterator protocol
it = ltiter <lt> ::                ~ iter(lt)
x = ltnext <it> ::                 ~ next(it)
x = ltnext <it, "default"> ::      ~ next(it, default)
x = ltslice <0, 5, 1> ::          ~ slice(0, 5, 1)
```

### Math

```
~ Already in Katlans
sum(lt)  min(a,b)  max(a,b)  abs(x)  round(x,n)  pow(x,y)

~ Additional
x = absof <x> ::    ~ abs(x) — same as |x|
```

### Object Attributes

```
x = getattr <obj, "name"> ::          ~ getattr(obj, "name")
x = getattr <obj, "name", default> :: ~ getattr(obj, "name", default)
setattr <obj, "name", val> ::          ~ setattr(obj, "name", val)
true/false = hasattr <obj, "name"> :: ~ hasattr(obj, "name")
delattr <obj, "name"> ::              ~ delattr(obj, "name")
```

### Eval & Exec

```
x = kleval <"2 + 2"> ::   ~ eval("2 + 2")    → 4
klexec <"x = 10"> ::      ~ exec("x = 10")
```

### Scope & Globals

```
ad g = klglobals() ::     ~ globals()
ad l = kllocals() ::      ~ locals()
```

### Bytes & Binary

```
x = tobytes <"hello", "utf-8"> ::    ~ bytes("hello", "utf-8")
x = tobytearray <"hello", "utf-8"> :: ~ bytearray()
x = tomemview <x> ::                  ~ memoryview(x)
```

### Frozen Set

```
st = frzset <1, 2, 3> ::   ~ frozenset([1,2,3])  → immutable set
```

### Generic Length

```
int n = lenof <x> ::   ~ len(x) — works on any type
```

### OOP Helpers

```
super() ::                  ~ super()
prop fxn myProp() ::        ~ @property
static fxn myFxn() ::       ~ @staticmethod
cls fxn myFxn() ::          ~ @classmethod
```

---

## 🔤 Complete String Methods (S prefix — capital S)

### Already in Katlans

```
Slen(x)          Scap(x)         Slow(x)         Snip(x)
Schop(x, sep)    Sswap(x,a,b)    Speeks          Sfront          Sback
++ (concat)      x[0;5] (slice)  "{name}" (template)
```

### New — Added Now

```
~ Case
Scapfirst <x> ::           ~ capitalize()    → "hello" → "Hello"
Sfold <x> ::               ~ casefold()      → aggressive lowercase for comparison
Sswapcase <x> ::           ~ swapcase()      → "Hello" → "hELLO"
Stitle <x> ::              ~ title()         → "hello world" → "Hello World"
Sisupper <x> ::            ~ isupper()       → true/false
Sislower <x> ::            ~ islower()       → true/false
Sistitle <x> ::            ~ istitle()       → true/false

~ Padding & Alignment
Scenter <x, 20> ::         ~ center(20)      → "  hello   "
Scenter <x, 20, "*"> ::   ~ center(20, "*") → "**hello***"
Sljust <x, 20> ::          ~ ljust(20)
Sljust <x, 20, "."> ::    ~ ljust(20, ".")
Srjust <x, 20> ::          ~ rjust(20)
Srjust <x, 20, "."> ::    ~ rjust(20, ".")
Szfill <x, 5> ::           ~ zfill(5)        → "42" → "00042"

~ Strip
Slstrip <x> ::             ~ lstrip()
Slstrip <x, "."> ::        ~ lstrip(".")
Srstrip <x> ::             ~ rstrip()
Srstrip <x, "."> ::        ~ rstrip(".")

~ Search & Find
int i = Sfind <x, "lo"> ::       ~ find()    → first index, -1 if not found
int i = Sfind <x, "lo", 2> ::   ~ find(sub, start)
int i = Srfind <x, "lo"> ::      ~ rfind()   → last index
int i = Sindex <x, "lo"> ::      ~ index()   → like find but error if not found
int i = Srindex <x, "lo"> ::     ~ rindex()
int n = Scount <x, "l"> ::       ~ count()   → count occurrences
int n = Scount <x, "l", 0, 5> :: ~ count(sub, start, end)

~ Split
lt lines = Ssplitlines <x> ::    ~ splitlines()
lt parts = Srsplit <x, " "> ::   ~ rsplit()
lt parts = Srsplit <x, " ", 1> :: ~ rsplit(maxsplit=1)

~ Partition
lt p = Spartition <x, "."> ::    ~ partition()  → ["hello", ".", "world"]
lt p = Srpartition <x, "."> ::   ~ rpartition() → splits from right

~ Prefix / Suffix
Sremoveprefix <x, "Mr. "> ::     ~ removeprefix()
Sremovesuffix <x, "!"> ::        ~ removesuffix()

~ Join
x = Sjoin <", ", myList> ::      ~ ", ".join(myList) → "a, b, c"

~ Encoding
x = Sencode <x, "utf-8"> ::      ~ encode("utf-8")

~ Tabs
x = Sexpand <x, 4> ::            ~ expandtabs(4)

~ Validation — all return true/false
Sisalnum <x> ::    ~ isalnum()     → letters and numbers only
Sisalpha <x> ::    ~ isalpha()     → letters only
Sisascii <x> ::    ~ isascii()     → all ASCII
Sisdecimal <x> ::  ~ isdecimal()   → decimal characters only
Sisdigit <x> ::    ~ isdigit()
Sisnumeric <x> ::  ~ isnumeric()
Sisprintable <x> :: ~ isprintable()
Sisspace <x> ::    ~ isspace()
Sisidentifier <x> :: ~ isidentifier() → valid variable name?

~ Format
x = Sformat <"{} is {}", "name", "John"> ::  ~ str.format()
```

---

## 📋 Complete List Methods (L prefix)

### Already in Katlans

```
Ladd     Ldrop    Llen    Lsort    Lflip    Lhas    lt[0]
```

### New — Added Now

```
~ Add / Insert
Lappend <lt, val> ::            ~ append()  → add to end (same as Ladd)
Lextend <lt1, lt2> ::           ~ extend()  → add all items of lt2 to lt1
Linsert <lt, 2, "x"> ::        ~ insert(index, val)

~ Remove
Lpop <lt> ::                    ~ pop()     → remove & return last item
Lpop <lt, 2> ::                 ~ pop(2)    → remove & return at index
Lclear <lt> ::                  ~ clear()   → remove all items

~ Search
int i = Lindex <lt, "val"> ::  ~ index(val)  → first index of value
int i = Lindex <lt, "val", 2> :: ~ index(val, start)
int n = Lcount <lt, "val"> ::  ~ count(val)  → count occurrences

~ Copy
lt copy = Lcopy <lt> ::         ~ copy()   → shallow copy

~ Sort
Lsort <lt> ::                   ~ sort()          → in-place ascending
Lsort <lt, reverse<true>> ::    ~ sort(reverse=True)
Lsort <lt, fxn key> ::          ~ sort(key=fn)

~ Concat / Multiply
lt result = lt1 ++ lt2 ::       ~ lt1 + lt2   → combine lists
lt result = lt * 3 ::           ~ lt * 3      → repeat list 3 times

~ Check
true/false = ltall <lt> ::      ~ all(lt)
true/false = ltany <lt> ::      ~ any(lt)
int n = lenof <lt> ::           ~ len(lt)
```

---

## 🗂️ Complete Adero (Dict) Methods (A prefix)

### Already in Katlans

```
Aadd    Adrop    Alen    ad[key]    Ahas
```

### New — Added Now

```
~ Access
x = Aget <ad, "key"> ::              ~ get(key)         → void if not found
x = Aget <ad, "key", "default"> ::  ~ get(key, default)
lt keys = Akeys <ad> ::              ~ keys()
lt vals = Avalues <ad> ::            ~ values()
lt pairs = Aitems <ad> ::            ~ items()  → [(k,v), (k,v)...]

~ Modify
Aupdate <ad1, ad2> ::               ~ update()  → merge ad2 into ad1
Aupdate <ad, {name,"John"}> ::      ~ update with single pair
x = Apop <ad, "key"> ::             ~ pop(key)
x = Apop <ad, "key", "default"> ::  ~ pop(key, default)
x = Apopitem <ad> ::                ~ popitem()  → remove & return last pair
Asetdefault <ad, "key", "val"> ::   ~ setdefault() → set if key missing

~ Copy & Create
ad copy = Acopy <ad> ::             ~ copy()
ad new = Afromkeys <lt, 0> ::       ~ fromkeys(lt, 0)  → {k:0 for k in lt}

~ Clear
Aclear <ad> ::                      ~ clear()

~ Iteration
deta k belongs in Akeys <ad> |>
    display k ::
<|::

deta v belongs in Avalues <ad> |>
    display v ::
<|::

deta k, v belongs in Aitems <ad> |>
    display k / v ::
<|::

~ Check
true/false = Ahas <ad, key> ::      ~ key in dict
int n = Alen <ad> ::                ~ len(dict)
```

---

## 📦 Complete Zelo (Tuple) Methods (Z prefix)

```
~ Zelo is immutable — only 2 methods

int n = Zcount <zl, val> ::    ~ count(val)  → count occurrences
int i = Zindex <zl, val> ::    ~ index(val)  → first index
int i = Zindex <zl, val, 2> :: ~ index(val, start)

~ Other operations
int n = lenof <zl> ::          ~ len(tuple)
true/false = val Zhas zl ::    ~ val in tuple
zl2 = zl ++ zl2 ::            ~ concatenate tuples
zl2 = zl * 3 ::               ~ repeat
lt result = ltrev <zl> ::      ~ reversed(tuple)
```

---

## 🔵 Complete Set Methods (ds prefix extended)

### Already in Katlans

```
dsadd    dsrem    dshas    dsunion    dsinter    dsdiff    dssize
```

### New — Added Now

```
~ Copy & Clear
st2 = dscopy <st> ::                    ~ copy()
dsclear <st> ::                         ~ clear()

~ Remove (safe vs unsafe)
dsrem <st, val> ::                      ~ remove()  → error if not found
dsdiscard <st, val> ::                  ~ discard() → no error if not found
x = dspop <st> ::                       ~ pop()     → remove & return arbitrary item

~ Set operations — return new set
st3 = dsunion <st1, st2> ::             ~ union()            st1 | st2
st3 = dsinter <st1, st2> ::             ~ intersection()     st1 & st2
st3 = dsdiff <st1, st2> ::             ~ difference()       st1 - st2
st3 = dssymdiff <st1, st2> ::          ~ symmetric_difference()  st1 ^ st2

~ In-place operations — modify st1 directly
dsunion_update <st1, st2> ::            ~ update()                  st1 |= st2
dsinter_update <st1, st2> ::            ~ intersection_update()     st1 &= st2
dsdiff_update <st1, st2> ::             ~ difference_update()       st1 -= st2
dssymdiff_update <st1, st2> ::          ~ symmetric_difference_update()

~ Comparison
true/false = dssubset <st1, st2> ::     ~ issubset()    → st1 ⊆ st2
true/false = dssuperset <st1, st2> ::   ~ issuperset()  → st1 ⊇ st2
true/false = dsdisjoint <st1, st2> ::   ~ isdisjoint()  → no common elements

~ Frozen set (immutable)
st = frzset <1, 2, 3> ::               ~ frozenset()
```

---

## 🔢 Int Methods (I prefix)

```
int n = Ibitlen <x> ::              ~ bit_length()     → 255 → 8
int n = Ibitcount <x> ::            ~ bit_count()      → count 1-bits
lt b = Itobytes <255, 2, "big"> ::  ~ to_bytes(2, "big")  → bytes
int n = Ifrombytes <b, "big"> ::    ~ from_bytes(b, "big")
int n = Iconjugate <x> ::           ~ conjugate() → same as x for real ints
```

---

## 🔣 Float Methods (F prefix)

```
true/false = Fisint <x> ::          ~ is_integer()        → 3.0 → true
lt r = Fasratio <x> ::              ~ as_integer_ratio()  → 0.5 → [1, 2]
flt n = Fconjugate <x> ::           ~ conjugate()
```

---

## 📊 Complete Examples

### String

```
x = "hello world" ::
display Scapfirst <x> ::     ~ "Hello world"
display Stitle <x> ::        ~ "Hello World"
display Scenter <x, 20, "*"> ::  ~ "****hello world*****"
display Scount <x, "l"> ::   ~ 3
display Sfind <x, "world"> :: ~ 6
display Spartition <x, " "> :: ~ ["hello", " ", "world"]
display Sjoin <"-", lt<"a","b","c">> :: ~ "a-b-c"
display Sisalpha <"hello"> :: ~ true
display Sisdigit <"123"> ::   ~ true
display Szfill <"42", 5> ::   ~ "00042"
```

### List

```
lt nums = 3, 1, 4, 1, 5 ::
Linsert <nums, 0, 99> ::      ~ [99, 3, 1, 4, 1, 5]
display Lcount <nums, 1> ::   ~ 2
display Lindex <nums, 4> ::   ~ 3
display Lpop <nums> ::        ~ 5 (and removes it)
Lextend <nums, lt<6,7,8>> ::  ~ adds 6,7,8 to end
lt copy = Lcopy <nums> ::
Lclear <nums> ::              ~ now empty
```

### Adero

```
ad person = {name,"John"}; {age,25} ::
display Aget <person, "name"> ::         ~ "John"
display Aget <person, "city", "N/A"> ::  ~ "N/A"
display Akeys <person> ::    ~ ["name", "age"]
display Avalues <person> ::  ~ ["John", 25]
display Aitems <person> ::   ~ [["name","John"],["age",25]]
Apop <person, "age"> ::      ~ removes age
Asetdefault <person, "city", "Delhi"> :: ~ adds city if missing
ad p2 = Acopy <person> ::
ad blank = Afromkeys <lt<"a","b","c">, 0> :: ~ {a:0, b:0, c:0}
```

### Built-ins

```
display typeof <42> ::        ~ "int"
display typeof <"hi"> ::      ~ "str"
display istype <42, "int"> :: ~ true
display tobin <10> ::         ~ "0b1010"
display tohex <255> ::        ~ "0xff"
display tochr <65> ::         ~ "A"
display tord <"A"> ::         ~ 65

lt nums = 1, 2, 3, 4, 5 ::
lt evens = ltfilter <nums, fxn isEven> ::
lt doubled = ltmap <nums, fxn double> ::
lt pairs = ltenum <nums> ::    ~ [(0,1),(1,2),(2,3)...]

display ltall <lt<true,true,false>> ::  ~ false
display ltany <lt<false,false,true>> :: ~ true

int a, int b = divmod <17, 5> ::
display a ::   ~ 3
display b ::   ~ 2

display fmt <3.14159, ".2f"> ::  ~ "3.14"
```