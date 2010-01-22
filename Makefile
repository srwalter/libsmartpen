CFLAGS=-Wall `pkg-config --cflags openobex`

export PKG_CONFIG_PATH=$(HOME)/sandboxes/openobex/lib/pkgconfig

all: obex

obex: obex.o
	gcc -o $@ $^ `pkg-config --libs openobex`
