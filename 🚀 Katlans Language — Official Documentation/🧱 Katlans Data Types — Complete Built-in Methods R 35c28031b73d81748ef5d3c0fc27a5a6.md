# 🧱 Katlans Data Types — Complete Built-in Methods Reference

> Every Python built-in datatype method, mapped to Katlans syntax.
> 

> Prefix system: `S` = String, `L` = List, `A` = Adero/Dict, `Z` = Zelo/Tuple, `ST` = Set, `N` = Numeric, `B` = Bytes
> 

---

## 🔤 String Methods (S prefix)

```
x = "Hello World" ::

~ Case
Scap(x)                        ~ "HELLO WORLD"       (upper)
Slow(x)                        ~ "hello world"       (lower)
Sswapcase(x)                   ~ "hELLO wORLD"
Stitle(x)                      ~ "Hello World"
Scapfirst(x)                   ~ "Hello world"       (capitalize)
Scasefold(x)                   ~ "hello world"       (aggressive lower)

~ Search
Sfind(x, "World")              ~ 6                   (first index, -1 if not found)
Srfind(x, "l")                 ~ 9                   (last index)
Sindex(x, "World")             ~ 6                   (raises error if not found)
Srindex(x, "l")                ~ 9
Scount(x, "l")                 ~ 3
Scount(x, "l", 0, 5)           ~ 2                   (count in range)

~ Check
x Sfront "Hello"               ~ true                (startswith)
x Sback "World"                ~ true                (endswith)
x Speeks "lo"                  ~ true                (contains)
Sisalpha(x)                    ~ false
Sisdigit(x)                    ~ false
Sisalnum(x)                    ~ false
Sisspace(x)                    ~ false
Sisupper(x)                    ~ false
Sislower(x)                    ~ false
Sistitle(x)                    ~ true
Sisnumeric(x)                  ~ false
Sisdecimal(x)                  ~ false
Sisascii(x)                    ~ true
Sisident(x)                    ~ false               (is valid identifier)
Sisprintable(x)                ~ true

~ Modify
Sswap(x, "World", "Katlans")   ~ "Hello Katlans"
Sswap(x, "l", "L", 1)          ~ "HeLlo World"       (max 1 replacement)
Snip(x)                        ~ strip both sides
Snip(x, "H")                   ~ strip specific char
Slnip(x)                       ~ lstrip
Srnip(x)                       ~ rstrip
Srmprefix(x, "Hello ")         ~ "World"             (removeprefix)
Srmsuffix(x, " World")         ~ "Hello"             (removesuffix)

~ Split & Join
Schop(x, " ")                  ~ ["Hello","World"]
Schop(x, " ", 1)               ~ split max 1 time
Srchop(x, " ", 1)              ~ rsplit
Schoplines(x)                  ~ split on newlines
Spart(x, " ")                  ~ ("Hello"," ","World")  (partition)
Srpart(x, " ")                 ~ rpartition
Sjoin(" ", myList)             ~ join list with separator

~ Pad & Align
Scenter(x, 20)                 ~ center in 20 chars
Scenter(x, 20, "*")            ~ center with fill char
Sljust(x, 20)                  ~ left justify
Srjust(x, 20)                  ~ right justify
Szfill(x, 20)                  ~ zero-fill left
Stabs(x, 4)                    ~ expand tabs

~ Encode
Senc(x, "utf-8")               ~ encode to bytes
Sdec(myBytes, "utf-8")         ~ decode bytes to string
Sformat("Hello {0}", "World")  ~ string formatting

~ Index & Length
x[0]                           ~ "H"
x[0;5]                         ~ "Hello"
x[-1]                          ~ "d"
Slen(x)                        ~ 11
Sord("A")                      ~ 65     (char to ASCII)
Schr(65)                       ~ "A"    (ASCII to char)
```

---

## 📊 List Methods (L prefix)

```
lt x = 1, 2, 3, 4, 5 ::

~ Add
Ladd(x, 6)                     ~ append to end
Linsert(x, 2, 99)              ~ insert at index
Lextend(x, myList)             ~ extend with another list

~ Remove
Ldrop(x, 3)                    ~ remove first occurrence of value
Lpop(x)                        ~ remove & return last item
Lpop(x, 2)                     ~ remove & return at index
Lclear(x)                      ~ empty list

~ Search
Lindex(x, 3)                   ~ first index of value
Lindex(x, 3, 1, 4)             ~ search in range
Lcount(x, 3)                   ~ count occurrences
Lhas(x, 3)                     ~ true

~ Order
Lsort(x)                       ~ sort ascending in place
Lsort(x, "desc")               ~ sort descending
Lsort(x, key<fxn myKey>)       ~ sort by key function
Lsorted(x)                     ~ new sorted list
Lflip(x)                       ~ reverse in place
Lreversed(x)                   ~ new reversed list

~ Copy & Slice
Lcopy(x)                       ~ shallow copy
Lslice(x, 1, 4)                ~ [2,3,4]
Lslice(x, 1, 5, 2)             ~ [2,4] step slice
x[0]                           ~ 1
x[-1]                          ~ 5
x[1;3]                         ~ [2,3]

~ Info
Llen(x)                        ~ 5
Lmin(x)                        ~ 1
Lmax(x)                        ~ 5
Lsum(x)                        ~ 15

~ Functional
Lmap(x, fxn double)            ~ apply function to each
Lfilter(x, fxn isEven)         ~ keep matching elements
Lreduce(x, fxn add)            ~ reduce to single value
Lany(x, fxn isEven)            ~ true if any pass
Lall(x, fxn isPos)             ~ true if all pass
Lzip(x, y)                     ~ zip two lists
Lenumerate(x)                  ~ (index, value) pairs
Lflatten(x)                    ~ flatten nested lists
Lunique(x)                     ~ remove duplicates
Lchunk(x, 2)                   ~ [[1,2],[3,4],[5]]
Lconcat(x, y)                  ~ combine two lists
Lrepeat(x, 3)                  ~ repeat list 3 times
```

