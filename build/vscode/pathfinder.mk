
# compiler include search paths
INCS = \
	-I../../../boost/boost1_78 \
	-I../../../raven-set \
	-I../../../windex/include

WEXLIBS = \
	-lstdc++fs -lgdiplus -lgdi32 -lcomdlg32

SDIR = ../../src
ODIR = ./obj

OPT = -O2
CCflags = -std=c++17 $(OPT)
STATLibs = -static-libstdc++ -static-libgcc -static

### OBJECT FILES

graphtestOBJs = $(ODIR)/graphtest.o $(ODIR)/cGraph.o $(ODIR)/cutest.o
pathtestOBJs =  $(ODIR)/pathtest.o \
	 $(ODIR)/cPathFinder.o $(ODIR)/cPathFinderReader.o $(ODIR)/cGraph.o \
	 $(ODIR)/cRunWatch.o
guiOBJs = $(ODIR)/pathgui.o \
	 $(ODIR)/cPathFinder.o $(ODIR)/cPathFinderReader.o $(ODIR)/cGraph.o \
	 $(ODIR)/cMaze.o $(ODIR)/cRunWatch.o

### COMPILE

$(ODIR)/cutest.o :
	g++ -o  $@ -c ../../../raven-set/cutest.cpp \
		$(CCflags)

$(ODIR)/cRunWatch.o :
	g++ -o $@ -c ../../../raven-set/cRunWatch.cpp \
	$(INCS) $(CCflags) 

$(ODIR)/%.o : $(SDIR)/%.cpp $(SDIR)/cGraph.h
	g++ -o $@ -c $< $(CCflags) $(INCS)

#### LINKS

graphtest : $(graphtestOBJs) 
	g++ -o ../../bin/graphtest.exe \
		$(graphtestOBJs)  $(STATLibs) -m64 $(OPT)

gui : $(guiOBJs)
	g++ -o ../../bin/pathfinder.exe \
		$(guiOBJs) $(WEXLIBS) $(STATLibs) $(OPT) -m64

pathtest : $(pathtestOBJs)
	g++ -o ../../bin/pathtest.exe \
	$(pathtestOBJs) $(STATLibs) \
	-Wl,--stack,33554432 $(OPT) -s -m64

### CLEAN

.PHONY: clean
clean:
	del /q obj