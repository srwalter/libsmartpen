#!/usr/bin/python

import sys

class BitReader(object):
    def __init__(self, stream):
        self.stream = stream
        self.byte = 0
        self.nbits = 0

    def get_bits(self, n=1):
        while n > self.nbits:
            x = self.stream.read(1)
            if x == "":
                print self.nbits
                print "%x" % self.byte
                raise EOFError

            x = ord(x)
            self.byte = (self.byte << 8) | x
            self.nbits += 8
        x = self.byte >> (self.nbits - n)
        self.byte -= x << (self.nbits - n)
        self.nbits -= n
        return x

f = file(sys.argv[1])
br = BitReader(f)

magic = br.get_bits(16)
if magic != 0x0100:
    print "bad magic %x" % magic
    sys.exit(1)

version = f.read(14)
if version != "Anoto STF v1.0":
    print "bad version %s" % version
    sys.exit(1)

print version

speed = br.get_bits(16)
print "Speed %d" % speed

header = br.get_bits(8)
if header != 0x18:
    print "bad header %x" % header

start_time = br.get_bits(64)
print "Start time: %d" % start_time

x0 = br.get_bits(16)
y0 = br.get_bits(16)
print "Start coord: %d, %d" % (x0, y0)

def decode(codebook, br, debug=False):
    code = ""
    while code not in codebook:
        bit = br.get_bits()
        if bit:
            code += "1"
        else:
            code += "0"
        if debug:
            print code
    return codebook[code]

def get_force(br):
    codebook = {
            '110010': 30,
    }
    f = decode(codebook, br)
    return f*2

def get_time(br):
    codebook = {
            '0': 1,
    }
    return decode(codebook, br)

def get_deltax(br):
    codebook = {
            '00110': 0,
            '10100': -1,
    }
    return decode(codebook, br, True)

def get_deltay(br):
    codebook = {
            '00000': 0,
            '10001': -1,
            '000011': 1,
    }
    return decode(codebook, br, True)

def get_deltaforce(br):
    codebook = {
            '111111111': -30,
            '0': 0,
    }
    return decode(codebook, br)

f0 = get_force(br)
print "Initial force: %d" % f0

while True:
    header = br.get_bits()
    assert header == 0

    time = get_time(br)
    print "Change in time: %d" % time

    deltax = get_deltax(br)
    print "Delta X: %d" % deltax

    deltay = get_deltay(br)
    print "Delta Y: %d" % deltay

    deltaf = get_deltaforce(br)
    print "Delta F: %d" % deltaf

    x0 += deltax * time
    y0 += deltay * time
    f0 += deltaf

    print "%d, %d, %d, %d" % (x0, y0, f0, start_time)
