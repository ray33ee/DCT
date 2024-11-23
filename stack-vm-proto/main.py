
pc = 0
sp = -1
bp = 0

op_stack = []
call_stack = [0] * 1000

# push immediate onto op stack (opushi)
# pop a value from the op stack and push it to the call stack (opopc)
# push a local onto the op stack (opushl)
# pop a variable from the op stack and store it in a variable (opopl)

#def main():
#    a = 20
#    b = 40
#    c = 3 + combine(a, b)

# def combine(x, y):
#    f = x * x + y * y # x x * y y * +
#    return f


machine_code = [
                "call", 3,
                "stop",
                "alloc", 3,
                "opushi", 20,
                "opopl", 1, # a = 20
                "opushi", 40,
                "opopl", 2, # b = 40
                "opushi", 3, # 3
                "opushl", 1,
                "opopc",    # param a
                "opushl", 2,
                "opopc",    # param b
                "call", 30, #combing(a, b)
                "add",  # 3 + combine(a, b)
                "opopl", 3,
                "opushl", 3,
                "ret", 0,
                "alloc", 1,
                "opushl", -2, #x
                "opushl", -2, #x,x
                "mul",      # x*x
                "opushl", -3,# x*x,y
                "opushl", -3,# x*x,y,y
                "mul",  # x*x,y*y
                "add",  # x*x+y*y
                "opopl", 1,
                "opushl", 1,
                "ret", 2,


                ]

timeout = 100000
count = 0

while pc < len(machine_code):
    fetched = machine_code[pc]

    if fetched == "opushi":
        op_stack.append(machine_code[pc+1])
        pc += 2
    elif fetched == "opopc":
        sp += 1
        call_stack[sp] = op_stack.pop()
        pc += 1
    elif fetched == "opushl":
        base_offset = machine_code[pc+1]
        op_stack.append(call_stack[bp+base_offset])
        pc += 2
    elif fetched == "opopl":
        base_offset = machine_code[pc+1]
        call_stack[bp+base_offset] = op_stack.pop()
        pc += 2
    elif fetched == "add":
        a = op_stack.pop()
        b = op_stack.pop()
        op_stack.append(a + b)
        pc += 1
    elif fetched == "mul":
        a = op_stack.pop()
        b = op_stack.pop()
        op_stack.append(a * b)
        pc += 1
    elif fetched == "call":
        link = pc + 2 # Get link address
        sp += 1
        call_stack[sp] = link # Push link address to call stack
        pc = machine_code[pc+1] # Jump to function
    elif fetched == "alloc": # function prelude & local variable allocate
        sp += 1
        call_stack[sp] = bp # Push base pointer onto call stack
        bp = sp # Setup new base pointer
        sp += machine_code[pc+1] # Allocate local variables
        pc += 2
    elif fetched == "ret":
        sp = bp

        bp = call_stack[sp]
        sp -= 1
        link = call_stack[sp]
        sp -= 1
        # drop parameters
        sp -= machine_code[pc+1]
        # jump to link
        pc = link
    elif fetched == "print":
        print(f"Call: {call_stack}")
        print(f"Call: {op_stack}")
        pc += 1
    elif fetched == "stop":
        break


    count += 1

    if count >= timeout:
        print("Machine timeout exceeded")
        break

print(call_stack)
print(op_stack)
print(sp)