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
    "one": 23,
    "neg": 24,
    "shl": 25,
    "shr": 26,

    # Control flow
    "jmp": 40,
    "alloc": 41,
    "call": 42,
    "ret": 43,



    # Data transfer
    "pushi": 60,
    "popc": 61,
    "pushl": 62,
    "popl": 63,

    # GPIO
    "setup": 80,
    "read": 81,
    "write": 82,

    # Timing
    "dla": 100,
    "dlb": 101,

    # Misc
    "halt": 250,
    "ubw": 251,
    "usr": 252,
}

LABEL_IDENTIFIER = "(?P<name>[_a-z]([_0-9a-z])*)"

LABEL_PATTERN = LABEL_IDENTIFIER + ":"

INTEGER_PATTERN = "(0x)?([0-9a-f]?[-]?)+"

LABEL_OPERAND_PATTERN = "@" + LABEL_IDENTIFIER

SPLITTER_PATTERN = "[\n\t\r ,]+"

LABEL_REGEX = re.compile(LABEL_PATTERN)

INTEGER_REGEX = re.compile(INTEGER_PATTERN)

LABEL_OPERAND_REGEX = re.compile(LABEL_OPERAND_PATTERN)

t = ["add", "r0", "r1", "start:", "setup", "13", "3", "jmpi", "@cake", "movi", "r12", "1", "write", "13", "r12", "halt", "cake:"]

t = ["setup", "13", "3", "movi", "r10", "500", "movi", "r11", "0", "movi", "r12", "1", "start:", "write", "13", "r12", "dla", "r10", "write", "13", "r11", "dla", "r10", "jmpi", "@start"]

t = ["movi", "r10", "5050", "ubw", "9600", "usr", "r10", "halt"]

t = ["movi", "r11", "2020", "movi", "r10", "1010", "start:", "call", "@func", "jmpi", "@start", "func:", "push", "$lr", "usr", "r10", "call", "@inner", "pop", "$lr", "ret", "inner:", "usr", "r11", "ret"]

t = ["movi", "r11", "500", "usr", "r11", "call", "@func", "usr", "r11", "halt", "func:", "push", "r11", "movi", "r11", "1000", "usr", "r11", "pop", "r11", "ret"]


def process_tokens(tokens):

    labels = {}

    unresolved_labels = {}

    bytecodes = bytearray()

    for token in tokens:
        if token in INSTRUCTION_OPCODES:
            print(f"Opcode '{token}' {INSTRUCTION_OPCODES[token]}")
            bytecodes.append(INSTRUCTION_OPCODES[token])
        elif LABEL_REGEX.fullmatch(token) is not None:
            print(f"Label - '{LABEL_REGEX.fullmatch(token).group("name")}'")
            labels[LABEL_REGEX.fullmatch(token).group("name")] = len(bytecodes)
        elif INTEGER_REGEX.fullmatch(token) is not None:
            print(f"Number {INTEGER_REGEX.fullmatch(token).string}")
            i = int(INTEGER_REGEX.fullmatch(token).string)
            b = i.to_bytes(4, 'little')
            bytecodes.extend(b)
            print(b)
        elif LABEL_OPERAND_REGEX.fullmatch(token) is not None:
            label = LABEL_OPERAND_REGEX.fullmatch(token).group("name")
            print(f"Label operand - '{label}'")
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

        print(address)

    print(unresolved_labels)
    print(labels)

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



source = """

    pushi 100
    write 13

"""

tokens = parse_source(source)

bytes = process_tokens(tokens)

print(f"uint8_t t[{len(bytes)}] = {{", end='')
for byte in bytes:
    print(f"{byte}, ", end='')
print("};")
