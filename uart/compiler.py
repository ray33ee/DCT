
from visitor import translate
from assembler import assemble

def compile(source):
    t = translate(source)
    print(t)
    return assemble(t)

