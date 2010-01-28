#!/usr/bin/python

import cairo
import sys
import parsestf

surface = cairo.ImageSurface(cairo.FORMAT_RGB24, 6000, 6000)
ctx = cairo.Context(surface)

ctx.set_source_rgb(255, 255, 255)
ctx.paint()
ctx.set_source_rgb(0,0,0)

def draw_point(x, y):
    print "%d, %d" % (x, y)
    ctx.move_to(x, y)
    ctx.line_to(x+10, y)
    ctx.line_to(x+10, y+10)
    ctx.line_to(x, y+10)
    ctx.close_path()
    ctx.fill()

class Parser(parsestf.STFParser):
    def handle_point(self, x, y, f, time):
        print time
        draw_point(x, y)

f = file(sys.argv[1])
p = Parser(f)
p.parse()

surface.write_to_png(sys.argv[2])
