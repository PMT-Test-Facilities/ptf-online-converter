# Makefile

CXXFLAGS = -g -O2 -Wall -Wuninitialized -I../libMidasInterface/ -march=native -mtune=native

# optional ZLIB library

CXXFLAGS += -DHAVE_ZLIB

# ROOT libraries

ifdef ROOTSYS
#ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
#RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

# optional MIDAS libraries

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
MIDASLIBS = $(MIDASSYS)/darwin/lib/libmidas.a
RPATH=
endif

endif




ALL:= 
OBJS:=
LIBS:=
ifdef ROOTSYS
ALL  += libAnalyzer.a analyzer_example.exe convert_to_root convert_to_root_mpmt
OBJS += TPeriodicClass.o
OBJS += TRootanaEventLoop.o
OBJS += TDataContainer.o
OBJS += TV1190Data.o
OBJS += TV1720RawData.o
OBJS += TV1730DppData.o
OBJS += TV1730RawData.o
OBJS += TV792Data.o
OBJS += TL2249Data.o
OBJS += TMesytecData.o
endif

ifdef ROOTSYS
CXXFLAGS += -DHAVE_LIBNETDIRECTORY -I../libNetDirectory -I../
LIBS     += ../libNetDirectory/libNetDirectory.a
endif


all: $(ALL)

libAnalyzer.a: $(OBJS)
	-rm -f $@
	ar -rv $@ $^

analyzer_example.exe: analyzer_example.cxx libAnalyzer.a ../libMidasInterface/libMidasInterface.a
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(ROOTGLIBS) -lm -lz -lpthread $(RPATH) $(LIBS)

convert_to_root: analyzer_convert_ptf_scan_to_rootTree.cxx libAnalyzer.a ../libMidasInterface/libMidasInterface.a
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(ROOTGLIBS) -lm -lz -lpthread $(RPATH) $(LIBS)

convert_to_root_mpmt: analyzer_convert_ptf_scan_to_rootTree_mpmt.cxx libAnalyzer.a ../libMidasInterface/libMidasInterface.a
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(ROOTGLIBS) -lm -lz -lpthread $(RPATH) $(LIBS)



#%Dict.cxx: %.h
#	LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTSYS)/bin/rootcint -f $@ -c $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe $(ALL)

# end
