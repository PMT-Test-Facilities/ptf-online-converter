# Makefile

ifndef MIDASSYS
MIDASSYS=$(HOME)/packages/midas
endif

ifndef ROOTANA
ROOTANA=$(HOME)/packages/rootana
endif

CFLAGS   =  -DOS_LINUX -g -O2 -Wall -Wuninitialized -I. -I$(MIDASSYS)/include -I$(MIDASSYS)/drivers/vme
CXXFLAGS = $(CFLAGS)

#LIBS = -lm -lz -lutil -lnsl -lpthread
LIBS=

# MIDAS library
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a

DRV_OBJS = v513.o v792.o v1729.o sis3320.o vf48.o VMENIMIO32.o

all:: gefvme.o
all:: vmescan_gef.exe
all:: srunner_vme_gef.exe
all:: test_vme_gef.exe
all:: test_vf48_gef.exe
#all:: test_vf48_root_gef.exe
all:: test_VMENIMIO32_gef.exe

all:: $(VMEDRV_UNIVERSE)
all:: vmescan.exe
all:: srunner_vme.exe
all:: test_vme.exe
all:: test_vf48.exe
all:: test_VMENIMIO32.exe

test_vf48.exe test_vf48_gef.exe: vf48.o UnpackVF48.o
test_VMENIMIO32.exe test_VMENIMIO32_gef.exe: VMENIMIO32.o

%_gef.exe: %.o gefvme.o
	$(CXX) -o $@ -O2 -g -Wall $^ $(LIBS)

%.exe: %.o vmicvme.o
	$(CXX) -o $@ -O2 -g -Wall $^ $(LIBS) -lvme

%.o: %.c
	$(CC) $(OSFLAGS) $(CFLAGS) -c $<

%.o: %.cxx
	$(CXX) $(OSFLAGS) $(CXXFLAGS) -c $<

vmicvme.o gefvme.o: %.o: $(MIDASSYS)/drivers/vme/vmic/%.c
	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include

%.o: $(MIDASSYS)/drivers/vme/%.c
	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include


#OSFLAGS  =  -m32
#CFLAGS   =  -DOS_LINUX -g -O2 -Wall -Wuninitialized -I. -I$(MIDASSYS)/include -I$(MIDASSYS)/drivers/vme -I$(MIDASSYS)/drivers/vme/vmic -I$(VMICHOME)/include -I$(HOME)/packages/rootana

#all:: gef vmicvme.o $(MODDRV) vmescan.exe test_v792.exe test_v513.exe test_v1729.exe test_sis3320.exe test_vf48.exe test_vf48_ana.exe
#all:: test_mdi2.exe
#gef: gefvme.o vmescan_gef.exe test_vf48_gef.exe srunner_vme_gef.exe

ifdef ROOTSYS
ROOTLIBS   = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --cflags)
RPATH     += -Wl,-rpath,$(ROOTSYS)/lib
ROOTCXXFLAGS  += -DHAVE_ROOT $(ROOTCFLAGS)
ROOTLIBS      += $(ROOTGLIBS) $(RPATH)

test_vf48a.o: test_vf48.cxx
	$(CXX) $(OSFLAGS) $(CXXFLAGS) -DHAVE_ROOT $(ROOTCFLAGS) -c -o $@ $<

test_vf48a_gef.exe: test_vf48a.o gefvme.o vf48.o UnpackVF48.o
	$(CXX) -o $@ -O2 -g -Wall $^ $(ROOTGLIBS) $(LIBS)

all:: test_vf48a_gef.exe

endif

test_vf48m.o: test_vf48.cxx
	$(CXX) $(OSFLAGS) $(CXXFLAGS) -DHAVE_MULTI -c -o $@ $<

vf48_multi.o: %.o: %.c
	$(CC) $(OSFLAGS) $(CXXFLAGS) -I$(MIDASSYS)/drivers/vme/vmic -c -o $@ $<

test_vf48m_gef.exe: test_vf48m.o gefvme.o vf48.o vf48_multi.o UnpackVF48.o
	$(CXX) -o $@ -O2 -g -Wall $^ $(MIDASLIBS) $(LIBS) -lrt -lutil

all:: test_vf48a_gef.exe

#%.exe: %.o $(MODDRV) $(VMEDRV)
#	g++ -o $@ -O2 -g -Wall $^ $(VMELIBS) $(LIBS)

#test_v792.exe test_v1729.exe: %.exe: %.o $(MODDRV) $(VMEDRV)
#	g++ -o $@ -O2 -g -Wall $^ $(VMELIBS) $(ROOTLIBS) $(LIBS)
#
#test_vf48.exe test_vf48_gef.exe test_vf48_analog.exe test_vf48_analog_gef.exe: UnpackVF48.o
#
#test_vf48_gef.exe: %.exe: test_vf48.o UnpackVF48.o vf48.o gefvme.o
#	g++ -o $@ -O2 -g -Wall $^ $(LIBS) $(ROOTGLIBS)
#
#test_vf48_ana.exe: %.exe: %.o UnpackVF48.o
#	g++ -o $@ -O2 -g -Wall $^ $(HOME)/packages/rootana/librootana.a -lXMLParser $(LIBS)
#
#test_VMENIMIO32.exe: %.exe: %.o VMENIMIO32.o $(VMEDRV)
#	g++ -o $@ -O2 -g -Wall $^ $(VMELIBS) $(LIBS) $(ROOTLIBS)
#
#test_mdi2.o: mdi2.cxx
#
#test_mdi2.exe: %.exe: %.o $(VMEDRV)
#	g++ -o $@ -O2 -g -Wall $^ $(VMELIBS) $(LIBS) $(ROOTLIBS)
#
#test_VMENIMIO32_gef.exe: %_gef.exe: %.o VMENIMIO32.o gefvme.o
#	g++ -o $@ -O2 -g -Wall $^ $(LIBS) $(ROOTLIBS)
#
#test_sis3820.exe: %.exe: %.o sis3820.o $(VMEDRV)
#	g++ -o $@ -O2 -g -Wall $^ $(VMELIBS) $(LIBS) $(ROOTLIBS)
#
#test_sis3820_gef.exe: %_gef.exe: %.o sis3820.o gefvme.o
#	g++ -o $@ -O2 -g -Wall $^ $(LIBS) $(ROOTLIBS)
#
#test_UnpackVF48.exe: %.exe: %.o UnpackVF48.o
#	g++ -o $@ -O2 -g -Wall $^  $(HOME)/packages/rootana/librootana.a -lz $(LIBS)
#
#vmicvme.o: $(MIDASSYS)/drivers/vme/vmic/vmicvme.c
#	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include
#
#vf48.o: vf48-lxe.c
#	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include
#
#vf48.o: %.o: %.c
#	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include
#
#
#%.o: $(MIDASSYS)/drivers/vme/vmic/%.c
#	gcc $(OSFLAGS) -c -o $@ -O2 -g -Wall -Wuninitialized $< -I$(MIDASSYS)/include
#
#%.o: %.cxx
#	$(CXX) $(OSFLAGS) $(CXXFLAGS) $(ROOTCXXFLAGS) -c $<

clean::
	-rm -f *.o

clean::
	-rm -f *.exe

# end
