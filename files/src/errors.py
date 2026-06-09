class KatlansError(Exception):
    def __init__(self, message, line=None, col=None, filename="<unknown>"):
        self.message = message
        self.line = line
        self.col = col
        self.filename = filename
        super().__init__(self._format())

    def _format(self):
        loc = ""
        if self.line is not None:
            loc = f" at line {self.line}"
            if self.col is not None:
                loc += f", col {self.col}"
        return f"\n\033[31m[Katlans Error]\033[0m {self.filename}{loc}:\n  {self.message}\n"

class LexerError(KatlansError):
    pass

class ParseError(KatlansError):
    pass

class CodeGenError(KatlansError):
    pass
