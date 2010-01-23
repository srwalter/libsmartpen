CFLAGS=-Wall `pkg-config --cflags glib-2.0 openobex` -g

export PKG_CONFIG_PATH=$(HOME)/sandboxes/openobex/lib/pkgconfig

all: obex

obex: test.o smartpen.o
	gcc -o $@ $^ `pkg-config --libs glib-2.0 openobex`
