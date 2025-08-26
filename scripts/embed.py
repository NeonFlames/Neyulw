import sys
from pathlib import Path

if len(sys.argv) != 3:
    exit(1)

basename = Path(sys.argv[1]).name
name = basename.replace('.','_').replace('-','_') # TODO: Improve
deposit = sys.argv[2]
raw_data: bytes

with open(sys.argv[1], 'rb') as file:
    raw_data = file.read()

with open(f'{deposit}/{basename}.h', 'w') as file:
    file.writelines([
        f'extern "C" unsigned char __{name}_data[];\n',
        f'extern "C" unsigned int __{name}_size;\n',
    ])

with open(f'{deposit}/{basename}.c', 'w') as file:
    file.writelines([
        f'unsigned char __{name}_data[] = {{0x{raw_data.hex(" ",1).replace(" ",",0x")}}};\n',
        f'unsigned int __{name}_size = {len(raw_data)};\n',
    ])
