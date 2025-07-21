
import ast
import symbols
import symtable

CONSTANTS = {
    "HIGH": 1,
    "LOW": 0,

    "P1": 0,
    "P2": 1,
    "P3": 2,
    "P4": 3,
    "P5": 4,
    "P6": 5,
    "P7": 6,
    "P8": 7,
    "P9": 8,
    "P10": 9,
    "P11": 10,
    "P12": 11,
    "P13": 12,
    "P14": 13,
    "P15": 14,
    "P16": 15,
}

BUILT_IN = {
    "delay": "dla",
    "delayMicroseconds": "dlb",
    "digitalWrite": "write",
    "digitalRead": "read",
    "success": "suc",
    "fail": "fail",
    "bool": "bool",
    "rng": "rng",
    "seed": "seed",

    "set_1": "set1",
    "set_2": "set2",
    "set_3": "set3",
    "set_4": "set4",
    "set_5": "set5",
    "set_6": "set6",
    "set_7": "set7",
    "set_8": "set8",
    "set_9": "set9",
    "set_10": "set10",
    "set_11": "set11",
    "set_12": "set12",
    "set_13": "set13",
    "set_14": "set14",
    "set_15": "set15",
    "set_16": "set16",

    "reset_1": "rst1",
    "reset_2": "rst2",
    "reset_3": "rst3",
    "reset_4": "rst4",
    "reset_5": "rst5",
    "reset_6": "rst6",
    "reset_7": "rst7",
    "reset_8": "rst8",
    "reset_9": "rst9",
    "reset_10": "rst10",
    "reset_11": "rst11",
    "reset_12": "rst12",
    "reset_13": "rst13",
    "reset_14": "rst14",
    "reset_15": "rst15",
    "reset_16": "rst16",

    "read_1": "get1",
    "read_2": "get2",
    "read_3": "get3",
    "read_4": "get4",
    "read_5": "get5",
    "read_6": "get6",
    "read_7": "get7",
    "read_8": "get8",
    "read_9": "get9",
    "read_10": "get10",
    "read_11": "get11",
    "read_12": "get12",
    "read_13": "get13",
    "read_14": "get14",
    "read_15": "get15",
    "read_16": "get16",

}

def translate(source):
    n = ast.parse(source)

    m = symbols.get_map(source, n)

    print(m)

    tr = Translator(m)

    tr.visit(n)

    return "\n".join(tr.code)


