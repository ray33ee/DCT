

# Registers
pc = 0
csp = 0 # call stack pointer
cbp = 0 # call stack base pointer
osp = 0 # operand stack pointer
obp = 0 # operand stack base pointer

# Stack
stack = []

# Code
instructions = ["pushi", 10, "pushi", 20, "add", "end", "opn", 2, "pushl", -2, "pushl", -3, "add"]

while True:
    fetched = instructions[pc]

    if fetched == "pushi":
        stack.append(instructions[pc+1])
        pc += 2
    elif fetched == "add":
        a = stack.pop()
        b = stack.pop()
        stack.append(a + b)
        pc += 1
    elif fetched == "end":
        break

print(stack)

