"""
Katlans Code Generator  —  AST → C source
"""
import os
from .ast_nodes import *
from .errors import CodeGenError


# Map built-in function names to C runtime calls
BUILTIN_MAP = {
    # ── Math ──────────────────────────────────────────────────────────────
    "sqrt":      "k_sqrt",   "abs":     "k_abs",    "floor":   "k_floor",
    "ceil":      "k_ceil",   "log":     "k_log",    "ln":      "k_ln",
    "exp":       "k_exp",    "sin":     "k_sin",    "cos":     "k_cos",
    "tan":       "k_tan",    "arcsin":  "k_asin",   "arccos":  "k_acos",
    "arctan":    "k_atan",   "sum":     "k_sum",    "len":     "k_len",
    "round":     "k_round",  "min":     "k_min2",   "max":     "k_max2",
    "fact":      "k_fact",   "gcd":     "k_gcd2",   "lcm":     "k_lcm2",
    "rand":      "k_rand_range",
    # ── Global built-ins ─────────────────────────────────────────────────
    "I":         "k_I",      "F":       "k_F",      "S":       "k_S",
    "type":      "k_type",   "all":     "k_all",    "any":     "k_any",
    "chr":       "k_chr",    "ord":     "k_ord",    "hex":     "k_hex",
    "oct":       "k_oct",    "bin":     "k_bin",    "divmod":  "k_divmod",
    "enumerate": "k_enumerate", "zip":  "k_zip",
    # ── String  S prefix ─────────────────────────────────────────────────
    "Scap":       "k_Scap",       "Slow":       "k_Slow",
    "Stitle":     "k_Stitle",     "Scapfirst":  "k_Scapfirst",
    "Sswapcase":  "k_Sswapcase",  "Scasefold":  "k_Scasefold",
    "Slen":       "k_Slen",       "Sfind":      "k_Sfind",
    "Srfind":     "k_Srfind",     "Sindex":     "k_Sindex",
    "Srindex":    "k_Srindex",    "Scount":     "k_Scount",
    "Speeks":     "k_Speeks",     "Sfront":     "k_Sfront",
    "Sback":      "k_Sback",      "Sswap":      "k_Sswap",
    "Snip":       "k_Snip",       "Snipleft":   "k_Snipleft",
    "Snipright":  "k_Snipright",  "Spad":       "k_Spad",
    "Spadleft":   "k_Spadleft",   "Spadright":  "k_Spadright",
    "Szfill":     "k_Szfill",     "Schop":      "k_Schop",
    "Schoplines": "k_Schoplines", "Sjoin":      "k_Sjoin",
    "Spartition": "k_Spartition", "Sremprefix": "k_Sremprefix",
    "Sremsuffix": "k_Sremsuffix", "Sslice":     "k_Sslice",
    "Schar":      "k_Schar",
    "Sisalpha":   "k_Sisalpha",   "Sisdigit":   "k_Sisdigit",
    "Sisalnum":   "k_Sisalnum",   "Sisspace":   "k_Sisspace",
    "Sislower":   "k_Sislower",   "Sisupper":   "k_Sisupper",
    "Sistitle":   "k_Sistitle",   "Sisascii":   "k_Sisascii",
    "Sisprintable":"k_Sisprintable","Sisnumeric":"k_Sisnumeric",
    "Sisdecimal": "k_Sisdecimal",
    # ── List  L prefix ────────────────────────────────────────────────────
    "Ladd":    "k_Ladd",    "Laddall":  "k_Laddall",  "Linsert": "k_Linsert",
    "Ldrop":   "k_Ldrop",   "Lpop":     "k_Lpop",     "Lclear":  "k_Lclear",
    "Lindex":  "k_Lindex",  "Lcount":   "k_Lcount",   "Lhas":    "k_Lhas",
    "Lsort":   "k_Lsort",   "Lflip":    "k_Lflip",    "Lcopy":   "k_Lcopy",
    "Lslice":  "k_Lslice",  "Lconcat":  "k_Lconcat",  "Ljoin":   "k_Ljoin",
    "Llen":    "k_Llen",    "Lmax":     "k_Lmax",     "Lmin":    "k_Lmin",
    "Lsum":    "k_Lsum",    "Lunique":  "k_Lunique",
    "Lreversed":"k_Lreversed","Lsorted":"k_Lsorted",
    # ── Dict  A prefix ────────────────────────────────────────────────────
    "Aget":     "k_Aget",    "Akeys":   "k_Akeys",   "Avals":   "k_Avals",
    "Aitems":   "k_Aitems",  "Aset":    "k_Aset",    "Aadd":    "k_Aadd",
    "Adrop":    "k_Adrop",   "Apop":    "k_Apop",    "Aupdate": "k_Aupdate",
    "Aclear":   "k_Aclear",  "Acopy":   "k_Acopy",   "Alen":    "k_Alen",
    "Ahas":     "k_Ahas",    "Amerge":  "k_Amerge",  "Afromkeys":"k_Afromkeys",
    # ── Tuple/Zelo  Z prefix ─────────────────────────────────────────────
    "Zcount":  "k_Zcount",  "Zindex":  "k_Zindex",  "Zlen":    "k_Zlen",
    "Zhas":    "k_Zhas",    "Zmax":    "k_Zmax",    "Zmin":    "k_Zmin",
    "Zsum":    "k_Zsum",    "Ztolt":   "k_Ztolt",   "Zsorted": "k_Zsorted",
    # ── File I/O  f prefix ────────────────────────────────────────────────
    "fread":  "k_fread",   "fwrite":    "k_fwrite",  "fadd":     "k_fadd",
    "fdel":   "k_fdel",    "fexists":   "k_fexists", "frename":  "k_frename",
    "fcopy":  "k_fcopy",   "fsize":     "k_fsize",   "flist":    "k_flist",
    "fmkdir": "k_fmkdir",  "frmdir":    "k_frmdir",  "fisdir":   "k_fisdir",
    "fisfile":"k_fisfile",  "fext":      "k_fext",    "fbase":    "k_fbase",
    "fdir":   "k_fdir",    "fjoin":     "k_fjoin",
    "freadlines":"k_freadlines","fwritelines":"k_fwritelines",
    # ── OS  os prefix ─────────────────────────────────────────────────────
    "osrun":  "k_osrun",   "osget":     "k_osget",   "onset":    "k_onset",
    "ospwd":  "k_ospwd",   "oscd":      "k_oscd",    "ossys":    "k_ossys",
    "osram":  "k_osram",   "osuptime":  "k_osuptime","osboot":   "k_osboot",
    "osuser": "k_osuser",  "oshalt":    "k_oshalt",  "osjoin":   "k_osjoin",
    "osbase": "k_osbase",  "osdir":     "k_osdir",   "osext":    "k_osext",
    "ospath": "k_ospath",  "osopen":    "k_osopen",
    "osclipget":"k_osclipget","osclipset":"k_osclipset","osnoti":"k_osnoti",
    # ── Regex  rx prefix ──────────────────────────────────────────────────
    "rxmatch":"k_rxmatch", "rxfull":    "k_rxfull",  "rxfind":   "k_rxfind",
    "rxswap": "k_rxswap",  "rxchop":    "k_rxchop",  "rxgroups": "k_rxgroups",
    "rxcount":"k_rxcount",
    # ── Data Formats  js / csv / xml ──────────────────────────────────────
    "jsparse":"k_jsparse",  "jsstring": "k_jsstring","jsread":   "k_jsread",
    "jswrite":"k_jswrite",
    "csvread":"k_csvread",  "csvheads": "k_csvheads","csvwrite":  "k_csvwrite",
    "csvparse":"k_csvparse",
    "xmlparse":"k_xmlparse","xmlget":   "k_xmlget",  "xmlattr":  "k_xmlattr",
    "xmlstring":"k_xmlstring",
    # ── Cryptography  cr prefix ────────────────────────────────────────────
    "crmd5":  "k_crmd5",   "crsha":     "k_crsha",   "crhash":   "k_crhash",
    "crenc":  "k_crenc",   "crdec":     "k_crdec",
    "crb64enc":"k_crb64enc","crb64dec": "k_crb64dec",
    "cruuid": "k_cruuid",  "crtoken":   "k_crtoken", "crcomp":   "k_crcomp",
    # ── CLI Tools  cli prefix ──────────────────────────────────────────────
    "clired":   "k_clired",   "cligreen":  "k_cligreen", "cliyellow":"k_cliyellow",
    "clibold":  "k_clibold",  "cliblue":   "k_cliblue",  "climagenta":"k_climagenta",
    "clicyan":  "k_clicyan",  "clidim":    "k_clidim",   "cliunder": "k_cliunder",
    "climenu":  "k_climenu",
    "cliprogress":"k_cliprogress","clipbset":"k_clipbset","clipbdone":"k_clipbdone",
    "cliargs":  "k_cliargs",  "cliflag":   "k_cliflag",  "cliopt":   "k_cliopt",
    # ── DateTime  dt prefix ───────────────────────────────────────────────
    "dtnow":    "k_dtnow",    "dtdate":    "k_dtdate",   "dttime":   "k_dttime",
    "dtformat": "k_dtformat", "dtparse":   "k_dtparse",
    "dtadd":    "k_dtadd",    "dtsub":     "k_dtsub",    "dtdiff":   "k_dtdiff",
    "dtconv":   "k_dtconv",   "dtstamp":   "k_dtstamp",  "dtfromstamp":"k_dtfromstamp",
    "dtwait":   "k_dtwait",   "dtleap":    "k_dtleap",
    "dtbefore": "k_dtbefore", "dtafter":   "k_dtafter",  "dtequal":  "k_dtequal",
    "dtzone":   "k_dtzone",
    # ── Networking  net prefix ────────────────────────────────────────────
    "netget":   "k_netget",   "netpost":   "k_netpost",  "netput":   "k_netput",
    "netpatch": "k_netpatch", "netdel":    "k_netdel",   "netjson":  "k_netjson",
    "netdns":   "k_netdns",   "netping":   "k_netping",  "netdl":    "k_netdl",
    # ── Concurrency  cx prefix ────────────────────────────────────────────
    "cxqueue":  "k_cxqueue",  "cxpush":    "k_cxpush",   "cxpop":    "k_cxpop",
    "cxlock":   "k_cxlock",   "cxacquire": "k_cxacquire","cxrelease":"k_cxrelease",
    "cxchan":   "k_cxchan",   "cxsend":    "k_cxsend",   "cxrecv":   "k_cxrecv",
    "cxsem":    "k_cxsem",    "cxfuture":  "k_cxfuture", "cxresolve":"k_cxresolve",
    "cxatom":   "k_cxatom",   "cxinc":     "k_cxinc",    "cxdec":    "k_cxdec",
    "cxjoin":   "k_cxjoin",   "cxstop":    "k_cxstop",   "cxcancel": "k_cxcancel",
    "cxabort":  "k_cxabort",  "cxpool":    "k_cxpool",   "cxsubmit": "k_cxsubmit",
    "cxretry":  "k_cxretry",  "cxtimeout": "k_cxtimeout","cxwait":   "k_cxwait_secs",
    "cxon":     "k_cxon",     "cxemit":    "k_cxemit",
    # ── Data Structures  ds prefix ────────────────────────────────────────
    "dsstack":  "k_dsstack",  "dspush":    "k_dspush",   "dspop":    "k_dspop",
    "dspeek":   "k_dspeek",   "dssize":    "k_dssize",
    "dsqueue":  "k_dsqueue",  "dsenqueue": "k_dsenqueue","dsdequeue":"k_dsdequeue",
    "dsqpeek":  "k_dsqpeek",
    "dsset":    "k_dsset",    "dsadd":     "k_dsadd",    "dsrem":    "k_dsrem",
    "dshas":    "k_dshas",    "dsunion":   "k_dsunion",  "dsinter":  "k_dsinter",
    "dsdiff":   "k_dsdiff",
    "dsll":     "k_dsll",     "dsllpush":  "k_dsllpush", "dsllpop":  "k_dsllpop",
    "dsllget":  "k_dsllget",
    "dstree":   "k_dstree",   "dstadd":    "k_dstadd",   "dstchildren":"k_dstchildren",
    "dstparent":"k_dstparent",
    "dsgraph":  "k_dsgraph",  "dsgedge":   "k_dsgedge",  "dsgnbr":   "k_dsgnbr",
    "dsgpath":  "k_dsgpath",
    "dsheap":   "k_dsheap",   "dshpush":   "k_dshpush",  "dshpop":   "k_dshpop",
    "dshpeek":  "k_dshpeek",
    "dsdeque":  "k_dsdeque",  "dqdpushl":  "k_dqdpushl", "dqdpushr": "k_dqdpushr",
    "dqdpopl":  "k_dqdpopl",  "dqdpopr":   "k_dqdpopr",
    "dsmat":    "k_dsmat",    "dsmatset":  "k_dsmatset", "dsmatget": "k_dsmatget",
    "dsmattrans":"k_dsmattrans","dsmatmul": "k_dsmatmul","dsmatprint":"k_dsmatprint",
}

