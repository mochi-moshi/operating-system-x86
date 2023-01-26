import sys
import os
import pickle
# 60 sectors in A File Allocation Directory
# 11 sectors in the cluster FAT

filetable_name = "../bin/filetable.bin"
clustertable_name = "../bin/clustertable.bin"
data_name = '../bin/data.bin'

def main():
    name = sys.argv[1]
    if name == '-package':
        buffer = bytearray()
        with open("../bin/bootcode.bin", 'rb') as f:
            buffer = bytearray(f.read())
        start = len(buffer)
        print(f"0x0000: Bootcode: {start} bytes, {(start) // 512} sectors")
        with open(clustertable_name, 'rb') as f:
            buffer += f.read()
        print(f"{hex(start)}: Cluster Table: {len(buffer) - start} bytes, {(len(buffer) - start) // 512} sectors")
        start = len(buffer)
        with open(filetable_name, 'rb') as f:
            buffer2 = f.read()
        buffer += buffer2
        buffer.extend([0 for _ in range(512*32 - len(buffer2))])
        print(f"{hex(start)}: Root Directory: {len(buffer2)} -> {len(buffer) - start} bytes, {(len(buffer) - start) // 512} sectors")
        start = len(buffer)
        with open(data_name, 'rb') as f:
            buffer += f.read()
        print(f"{hex(start)}: Data: {len(buffer) - start} bytes, {(len(buffer) - start) // 512} sectors")
        with open("../bin/temp.bin", 'wb') as f:
            f.write(buffer)
        return
    if name == '-init':
        with open("../bin/bootcode.bin", 'rb') as f:
            reserved_sectors = len(f.read()) // 512 + 12 + 32
        with open("clust", 'wb') as f:
            pickle.dump(reserved_sectors, f)
        with open(data_name, 'wb') as f:
            pass
        with open(filetable_name, 'wb') as f:
            buffer = b'volume A   \x00'
            buffer += b'\x08\x00\x00\x00\x00\x00'
            buffer += b'\x00\x00\x00\x00'
            buffer += b'\x00\x00\x00\x00'
            buffer += b'\x00\x00'
            buffer += int.to_bytes(512*32, 4, 'little', signed=False)
            f.write(buffer)
        with open(clustertable_name, 'wb') as f:
            buffer = bytearray([0xF8, 0xFF, 0xFF, 0xFF])
            buffer.extend([0xF0 if i % 2 == 0 else 0xFF for i in range((reserved_sectors-2)*2)])
            buffer.extend([0 for _ in range(12*512 - len(buffer))])
            f.write(buffer)
        return
    with open("clust", 'rb') as f:
        current_cluster = pickle.load(f)
    org_name = name
    if '.' in name:
        name, ext = name.split('.')
    else:
        ext = ''
    if len(name) > 8:
        print(f"'{name}' is too long, truncating to {name[:8]}")
        name = name[:8]
    if len(ext) > 3:
        print(f"'{ext}' is too long")
        return
    with open("../bin/"+org_name, 'rb') as f:
        file_cont = bytearray(f.read())
        file_size = len(file_cont)
    name = name.ljust(8, '\x20') + ext.ljust(3, '\x20')
    with open(filetable_name, 'ab') as f:
        buffer = bytearray(bytes(name, 'utf-8')) + b'\x00'
        buffer += b'\x00\x00\x00\x00\x00\x00'
        buffer += b'\x00\x00\xFF\xF0'
        buffer += b'\x00\x00\x00\x00'
        buffer += int.to_bytes(current_cluster, 2, 'little', signed=False)
        buffer += int.to_bytes(file_size, 4, 'little', signed=False)
        f.write(buffer)
    with open(clustertable_name, 'rb') as f:
        clusters = bytearray(f.read())
    start_custer = current_cluster
    print(f"{name} at: {hex(current_cluster)}", end="")
    for _ in range(file_size // 512 - (1 if file_size % 512 == 0 else 0)):
        next_clust = int.to_bytes(current_cluster+1, 2, 'little', signed=False)
        clusters[current_cluster*2] = next_clust[0]
        clusters[current_cluster*2+1] = next_clust[1]
        current_cluster += 1
        print(f", {hex(current_cluster)}", end="")
    clusters[current_cluster*2] = 0xFF
    clusters[current_cluster*2+1] = 0xFF
    current_cluster += 1
    print(f" ({file_size} bytes)")
    with open(data_name, 'ab') as f:
        f.write(file_cont)
        f.write(bytes([((current_cluster >> (4 if i % 2 else 0)) & 0xFF) for i in range(max((current_cluster-start_custer)*512 - len(file_cont), 0))]))
    with open(clustertable_name, 'wb') as f:
        f.write(clusters)
    with open("clust", 'wb') as f:
        pickle.dump(current_cluster, f)
        
if __name__ == '__main__':
    main()