import hashlib
import time

max_nonce = 2 ** 14 # 4 billion

def proof_of_work(header, difficulty_bits):

    # calculate the difficulty target
    target = 2 ** (256-difficulty_bits)

    for nonce in range(max_nonce):

        # string_object =  str(header)+str(nonce)
        # block_string = string_object.encode()
        #
        # raw_hash = hashlib.sha256(block_string)
        # hex_hash = raw_hash.hexdigest()

        hash_result = hashlib.sha256((str(header)+str(nonce)).encode()).hexdigest()
        # print (hash_result) # -> Print the hash value
        # check if this is a valid result, below the target
        if int(hash_result, 16) < target:
            print ("Success with nonce %d" % nonce)
            print ("Hash is %s" % hash_result)
            return (hash_result,nonce)



    print ("Failed after %d (max_nonce) tries" % nonce)
    return hash_result, nonce


if __name__ == '__main__':

    nonce = 0
    hash_result = ''


    # difficulty from 0 to 31 bits
    original_max_range = 32
    test_range = 32
    for difficulty_bits in range(test_range):

        difficulty = 2 ** difficulty_bits
        print ("Difficulty: %ld (%d bits)" % (difficulty, difficulty_bits))

        print ("Starting search...")

        # checkpoint the current time
        start_time = time.time()

        # make a new block which includes the hash from the previous block
        # we fake a block of transactions - just a string
        new_block = 'test block with transactions' + hash_result

        # find a valid nonce for the new block
        (hash_result, nonce) = proof_of_work(new_block, difficulty_bits)

        # checkpoint how long it took to find a result
        end_time = time.time()

        elapsed_time = end_time - start_time
        print ("Elapsed Time: %.4f seconds" % elapsed_time)
        # print ("Total Count", count_total)
        # print ("Total Success", count_suc)

        if elapsed_time > 0:

            # estimate the hashes per second
            hash_power = float(int(nonce)/elapsed_time)
            print ("Hashing Power: %ld hashes per second" % hash_power)
