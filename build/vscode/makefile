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
_GUI = pathgui.o cPathFinder.o cPathFinderReader.o cGraph.o
GUI = $(patsubst %,$(ODIR)/%,$(_GUI))

_TST = pathtest.o cPathFinder.o cPathFinderReader.o cGraph.o cRunWatch.o
TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/%.o: %.cpp $(DEPS)
	g++   -std=c++17 -g -m64 -c -o $@ $< $(INCS) $(WEXINC)

gui: $(GUI)
	g++ -o ../../bin/gui.exe $^ $(WEXLIBS)

test: $(TST)
	g++ -Wl,--stack,33554432 \
	-static-libstdc++ -static-libgcc -static -m64 -g \
	-o ../../bin/pathtest.exe $^

.PHONY: clean

clean:
	del /q obj