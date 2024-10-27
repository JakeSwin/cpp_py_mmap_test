import mmap
import time
import numpy as np

def create_shared_memory():
    # Create a memory mapped file
    with open("../shared_memory.bin", "wb") as f:
        # Initialize with zeros
        f.write(b'\0' * 256)
    
    # Open the file and create memory mapping
    with open("../shared_memory.bin", "r+b") as f:
        # Create memory map with read/write access
        mm = mmap.mmap(f.fileno(), 0)
        return mm

def write_to_shared_memory(mm, message):
    # Clear previous content
    mm.seek(0)
    mm.write(b'\0' * mm.size())
    
    # Write new message
    mm.seek(0) 
    mm.write(message.encode())
    mm.flush()

def read_from_shared_memory(mm):
    mm.seek(0)
    data = mm.read().decode().rstrip('\0')
    return data

while True:
    # Create and write to shared memory
    mm = create_shared_memory()
    write_to_shared_memory(mm, "Hello from Python!")
    data = read_from_shared_memory(mm)
    print(f"Read from shared memory: {data}")
    time.sleep(0.2)