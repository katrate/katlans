"""
Katlans Parser  —  tokens → AST
"""
from .lexer import Token, TT
from .ast_nodes import *
from .errors import ParseError


class Parser:
    def __init__(self, tokens: list[Token], filename: str = "<stdin>"):
        self.tokens      = tokens
        self.pos         = 0
        self.filename    = filename
        self._angle_depth = 0   # track <> nesting to know when > closes bracket

    # ── Token helpers ─────────────────────────────────────────────────────

    def _cur(self) -> Token:
        if self.pos >= len(self.tokens):
            return Token(TT.EOF, None, 0, 0)
        return self.tokens[self.pos]

    def _peek(self, offset: int = 1) -> Token:
        p = self.pos + offset
        return self.tokens[p] if p < len(self.tokens) else Token(TT.EOF, None, 0, 0)

    def _advance(self) -> Token:
        tok = self.tokens[self.pos]
        if tok.type != TT.EOF:
            self.pos += 1
        return tok

    def _check(self, *types: TT) -> bool:
        return self._cur().type in types

    def _match(self, *types: TT) -> bool:
        if self._check(*types):
            self._advance()
            return True
        return False

    def _expect(self, tt: TT, msg: str = "") -> Token:
        if self._cur().type != tt:
            tok = self._cur()
            desc = msg or f"expected {tt.name}, got {tok.type.name} ({tok.value!r})"
            raise ParseError(desc, tok.line, tok.col, self.filename)
        return self._advance()

    def _expect_dcolon(self):
        self._expect(TT.DCOLON, "expected '::' to end statement")

    def _error(self, msg: str):
        tok = self._cur()
        raise ParseError(msg, tok.line, tok.col, self.filename)

    def _read_angle_ident_list(self) -> list[str]:
        """Read <name, name, ...> returning list of ident strings."""
        self._expect(TT.LT_OP)
        self._angle_depth += 1
        names = []
        if not self._check(TT.GT_OP):
            names.append(self._expect(TT.IDENT).value)
            while self._check(TT.COMMA):
                self._advance()
                names.append(self._expect(TT.IDENT).value)
        self._angle_depth -= 1
        self._expect(TT.GT_OP)
        return names

    def _read_angle_expr_list(self) -> list:
        """Read <expr, expr, ...> returning list of expression nodes."""
        self._expect(TT.LT_OP)
        self._angle_depth += 1
        args = []
        if not self._check(TT.GT_OP):
            args.append(self._parse_expr())
            while self._check(TT.COMMA):
                self._advance()
                args.append(self._parse_expr())
        self._angle_depth -= 1
        self._expect(TT.GT_OP)
        return args

    # ── Entry point ───────────────────────────────────────────────────────

    def parse(self) -> Program:
        prog = Program(line=1, col=1)
        while not self._check(TT.EOF):
            stmt = self._parse_stmt()
            if stmt is not None:
                prog.body.append(stmt)
        return prog

    # ── Statement dispatcher ──────────────────────────────────────────────

    def _parse_stmt(self):
        tok = self._cur()

        # Variable declarations
        if tok.type in (TT.KW_INT, TT.KW_FLT, TT.KW_LT, TT.KW_ZL, TT.KW_AD):
            return self._parse_typed_decl()

        # Display
        if tok.type == TT.DISPLAY:
            return self._parse_display()

        # if / elif / else
        if tok.type == TT.IF:
            return self._parse_if()

        # for loop
        if tok.type == TT.DETA:
            return self._parse_for()

        # while loop
        if tok.type == TT.DENTA:
            return self._parse_while()

        # halt / continue
        if tok.type == TT.HALT:
            self._advance()
            self._expect_dcolon()
            return HaltStmt(line=tok.line, col=tok.col)

        if tok.type == TT.CONTINUE:
            self._advance()
            self._expect_dcolon()
            return ContinueStmt(line=tok.line, col=tok.col)

        # return
        if tok.type == TT.RETURN:
            return self._parse_return()

        # function declaration
        if tok.type == TT.FXN and self._peek().type == TT.DEC:
            return self._parse_func_decl()

        # async function declaration
        if tok.type == TT.FXN and self._peek().type == TT.ASYNC:
            return self._parse_func_decl()

        # function call:  fxn name <args> ::
        if tok.type == TT.FXN:
            return self._parse_func_call_stmt()

        # class declaration
        if tok.type == TT.CLS and self._peek().type == TT.DEC:
            return self._parse_class_decl()

        # class instantiation:  ClassName varName = cls ClassName <args> ::
        if tok.type == TT.CLS:
            return self._parse_class_inst()

        # use
        if tok.type == TT.USE:
            return self._parse_use()

        # try / catch
        if tok.type == TT.TRY:
            return self._parse_try()

        # test dec <name> |> ... <|::  — test declaration
        if tok.type == TT.TEST:
            return self._parse_test_decl()

        # testcheck <expr> ::  — test assertion (inside test block)
        if tok.type == TT.TESTCHECK:
            return self._parse_testcheck()

        # testrun() ::  — run all tests
        if tok.type == TT.TESTRUN:
            return self._parse_testrun()

        # Built-in function call:  name <args> ::
        if tok.type == TT.IDENT and self._peek().type == TT.LT_OP:
            return self._parse_builtin_call_stmt()

        # self keyword (in class methods) — treat as identifier
        if tok.type == TT.SELF:
            return self._parse_ident_stmt()

        # Identifier-led: assignment or expression
        if tok.type == TT.IDENT:
            return self._parse_ident_stmt()

        # skip stray ::
        if tok.type == TT.DCOLON:
            self._advance()
            return None

        self._error(f"Unexpected token: {tok.type.name} ({tok.value!r})")

    # ── Built-in call statement ────────────────────────────────────────────
    # fwrite <"file.txt", "content"> ::
    # ossys() ::
    
    def _parse_builtin_call_stmt(self) -> BuiltinCall:
        tok = self._advance()   # identifier
        name = tok.value
        args = []
        if self._check(TT.LT_OP):
            args = self._read_angle_expr_list()
        else:
            # Could be a regular function call with parentheses: fwrite("file")
            # This is handled in _parse_primary, but at statement level we handle lt_style
            pass
        self._expect_dcolon()
        return BuiltinCall(func=name, args=args, line=tok.line, col=tok.col)

    # ── Typed variable declaration ─────────────────────────────────────────
    # int x = 10 ::
    # lt x = 1, 2, 3 ::
    # ad x = {key,"val"}; {key2,"val2"} ::

    def _parse_typed_decl(self) -> VarDecl:
        tok   = self._advance()
        dtype = tok.value            # "int" | "flt" | "lt" | "zl" | "ad"
        name_tok = self._expect(TT.IDENT, "expected variable name")
        self._expect(TT.ASSIGN, "expected '='")

        if dtype == "lt":
            value = self._parse_list_literal()
        elif dtype == "zl":
            value = self._parse_tuple_literal()
        elif dtype == "ad":
            if self._check(TT.LBRACE):
                value = self._parse_dict_literal()
            else:
                value = self._parse_expr()
        else:
            value = self._parse_expr()

        self._expect_dcolon()
        return VarDecl(dtype=dtype, name=name_tok.value, value=value,
                       line=tok.line, col=tok.col)

    # ── List / tuple / dict literals ──────────────────────────────────────

    def _parse_list_literal(self) -> ListLiteral:
        line, col = self._cur().line, self._cur().col
        elements = [self._parse_expr()]
        while self._check(TT.COMMA):
            self._advance()
            elements.append(self._parse_expr())
        return ListLiteral(elements=elements, line=line, col=col)

    def _parse_tuple_literal(self) -> TupleLiteral:
        line, col = self._cur().line, self._cur().col
        elements = [self._parse_expr()]
        while self._check(TT.COMMA):
            self._advance()
            elements.append(self._parse_expr())
        return TupleLiteral(elements=elements, line=line, col=col)

    def _parse_dict_literal(self) -> DictLiteral:
        line, col = self._cur().line, self._cur().col
        pairs = []
        pairs.append(self._parse_dict_pair())
        while self._check(TT.SEMICOLON):
            self._advance()
            pairs.append(self._parse_dict_pair())
        return DictLiteral(pairs=pairs, line=line, col=col)

    def _parse_dict_pair(self):
        self._expect(TT.LBRACE, "expected '{'")
        # Key: bare IDENT treated as string key, otherwise parse as expr
        if self._check(TT.IDENT) and self._peek().type == TT.COMMA:
            key_tok = self._advance()
            key = StrLiteral(value=key_tok.value, line=key_tok.line, col=key_tok.col)
        else:
            key = self._parse_expr()
        self._expect(TT.COMMA, "expected ','")
        val = self._parse_expr()
        self._expect(TT.RBRACE, "expected '}'")
        return (key, val)

    # ── Display ───────────────────────────────────────────────────────────
    # display x ::
    # display "hello" ::

    def _parse_display(self) -> DisplayStmt:
        tok = self._advance()
        val = self._parse_expr()
        self._expect_dcolon()
        return DisplayStmt(value=val, line=tok.line, col=tok.col)

    # ── If / elif / else ──────────────────────────────────────────────────

    def _parse_if(self) -> IfStmt:
        tok = self._advance()   # consume 'if'
        cond = self._parse_expr()
        self._expect(TT.BLOCK_OPEN, "expected '|>' after if condition")
        then_body = self._parse_block()

        elif_clauses = []
        else_body = []

        while self._check(TT.ELIF):
            self._advance()
            ec = self._parse_expr()
            self._expect(TT.BLOCK_OPEN, "expected '|>' after elif condition")
            eb = self._parse_block()
            elif_clauses.append((ec, eb))

        if self._check(TT.ELSE):
            self._advance()
            self._expect(TT.BLOCK_OPEN, "expected '|>' after else")
            else_body = self._parse_block()

        return IfStmt(condition=cond, then_body=then_body,
                      elif_clauses=elif_clauses, else_body=else_body,
                      line=tok.line, col=tok.col)

    def _parse_block(self) -> list:
        """Parse statements until <|::"""
        body = []
        while not self._check(TT.EOF):
            # <|::  end of block
            if self._check(TT.BLOCK_CLOSE):
                self._advance()               # consume <|
                self._expect_dcolon()         # consume ::
                break
            stmt = self._parse_stmt()
            if stmt is not None:
                body.append(stmt)
        return body

    # ── For loops ─────────────────────────────────────────────────────────
    # deta i belongs range(1;10;1) |> ... <|::
    # deta i belongs in x |> ... <|::

    def _parse_for(self) -> Node:
        tok = self._advance()   # consume 'deta'
        var_tok = self._expect(TT.IDENT, "expected loop variable name")
        self._expect(TT.BELONGS, "expected 'belongs'")

        if self._check(TT.IN):
            # for-each
            self._advance()
            iterable = self._parse_expr()
            self._expect(TT.BLOCK_OPEN, "expected '|>'")
            body = self._parse_block()
            return ForEachStmt(var=var_tok.value, iterable=iterable, body=body,
                               line=tok.line, col=tok.col)

        # range  —  range(start;end;step)
        self._expect(TT.RANGE, "expected 'range' or 'in'")
        self._expect(TT.LPAREN, "expected '('")
        start = self._parse_expr()
        self._expect(TT.SEMICOLON, "expected ';'")
        end   = self._parse_expr()
        step  = IntLiteral(value=1)
        if self._check(TT.SEMICOLON):
            self._advance()
            step = self._parse_expr()
        self._expect(TT.RPAREN, "expected ')'")
        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        body = self._parse_block()
        return ForRangeStmt(var=var_tok.value, start=start, end=end, step=step,
                            body=body, line=tok.line, col=tok.col)

    # ── While ─────────────────────────────────────────────────────────────
    # denta x > 0 |> ... <|::

    def _parse_while(self) -> WhileStmt:
        tok = self._advance()
        cond = self._parse_expr()
        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        body = self._parse_block()
        return WhileStmt(condition=cond, body=body, line=tok.line, col=tok.col)

    # ── Return ────────────────────────────────────────────────────────────
    # return a + b ::
    # return min(lt), max(lt) ::

    def _parse_return(self) -> ReturnStmt:
        tok = self._advance()
        values = []
        if not self._check(TT.DCOLON):
            values.append(self._parse_expr())
            while self._check(TT.COMMA):
                self._advance()
                values.append(self._parse_expr())
        self._expect_dcolon()
        return ReturnStmt(values=values, line=tok.line, col=tok.col)

    # ── Use (file include) ─────────────────────────────────────────────────────
    # use "filename.kl" ::

    def _parse_use(self) -> UseStmt:
        tok = self._advance()   # consume 'use'
        fname = self._expect(TT.STR_LIT, "expected filename string after 'use'").value
        self._expect_dcolon()
        return UseStmt(filepath=fname, line=tok.line, col=tok.col)

    # ── Function declaration ───────────────────────────────────────────────
    # fxn dec <name> <params> |> ... <|::
    # fxn dec async <name> <params> |> ... <|::

    def _parse_func_decl(self) -> FuncDecl:
        tok = self._advance()   # fxn
        is_async = False
        if self._check(TT.ASYNC):
            self._advance(); is_async = True
        else:
            self._expect(TT.DEC, "expected 'dec'")

        # <name>
        self._expect(TT.LT_OP, "expected '<'")
        self._angle_depth += 1
        name_tok = self._expect(TT.IDENT, "expected function name")
        self._angle_depth -= 1
        self._expect(TT.GT_OP, "expected '>'")

        # <params>
        params = []
        if self._check(TT.LT_OP):
            params = self._read_angle_ident_list()

        # <out_params>
        out_params = []
        if self._check(TT.LT_OP):
            out_params = self._read_angle_ident_list()

        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        body = self._parse_block()
        return FuncDecl(name=name_tok.value, params=params, out_params=out_params,
                        body=body, is_async=is_async,
                        line=tok.line, col=tok.col)

    # ── Function call statement ────────────────────────────────────────────
    # fxn name <args> ::
    # fxn name ::

    def _parse_func_call_stmt(self):
        tok = self._advance()   # fxn
        # Parse a general expression (handles obj.method via postfix)
        expr = self._parse_postfix()
        args = []
        if self._check(TT.LT_OP):
            args = self._read_angle_expr_list()
        self._expect_dcolon()
        
        if isinstance(expr, Identifier):
            # fxn name <args> ::
            return FuncCall(name=expr.name, args=args, line=tok.line, col=tok.col)
        elif isinstance(expr, MemberAccess):
            # fxn obj.method <args> ::
            return FuncCall(name=expr.member, args=args, obj=expr.obj,
                           line=tok.line, col=tok.col)
        self._error(f"Invalid function call target")

    # ── Class declaration ─────────────────────────────────────────────────
    # cls dec <Animal> |> ... <|::
    # cls dec <Dog> from <Animal> borrow * |> ... <|::

    def _parse_class_decl(self) -> ClassDecl:
        tok = self._advance()   # cls
        self._expect(TT.DEC, "expected 'dec'")
        self._expect(TT.LT_OP); self._angle_depth += 1
        name_tok = self._expect(TT.IDENT, "expected class name")
        self._angle_depth -= 1; self._expect(TT.GT_OP)

        parent  = ""
        borrows = []
        if self._check(TT.FROM):
            self._advance()
            self._expect(TT.LT_OP); self._angle_depth += 1
            parent = self._expect(TT.IDENT).value
            self._angle_depth -= 1; self._expect(TT.GT_OP)
            self._expect(TT.BORROW, "expected 'borrow'")
            if self._check(TT.STAR):
                self._advance(); borrows = ["*"]
            else:
                borrows.append(self._expect(TT.IDENT).value)
                while self._check(TT.COMMA):
                    self._advance()
                    borrows.append(self._expect(TT.IDENT).value)

        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        body = self._parse_block()
        return ClassDecl(name=name_tok.value, parent=parent, borrows=borrows,
                         body=body, line=tok.line, col=tok.col)

    # ── Class instantiation ────────────────────────────────────────────────
    # Animal dog = cls Animal <"Rex", 5> ::

    def _parse_class_inst(self) -> ClassInstantiate:
        tok = self._advance()   # cls
        cls_name = self._expect(TT.IDENT, "expected class name").value
        args = []
        if self._check(TT.LT_OP):
            args = self._read_angle_expr_list()
        self._expect_dcolon()
        return ClassInstantiate(cls_name=cls_name, var_name="", args=args,
                                line=tok.line, col=tok.col)

    def _parse_try(self) -> TryCatch:
        tok = self._advance()   # try
        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        try_body = self._parse_block()
        self._expect(TT.CATCH, "expected 'catch'")
        self._expect(TT.LT_OP); self._angle_depth += 1
        err_tok = self._expect(TT.IDENT, "expected error variable")
        self._angle_depth -= 1; self._expect(TT.GT_OP)
        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        catch_body = self._parse_block()
        return TryCatch(try_body=try_body, error_var=err_tok.value,
                        catch_body=catch_body, line=tok.line, col=tok.col)

    # ── Test declaration ───────────────────────────────────────────────────
    # test dec <name> |> ... <|::
    
    def _parse_test_decl(self):
        tok = self._advance()   # test
        self._expect(TT.DEC, "expected 'dec'")
        self._expect(TT.LT_OP); self._angle_depth += 1
        name_tok = self._expect(TT.IDENT, "expected test name")
        self._angle_depth -= 1; self._expect(TT.GT_OP)
        self._expect(TT.BLOCK_OPEN, "expected '|>'")
        body = self._parse_block()
        return TestDecl(name=name_tok.value, body=body, line=tok.line, col=tok.col)
    
    def _parse_testcheck(self):
        tok = self._advance()   # testcheck
        self._expect(TT.LT_OP); self._angle_depth += 1
        expr = self._parse_expr()
        self._angle_depth -= 1; self._expect(TT.GT_OP)
        self._expect_dcolon()
        return BuiltinCall(func="testcheck", args=[expr], line=tok.line, col=tok.col)
    
    def _parse_testrun(self):
        tok = self._advance()   # testrun
        self._expect(TT.LPAREN)
        self._expect(TT.RPAREN)
        self._expect_dcolon()
        return BuiltinCall(func="testrun_summary", args=[], line=tok.line, col=tok.col)

    # ── Identifier-led statement ───────────────────────────────────────────
    # x = expr ::
    # x.member = expr ::
    # TypeName varName = cls ClassName <args> ::

    def _parse_ident_stmt(self):
        tok = self._cur()

        # Class instantiation: ClassName varName = cls ClassName <args> ::
        if self._check(TT.IDENT) and self._peek().type == TT.IDENT:
            cls_name  = self._advance().value
            var_name  = self._advance().value
            self._expect(TT.ASSIGN)
            self._expect(TT.CLS)
            inst_cls  = self._expect(TT.IDENT).value
            args = []
            if self._check(TT.LT_OP):
                args = self._read_angle_expr_list()
            self._expect_dcolon()
            node = ClassInstantiate(cls_name=inst_cls, var_name=var_name,
                                    args=args, line=tok.line, col=tok.col)
            return node

        # x = expr  or  x.member = expr
        target = self._parse_postfix()

        if self._check(TT.ASSIGN):
            self._advance()
            val = self._parse_expr()
            self._expect_dcolon()
            return Assignment(target=target, value=val,
                              line=tok.line, col=tok.col)

        # Infer untyped string declaration:  x = "hello" (already handled above)
        self._expect_dcolon()
        return target   # expression as statement

    # ── Template string parser ──────────────────────────────────────────
    # "Hello {name}!" → parts = [StrLiteral("Hello "), Identifier("name"), StrLiteral("!")]
    
    def _parse_template_string(self, value: str, line: int, col: int) -> "TemplateString":
        """Parse a template string like "Hello {name}!" into TemplateString node."""
        parts = []
        i = 0
        while i < len(value):
            brace_open = value.find("{", i)
            if brace_open == -1:
                parts.append(StrLiteral(value=value[i:], line=line, col=col))
                break
            if brace_open > i:
                parts.append(StrLiteral(value=value[i:brace_open], line=line, col=col))
            # Find matching closing brace
            brace_depth = 1
            j = brace_open + 1
            while j < len(value) and brace_depth > 0:
                if value[j] == '{': brace_depth += 1
                elif value[j] == '}': brace_depth -= 1
                j += 1
            expr_str = value[brace_open+1:j-1]
            # Parse the expression string using a temporary lexer+parser
            from .lexer import Lexer
            expr_lexer = Lexer(expr_str, self.filename)
            expr_tokens = expr_lexer.tokenise()
            # Keep EOF token as sentinel
            if not expr_tokens:
                parts.append(StrLiteral(value="", line=line, col=col))
            else:
                expr_parser = Parser(expr_tokens, self.filename)
                expr_node = expr_parser._parse_expr()
                parts.append(expr_node)
            i = j
        return TemplateString(parts=parts, line=line, col=col)

    # ── Infix operator set ────────────────────────────────────────────────
    
    INFIX_OPS = {"Speeks", "Lhas", "Zhas", "Sfront", "Sback", "Ahas"}
    
    # ── Expressions ───────────────────────────────────────────────────────

    def _parse_expr(self):
        return self._parse_infix()

    def _parse_infix(self):
        """Handle infix operators like: x Speeks "World", nums Lhas 3"""
        left = self._parse_or()
        while self._check(TT.IDENT) and self._cur().value in self.INFIX_OPS:
            op = self._advance().value
            right = self._parse_or()
            # Map infix operators to built-in calls
            func_map = {
                "Speeks": "Speeks",
                "Lhas": "Lhas",
                "Zhas": "Zhas",
                "Sfront": "Sfront",
                "Sback": "Sback",
                "Ahas": "Ahas",
            }
            left = BuiltinCall(func=func_map[op], args=[left, right], line=left.line, col=left.col)
        return left

    def _parse_or(self):
        left = self._parse_and()
        while self._check(TT.OR):
            op = self._advance().value
            right = self._parse_and()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_and(self):
        left = self._parse_not()
        while self._check(TT.AND):
            op = self._advance().value
            right = self._parse_not()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_not(self):
        if self._check(TT.NOT):
            tok = self._advance()
            operand = self._parse_comparison()
            return UnaryOp(op="!&", operand=operand, line=tok.line, col=tok.col)
        return self._parse_comparison()

    def _parse_comparison(self):
        left = self._parse_concat()
        # Only allow < and > as comparisons when NOT inside <> brackets
        while True:
            if self._check(TT.EQ, TT.NEQ, TT.GTE, TT.LTE):
                op = self._advance().value
            elif self._check(TT.LT_OP) and self._angle_depth == 0:
                op = self._advance().value
            elif self._check(TT.GT_OP) and self._angle_depth == 0:
                op = self._advance().value
            else:
                break
            right = self._parse_concat()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_concat(self):
        left = self._parse_add()
        while self._check(TT.DPLUS):
            op = self._advance().value
            right = self._parse_add()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_add(self):
        left = self._parse_mul()
        while self._check(TT.PLUS, TT.MINUS):
            op = self._advance().value
            right = self._parse_mul()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_mul(self):
        left = self._parse_power()
        while self._check(TT.STAR, TT.SLASH, TT.DSLASH, TT.TSLASH, TT.PERCENT):
            op = self._advance().value
            right = self._parse_power()
            left = BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_power(self):
        left = self._parse_unary()
        if self._check(TT.CARET):
            op = self._advance().value
            right = self._parse_power()   # right-associative
            return BinOp(op=op, left=left, right=right,
                         line=left.line, col=left.col)
        return left

    def _parse_unary(self):
        if self._check(TT.MINUS):
            tok = self._advance()
            operand = self._parse_postfix()
            return UnaryOp(op="-", operand=operand, line=tok.line, col=tok.col)
        return self._parse_postfix()

    def _parse_postfix(self):
        node = self._parse_primary()
        while True:
            if self._check(TT.DOT):
                self._advance()
                member = self._expect(TT.IDENT, "expected attribute name")
                node = MemberAccess(obj=node, member=member.value,
                                    line=member.line, col=member.col)
            elif self._check(TT.LBRACKET):
                self._advance()
                idx = self._parse_expr()
                end = step = None
                if self._check(TT.SEMICOLON):
                    self._advance()
                    end = self._parse_expr()
                if self._check(TT.SEMICOLON):
                    self._advance()
                    step = self._parse_expr()
                self._expect(TT.RBRACKET)
                node = IndexAccess(obj=node, index=idx, end=end, step=step,
                                   line=node.line, col=node.col)
            else:
                break
        return node

    def _parse_primary(self):
        tok = self._cur()

        if tok.type == TT.INT_LIT:
            self._advance()
            return IntLiteral(value=tok.value, line=tok.line, col=tok.col)

        if tok.type == TT.FLT_LIT:
            self._advance()
            return FltLiteral(value=tok.value, line=tok.line, col=tok.col)

        if tok.type == TT.STR_LIT:
            self._advance()
            # Check for template string {expr}
            if "{" in tok.value and "}" in tok.value:
                return self._parse_template_string(tok.value, tok.line, tok.col)
            return StrLiteral(value=tok.value, line=tok.line, col=tok.col)

        if tok.type == TT.BOOL_LIT:
            self._advance()
            return BoolLiteral(value=(tok.value == "true"), line=tok.line, col=tok.col)

        if tok.type == TT.VOID:
            self._advance()
            return VoidLiteral(line=tok.line, col=tok.col)

        if tok.type == TT.LPAREN:
            self._advance()
            expr = self._parse_expr()
            self._expect(TT.RPAREN)
            return expr

        # ask <"prompt">
        if tok.type == TT.ASK:
            self._advance()
            self._expect(TT.LT_OP); self._angle_depth += 1
            prompt = self._parse_expr()
            self._angle_depth -= 1; self._expect(TT.GT_OP)
            return AskExpr(prompt=prompt, line=tok.line, col=tok.col)

        # fxn call inside expression: fxn add <a, b>
        if tok.type == TT.FXN:
            self._advance()
            name = self._expect(TT.IDENT).value
            args = []
            if self._check(TT.LT_OP):
                args = self._read_angle_expr_list()
            return FuncCall(name=name, args=args, line=tok.line, col=tok.col)

        if tok.type == TT.SELF:
            self._advance()
            return Identifier(name="self", line=tok.line, col=tok.col)

        if tok.type == TT.IDENT:
            self._advance()
            # function call without fxn keyword (built-ins like sqrt, len, etc.)
            if self._check(TT.LPAREN):
                self._advance()
                args = []
                if not self._check(TT.RPAREN):
                    args.append(self._parse_expr())
                    while self._check(TT.COMMA):
                        self._advance()
                        args.append(self._parse_expr())
                self._expect(TT.RPAREN)
                return BuiltinCall(func=tok.value, args=args,
                                   line=tok.line, col=tok.col)
            return Identifier(name=tok.value, line=tok.line, col=tok.col)

        self._error(f"Unexpected token in expression: {tok.type.name} ({tok.value!r})")
