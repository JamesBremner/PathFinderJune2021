
# compiler include search paths
INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set

WEXLIBS = \
	-lstdc++fs -lgdiplus -lgdi32 -lcomdlg32

SDIR = ../../src
ODIR = ./obj

OPT = -g
CCflags = -std=c++17 $(OPT)
STATLibs = -static-libstdc++ -static-libgcc -static

### OBJECT FILES

graphtestOBJs = $(ODIR)/graphtest.o $(ODIR)/cgraph.o $(ODIR)/cutest.o
pathtestOBJs =  $(ODIR)/pathtest.o \
	 $(ODIR)/cpathfinder.o $(ODIR)/cpathfinderreader.o $(ODIR)/cgraph.o \
	 $(ODIR)/cRunWatch.o
guiOBJs = $(ODIR)/pathgui.o \
	 $(ODIR)/cpathfinder.o $(ODIR)/cpathfinderreader.o $(ODIR)/cgraph.o \
	 $(ODIR)/cRunWatch.o

### COMPILE

$(ODIR)/cutest.o :
	g++ -o  $@ -c ../../../raven-set/cutest.cpp \
		$(CCflags)

$(ODIR)/cRunWatch.o :
	g++ -o $@ -c ../../../raven-set/cRunWatch.cpp \
	$(INCS) $(CCflags) 

$(ODIR)/graphtest.o : $(SDIR)/cgraph.h
	g++ -o $@ -c $(SDIR)/graphtest.cpp \
		$(INCS) $(CCflags)

$(ODIR)/pathtest.o : $(SDIR)/cgraph.h
	g++ -o $@ -c $(SDIR)/pathtest.cpp \
		$(INCS) $(CCflags)

$(ODIR)/cpathfinderreader.o : $(SDIR)/cgraph.h
	g++ -o $@ -c $(SDIR)/cpathfinderreader.cpp \
		$(INCS) $(CCflags)

$(ODIR)/pathgui.o :
	g++ -o $@ \
		-c $(SDIR)/pathgui.cpp \
		-I../../../windex/include \
		$(INCS) $(CCflags)

$(ODIR)/cpathfinder.o : $(SDIR)/cgraph.h
	g++ -o $@ -c $(SDIR)/cpathfinder.cpp $(INCS) $(CCflags)

$(ODIR)/cgraph.o : $(SDIR)/cgraph.h
	g++ -o $@ -c $(SDIR)/cgraph.cpp $(CCflags)

#### LINKS

graphtest : $(graphtestOBJs) 
	g++ -o ../../bin/graphtest.exe \
		$(graphtestOBJs)  $(STATLibs) -m64 $(OPT)

gui : $(guiOBJs)
	g++ -o ../../bin/pathfinder.exe \
		$(guiOBJs) $(WEXLIBS) $(STATLibs) $(OPT) -s -m64

pathtest : $(pathtestOBJs)
	g++ -o ../../bin/pathtest.exe \
	$(pathtestOBJs) $(STATLibs) \
	-Wl,--stack,33554432 $(OPT) -s -m64

### CLEAN

.PHONY: clean
clean:
	del /q obj