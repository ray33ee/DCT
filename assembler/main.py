import re

INSTRUCTION_OPCODES = {
    # Math
    "add": 0,
    "addi": 1,
    "sub": 2,
    "subi": 3,
    "inc": 4,
    "dec": 5,
    "mul": 6,
    "muli": 7,

    # Bitwise
    "or": 20,
    "ori": 21,
    "xor": 22,
    "xori": 23,
    "and": 24,
    "andi": 25,
    "one": 26,
    "neg": 27,
    "shl": 28,
    "shr": 29,

    # Control flow
    "jmp": 40,
    "jmpi": 41,
    "call": 42,
    "ret": 43,
    "cmp": 44,
    "cmpi": 45,
    "je": 46,
    "jne": 47,
    "jl": 48,
    "jle": 49,
    "jg": 50,
    "jge": 51,
    "jz": 52,
    "jnz": 53,

    # Data transfer
    "mov": 60,
    "movi": 61,
    "push": 62,
    "pop": 63,

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

SPECIAL_REGISTERS = {
    "$pc": 0,
    "$sp": 1,
    "$lr": 2,
    "$sn": 3,
    "$cm": 4,
    "$io": 5,
}

LABEL_IDENTIFIER = "(?P<name>[_a-z]([_0-9a-z])*)"

LABEL_PATTERN = LABEL_IDENTIFIER + ":"

REGISTER_PATTERN = "r(?P<index>[0-9]+)"

INTEGER_PATTERN = "(0x)?([0-9a-f]?[-]?)+"

LABEL_OPERAND_PATTERN = "@" + LABEL_IDENTIFIER

SPLITTER_PATTERN = "[\n\t\r ,]+"

LABEL_REGEX = re.compile(LABEL_PATTERN)

REGISTER_REGEX = re.compile(REGISTER_PATTERN)

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
        elif token in SPECIAL_REGISTERS:
            bytecodes.append(SPECIAL_REGISTERS[token])
        elif LABEL_REGEX.fullmatch(token) is not None:
            print(f"Label - '{LABEL_REGEX.fullmatch(token).group("name")}'")
            labels[LABEL_REGEX.fullmatch(token).group("name")] = len(bytecodes)
        elif REGISTER_REGEX.fullmatch(token) is not None:
            print(f"Register {REGISTER_REGEX.fullmatch(token).group("index")}")
            bytecodes.append(int(REGISTER_REGEX.fullmatch(token).group("index")))
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

    SETUP   13, 3         ; Setup pin 13 (built in led on arduino nano) as output
    MOVI    R10, 500      ; Move into register 10 the value of 500, we will use this in our delay 
    MOVI    R11, 0        ; Move into register 11 the value of 0, we will use this as our low output
    MOVI    R12, 1        ; Move into register 11 the value of 1, we will use this as our high output
start:
    WRITE   13, R12       ; Turn the LED on
    DLA     R10           ; Wait half a second
    WRITE   13, R11       ; Turn the LED off
    DLA     R10           ; Wait another half second  
    JMP     @start         ; Loop back to start

"""

source  = """

    setup 13, 3
    movi r10, 1
    movi r11, 0
    movi r12, 500
    
start:
    write 13, r10
    DLA     R12           ; Wait half a second
    write 13, r11
    DLA     R12           ; Wait half a second
    JMPI     @start         ; Loop back to start
    
    halt

"""

source = """

    setup 13, 3
    movi r12, 500

loop:
    call @on
    call @delay
    call @off
    call @delay
    jmpi @loop
    
on:
    movi r10, 1
    write 13, r10
    ret

off:
    movi r10, 0
    write 13, r10
    ret
    
delay:
    push r10
    movi r10, 500
    dla r10
    pop r10
    ret

"""

source = """


    movi r26, 4     ; b = 4
    
    mov r6, r26     ; Load b as an argument
    call @square     ; call square
    mov r27, r6     ; Get the return value into a
    
    usr r27         ; uart_send(a)
    
    halt

square:
    push r26
    
    mov r26, r6     ; Load the argument into a local variable register
    mul r26, r26
    mov r6, r26     ; Put the result into a return value register
    
    pop r26
    
    ret


"""

tokens = parse_source(source)

bytes = process_tokens(tokens)

print(f"uint8_t t[{len(bytes)}] = {{", end='')
for byte in bytes:
    print(f"{byte}, ", end='')
print("};")
