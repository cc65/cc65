blen = 0

def gb(val):
    b = val[0] << 6
    b = b + (val[1] << 4)
    b = b + (val[2] << 2)
    b = b + val[3]
    return '$' + hex(b)[2:4]

def printline(f, val):
    f.write("    .byte " +
        gb(val[0:4]) + ', ' +
        gb(val[4:8]) + ', ' +
        gb(val[8:12]) + ', ' +
        gb(val[12:16]) + ', ' +
        gb(val[16:20]) + ', ' +
        gb(val[20:24]) + ', ' +
        gb(val[24:28]) + ', ' +
        gb(val[28:32]) + '\n')

fname='font160'
with open(fname + '.data', 'rb') as f:
    data = f.read()
fname= fname + '.s'
with open(fname, 'w') as f:
    f.write("    .export _font160\n")
    f.write('    .rodata\n')
    f.write('    .align 256\n')
    f.write("_font160:\n")
    for i in range(0, int(len(data)/32)):
        printline(f, data[i * 32:i * 32 + 32])
