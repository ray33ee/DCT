
import serial
from compiler import compile
import json

# Send start sequence to wake device
# On exit send exit sequence



def send_source(source, timeout):
    bs = compile(source)

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



def test():
    ser.write(b"5\n")

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

ser = serial.Serial("com3", 115200)

NC = b"\x00"
INPUT = b"\x01"
OUTPUT = b"\x02"
VCC = b"\x03"
GND = b"\x04"


pins = [
    OUTPUT, VCC,        # 1A    VCC
    INPUT, OUTPUT,      # 1Y    6A
    OUTPUT, INPUT,      # 2A    6Y
    INPUT, OUTPUT,      # 2Y    5A
    OUTPUT, INPUT,      # 3A    5Y
    INPUT, OUTPUT,      # 3Y    4A
    GND, INPUT,         # GND   4Y
    NC, NC

]

setup_pins(b"".join(pins))

set_voltage(1000)

send_source(s, 5000)

print("Run result: " + str(prettify_result(run())))

print("Machine state:")
print(json.dumps(get_state(), indent=4))

