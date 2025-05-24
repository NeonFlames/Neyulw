import json
import re
import sys

rsr = re.compile(r'^r\..*', re.IGNORECASE)
ssr = re.compile(r'^s\..*', re.IGNORECASE)
tmr = re.compile(r'^t\..*', re.IGNORECASE)
sgr = re.compile(r'^sg\..*', re.IGNORECASE)
slr = re.compile(r'^slate\..*', re.IGNORECASE)
fxr = re.compile(r'^fx\..*', re.IGNORECASE)
nir = re.compile(r'^niagara\..*', re.IGNORECASE)
gcr = re.compile(r'^gc\..*', re.IGNORECASE)
anr = re.compile(r'^a\..*', re.IGNORECASE)
psr = re.compile(r'^pso\..*', re.IGNORECASE)
csr = re.compile(r'^cook\..*', re.IGNORECASE)

isvar = re.compile(r'^var', re.IGNORECASE)

rs = [] #
ss = [] #
tm = [] #
sg = [] #
sl = [] #
fx = [] #
ni = [] #
gc = [] #
an = [] #
ps = [] #
cs = [] #
s = [] # 

with open(sys.argv[1]) as data:
    raw = json.loads(data.read())
    for option in raw:
        if isvar.match(option['type']) == None:
            continue
        if rsr.match(option['name']) != None:
            rs.append(option)
        elif ssr.match(option['name']) != None:
            ss.append(option)
        elif tmr.match(option['name']) != None:
            tm.append(option)
        elif sgr.match(option['name']) != None:
            sg.append(option)
        elif slr.match(option['name']) != None:
            sl.append(option)
        elif fxr.match(option['name']) != None:
            fx.append(option)
        elif nir.match(option['name']) != None:
            ni.append(option)
        elif gcr.match(option['name']) != None:
            gc.append(option)
        elif anr.match(option['name']) != None:
            an.append(option)
        elif psr.match(option['name']) != None:
            ps.append(option)
        elif csr.match(option['name']) != None:
            cs.append(option)
        else:
            s.append(option)

with open(sys.argv[2], mode='w') as out:
    def write_option(o):
        out.write(f'        {o["name"]}:\n          hint: |-\n            {o["help"].replace("\n", "\n            ")}\n')

    out.writelines(['name:\n', 'vars:\n', 'targets:\n', 'presets:\n', 'configs:\n', '  engine:\n', '    SystemSettings:\n', '      name: System\n', '      options:\n'])
    for opt in s:
        write_option(opt)
    for opt in fx:
        write_option(opt)
    for opt in sg:
        write_option(opt)
    for opt in ps:
        write_option(opt)
    for opt in ni:
        write_option(opt)
    for opt in tm:
        write_option(opt)
    for opt in sl:
        write_option(opt)

    out.writelines(['    /Script/Engine.RendererSettings\n', '      name: Rendering\n', '      options:\n'])
    for opt in rs:
        write_option(opt)

    out.writelines(['    /Script/Engine.AnimationSettings\n', '      name: Animation\n', '      options:\n'])
    for opt in an:
        write_option(opt)
    
    out.writelines(['    /Script/Engine.StreamingSettings\n', '      name: Streaming\n', '      options:\n'])
    for opt in ss:
        write_option(opt)

    out.writelines(['    /Script/Engine.GarbageCollectionSettings\n', '      name: Garbage Collection\n', '      options:\n'])
    for opt in gc:
        write_option(opt)
    
    if len(cs) > 0:
        out.writelines(['    /Script/UnrealEd.CookerSettings\n', '      name: Cooker\n', '      options:\n'])
        for opt in cs:
            write_option(opt)

