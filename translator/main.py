
import ast

# Convert python like code into our assembly language
# No stack variables, only registers
# Only 32 bit integers
# No heap or load/store

# Expressions

# x = i * (a + b)



n = ast.parse("""

b = 4

a = square(b)

uart_send(a)

def square(x):
    return x * x
 
""")

t = """


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

print(ast.dump(n, indent=4))
