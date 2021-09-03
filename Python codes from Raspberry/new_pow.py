import string
import random
import hashlib
import time
import json
import serial

ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()

veriHash= hashlib.sha256()
shaHash = hashlib.sha256()

node_data = {
    'id': 24,
    'timestamp': time.time(),
    'transaction': "MNO1 to MNO2, 100kHz-110kHz, 0245h-0415h",
    'prev_hash': "87751dd7dffdf6a56cca72842f6e663d7396f222bd2758d1f347a63044fa8ce5",
    }

# block_string = json.dumps(block, sort_keys=True).encode()
# example_challenge = '9KzssDFSgdsfgSDFGFGDdfg'
example_challenge = json.dumps(node_data)

hash_difficulty = '00000' # from single 0 to 6 zeros

def generation(challenge=example_challenge, size=25):
    # answer = ''.join(random.choice(string.ascii_lowercase + string.ascii_uppercase + string.digits) for x in range(size))
    # answer = ''.join(random.choice(string.digits) for x in range(size))
    nonce = random.randint(0,4294967295) #32bit nonce --> all 0 to all 1
    attempt = challenge + str(nonce)
    return attempt, nonce


def testAttempt():
    Found = False


    while Found == False:
        attempt, nonce = generation()
        #
        # shaHash.update(attempt.encode())
        # solution = shaHash.hexdigest()
        solution = hashlib.sha256(attempt.encode()).hexdigest()


        if solution.startswith(hash_difficulty):

            # print("Winner Hash", solution)
            # print("Nonce: ", nonce)

            Found = True
            # print(solution.startswith(hash_difficulty))
            return nonce, attempt

def verification(winner_nonce):
    nonce = winner_nonce
    full_str = example_challenge + str(nonce)
    solution = hashlib.sha256(full_str.encode()).hexdigest()
    print("Verification: ", solution.startswith(hash_difficulty))





# def varifierCalc(attempt):
#     verifier_hash = hashlib.sha256(attempt.encode()).hexdigest()
#     print(attempt)
#     print(verifier_hash)
#     print(verifier_hash.startswith(hash_difficulty))
ser.write(str(11).encode('utf-8'))
start = time.time()
win_nonce, full_str = testAttempt()
timeTook = time.time() - start
print("Time took for Winner", timeTook)

start2 = time.time()
verification(win_nonce)
timeTook2 = time.time() - start2
print("Time took for Verification", timeTook2)
ser.write(str(0).encode('utf-8'))
# varifierCalc(attempt)

# for x in range (0,1000000):
#     testAttempt()