# Built-ins that modify in-place and return void
VOID_BUILTINS = {
    "Ladd","Laddall","Linsert","Ldrop","Lclear","Lsort","Lflip",
    "Aset","Aadd","Adrop","Aupdate","Aclear",
    "clired","cligreen","cliyellow","clibold","cliblue",
    "climagenta","clicyan","clidim","cliunder",
    "clipbset","clipbdone","osclipset","osnoti","osopen","oscd",
    "dspush","dsenqueue","dsadd","dsrem",
    "dsllpush","dsllpop","dstadd","dsgedge",
    "dshpush","dqdpushl","dqdpushr","dsmatset","dsmatprint",
    "cxpush","cxsend","cxjoin","cxstop","cxacquire","cxrelease",
    "cxabort","cxsubmit","cxinc","cxdec","dtwait",
}


class CodeGen:
    def __init__(self, runtime_path: str, filename: str = "<stdin>"):
        self.runtime   = runtime_path
        self.filename  = filename
        self._indent   = 0
        self._lines: list[str] = []
        self._func_names: set[str] = set()
        self._forward_decls: list[str] = []
        # Track declared variable names per scope to emit KVal* on first use
        self._declared: list[set] = [set()]   # stack of scopes

    def _scope_push(self):  self._declared.append(set())
    def _scope_pop(self):   self._declared.pop()

    def _is_declared(self, name: str) -> bool:
        for scope in reversed(self._declared):
            if name in scope: return True
        return False

    def _declare(self, name: str):
        self._declared[-1].add(name)

    # ── Output helpers ────────────────────────────────────────────────────

    def _emit(self, line: str = ""):
        self._lines.append("    " * self._indent + line)

    def _emit_raw(self, line: str):
        self._lines.append(line)

    def _indent_in(self):  self._indent += 1
    def _indent_out(self): self._indent -= 1

    def _gen_block(self, body: list):
        self._scope_push()
        for s in body:
            self._gen_stmt(s)
        self._scope_pop()

    def _error(self, msg: str, node=None):
        line = getattr(node, "line", None)
        col  = getattr(node, "col",  None)
        raise CodeGenError(msg, line, col, self.filename)

    # ── Entry point ───────────────────────────────────────────────────────

    def generate(self, program: Program) -> str:
        # Preamble
        self._emit_raw(f'#include "{self.runtime}"')
        self._emit_raw("")

        # Collect forward declarations for user functions
        self._collect_funcs(program.body)

        # Emit forward declarations
        for fd in self._forward_decls:
            self._emit_raw(fd)
        if self._forward_decls:
            self._emit_raw("")

        # Emit top-level function definitions first
        for node in program.body:
            if isinstance(node, FuncDecl):
                self._gen_func_decl(node)

        # Emit top-level class definitions
        for node in program.body:
            if isinstance(node, ClassDecl):
                self._gen_class_decl(node)

        # Main function
        self._emit_raw("int main(void) {")
        self._indent_in()

        for node in program.body:
            if not isinstance(node, (FuncDecl, ClassDecl)):
                self._gen_stmt(node)

        self._emit("return 0;")
        self._indent_out()
        self._emit_raw("}")

        return "\n".join(self._lines)

    # ── Forward declarations ───────────────────────────────────────────────

    def _collect_funcs(self, body: list):
        for node in body:
            if isinstance(node, FuncDecl):
                self._func_names.add(node.name)
                params = ", ".join(["KVal*"] * len(node.params))
                self._forward_decls.append(f"KVal* kf_{node.name}({params});")
            elif isinstance(node, ClassDecl):
                # Find init params
                init_params = 0
                for item in node.body:
                    if isinstance(item, FuncDecl) and item.name == "init":
                        init_params = len(item.params)
                        break
                params = ", ".join(["KVal*"] * init_params)
                self._forward_decls.append(f"KVal* kclass_{node.name}({params});")
                for item in node.body:
                    if isinstance(item, FuncDecl) and item.name != "init":
                        self._func_names.add(item.name)
                        nparams = len(item.params) + 1
                        params  = ", ".join(["KVal*"] * nparams)
                        self._forward_decls.append(f"KVal* kf_{item.name}({params});")

    # ── Statement dispatch ─────────────────────────────────────────────────

    def _gen_stmt(self, node):
        if isinstance(node, VarDecl):
            self._gen_var_decl(node)
        elif isinstance(node, Assignment):
            self._gen_assignment(node)
        elif isinstance(node, DisplayStmt):
            self._gen_display(node)
        elif isinstance(node, IfStmt):
            self._gen_if(node)
        elif isinstance(node, ForRangeStmt):
            self._gen_for_range(node)
        elif isinstance(node, ForEachStmt):
            self._gen_for_each(node)
        elif isinstance(node, WhileStmt):
            self._gen_while(node)
        elif isinstance(node, HaltStmt):
            self._emit("break;")
        elif isinstance(node, ContinueStmt):
            self._emit("continue;")
        elif isinstance(node, ReturnStmt):
            self._gen_return(node)
        elif isinstance(node, FuncCall):
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if node.obj:
                all_args = node.obj + (f", {args_c}" if args_c else "")
                self._emit(f"kf_{node.name}({all_args});")
            else:
                self._emit(f"kf_{node.name}({args_c});")
        elif isinstance(node, BuiltinCall):
            expr = self._gen_builtin(node)
            self._emit(f"{expr};")
        elif isinstance(node, TryCatch):
            self._gen_try(node)
        elif isinstance(node, TestDecl):
            self._gen_test_decl(node)
        elif isinstance(node, TestCheck):
            escaped = node.expr_str.replace('"', '\\"')
            cond    = self._gen_expr(node.expr)
            self._emit(f'k_testcheck({cond}, "{escaped}");')
        elif isinstance(node, TestRun):
            self._emit("k_testrun_summary();")
        elif isinstance(node, TestErr):
            # simplified — just call and note it should error
            func_c = self._gen_expr(node.func) if node.func else "kv_void()"
            self._emit(f"/* testerr */ (void)({func_c});")
        elif isinstance(node, ClassInstantiate):
            self._gen_class_inst(node)
        elif isinstance(node, FuncDecl):
            pass   # already emitted above main()
        elif isinstance(node, ClassDecl):
            pass
        elif node is None:
            pass
        else:
            # expression as statement
            self._emit(f"{self._gen_expr(node)};")

    # ── Variable declaration ──────────────────────────────────────────────

    def _gen_var_decl(self, node: VarDecl):
        self._declare(node.name)

        # For lt/zl/ad — check if value is a literal or an expression
        if node.dtype == "lt":
            if isinstance(node.value, ListLiteral):
                self._emit(f"KVal* {node.name} = kv_list();")
                for el in node.value.elements:
                    ev = self._gen_expr(el)
                    self._emit(f"k_Ladd({node.name}, {ev});")
            else:
                # Expression result (e.g. lt x = someFunc() ::)
                self._emit(f"KVal* {node.name} = {self._gen_expr(node.value)};")

        elif node.dtype == "zl":
            if isinstance(node.value, TupleLiteral):
                self._emit(f"KVal* {node.name} = kv_list();")
                for el in node.value.elements:
                    ev = self._gen_expr(el)
                    self._emit(f"k_Ladd({node.name}, {ev});")
            else:
                self._emit(f"KVal* {node.name} = {self._gen_expr(node.value)};")

        elif node.dtype == "ad":
            if isinstance(node.value, DictLiteral):
                self._emit(f"KVal* {node.name} = kv_dict();")
                for key, val_node in node.value.pairs:
                    kv  = self._gen_expr(key)
                    vv  = self._gen_expr(val_node)
                    key_s = f"{kv}->s" if not isinstance(key, StrLiteral) else f"k_S({kv})->s"
                    if isinstance(key, StrLiteral):
                        self._emit(f'kdict_set({node.name}->dict, {kv}->s, {vv});')
                    else:
                        self._emit(f'kdict_set({node.name}->dict, k_S({kv})->s, {vv});')
            else:
                # Expression result (e.g. ad x = jsread("f.json") ::)
                self._emit(f"KVal* {node.name} = {self._gen_expr(node.value)};")

        else:
            val = self._gen_expr(node.value)
            self._emit(f"KVal* {node.name} = {val};")

    # ── Assignment ────────────────────────────────────────────────────────

    def _gen_assignment(self, node: Assignment):
        val = self._gen_expr(node.value)
        if isinstance(node.target, Identifier):
            name = node.target.name
            if not self._is_declared(name):
                self._declare(name)
                self._emit(f"KVal* {name} = {val};")
            else:
                self._emit(f"{name} = {val};")
        elif isinstance(node.target, MemberAccess):
            obj = self._gen_expr(node.target.obj)
            self._emit(f'kdict_set({obj}->dict, "{node.target.member}", {val});')
        else:
            self._emit(f"{self._gen_expr(node.target)} = {val};")

    # ── Display ───────────────────────────────────────────────────────────

    def _gen_display(self, node: DisplayStmt):
        val = self._gen_expr(node.value)
        self._emit(f"k_display({val});")

    # ── If ────────────────────────────────────────────────────────────────

    def _gen_if(self, node: IfStmt):
        cond = self._gen_expr(node.condition)
        self._emit(f"if (k_truthy({cond})) {{")
        self._indent_in()
        self._gen_block(node.then_body)
        self._indent_out()
        self._emit("}")
        for ec, eb in node.elif_clauses:
            econd = self._gen_expr(ec)
            self._emit(f"else if (k_truthy({econd})) {{")
            self._indent_in()
            self._gen_block(eb)
            self._indent_out()
            self._emit("}")
        if node.else_body:
            self._emit("else {")
            self._indent_in()
            self._gen_block(node.else_body)
            self._indent_out()
            self._emit("}")

    # ── For range ─────────────────────────────────────────────────────────

    def _gen_for_range(self, node: ForRangeStmt):
        start = self._gen_expr(node.start)
        end   = self._gen_expr(node.end)
        step  = self._gen_expr(node.step)
        v     = node.var
        self._emit(f"for (long long _k_{v} = (long long)_knum({start});"
                   f" _k_{v} < (long long)_knum({end});"
                   f" _k_{v} += (long long)_knum({step})) {{")
        self._indent_in()
        self._emit(f"KVal* {v} = kv_int(_k_{v});")
        self._scope_push(); self._declare(v)
        for s in node.body: self._gen_stmt(s)
        self._scope_pop()
        self._indent_out()
        self._emit("}")

    def _gen_for_each(self, node: ForEachStmt):
        iterable = self._gen_expr(node.iterable)
        v = node.var
        idx = f"_ki_{v}"
        self._emit(f"for (size_t {idx} = 0; {idx} < {iterable}->list->len; {idx}++) {{")
        self._indent_in()
        self._emit(f"KVal* {v} = {iterable}->list->items[{idx}];")
        self._scope_push(); self._declare(v)
        for s in node.body: self._gen_stmt(s)
        self._scope_pop()
        self._indent_out()
        self._emit("}")

    def _gen_while(self, node: WhileStmt):
        cond = self._gen_expr(node.condition)
        self._emit(f"while (k_truthy({cond})) {{")
        self._indent_in()
        self._gen_block(node.body)
        self._indent_out()
        self._emit("}")

    # ── Return ────────────────────────────────────────────────────────────

    def _gen_return(self, node: ReturnStmt):
        if not node.values:
            self._emit("return kv_void();")
        elif len(node.values) == 1:
            self._emit(f"return {self._gen_expr(node.values[0])};")
        else:
            # Multiple return: pack into list
            self._emit("{ KVal* _ret = kv_list();")
            self._indent_in()
            for v in node.values:
                self._emit(f"k_Ladd(_ret, {self._gen_expr(v)});")
            self._emit("return _ret;")
            self._indent_out()
            self._emit("}")

    # ── Function declaration ───────────────────────────────────────────────

    def _gen_func_decl(self, node: FuncDecl):
        params_c = ", ".join(f"KVal* {p}" for p in node.params)
        self._emit_raw(f"KVal* kf_{node.name}({params_c}) {{")
        self._indent_in()
        self._scope_push()
        for p in node.params: self._declare(p)
        for op in node.out_params:
            self._emit(f"KVal* {op} = kv_void();")
            self._declare(op)
        for s in node.body:
            self._gen_stmt(s)
        self._scope_pop()
        self._emit("return kv_void();")
        self._indent_out()
        self._emit_raw("}")
        self._emit_raw("")

    # ── Class (simple struct-style) ───────────────────────────────────────

    def _gen_class_decl(self, node: ClassDecl):
        init_func = None
        methods   = []
        props     = []

        for item in node.body:
            if isinstance(item, FuncDecl) and item.name == "init":
                init_func = item
            elif isinstance(item, FuncDecl):
                methods.append(item)
            elif isinstance(item, VarDecl):
                props.append(item)

        # Constructor params from init
        params   = init_func.params if init_func else []
        params_c = ", ".join(f"KVal* {p}" for p in params)

        self._emit_raw(f"KVal* kclass_{node.name}({params_c}) {{")
        self._indent_in()
        self._scope_push()
        self._emit("KVal* self = kv_dict();")
        self._declare("self")

        # If inheriting, copy parent properties first
        if node.parent:
            if node.borrows == ["*"]:
                # Call parent constructor with no args to get base instance
                self._emit(f"KVal* _parent = kclass_{node.parent}();")
                self._emit(f"k_Aupdate(self, _parent);")
            else:
                # Borrow specific methods - done via method references
                pass

        # Default props
        for p in props:
            pval = self._gen_expr(p.value)
            self._emit(f'kdict_set(self->dict, "{p.name}", {pval});')

        # Run init body
        if init_func:
            for p in params:
                self._declare(p)
            for s in init_func.body:
                self._gen_stmt_cls(s, "self")

        self._emit("return self;")
        self._scope_pop()
        self._indent_out()
        self._emit_raw("}")
        self._emit_raw("")

        # Emit methods — self is always first arg
        for m in methods:
            mparams_c = "KVal* self" + (", " if m.params else "") + \
                        ", ".join(f"KVal* {p}" for p in m.params)
            self._emit_raw(f"KVal* kf_{m.name}({mparams_c}) {{")
            self._indent_in()
            self._scope_push()
            self._declare("self")
            for p in m.params:
                self._declare(p)
            for s in m.body:
                self._gen_stmt_cls(s, "self")
            self._emit("return kv_void();")
            self._scope_pop()
            self._indent_out()
            self._emit_raw("}")
            self._emit_raw("")

    def _gen_stmt_cls(self, node, self_var: str):
        """Generate stmt inside a class method — rewrite self.x = y"""
        if isinstance(node, Assignment) and isinstance(node.target, MemberAccess):
            if isinstance(node.target.obj, Identifier) and node.target.obj.name == "self":
                val = self._gen_expr(node.value)
                self._emit(f'kdict_set({self_var}->dict, "{node.target.member}", {val});')
                return
        self._gen_stmt(node)

    # ── Class instantiation ────────────────────────────────────────────────

    def _gen_class_inst(self, node: ClassInstantiate):
        args_c = ", ".join(self._gen_expr(a) for a in node.args)
        if node.var_name:
            self._emit(f"KVal* {node.var_name} = kclass_{node.cls_name}({args_c});")
        else:
            self._emit(f"kclass_{node.cls_name}({args_c});")

    # ── Try / Catch ───────────────────────────────────────────────────────

    def _gen_test_decl(self, node):
        self._emit(f'k_test_begin("{node.name}");')
        self._emit(f'printf("\\nTest: {node.name}\\n");')
        self._scope_push()
        for s in node.body:
            self._gen_stmt(s)
        self._scope_pop()

    def _gen_try(self, node: TryCatch):
        # C has no exceptions — wrap in a basic signal-free try block
        self._emit("/* try */ {")
        self._indent_in()
        for s in node.try_body:
            self._gen_stmt(s)
        self._indent_out()
        self._emit("}")
        # catch block (simplified — always runs for now in v1)
        self._emit(f"/* catch ({node.error_var}) */")

    # ── Expression codegen ────────────────────────────────────────────────

    def _gen_expr(self, node) -> str:
        if isinstance(node, IntLiteral):
            return f"kv_int({node.value}LL)"

        if isinstance(node, FltLiteral):
            return f"kv_flt({node.value})"

        if isinstance(node, StrLiteral):
            escaped = node.value.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n").replace("\t", "\\t")
            return f'kv_str("{escaped}")'

        if isinstance(node, BoolLiteral):
            return f"kv_bool({'true' if node.value else 'false'})"

        if isinstance(node, VoidLiteral):
            return "kv_void()"

        if isinstance(node, Identifier):
            if node.name == "PI": return "K_PI"
            if node.name == "E":  return "K_E"
            return node.name

        if isinstance(node, MemberAccess):
            obj = self._gen_expr(node.obj)
            return f'kdict_get({obj}->dict, "{node.member}")'

        if isinstance(node, IndexAccess):
            obj = self._gen_expr(node.obj)
            idx = self._gen_expr(node.index)
            if node.end is None:
                # Single index — works for both list and string
                return f"(({obj})->type==KT_STR ? k_Schar({obj},{idx}) : k_Lget({obj},{idx}))"
            end  = self._gen_expr(node.end)
            step = self._gen_expr(node.step) if node.step else "kv_int(1LL)"
            # Slice — works for both string and list
            return (f"(({obj})->type==KT_STR"
                    f" ? k_Sslice({obj},{idx},{end})"
                    f" : k_Lslice({obj},{idx},{end}))")

        if isinstance(node, BinOp):
            return self._gen_binop(node)

        if isinstance(node, UnaryOp):
            operand = self._gen_expr(node.operand)
            if node.op == "-":
                return f"kv_flt(-_knum({operand}))"
            if node.op == "!&":
                return f"k_not({operand})"
            return operand

        if isinstance(node, FuncCall):
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if node.obj:
                all_args = node.obj + (f", {args_c}" if args_c else "")
                return f"kf_{node.name}({all_args})"
            return f"kf_{node.name}({args_c})"

        if isinstance(node, BuiltinCall):
            return self._gen_builtin(node)

        if isinstance(node, AskExpr):
            prompt = self._gen_expr(node.prompt)
            return f"k_ask({prompt}->s)"

        if isinstance(node, TupleLiteral):
            # Tuples stored as lists (immutable by convention)
            parts = "; ".join(f"klist_push(_tmp_zl->list, {self._gen_expr(e)})"
                              for e in node.elements)
            return f"({{ KVal* _tmp_zl = kv_list(); {parts}; _tmp_zl; }})"

        if isinstance(node, DictLiteral):
            return self._gen_inline_dict(node)

        if isinstance(node, ListLiteral):
            # inline list — generate temp
            return self._gen_inline_list(node)

        self._error(f"Cannot generate expression for {type(node).__name__}", node)

    def _gen_binop(self, node: BinOp) -> str:
        l = self._gen_expr(node.left)
        r = self._gen_expr(node.right)
        ops = {
            "+":   f"k_add({l},{r})",
            "-":   f"k_sub({l},{r})",
            "*":   f"k_mul({l},{r})",
            "/":   f"k_div({l},{r})",
            "//":  f"k_idiv({l},{r})",
            "///": f"k_mod({l},{r})",
            "^":   f"k_pow({l},{r})",
            "%":   f"k_pct({l},{r})",
            "++":  f"k_concat({l},{r})",
            "==":  f"k_eq({l},{r})",
            "=\\": f"k_neq({l},{r})",
            "<":   f"k_lt({l},{r})",
            ">":   f"k_gt({l},{r})",
            "=<":  f"k_lte({l},{r})",
            "=>":  f"k_gte({l},{r})",
            "&&":  f"k_and({l},{r})",
            "&+":  f"k_or({l},{r})",
        }
        result = ops.get(node.op)
        if result is None:
            self._error(f"Unknown operator: {node.op}", node)
        return result

    def _gen_builtin(self, node: BuiltinCall) -> str:
        fn = node.func
        c  = BUILTIN_MAP.get(fn)
        if c:
            # ── Optional / variable-arg special cases ─────────────────────
            if fn == "Aget":
                obj  = self._gen_expr(node.args[0])
                key  = self._gen_expr(node.args[1])
                defv = self._gen_expr(node.args[2]) if len(node.args) > 2 else "NULL"
                return f"k_Aget({obj},{key},{defv})"

            if fn == "Lsort":
                obj  = self._gen_expr(node.args[0])
                dirv = self._gen_expr(node.args[1]) if len(node.args) > 1 else "NULL"
                return f"((k_Lsort({obj},{dirv})), kv_void())"

            if fn == "Lpop":
                obj  = self._gen_expr(node.args[0])
                idxv = self._gen_expr(node.args[1]) if len(node.args) > 1 else "NULL"
                return f"k_Lpop({obj},{idxv})"

            # ── DateTime special cases ─────────────────────────────────────
            if fn == "dtwait":
                n    = self._gen_expr(node.args[0])
                unit = self._gen_expr(node.args[1]) if len(node.args) > 1 else "NULL"
                return f"((k_dtwait({n},{unit})), kv_void())"

            if fn == "dtdiff":
                a = self._gen_expr(node.args[0])
                b = self._gen_expr(node.args[1])
                u = self._gen_expr(node.args[2]) if len(node.args) > 2 else 'kv_str("sec")'
                return f"k_dtdiff({a},{b},{u})"

            if fn == "dtadd" or fn == "dtsub":
                dt  = self._gen_expr(node.args[0])
                n   = self._gen_expr(node.args[1])
                u   = self._gen_expr(node.args[2]) if len(node.args) > 2 else 'kv_str("sec")'
                return f"{c}({dt},{n},{u})"

            if fn == "dtzone":
                dt   = self._gen_expr(node.args[0])
                frm  = self._gen_expr(node.args[1]) if len(node.args) > 1 else 'kv_str("UTC")'
                to   = self._gen_expr(node.args[2]) if len(node.args) > 2 else 'kv_str("UTC")'
                return f"k_dtzone({dt},{frm},{to})"

            if fn == "dtconv":
                val  = self._gen_expr(node.args[0])
                frm  = self._gen_expr(node.args[1])
                to   = self._gen_expr(node.args[2])
                return f"k_dtconv({val},{frm},{to})"

            # ── Networking special cases ───────────────────────────────────
            if fn in ("netpost", "netput", "netpatch"):
                url  = self._gen_expr(node.args[0])
                body = self._gen_expr(node.args[1]) if len(node.args) > 1 else "kv_void()"
                return f"{c}({url},{body})"

            # ── Data structure special cases ───────────────────────────────
            if fn == "dstadd":
                tr     = self._gen_expr(node.args[0])
                nname  = self._gen_expr(node.args[1])
                parent = self._gen_expr(node.args[2]) if len(node.args) > 2 else "kv_void()"
                return f"((k_dstadd({tr},{nname},{parent})), kv_void())"

            if fn == "dsgedge":
                g    = self._gen_expr(node.args[0])
                frm  = self._gen_expr(node.args[1])
                to   = self._gen_expr(node.args[2])
                return f"((k_dsgedge({g},{frm},{to})), kv_void())"

            if fn == "dsmatset":
                m   = self._gen_expr(node.args[0])
                r   = self._gen_expr(node.args[1])
                col = self._gen_expr(node.args[2])
                val = self._gen_expr(node.args[3])
                return f"((k_dsmatset({m},{r},{col},{val})), kv_void())"

            if fn == "dsset":
                if len(node.args) == 1:
                    lst = self._gen_expr(node.args[0])
                    return f"k_dsset({lst})"
                elif len(node.args) > 1:
                    # dsset(1, 2, 3) — build inline list then make set
                    parts = "; ".join(
                        f"klist_push(_tmp_sl->list, {self._gen_expr(a)})"
                        for a in node.args)
                    return f"({{ KVal* _tmp_sl=kv_list(); {parts}; k_dsset(_tmp_sl); }})"
                return "kv_list()"

            if fn == "dsgpath":
                g   = self._gen_expr(node.args[0])
                frm = self._gen_expr(node.args[1])
                to  = self._gen_expr(node.args[2])
                return f"k_dsgpath({g},{frm},{to})"

            # ── Concurrency special cases ──────────────────────────────────
            if fn == "cxretry":
                res  = self._gen_expr(node.args[0])
                n    = self._gen_expr(node.args[1]) if len(node.args) > 1 else "kv_int(3LL)"
                mode = self._gen_expr(node.args[2]) if len(node.args) > 2 else 'kv_str("backoff")'
                return f"k_cxretry({res},{n},{mode})"

            if fn == "cxtimeout":
                res  = self._gen_expr(node.args[0])
                secs = self._gen_expr(node.args[1]) if len(node.args) > 1 else "kv_int(30LL)"
                return f"k_cxtimeout({res},{secs})"

            # ── Default path ───────────────────────────────────────────────
            args_c = ", ".join(self._gen_expr(a) for a in node.args)
            if fn in VOID_BUILTINS:
                return f"(({c}({args_c})), kv_void())"
            return f"{c}({args_c})"

        if fn == "range":
            return "kv_void()"

        self._error(f"Unknown built-in: '{fn}'", node)

    def _gen_inline_dict(self, node: DictLiteral) -> str:
        # Emit as a compound statement expression using GCC extension
        parts = []
        for key, val_node in node.pairs:
            kv = self._gen_expr(key)
            vv = self._gen_expr(val_node)
            if isinstance(key, StrLiteral):
                parts.append(f'kdict_set(_tmp_d->dict, {kv}->s, {vv})')
            else:
                parts.append(f'kdict_set(_tmp_d->dict, k_S({kv})->s, {vv})')
        inner = "; ".join(parts)
        return f"({{ KVal* _tmp_d = kv_dict(); {inner}; _tmp_d; }})"

    def _gen_inline_list(self, node: ListLiteral) -> str:
        # Build inline list expression using a comma expression trick
        # We emit a helper block — not clean but works in statement context
        # For inline, we create a temporary
        parts = [f"k_Ladd(_tmp_l, {self._gen_expr(e)})" for e in node.elements]
        return "(({KVal* _tmp_l = kv_list(); " + "; ".join(parts) + "; _tmp_l;}))"
