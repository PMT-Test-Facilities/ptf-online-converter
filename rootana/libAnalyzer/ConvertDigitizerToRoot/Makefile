# Makefile

# TF: Needed -fPIC for making a .so file later
CXXFLAGS = -g -O2 -fPIC -Wall -Wuninitialized -I$(ROOTANASYS)/include

# optional ZLIB library

CXXFLAGS += -DHAVE_ZLIB

# ROOT libraries

ifdef ROOTSYS
#ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
ROOTANASLIBS = -L$(ROOTANASYS)/lib/ 
#RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags)
endif

# optional MIDAS libraries

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt -L$(MIDASSYS)/linux/lib        # Don't need this for making the .o files, only for the linking.
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
CXXFLAGS += -DOS_LINUX -DOS_DARWIN
MIDASLIBS = $(MIDASSYS)/darwin/lib/
RPATH=
endif

endif

ALL:= 
OBJS:=
LIBS:=
ifdef ROOTSYS
All  += PTFDict.cc
ALL  += ptfdataskim1
OBJS += SkimSingle.o
OBJS += PTFDict.o
endif

ifdef ROOTSYS
LIBS     += -lrootana
LIBS     += -lmidas
endif


all: $(ALL)

ptfdataskim1: ptfdataskim.o $(OBJS)  
	$(CXX) -o $@ $(CXXFLAGS) $^ $(MIDASLIBS) $(ROOTANASLIBS)  $(ROOTGLIBS) -lm -lz -lpthread $(RPATH) $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(ROOTANASLIBS) $(LIBS) $(ROOTGLIBS) -c $<

PTFDict.cc:
	$(ROOTSYS)/bin/rootcint -f PTFDict.cc -c $(INC) Pulse.hh ptfEvent.hh LinkDef.h

dox:
	doxygen

clean::
	-rm -f *.o *.a *.exe $(ALL) PTFDict.cc PTFDict.h

# end


#ToDo: add this properly in the MakeFile if wanted:
# Need to explicity do "make PTFDict.cc" and "make PTFDict.o". Should be taken care of by the MakeFile!


#g++ -fPic -shared -O PTFDict.o -o libPTF.so -L/home1/midptf/packages/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic
