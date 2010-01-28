CFLAGS=-Wall -fPIC -I/usr/include/python2.5 `pkg-config --cflags glib-2.0 openobex` -g

export PKG_CONFIG_PATH=$(HOME)/sandboxes/openobex/lib/pkgconfig

all: obex pysmartpen.so

pysmartpen.so: smartpen.o pysmartpen.o
	gcc -o $@ -shared $^ `pkg-config --libs glib-2.0 openobex`

pysmartpen.c: pysmartpen.pyx
	pyrexc $^

obex: test.o smartpen.o
	gcc -o $@ $^ `pkg-config --libs glib-2.0 openobex`