---

## 📘 Adero / Dict Methods (A prefix)

```
ad x = {name,"John"}; {age,25}; {city,"Delhi"} ::

~ Access
x[name]                        ~ "John"
Aget(x, name)                  ~ "John"
Aget(x, name, "Unknown")       ~ with default if missing
Akeys(x)                       ~ ["name","age","city"]
Avals(x)                       ~ ["John",25,"Delhi"]
Aitems(x)                      ~ [("name","John"),("age",25)]

~ Modify
Aadd(x, {email,"j@j.com"})     ~ add new key-value
Aset(x, name, "Jane")          ~ update value
Aupdate(x, otherDict)          ~ merge another dict
Asetdef(x, score, 0)           ~ set only if missing (setdefault)
Apop(x, age)                   ~ remove & return value
Apopitem(x)                    ~ remove & return last item
Aclear(x)                      ~ empty dict

~ Check
Ahas(x, name)                  ~ key exists
Ahasval(x, "John")             ~ value exists
Alen(x)                        ~ 3

~ Copy & Create
Acopy(x)                       ~ shallow copy
Afromkeys(myList, 0)           ~ dict from list with default value
Amerge(x, y)                   ~ merge two dicts (new dict)

~ Iterate
deta k belongs in Akeys(x) |>
    display k ::
<|::

deta k, v belongs in Aitems(x) |>
    display k ++ ": " ++ S(v) ::
<|::
```

---

## 🔵 Zelo / Tuple Methods (Z prefix)

```
zl x = "hello", 10, 10.3, true ::

Zlen(x)                        ~ 4
Zindex(x, 10)                  ~ 1     (first index)
Zcount(x, 10)                  ~ 1
Zhas(x, 10)                    ~ true
Zmin(x)                        ~ min (same-type tuples)
Zmax(x)
Zsum(x)
Zsorted(x)                     ~ returns new sorted list

~ Access
x[0]                           ~ "hello"
x[-1]                          ~ true
x[1;3]                         ~ (10, 10.3)

~ Convert
Ztolist(x)                     ~ convert to lt
Zfromlist(myList)              ~ create from list

~ Unpack
a, b, c, d = x ::
```

---

## 🔶 Set Methods (ST prefix)

```
st x = dsset <1, 2, 3, 4, 5> ::
st y = dsset <3, 4, 5, 6, 7> ::

~ Modify
STadd(x, 6)                    ~ add element
STrem(x, 3)                    ~ remove (error if missing)
STdiscard(x, 99)               ~ remove (no error if missing)
STpop(x)                       ~ remove & return arbitrary element
STclear(x)                     ~ empty set

~ Set Operations
STunion(x, y)                  ~ {1,2,3,4,5,6,7}
STinter(x, y)                  ~ {3,4,5}
STdiff(x, y)                   ~ {1,2}
STsymdiff(x, y)                ~ {1,2,6,7}

~ In-place
STupdate(x, y)                 ~ x = union
STinterupdate(x, y)            ~ x = intersection
STdiffupdate(x, y)             ~ x = difference
STsymdiffupdate(x, y)          ~ x = symmetric difference

~ Check
SThas(x, 3)                    ~ true
STdisjoint(x, y)               ~ no common elements?
STsubset(x, y)                 ~ x subset of y?
STsuperset(x, y)               ~ x superset of y?
STlen(x)                       ~ 5
STcopy(x)                      ~ copy
STfrozen(x)                    ~ immutable frozenset
```

---

## 🔢 Numeric Methods (N prefix)

