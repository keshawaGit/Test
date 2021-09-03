import tinyec.ec as ec
import tinyec.registry as reg
import random
from time import time
from timer import Timer
import serial

t = Timer()
ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()

# select curve
curve = reg.get_curve("secp192r1")

# generator for secp192r1 --> https://neuromancer.sk/std/secg/secp192r1#
G = ec.Point(curve, 0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012, 0x07192b95ffc8da78631011ed6b24cdd573f977a11e794811)


#private key 1
key1 = random.getrandbits(192)

#private key 2
key2 = random.getrandbits(192)

#Point 1
p1 = key1*G

#Point 2
p2 = key2*G

ser.write(str(11).encode('utf-8'))
t.start()
#Point Addition
p3 = p1 + p2
t.stop()
ser.write(str(0).encode('utf-8'))

print(p3)
