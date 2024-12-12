
import ast
import visitor
import symtable
import symbols

# Convert python like code into our assembly language
# No stack variables, only registers
# Only 32 bit integers
# No heap or load/store

# Expressions

# x = i * (a + b)

s = """

def main():

    LED_PIN = 13
    
    pinMode(LED_PIN, OUTPUT)

    while True:
        digitalWrite(LED_PIN, HIGH)
        delay(500)
        digitalWrite(LED_PIN, LOW)
        delay(500)
 
"""

s = """

def main():
    a = something()

def something():
    return -4 << 77

"""

s = """

def main():
    serialSend(~100 - 2)

"""


n = ast.parse(s)

sym = symtable.symtable(s, "", "exec")

map = symbols.get_map(s, n)
print(map)

print(sym.get_children())



print(ast.dump(n, indent=4))

tr = visitor.Translator(map)

tr.visit(n)

print(tr.code)

print("\n".join(tr.code))
