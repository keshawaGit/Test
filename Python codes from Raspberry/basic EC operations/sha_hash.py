import string
import random
import hashlib
import json
from time import time
from timer import Timer
import serial

t = Timer()
ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()

hashdata = {
    'id': 24,
    'timestamp': time(),
    'transaction': "MNO1 to MNO2, 100kHz-110kHz, 0245h-0415h",
    'prev_hash': "87751dd7dffdf6a56cca72842f6e663d7396f222bd2758d1f347a63044fa8ce5",
    }

hashdata_str = json.dumps(hashdata)

shaHash = hashlib.sha256()
ser.write(str(11).encode('utf-8'))
t.start()
shaHash.update(hashdata_str.encode())
t.stop()
ser.write(str(0).encode('utf-8'))
print(shaHash.hexdigest())
