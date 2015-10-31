
# Module "util"
import string

# Source: http://stackoverflow.com/a/2257449
# Source: http://stackoverflow.com/a/23728630/2213647:
def generate_random_id(length=6, chars=string.ascii_uppercase + string.digits):
    import random
    return ''.join(random.SystemRandom().choice(chars) for _ in range(length))
    
def generate_random_unique_id(
        target_dict, 
        prefix="", 
        length=6, 
        chars=string.ascii_uppercase + string.digits):
    while True:
        tmp_id = prefix + generate_random_id()
        if tmp_id not in target_dict:
            return tmp_id

