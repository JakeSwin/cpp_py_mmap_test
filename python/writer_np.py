import numpy as np
import random
import time

mm_array = np.memmap("/home/swin/shmtest/uavdata.bin",
                     dtype=np.float32,
                     mode='w+',
                     shape=(8,))

mm_array[:] = np.arange(1, 9, 1)
mm_array.flush()

next_idx = 9
while True:
    if (mm_array == np.full((8,), -1)).all():
        mm_array[:] = np.arange(next_idx, next_idx+8, 1)
        next_idx = next_idx + 8
        time.sleep(random.uniform(0.02, 0.1))
    else:
        print(mm_array)
        time.sleep(0.02)