
import serial
from compiler import compile
import json

# Send start sequence to wake device
# On exit send exit sequence



def send_bytecode(bs, timeout):

    ser.write(b"0\n")

    ser.write(str(timeout).encode('utf-8') + b"\n")

    ser.write(str(len(bs)).encode('utf-8') + b"\n")

    for b in bs:
        ser.write(str(b).encode('utf-8') + b"\n")

    r = ser.readline()

    print(r)

    assert r == b"ack\n"


def get_state():
    ser.write(b"1\n")

    pc = int(ser.readline()[:-1])
    osp = int(ser.readline()[:-1])
    csp = int(ser.readline()[:-1])
    instruction = int(ser.readline()[:-1])
    if osp != -1:
        peek = int(ser.readline()[:-1])
    else:
        peek = None
    bp = int(ser.readline()[:-1])

    r = ser.readline()

    print(r)

    assert r == b"ack\n"

    return {
        "pc": pc,
        "osp": osp,
        "csp": csp,
        "instruction": instruction,
        "peek": peek,
        "bp": bp,
    }

def run():
    ser.write(b"2\n")

    print("Awaiting run result...")

    r = ser.readline()

    print(r)

    assert r == b"done\n"

    result = int(ser.readline()[:-1])

    if result == 2: # Test resulted in a failure, get the fail code
        code = int(ser.readline()[:-1])
    else:
        code = None

    return result, code

def prettify_result(r):
    result, code = r

    if result == 1:
        return "Test passed!"
    elif result == 2:
        return f"Test failed with code {code}"
    elif result == 3:
        return "Operator stack overflow"
    elif result == 4:
        return "Call stack overflow"
    elif result == 5:
        return "Timeout exceeded"
    elif result == 6:
        return "Program counter overflow"


def setup_pins(pins):
    ser.write(b"3\n")

    ser.write(pins)

    r = ser.readline()

    print(r)

    assert r == b"ack\n"

def set_voltage(v):
    ser.write(b"4\n")

    ser.write(str(v).encode('utf-8') + b"\n")

    r = ser.readline()

    print(r)

    r = ser.readline()

    print(r)

    assert r == b"ack\n"



s = """

def main():
    set_15()
    reset_15()
    set_15()
    reset_15()
    set_15()
    reset_15()
    set_15()
    reset_15()
    set_15()
    reset_15()

"""

s = """


def main():
    set_1()
    set_2()
    set_3()
    set_4()
    set_5()
    set_6()
    set_7()
    set_8()
    set_9()
    set_10()
    set_11()
    set_12()
    set_13()
    set_14()
    set_15()
    set_16()
"""

s = """

def main():
    read_14()

"""

s = """


def main():
    reset_1()
    reset_2()
    reset_3()
    reset_4()
    reset_5()
    reset_6()
    reset_7()
    reset_8()
    reset_9()
    reset_10()
    reset_11()
    reset_12()
    reset_13()
    reset_14()
    reset_15()
    reset_16()
"""
# 1, 4, 5, 8, 9, 12

s = """

def test_low(a, y):
    digitalWrite(a, LOW)

    if digitalRead(y) != HIGH:
        fail(-1 * a)

def test_high(a, y):
    digitalWrite(a, HIGH)

    if digitalRead(y) != LOW:
        fail(-10 * a)

def test_all(a, y):
    test_low(a, y)
    test_high(a, y)

def test():
    test_all(P1, P3)    #1
    test_all(P5, P7)    #2
    test_all(P9, P11)   #3
    test_all(P12, P14)  #4
    test_all(P8, P10)   #5
    test_all(P4, P6)    #6

def main():
    test()


"""

s = """

def main():
    pass

"""

s = """

x = 8

def main():
    pass

"""

a = """
    alloc 1
 	pushi 0
 	pushi 4
 	pushi 0
 	pushi 1
 	pushi 2
 	pushi 3
 	pushm 5
 	popg 0
    call @main
    suc
main:
    alloc 0
    pushg 0
    vpi
    ret 0
    
"""

s = """

A = VPO([P1, P5, P9, P12, P8, P4], 0)
Y = VPI([P3, P7, P11, P14, P10, P6], 0)

def main():
    start = 0
    while start < 63:
        A = start
        if 63 - Y != start:
            fail()
        start = start + 1

"""

ser = serial.Serial("com3", 115200)

NC = b"\x00"
INPUT = b"\x01"
OUTPUT = b"\x02"
VCC = b"\x03"
GND = b"\x04"


pins = [
    OUTPUT, VCC,        #1  1A    VCC   2
    INPUT, OUTPUT,      #3  1Y    6A    4
    OUTPUT, INPUT,      #5  2A    6Y    6
    INPUT, OUTPUT,      #7  2Y    5A    8
    OUTPUT, INPUT,      #9  3A    5Y    10
    INPUT, OUTPUT,      #11 3Y    4A    12
    GND, INPUT,         #13 GND   4Y    14
    NC, NC              #15             16

]

from assembler import assemble
from visitor import translate
from compiler import compile

bs = compile(s)



setup_pins(b"".join(pins))

set_voltage(1000)

send_bytecode(bs, 5000)

print("Run result: " + str(prettify_result(run())))

print("Machine state:")
print(json.dumps(get_state(), indent=4))

