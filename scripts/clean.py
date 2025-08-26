import sys

if len(sys.argv) != 4:
    exit(1)

def replace(data: bytearray, pat: bytes):
    pat_len = len(pat)
    index = data.find(pat)
    while index != -1:
        eos = data.find(0, index)
        straw = data[index+pat_len:eos]
        straw_len = len(straw)
        for i in range(index, eos):
            si = i-index
            if si < straw_len:
                data[i] = straw[si]
            else:
                data[i] = 0
        index = data.find(pat)

pat_utf8 = sys.argv[3].encode()
pat_ascii = sys.argv[3].encode('ascii')
data: bytearray

with open(sys.argv[1], 'rb') as file:
    data = bytearray(file.read())

replace(data, pat_utf8)
replace(data, pat_ascii)

with open(sys.argv[2], 'wb') as file:
    file.write(data)
