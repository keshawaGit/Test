import hashlib
import tinyec.ec as ec
import tinyec.registry as reg
from time import time
from timer import Timer
import serial

t = Timer()
ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()
# extrapoint = ec.Point(curve, 0xf22c4395213e9ebe67ddecdd87fdbd01be16fb059b9753a4, 0x264424096af2b3597796db48f8dfb41fa9cecc97691a9c79)
ser.write(str(11).encode('utf-8'))
t.start()
# select curve
curve = reg.get_curve("secp192r1")

# generator for secp192r1 --> https://neuromancer.sk/std/secg/secp192r1#
G = ec.Point(curve, 0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012, 0x07192b95ffc8da78631011ed6b24cdd573f977a11e794811)

# constant x --> B = x.G
x = 0xa78a123d60baec0c5dd41b33a542463a8255391af64c74ee

# point B --> B = x.G
B = x*G

# point P
P = ec.Point(curve, 0xd458e7d127ae671b0c330266d246769353a012073e97acf8, 0x325930500d851f336bddc050cf7fb11b5673a1645086df3b)

# Gerenate r and A --> A = r.G
#t.start() #start count


r = 0xa12a123d60baec0c0dd41b13a542483a8255391af64c6400
A = r*G

# string concatenation
str1 = str(x*P)
str2 = str(r*P)
str3 = str(A)
prover_hash = str1 + str2 + str3

# hashing
prover_hash = hashlib.sha1(prover_hash.encode()).hexdigest()

# converting hash value to an integer
c = int(prover_hash,16)

# calcualting s
s = r + c*x
t.stop()
ser.write(str(0).encode('utf-8'))

# --------------------VERIFIERS------------------------
# knows EC, G, P, B, s, xP, rP, A

# string concatenation
str4 = str(x*P)
str5 = str(r*P)
str6 = str(A)
verifier_hash = str1 + str2 + str3

# hashing
verifier_hash = hashlib.sha1(verifier_hash.encode()).hexdigest()

# converting hash value to an integer
c2 = int(verifier_hash,16)

print(s*G == A + c2*B)
print(s*P == r*P + (c*x)*P)

#t.stop() #end count
