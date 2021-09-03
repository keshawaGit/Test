import string
import random
import hashlib
import time
import json
import serial

node_data = {
    'id': 24,
    'timestamp': time.time(),
    'transaction': "MNO1 to MNO2, 100kHz-110kHz, 0245h-0415h",
    'prev_hash': "87751dd7dffdf6a56cca72842f6e663d7396f222bd2758d1f347a63044fa8ce5",
    }

# block_string = json.dumps(block, sort_keys=True).encode()
# example_challenge = '9KzssDFSgdsfgSDFGFGDdfg'
example_challenge = json.dumps(node_data)
ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()

hash_difficulty = '0000000'  # 0 to 6 zeroes

def generation(challenge=example_challenge, size=25):
    # answer = ''.join(random.choice(string.ascii_lowercase + string.ascii_uppercase + string.digits) for x in range(size))
    # answer = ''.join(random.choice(string.digits) for x in range(size))
    nonce = random.randint(0,4294967295) #32bit nonce --> all 0 to all 1
    attempt = challenge + str(nonce)
    return attempt, nonce


shaHash = hashlib.sha256()

def testAttempt():
    Found = False

    #ser.write(b"15\n")
    ser.write(str(10+len(hash_difficulty)).encode('utf-8'))
    time.sleep(1)
    start = time.time()

    while Found == False:
        attempt, nonce = generation()
        shaHash.update(attempt.encode())
        solution = shaHash.hexdigest()


        if solution.startswith(hash_difficulty):
            stop =time.time()
            timeTook = stop - start
            #ser.write(b"0\n")
            ser.write(str(0).encode('utf-8'))
            print(solution)
            print("Nonce: ", nonce)
            print("start Time", start)
            print("End Time", stop)
            print("Hash Difficulty", hash_difficulty)
            print("Time took", timeTook)
            Found = True
            # print(solution.startswith(hash_difficulty))
            return attempt




# def varifierCalc(attempt):
#     verifier_hash = hashlib.sha256(attempt.encode()).hexdigest()
#     print(attempt)
#     print(verifier_hash)
#     print(verifier_hash.startswith(hash_difficulty))


testAttempt()
# attempt = testAttempt()
# varifierCalc(attempt)

# for x in range (0,1000000):
#     testAttempt()
