import re

INSTRUCTION_OPCODES = {
    # Math
    "add": 0,
    "sub": 1,
    "inc": 2,
    "dec": 3,
    "mul": 4,

    # Bitwise
    "or": 20,
    "xor": 21,
    "and": 22,
    "inv": 23,
    "neg": 24,
    "shl": 25,
    "shr": 26,
    "bool": 27,

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
    "setup": 80,
    "read": 81,
    "write": 82,

    # Timing
    "dla": 100,

    # Misc
    "halt": 200,
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