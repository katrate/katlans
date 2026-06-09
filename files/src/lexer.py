from enum import Enum, auto
from .errors import LexerError


class TT(Enum):
    # ── Literals ────────────────────────────────
    INT_LIT  = auto()
    FLT_LIT  = auto()
    STR_LIT  = auto()
    BOOL_LIT = auto()
    VOID     = auto()

    # ── Type keywords ────────────────────────────
    KW_INT  = auto()   # int
    KW_FLT  = auto()   # flt
    KW_LT   = auto()   # lt
    KW_ZL   = auto()   # zl
    KW_AD   = auto()   # ad

    # ── Control flow ─────────────────────────────
    IF       = auto()
    ELIF     = auto()
    ELSE     = auto()
    DETA     = auto()   # for
    BELONGS  = auto()
    RANGE    = auto()
    IN       = auto()
    DENTA    = auto()   # while
    HALT     = auto()   # break
    CONTINUE = auto()

    # ── Functions & classes ───────────────────────
    FXN     = auto()
    DEC     = auto()
    RETURN  = auto()
    ASYNC   = auto()
    CLS     = auto()
    SELF    = auto()
    FROM    = auto()
    BORROW  = auto()

    # ── Error handling ────────────────────────────
    TRY   = auto()
    CATCH = auto()

    # ── Testing ───────────────────────────────────
    TEST     = auto()
    TESTRUN  = auto()
    TESTCHECK = auto()

    # ── I/O ──────────────────────────────────────
    DISPLAY = auto()
    ASK     = auto()

    # ── Arithmetic operators ──────────────────────
    PLUS    = auto()   # +
    MINUS   = auto()   # -
    STAR    = auto()   # *
    SLASH   = auto()   # /
    DSLASH  = auto()   # //  integer division
    TSLASH  = auto()   # /// modulo
    CARET   = auto()   # ^   power
    PERCENT = auto()   # %   percentage
    DPLUS   = auto()   # ++  string concat

    # ── Comparison operators ──────────────────────
    EQ     = auto()   # ==
    NEQ    = auto()   # =\
    GTE    = auto()   # =>
    LTE    = auto()   # =<
    ASSIGN = auto()   # =
    LT_OP  = auto()   # <
    GT_OP  = auto()   # >

    # ── Logical operators ─────────────────────────
    AND = auto()   # &&
    OR  = auto()   # &+
    NOT = auto()   # !&

    # ── Delimiters ────────────────────────────────
    DCOLON      = auto()   # ::
    BLOCK_OPEN  = auto()   # |>
    BLOCK_CLOSE = auto()   # <|
    LPAREN      = auto()   # (
    RPAREN      = auto()   # )
    LBRACKET    = auto()   # [
    RBRACKET    = auto()   # ]
    LBRACE      = auto()   # {
    RBRACE      = auto()   # }
    COMMA       = auto()   # ,
    SEMICOLON   = auto()   # ;
    DOT         = auto()   # .
    PIPE        = auto()   # |
    STAR_GLOB   = auto()   # * (borrow *)

    # ── Identifier ────────────────────────────────
    IDENT = auto()

    # ── End of file ───────────────────────────────
    EOF = auto()


KEYWORDS = {
    "int":      TT.KW_INT,
    "flt":      TT.KW_FLT,
    "lt":       TT.KW_LT,
    "zl":       TT.KW_ZL,
    "ad":       TT.KW_AD,
    "true":     TT.BOOL_LIT,
    "false":    TT.BOOL_LIT,
    "void":     TT.VOID,
    "if":       TT.IF,
    "elif":     TT.ELIF,
    "else":     TT.ELSE,
    "deta":     TT.DETA,
    "belongs":  TT.BELONGS,
    "range":    TT.RANGE,
    "in":       TT.IN,
    "denta":    TT.DENTA,
    "halt":     TT.HALT,
    "continue": TT.CONTINUE,
    "fxn":      TT.FXN,
    "dec":      TT.DEC,
    "return":   TT.RETURN,
    "async":    TT.ASYNC,
    "cls":      TT.CLS,
    "self":     TT.SELF,
    "from":     TT.FROM,
    "borrow":   TT.BORROW,
    "try":       TT.TRY,
    "catch":     TT.CATCH,
    "test":      TT.TEST,
    "testrun":   TT.TESTRUN,
    "testcheck": TT.TESTCHECK,
    "display":   TT.DISPLAY,
    "ask":       TT.ASK,
}


