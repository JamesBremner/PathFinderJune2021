#source file search paths
VPATH = ../../src ../../../raven-set

# compiler include search paths
INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set

DEPS = cPathFinder.h cGraph.h

WEXINC = \
	-I../../../windex/include

WEXLIBS = \
	-lstdc++fs -lgdiplus -lgdi32 -lcomdlg32

# folder for .o files
ODIR=./obj

# sources
_GUI = pathgui.o cPathFinder.o cPathFinderReader.o cGraph.o cRunWatch.o
GUI = $(patsubst %,$(ODIR)/%,$(_GUI))

_TST = pathtest.o cPathFinder.o cPathFinderReader.o cGraph.o cRunWatch.o
TST = $(patsubst %,$(ODIR)/%,$(_TST))

OPT = -O2

$(ODIR)/%.o: %.cpp $(DEPS)
	g++   -std=c++17 $(OPT) -m64 -c -o $@ $< $(INCS) $(WEXINC)

gui: $(GUI)
	g++ -o ../../bin/pathfinder.exe -O2 $^ $(WEXLIBS)

test: $(TST)
	g++ -Wl,--stack,33554432 $(OPT) -s \
	-static-libstdc++ -static-libgcc -static -m64 $(OPT) \
	-o ../../bin/pathtest.exe $^

.PHONY: clean

clean:
	del /q obj