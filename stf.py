#!/usr/bin/python

import cairo
import sys
import parsestf

surface = cairo.ImageSurface(cairo.FORMAT_RGB24, 6000, 6000)
ctx = cairo.Context(surface)

ctx.set_source_rgb(255, 255, 255)
ctx.paint()
ctx.set_source_rgb(0,0,0)

class Parser(parsestf.STFParser):
    def __init__(self, *args):
        super(Parser, self).__init__(*args)
        self.last_force=0

    def handle_point(self, x, y, f, time):
        if self.last_force:
            ctx.line_to(x, y)
        else:
            ctx.stroke()
            ctx.move_to(x, y)
        self.last_force = f

f = file(sys.argv[1])
p = Parser(f)
p.parse()

ctx.stroke()
surface.write_to_png(sys.argv[2])
