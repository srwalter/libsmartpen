# vim: set noet sw=8 :
LOCAL_CFLAGS=-Wall -fPIC -I/usr/include/python2.5 `pkg-config --cflags glib-2.0 openobex libusb-1.0` -g

export PKG_CONFIG_PATH=$(HOME)/sandboxes/openobex/lib/pkgconfig

all: libsmartpen.so

%.o: %.c
	gcc -o $@ -c $^ $(CFLAGS) $(LOCAL_CFLAGS)

libsmartpen.so: smartpen.o
	gcc -o $@ -shared $^ `pkg-config --libs glib-2.0 openobex libusb-1.0`

pysmartpen.so: smartpen.o pysmartpen.o
	gcc -o $@ -shared $^ `pkg-config --libs glib-2.0 openobex libusb-1.0`

pysmartpen.c: pysmartpen.pyx
	pyrexc $^

obex: test.o smartpen.o
	gcc -o $@ $^ `pkg-config --libs glib-2.0 openobex libusb-1.0` -L. -lsmartpen

.PHONY: install clean

clean:
	rm -f libsmartpen.so *.o pysmartpen.c

install: libsmartpen.so
	install -m755 -D libsmartpen.so $(DESTDIR)/usr/lib/libsmartpen.so
	install -m644 -D 75-smartpen.rules $(DESTDIR)/lib/udev/rules.d/75-smartpen.rules