```
int x = 255 ::
flt f = 3.14159 ::

~ Integer
Nbits(x)                       ~ 8       (bit_length)
Nbitcount(x)                   ~ 8       (count set bits)
Ntobytes(x, 2, "big")          ~ to bytes
Nfrombytes(b, "big")           ~ from bytes
Nabs(x)                        ~ absolute value
Npow(x, 2)                     ~ power
Ndivmod(x, 10)                 ~ (quotient, remainder)
Nhex(x)                        ~ "0xff"
Noct(x)                        ~ "0o377"
Nbin(x)                        ~ "0b11111111"

~ Float
Nisint(f)                      ~ is whole number?
Nratio(f)                      ~ as integer ratio
Nfloor(f)                      ~ 3
Nceil(f)                       ~ 4
Ntrunc(f)                      ~ 3
Nround(f, 2)                   ~ 3.14
Nisinf(f)                      ~ false
Nisnan(f)                      ~ false
Nisfinite(f)                   ~ true
Ninf()                         ~ infinity
Nnan()                         ~ NaN

~ Complex
cx = N.complex(3, 4) ::        ~ 3+4j
Nreal(cx)                      ~ 3.0
Nimag(cx)                      ~ 4.0
Nconj(cx)                      ~ 3-4j  (conjugate)
Nabs(cx)                       ~ 5.0   (magnitude)

~ Convert
Ntoint(x)                      ~ to int
Ntoflt(x)                      ~ to float
Ntostr(x)                      ~ to string
Nfrombin("1010")               ~ 10    (binary to int)
Nfromoct("12")                 ~ 10    (octal to int)
Nfromhex("ff")                 ~ 255   (hex to int)
```

---

## 📦 Bytes Methods (B prefix)

```
b = B.create("hello", "utf-8") ::
b = B.fromhex("68656c6c6f") ::
b = B.zeros(10) ::             ~ 10 zero bytes

Blen(b)                        ~ 5
Bhex(b)                        ~ "68656c6c6f"
Bfind(b, "he")                 ~ 0
Bindex(b, "he")                ~ 0
Bcount(b, "l")                 ~ 2
Breplace(b, "l", "L")          ~ b"heLLo"
Bstrip(b)                      ~ strip whitespace
Bchop(b, ".")                  ~ split on delimiter
Bjoin(sep, parts)              ~ join byte sequences
Bcap(b) / Blow(b) / Bswapcase(b) / Btitle(b)
Bisalpha(b) / Bisdigit(b) / Bisspace(b)
Bisupper(b) / Bislower(b) / Bisalnum(b)
Bdec(b, "utf-8")               ~ decode to string
Btolist(b)                     ~ list of integers

~ Bytearray (mutable bytes)
ba = BA.create("hello", "utf-8") ::
BAset(ba, 0, 72)               ~ modify at index
BAadd(ba, 33)                  ~ append byte
BAinsert(ba, 0, 72)            ~ insert at index
BApop(ba)                      ~ remove last
BArem(ba, 72)                  ~ remove value
BAextend(ba, b)                ~ extend
BAclear(ba) / BAcopy(ba) / BAflip(ba)
```

---

## 🔀 Iteration Utilities (R prefix)

```
~ Range
r = Rrange(0, 10) ::
r = Rrange(0, 10, 2) ::
Rlen(r) / Rhas(r, 4) / Rtolist(r)

~ Functional
lt doubled = Rmap(myList, fxn double) ::
lt evens = Rfilter(myList, fxn isEven) ::
x = Rreduce(myList, fxn add) ::
x = Rreduce(myList, fxn add, 0) ::     ~ with initial value

~ Order
lt s = Rsorted(myList) ::
lt s = Rsorted(myList, "desc") ::
lt s = Rsorted(myList, key<fxn myKey>) ::
lt r = Rreversed(myList) ::

~ Aggregation
x = Rmin(myList) / Rmax(myList) / Rsum(myList) ::
true/false = Rany(myList, fxn isEven) ::
true/false = Rall(myList, fxn isPos) ::

~ Combine
lt c = Rchain(list1, list2, list3) ::  ~ chain iterables
lt p = Rproduct(list1, list2) ::       ~ cartesian product
lt c = Rcombinations(myList, 2) ::     ~ all combos of size 2
lt p = Rpermutations(myList, 2) ::     ~ all perms of size 2

deta i, v belongs in Renumerate(myList) |>
    display i ++ ": " ++ S(v) ::
<|::

deta a, b belongs in Rzip(list1, list2) |>
    display S(a) ++ " - " ++ S(b) ::
<|::
```

---

## 🎓 Type Checking

```
type(x)                        ~ "string"/"int"/"flt"/"lt"/"zl"/"ad"/"bool"
type(x) == "int"               ~ true/false
istype(x, "int")               ~ true/false

~ Cast
I(x)  F(x)  S(x)  B(x)
```

---

## 📋 Quick Reference

| Type | Katlans Name | Prefix | Mutable |
| --- | --- | --- | --- |
| String | default | `S` | No |
| Integer | `int` | `N` | No |
| Float | `flt` | `N` | No |
| List | `lt` | `L` | Yes |
| Tuple | `zl` Zelo | `Z` | No |
| Dict | `ad` Adero | `A` | Yes |
| Set | `dsset` | `ST` | Yes |
| Bool | `true/false` | `N` | No |
| Bytes | `B.create` | `B` | No |
| Bytearray | `BA.create` | `BA` | Yes |
| Range | `Rrange` | `R` | No |