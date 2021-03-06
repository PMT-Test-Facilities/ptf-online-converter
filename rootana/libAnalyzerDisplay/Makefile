# Makefile

CXXFLAGS = -g -O2 -Wall -Wuninitialized -I../libMidasInterface/ -I../libAnalyzer/

# optional ZLIB library

CXXFLAGS += -DHAVE_ZLIB

# ROOT libraries

ifdef ROOTSYS
ROOTLIBDIR := $(shell $(ROOTSYS)/bin/root-config --libdir)
#ROOTLIBS  = -L$(ROOTLIBDIR) $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -L$(ROOTLIBDIR) $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
#RPATH    += -Wl,-rpath,$(ROOTLIBDIR)
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
ifdef ROOTSYS
ALL  += libAnalyzerDisplay.a display_example.exe
OBJS += TComplicatedExampleCanvas.o TMainDisplayWindow.o TRootanaDisplay.o TSimpleExampleCanvas.o
OBJS += TMainDisplayWindowDict.o TRootanaDisplayDict.o
OBJS += TFancyHistogramCanvas.o TFancyHistogramCanvasDict.o
endif

ifdef ROOTSYS
CXXFLAGS += -DHAVE_LIBNETDIRECTORY -I../libNetDirectory -I../
LIBS     += ../libNetDirectory/libNetDirectory.a
endif

all: $(ALL)

libAnalyzerDisplay.a: $(OBJS)
	-rm -f $@
	ar -rv $@ $^

display_example.exe: display_example.cxx libAnalyzerDisplay.a ../libAnalyzer/libAnalyzer.a ../libMidasInterface/libMidasInterface.a
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(ROOTGLIBS) -lm -lz -lpthread $(RPATH) $(LIBS)


%Dict.o: %Dict.cxx
	$(CXX) $(CXXFLAGS) -c $<

%Dict.cxx: %.hxx %_LinkDef.h
	LD_LIBRARY_PATH=$(ROOTLIBDIR) $(ROOTSYS)/bin/rootcint -f $@ -c -p $(CXXFLAGS) $^


%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe *Dict.h *Dict.cxx $(ALL)

# end