class Translator(ast.NodeVisitor):


    def __init__(self, table):
        self.code = ["    call @main", "    suc"]
        self.table = table
        self.context = None

    def get_offset(self, identifier):
        if identifier in self.table[self.context]["params"]:
            index = -self.table[self.context]["params"][identifier]-2
        elif identifier in self.table[self.context]["locals"]:
            index = self.table[self.context]["locals"][identifier]+1
        else:
            print(f"Could not identify '{identifier}' in {self.context}")
            exit(-2)

        return index

    def push_label(self, label):
        self.code.append(label + ":")

    def push_instruction(self, instruction, operands=[]):
        self.code.append("    " + instruction + " " + " ".join(operands))

    def get_unique_label(self, node, index):
        return "_" + str(id(node)) + "_" + str(index)

    def traverse(self, node):
        if isinstance(node, list):
            for n in node:
                self.traverse(n)
        else:
            return super().visit(node)

    # /* Declarations */

    def visit_FunctionDef(self, node):
        self.push_label(node.name)
        self.push_instruction("alloc", [str(len(self.table[node.name]["locals"]))])

        self.context = node.name
        self.traverse(node.body)
        self.context = None

        self.push_instruction("ret", [str(len(node.args.args))])

    # /* Statements */

    def visit_Return(self, node):
        if node.value is not None:
            self.traverse(node.value)
        else:
            print("Empty return statements not supported (we need to make sure the caller pops values off the stack if they are not used)")
            exit(-6)

    def visit_Assign(self, node):

        # Evaluate the expression
        self.traverse(node.value)
        # Pop a value off the op stack and into each node.targets

        for target in node.targets:
            if type(target) is ast.Name:
                index = self.get_offset(target.id)
                self.push_instruction("popl", [str(index)])
            else:
                print(f"Invalud assignment target ({target})")
                exit(-3)

    def visit_If(self, node):
        self.traverse(node.test)

        self.push_instruction("jf", ["@" + self.get_unique_label(node, "else")])

        self.traverse(node.body)

        self.push_instruction("jmp", ["@" + self.get_unique_label(node, "end")])

        self.push_label(self.get_unique_label(node, "else"))

        self.traverse(node.orelse)

        self.push_label(self.get_unique_label(node, "end"))

    def visit_While(self, node):

        start = self.get_unique_label(node, "start")
        end = self.get_unique_label(node, "end")

        self.push_label(start)

        self.traverse(node.test)

        self.push_instruction("jf", ["@" + end])

        self.traverse(node.body)

        self.push_instruction("jmp", ["@" + start])

        self.push_label(end)

    def visit_Expr(self, node):
        self.traverse(node.value)

        # If the Expr is a call to a non-built in function, then drop the unused values from the stack
        if type(node.value) is ast.Call:
            if node.value.func.id not in BUILT_IN:
                print(self.table[node.value.func.id]["return"])
                for i in range(self.table[node.value.func.id]["return"]):
                    self.push_instruction("drop")

    # /* Compound expressions */

    def visit_BinOp(self, node):
        self.traverse(node.left)
        self.traverse(node.right)

        if isinstance(node.op, ast.Mult):
            self.push_instruction("mul")
        elif isinstance(node.op, ast.Add):
            self.push_instruction("add")
        elif isinstance(node.op, ast.Sub):
            self.push_instruction("sub")
        elif isinstance(node.op, ast.LShift):
            self.push_instruction("shl")
        elif isinstance(node.op, ast.RShift):
            self.push_instruction("shr")
        elif isinstance(node.op, ast.BitOr):
            self.push_instruction("or")
        elif isinstance(node.op, ast.BitAnd):
            self.push_instruction("and")
        elif isinstance(node.op, ast.BitXor):
            self.push_instruction("xor")

        else:
            print("Not implemented")
            exit(-4)

    def visit_UnaryOp(self, node):
        self.traverse(node.operand)

        if isinstance(node.op, ast.Invert):
            self.push_instruction("inv")
        elif isinstance(node.op, ast.Not):
            self.push_instruction("bool")
            self.push_instruction("not")
        elif isinstance(node.op, ast.UAdd):
            pass
        elif isinstance(node.op, ast.USub):
            self.push_instruction("neg")

    def visit_BoolOp(self, node):

        for value in node.values:
            self.traverse(value)
            self.push_instruction("bool")

        ins = None
        if isinstance(node.op, ast.And):
            ins = ("and")
        elif isinstance(node.op, ast.Or):
            ins = ("or")

        for i in range(len(node.values)-1):
            self.push_instruction(ins)

    def visit_Call(self, node):
        assert type(node.func) is ast.Name

        if node.func.id in BUILT_IN:
            ins = BUILT_IN[node.func.id]
        else:
            ins = None

        for a in node.args:
            self.traverse(a)
            if ins is None:
                self.push_instruction("popc")

        # CHeck node.func.id for any recognised built in functions

        if ins is not None:
            self.push_instruction(ins)
        else:
            assert node.func.id in self.table
            self.push_instruction("call", ["@" + node.func.id])

    # /* Simple expressions */

    def visit_Name(self, node):

        if node.id in CONSTANTS:
            self.push_instruction("pushi", [str(CONSTANTS[node.id])])
        else:

            index = self.get_offset(node.id)

            self.push_instruction("pushl", [str(index)])

    def visit_Constant(self, node):
        if isinstance(node.value, bool):
            self.push_instruction("pushi", ["1" if node.value else "0"])
        elif isinstance(node.value, int):
            self.push_instruction("pushi", [str(node.value)])
        else:
            print("Constants must be integers or booleans only")
            exit(-3)

    def visit_Compare(self, node):

        if len(node.ops) != 1 and len(node.comparators) != 1:
            print("Chained comparisons not supported")
            exit(-4)

        self.traverse(node.left)
        self.traverse(node.comparators[0])

        comp = node.ops[0]

        if isinstance(comp, ast.Eq):
            self.push_instruction("eq")
        elif isinstance(comp, ast.NotEq):
            self.push_instruction("ne")
        elif isinstance(comp, ast.Lt):
            self.push_instruction("lt")
        elif isinstance(comp, ast.Gt):
            self.push_instruction("gt")
        elif isinstance(comp, ast.LtE):
            self.push_instruction("le")
        elif isinstance(comp, ast.GtE):
            self.push_instruction("ge")
        else:
            print(f"is, is not, in and not in not supported")
            exit(-5)