class Token:
    __slots__ = ("type", "value", "line", "col")

    def __init__(self, type_: TT, value, line: int, col: int):
        self.type  = type_
        self.value = value
        self.line  = line
        self.col   = col

    def __repr__(self):
        return f"Token({self.type.name}, {self.value!r}, {self.line}:{self.col})"


class Lexer:
    def __init__(self, source: str, filename: str = "<stdin>"):
        self.src      = source
        self.filename = filename
        self.pos      = 0
        self.line     = 1
        self.col      = 1
        self.tokens: list[Token] = []

    # ── Helpers ──────────────────────────────────────────────────────────

    def _cur(self) -> str:
        return self.src[self.pos] if self.pos < len(self.src) else ""

    def _peek(self, offset: int = 1) -> str:
        p = self.pos + offset
        return self.src[p] if p < len(self.src) else ""

    def _advance(self) -> str:
        ch = self.src[self.pos]
        self.pos += 1
        if ch == "\n":
            self.line += 1
            self.col = 1
        else:
            self.col += 1
        return ch

    def _tok(self, tt: TT, value=None) -> Token:
        return Token(tt, value, self.line, self.col)

    def _error(self, msg: str):
        raise LexerError(msg, self.line, self.col, self.filename)

    # ── Skip whitespace & comments ────────────────────────────────────────

    def _skip(self):
        while self.pos < len(self.src):
            ch = self._cur()

            # whitespace
            if ch in " \t\r\n":
                self._advance()

            # multi-line comment  ~~ ... ~~
            elif ch == "~" and self._peek() == "~":
                self._advance(); self._advance()
                while self.pos < len(self.src):
                    if self._cur() == "~" and self._peek() == "~":
                        self._advance(); self._advance()
                        break
                    self._advance()

            # single-line comment  ~ ...
            elif ch == "~":
                while self.pos < len(self.src) and self._cur() != "\n":
                    self._advance()

            else:
                break

    # ── String literal ────────────────────────────────────────────────────

    def _read_string(self) -> Token:
        line, col = self.line, self.col
        self._advance()  # opening "
        buf = []
        while self.pos < len(self.src):
            ch = self._cur()
            if ch == '"':
                self._advance()
                return Token(TT.STR_LIT, "".join(buf), line, col)
            if ch == "\\":
                self._advance()
                esc = self._advance()
                buf.append({"n": "\n", "t": "\t", "r": "\r",
                             '"': '"', "\\": "\\"}.get(esc, esc))
            else:
                buf.append(self._advance())
        self._error("Unterminated string literal")

    # ── Number literal ────────────────────────────────────────────────────

    def _read_number(self) -> Token:
        line, col = self.line, self.col
        buf = []
        is_float = False
        while self.pos < len(self.src) and (self._cur().isdigit() or self._cur() == "."):
            if self._cur() == ".":
                if is_float:
                    break
                is_float = True
            buf.append(self._advance())
        raw = "".join(buf)
        if is_float:
            return Token(TT.FLT_LIT, float(raw), line, col)
        return Token(TT.INT_LIT, int(raw), line, col)

    # ── Identifier / keyword ──────────────────────────────────────────────

    def _read_ident(self) -> Token:
        line, col = self.line, self.col
        buf = []
        while self.pos < len(self.src) and (self._cur().isalnum() or self._cur() in "_"):
            buf.append(self._advance())
        word = "".join(buf)
        tt = KEYWORDS.get(word, TT.IDENT)
        return Token(tt, word, line, col)

    # ── Absolute value  |x|  ─────────────────────────────────────────────
    # Handled in the parser as a built-in function call.

    # ── Main tokenise loop ────────────────────────────────────────────────

    def tokenise(self) -> list[Token]:
        while True:
            self._skip()
            if self.pos >= len(self.src):
                self.tokens.append(self._tok(TT.EOF, None))
                break

            ch  = self._cur()
            nx  = self._peek()
            nx2 = self._peek(2)
            line, col = self.line, self.col

            # ── String ──────────────────────────────────
            if ch == '"':
                self.tokens.append(self._read_string())

            # ── Number ──────────────────────────────────
            elif ch.isdigit():
                self.tokens.append(self._read_number())

            # ── Identifier / keyword ─────────────────────
            elif ch.isalpha() or ch == "_":
                self.tokens.append(self._read_ident())

            # ── :: statement end ─────────────────────────
            elif ch == ":" and nx == ":":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.DCOLON, "::", line, col))

            # ── |> block open ────────────────────────────
            elif ch == "|" and nx == ">":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.BLOCK_OPEN, "|>", line, col))

            # ── <| block close ───────────────────────────
            elif ch == "<" and nx == "|":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.BLOCK_CLOSE, "<|", line, col))

            # ── ++ string concat ─────────────────────────
            elif ch == "+" and nx == "+":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.DPLUS, "++", line, col))

            # ── && logical AND ───────────────────────────
            elif ch == "&" and nx == "&":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.AND, "&&", line, col))

            # ── &+ logical OR ────────────────────────────
            elif ch == "&" and nx == "+":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.OR, "&+", line, col))

            # ── !& logical NOT ───────────────────────────
            elif ch == "!" and nx == "&":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.NOT, "!&", line, col))

            # ── /// modulo ───────────────────────────────
            elif ch == "/" and nx == "/" and nx2 == "/":
                self._advance(); self._advance(); self._advance()
                self.tokens.append(Token(TT.TSLASH, "///", line, col))

            # ── // integer division ──────────────────────
            elif ch == "/" and nx == "/":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.DSLASH, "//", line, col))

            # ── == equality ──────────────────────────────
            elif ch == "=" and nx == "=":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.EQ, "==", line, col))

            # ── =\ not equal ─────────────────────────────
            elif ch == "=" and nx == "\\":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.NEQ, "=\\", line, col))

            # ── => greater or equal ──────────────────────
            elif ch == "=" and nx == ">":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.GTE, "=>", line, col))

            # ── =< less or equal ─────────────────────────
            elif ch == "=" and nx == "<":
                self._advance(); self._advance()
                self.tokens.append(Token(TT.LTE, "=<", line, col))

            # ── = assignment ─────────────────────────────
            elif ch == "=":
                self._advance()
                self.tokens.append(Token(TT.ASSIGN, "=", line, col))

            # ── Single-char tokens ───────────────────────
            elif ch == "+":
                self._advance()
                self.tokens.append(Token(TT.PLUS, "+", line, col))
            elif ch == "-":
                self._advance()
                self.tokens.append(Token(TT.MINUS, "-", line, col))
            elif ch == "*":
                self._advance()
                self.tokens.append(Token(TT.STAR, "*", line, col))
            elif ch == "/":
                self._advance()
                self.tokens.append(Token(TT.SLASH, "/", line, col))
            elif ch == "^":
                self._advance()
                self.tokens.append(Token(TT.CARET, "^", line, col))
            elif ch == "%":
                self._advance()
                self.tokens.append(Token(TT.PERCENT, "%", line, col))
            elif ch == "<":
                self._advance()
                self.tokens.append(Token(TT.LT_OP, "<", line, col))
            elif ch == ">":
                self._advance()
                self.tokens.append(Token(TT.GT_OP, ">", line, col))
            elif ch == "(":
                self._advance()
                self.tokens.append(Token(TT.LPAREN, "(", line, col))
            elif ch == ")":
                self._advance()
                self.tokens.append(Token(TT.RPAREN, ")", line, col))
            elif ch == "[":
                self._advance()
                self.tokens.append(Token(TT.LBRACKET, "[", line, col))
            elif ch == "]":
                self._advance()
                self.tokens.append(Token(TT.RBRACKET, "]", line, col))
            elif ch == "{":
                self._advance()
                self.tokens.append(Token(TT.LBRACE, "{", line, col))
            elif ch == "}":
                self._advance()
                self.tokens.append(Token(TT.RBRACE, "}", line, col))
            elif ch == ",":
                self._advance()
                self.tokens.append(Token(TT.COMMA, ",", line, col))
            elif ch == ";":
                self._advance()
                self.tokens.append(Token(TT.SEMICOLON, ";", line, col))
            elif ch == ".":
                self._advance()
                self.tokens.append(Token(TT.DOT, ".", line, col))
            elif ch == "|":
                self._advance()
                self.tokens.append(Token(TT.PIPE, "|", line, col))
            else:
                self._error(f"Unexpected character: {ch!r}")

        return self.tokens
