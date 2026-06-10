"""
AST Node definitions for Katlans.
Every node is a plain dataclass for simplicity.
"""
from dataclasses import dataclass, field
from typing import Any, Optional


# ── Base ──────────────────────────────────────────────────────────────────────

@dataclass
class Node:
    line: int = 0
    col:  int = 0


# ── Program ───────────────────────────────────────────────────────────────────

@dataclass
class Program(Node):
    body: list = field(default_factory=list)


# ── Literals ──────────────────────────────────────────────────────────────────

@dataclass
class IntLiteral(Node):
    value: int = 0

@dataclass
class FltLiteral(Node):
    value: float = 0.0

@dataclass
class StrLiteral(Node):
    value: str = ""

@dataclass
class BoolLiteral(Node):
    value: bool = False

@dataclass
class VoidLiteral(Node):
    pass


# ── Template strings ───────────────────────────────────────────────────────────

@dataclass
class TemplateString(Node):
    """e.g. "Hello {name}!" → parts = ["Hello ", Identifier(name), "!"]"""
    parts: list = field(default_factory=list)   # alternating StrLiteral and expressions


# ── Identifiers & access ──────────────────────────────────────────────────────

@dataclass
class Identifier(Node):
    name: str = ""

@dataclass
class MemberAccess(Node):
    obj:    Any = None   # node
    member: str = ""

@dataclass
class IndexAccess(Node):
    obj:   Any = None
    index: Any = None          # node  e.g. x[0]
    end:   Any = None          # node  e.g. x[0;5]
    step:  Any = None          # node  e.g. x[0;5;2]


# ── Variable declarations ──────────────────────────────────────────────────────

@dataclass
class VarDecl(Node):
    """int x = 10 ::  /  x = "hello" ::"""
    dtype:  str = ""          # "int" | "flt" | "lt" | "zl" | "ad" | "str" | ""
    name:   str = ""
    value:  Any = None        # expression node


# ── Assignment ────────────────────────────────────────────────────────────────

@dataclass
class Assignment(Node):
    target: Any = None        # Identifier or MemberAccess
    value:  Any = None


# ── Operators ─────────────────────────────────────────────────────────────────

@dataclass
class BinOp(Node):
    op:    str = ""
    left:  Any = None
    right: Any = None

@dataclass
class UnaryOp(Node):
    op:      str = ""
    operand: Any = None


# ── Collections ───────────────────────────────────────────────────────────────

@dataclass
class ListLiteral(Node):
    elements: list = field(default_factory=list)

@dataclass
class TupleLiteral(Node):
    elements: list = field(default_factory=list)

@dataclass
class DictLiteral(Node):
    pairs: list = field(default_factory=list)   # list of (key_node, val_node)


# ── I/O ───────────────────────────────────────────────────────────────────────

@dataclass
class DisplayStmt(Node):
    value: Any = None

@dataclass
class AskExpr(Node):
    prompt: Any = None


# ── Control flow ──────────────────────────────────────────────────────────────

@dataclass
class IfStmt(Node):
    condition:  Any  = None
    then_body:  list = field(default_factory=list)
    elif_clauses: list = field(default_factory=list)   # list of (cond, body)
    else_body:  list = field(default_factory=list)

@dataclass
class ForRangeStmt(Node):
    var:   str = ""
    start: Any = None
    end:   Any = None
    step:  Any = None
    body:  list = field(default_factory=list)

@dataclass
class ForEachStmt(Node):
    var:      str = ""
    iterable: Any = None
    body:     list = field(default_factory=list)

@dataclass
class WhileStmt(Node):
    condition: Any  = None
    body:      list = field(default_factory=list)

@dataclass
class HaltStmt(Node):   # break
    pass

@dataclass
class ContinueStmt(Node):
    pass

@dataclass
class ReturnStmt(Node):
    values: list = field(default_factory=list)


# ── Functions ─────────────────────────────────────────────────────────────────

@dataclass
class FuncDecl(Node):
    name:       str  = ""
    params:     list = field(default_factory=list)   # list of str
    out_params: list = field(default_factory=list)   # output vars
    body:       list = field(default_factory=list)
    is_async:   bool = False

@dataclass
class FuncCall(Node):
    name:   str  = ""
    args:   list = field(default_factory=list)
    obj:    Any  = None   # for method calls: obj.method


# ── Built-in function call ────────────────────────────────────────────────────

@dataclass
class BuiltinCall(Node):
    func:   str  = ""
    args:   list = field(default_factory=list)


# ── Classes ───────────────────────────────────────────────────────────────────

@dataclass
class ClassDecl(Node):
    name:       str  = ""
    parent:     str  = ""          # "" if no parent
    borrows:    list = field(default_factory=list)   # ["*"] or ["speak"]
    body:       list = field(default_factory=list)

@dataclass
class ClassInstantiate(Node):
    cls_name: str  = ""
    var_name: str  = ""
    args:     list = field(default_factory=list)


# ── Testing ───────────────────────────────────────────────────────────────────

@dataclass
class TestDecl(Node):
    """test dec <name> |> body <|::"""
    name: str = ""
    body: list = field(default_factory=list)


# ── Error handling ────────────────────────────────────────────────────────────

@dataclass
class UseStmt(Node):
    """use "filename.kl" ::  — include another file"""
    filepath: str = ""
    body:     list = field(default_factory=list)   # populated during codegen

@dataclass
class TryCatch(Node):
    try_body:   list = field(default_factory=list)
    error_var:  str  = ""
    catch_body: list = field(default_factory=list)
