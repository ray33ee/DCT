
import serial
from compiler import compile

# Send start sequence to wake device
# On exit send exit sequence

s = """

def main():
    success()

"""

def send_source(source):
    bs = compile(source)

    ser.write(b"0\n")

    ser.write(str(len(bs)).encode('utf-8') + b"\n")

    ser.write(bs)


def get_state():
    ser.write(b"1\n")

    pc = int(ser.readline()[:-1])
    osp = int(ser.readline()[:-1])
    csp = int(ser.readline()[:-1])
    instruction = int(ser.readline()[:-1])
    peek = int(ser.readline()[:-1])
    bp = int(ser.readline()[:-1])

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

    r = ser.readline()

    assert r == b"done\n"

    result = int(ser.readline()[:-1])

    if result == 2: # Test resulted in a failure, get the fail code
        code = int(ser.readline()[:-1])
    else:
        code = None

    return result, code


ser = serial.Serial("com3", 115200)

send_source(s)

print("Run result: " + str(run()))

print(get_state())

