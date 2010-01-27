#!/usr/bin/python

class BitReader(object):
    def __init__(self, stream):
        self.stream = stream
        self.byte = 0
        self.nbits = 0

    def flush(self):
        self.nbits=0
        self.byte=0

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

def decode(codebook, br, debug=False):
    code = ""
    while code not in codebook:
	if len(code) > 256:
	    raise RuntimeError("code too long")
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
            '1100': 0,
    }
    return decode(codebook, br)

def get_deltax(br):
    codebook = {
            '00110': 0,
            '00111': 1,
            '10100': -1,
            '01000': 2,
            '10011': -2,
            '0000' : 3,
            '10010': -3,
            '0001' : 4,
            '10001': -4,
            '0010' : 5,
            '10000': -5,
            '01001': 6,
            '01111': -6,
            '01010': 7,
            '01110': -7,
            '01011': 8,
            '01101': -8,
            '01100': 9,
            '110010': -9,
            '101010': 10,
            '110001': -10,
            '101011': 11,
            '110000': -11,
            '101100': 12,
            '101111': -12,
            '101101': 13,
            '101110': -13,
	    '1110000': -14,
            '1100111' : 15,
            '1101111' : -15,
    }
    return decode(codebook, br)

def get_deltay(br):
    codebook = {
            '00000': 0,
            '00001': 1,
            '10001': -1,
            '00010': 2,
            '10000': -2,
            '00011': 3,
            '01111': -3,
            '00100': 4,
            '01110': -4,
            '00101': 5,
            '01101': -5,
            '00110': 6,
            '01100': -6,
            '00111': 7,
            '01011': -7,
            '01000': 8,
            '01010': -8,
            '100100': 9,
            '01001': -9,
            '100101': 10,
            '101101': -10,
            '100110': 11,
            '101100': -11,
            '100111': 12,
            '101011': -12,
            '101000': 13,
            '101010': -13,
            '1011100': 14,
            '101001': -14,
            '1101000': -15,
            '1011110': 16,
            '1011111': 17,
            '1100110': -17,
            '1100000': 18,
	    '11010010': 21,
            '11010110': 25,
    }
    return decode(codebook, br)

def get_deltaforce(br):
    codebook = {
            '0': 0,
	    '1010': -1,
	    '1100111': -4,
	    '11011110': -6,
	    '1111111010': -24,
	    '1111111000': -26,
	    '1111110101': -29,
            '11111111100': -30,
    }
    return 2*decode(codebook, br)


class STFParser(object):
    def __init__(self, stream):
        br = BitReader(stream)
        self.br = br
        magic = br.get_bits(16)
        if magic != 0x0100:
            raise RuntimeError("bad magic %x" % magic)

        version = stream.read(14)
        if version != "Anoto STF v1.0":
            raise RuntimeError("bad version %s" % version)

        speed = br.get_bits(16)
        self.speed = speed

    def parse(self):
        start_time = 0
        br = self.br

        while True:
            br.flush()
            header = br.get_bits(8)

            if header == 0x18:
                time = br.get_bits(64)
            elif header == 0x10:
                time = br.get_bits(32)
            elif header == 0x08:
                time = br.get_bits(16)
            elif header == 0x80:
                break
            else:
                raise RuntimeError("bad header %x" % header)

            start_time += time
            x0 = br.get_bits(16)
            y0 = br.get_bits(16)
            f0 = get_force(br)

            self.handle_point(x0, y0, f0, start_time)

            xa=0
            ya=0
            while True:
                header = br.get_bits()
                assert header == 0

                time = get_time(br)
                if time == 0:
                    break

                deltax = get_deltax(br)
                deltay = get_deltay(br)
                deltaf = get_deltaforce(br)

                xa = deltax + (xa * time) / 256
                x0 += xa
                xa *= 256

                ya = deltay + (ya * time) / 256
                y0 += ya
                ya *= 256
                f0 += deltaf 

                self.handle_point(x0, y0, f0, start_time)
                pass
        pass

    def handle_point(self, x, y, force, time):
        print "Override STFParser and provide your own!"
        assert False

if __name__ == "__main__":
    import sys

    class TestParser(STFParser):
        def handle_point(self, x, y, f, time):
            print "%d, %d, %d, %d" % (x, y, f, time)

    f = file(sys.argv[1])
    tp = TestParser(f)
    print "Speed is %d" % tp.speed
    tp.parse()
