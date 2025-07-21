import re

INSTRUCTION_OPCODES = {
    # Math
    "add": 0,
    "sub": 1,
    "mul": 2,
    "rng": 3,
    "seed": 4,

    # Bitwise
    "or": 20,
    "xor": 21,
    "and": 22,
    "inv": 23,
    "neg": 24,
    "shl": 25,
    "shr": 26,
    "bool": 27,
    "not": 28,

    # Control flow
    "jmp": 40,
    "alloc": 41,
    "call": 42,
    "ret": 43,
    "eq": 44,
    "ne": 45,
    "gt": 46,
    "lt": 47,
    "ge": 48,
    "le": 49,
    "jt": 50,
    "jf": 51,

    # Data transfer
    "pushi": 60,
    "popc": 61,
    "pushl": 62,
    "popl": 63,
    "drop": 64,

    # GPIO
    "read": 81,
    "write": 82,

    # Timing
    "dla": 100,
    "dlb": 101,

    # Set
    "set1": 111,
    "set2": 112,
    "set3": 113,
    "set4": 114,
    "set5": 115,
    "set6": 116,
    "set7": 117,
    "set8": 118,
    "set9": 119,
    "set10": 120,
    "set11": 121,
    "set12": 122,
    "set13": 123,
    "set14": 124,
    "set15": 125,
    "set16": 126,

    # Reset

    "rst1": 141,
    "rst2": 142,
    "rst3": 143,
    "rst4": 144,
    "rst5": 145,
    "rst6": 146,
    "rst7": 147,
    "rst8": 148,
    "rst9": 149,
    "rst10": 150,
    "rst11": 151,
    "rst12": 152,
    "rst13": 153,
    "rst14": 154,
    "rst15": 155,
    "rst16": 156,

    # Read
    "get1": 171,
    "get2": 172,
    "get3": 173,
    "get4": 174,
    "get5": 175,
    "get6": 176,
    "get7": 177,
    "get8": 178,
    "get9": 179,
    "get10": 180,
    "get11": 181,
    "get12": 182,
    "get13": 183,
    "get14": 184,
    "get15": 185,
    "get16": 186,

    # Misc
    "suc": 201,
    "fail": 202,
}

LABEL_IDENTIFIER = "(?P<name>[_a-z]([_0-9a-z])*)"

LABEL_PATTERN = LABEL_IDENTIFIER + ":"

INTEGER_PATTERN = "(0x)?([0-9a-f]?[-]?)+"

LABEL_OPERAND_PATTERN = "@" + LABEL_IDENTIFIER

SPLITTER_PATTERN = "[\n\t\r ,]+"

LABEL_REGEX = re.compile(LABEL_PATTERN)

INTEGER_REGEX = re.compile(INTEGER_PATTERN)

LABEL_OPERAND_REGEX = re.compile(LABEL_OPERAND_PATTERN)

def process_tokens(tokens):

    labels = {}

    unresolved_labels = {}

    bytecodes = bytearray()

    for token in tokens:
        if token in INSTRUCTION_OPCODES:
            #print(f"Opcode '{token}' {INSTRUCTION_OPCODES[token]}")
            bytecodes.append(INSTRUCTION_OPCODES[token])
        elif LABEL_REGEX.fullmatch(token) is not None:
            #print(f"Label - '{LABEL_REGEX.fullmatch(token).group("name")}'")
            labels[LABEL_REGEX.fullmatch(token).group("name")] = len(bytecodes)
        elif INTEGER_REGEX.fullmatch(token) is not None:
            #print(f"Number {INTEGER_REGEX.fullmatch(token).string}")
            i = int(INTEGER_REGEX.fullmatch(token).string)
            b = i.to_bytes(4, 'little', signed = True)
            bytecodes.extend(b)
        elif LABEL_OPERAND_REGEX.fullmatch(token) is not None:
            label = LABEL_OPERAND_REGEX.fullmatch(token).group("name")
            #print(f"Label operand - '{label}'")
            i = 0
            if label in labels:
                i = labels[label]
            else:
                unresolved_labels[len(bytecodes)] = label
            b = i.to_bytes(4, 'little')
            bytecodes.extend(b)

        else:
            print(f"Unknown token - {token}")
            exit(-1)

    for location in unresolved_labels:

        label = unresolved_labels[location]

        address = labels[label]

        b = address.to_bytes(4, 'little')

        # Replace the placeholder value with the label address
        for i, byte in enumerate(b):
            bytecodes[location + i] = byte

    return bytecodes

def parse_source(source):
    source = source.lower()
    l = []
    for item in re.split("\n", source):
        code = re.split(";", item)[0]
        for s in re.split(SPLITTER_PATTERN, code):
            if s != "":
                l.append(s)
    return l


def assemble(source):
    tokens = parse_source(source)

    return process_tokens(tokens)