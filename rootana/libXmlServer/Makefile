#
# Makefile for the libXmlServer package
#

CXXFLAGS = -g -O2 -Wall -Wuninitialized

# ROOT libraries

ifdef ROOTSYS
ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

ALL:= xmlServer.o
#ALL+= libNetDirectory.a

OBJS = TNetDirectory.o TNetDirectoryDict.o netDirectoryServer.o RootLock.o

all: $(ALL)

libNetDirectory.a: $(OBJS)
	-rm -f $@
	ar -rv $@ $^

%Dict.cxx: %.h
	LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTSYS)/bin/rootcint -f $@ -c $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $<

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe $(ALL)

# end
