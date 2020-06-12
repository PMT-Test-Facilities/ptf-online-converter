# Makefile

BINDIR= ../bin
OBJS= controller.o dataxfer.o telnet.o devcfg.o readconfig.o selector.o ser2net.o utils.o usbFindSerial.o

all: ser2net ser2net.man

install: ser2net.install

clean::
	rm -f *.o
	rm -f ser2net ser2net.man

ser2net: $(OBJS)
	$(CC) -m32 -o $@ $^ -lwrap -lnsl

%.man: %.8
	nroff -man $< > $@

%.o: %.c
	$(CC) -c $< -m32 -O2 -g -Wall -DPACKAGE=\"ser2net\" -DVERSION=\"xxx\" -DHAVE_DLFCN_H=1 -DSTDC_HEADERS=1 -DHAVE_TCPD_H=1 -DHAVE_LIBNSL=1 -DHAVE_LIBWRAP=1  -I.

%.install: %
	-mv $(BINDIR)/$< $(BINDIR)/$<.previous
	cp -f $< $(BINDIR)

controller.o: controller.c controller.h selector.h dataxfer.h
dataxfer.o:   dataxfer.c dataxfer.h controller.h selector.h devcfg.h utils.h
devcfg.o:     devcfg.c devcfg.h controller.h
readconfig.o: readconfig.c dataxfer.h controller.h readconfig.h
selector.o:   selector.c selector.h
ser2net.o:    ser2net.c readconfig.h controller.h utils.h selector.h dataxfer.h
utils.o:      utils.c utils.h

#end file
