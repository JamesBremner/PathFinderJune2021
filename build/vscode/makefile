#source file search paths
VPATH = ../../src 

# compiler include search paths
INCS = \
	-I../../../boost/boost1_72

#DEPS = cPathFinder.h

WEXINC = \
	-I../../../windex/include

WEXLIBS = \
	-lstdc++fs -lgdiplus -lgdi32 -lcomdlg32

# folder for .o files
ODIR=./obj

# sources
_GUI = pathgui.o cPathFinder.o cPathFinderReader.o
GUI = $(patsubst %,$(ODIR)/%,$(_GUI))

_TST = pathtest.o cPathFinder.o
TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/%.o: %.cpp $(DEPS)
	g++   -std=c++17 -g -c -o $@ $< $(INCS) $(WEXINC)

gui: $(GUI)
	g++ -o ../../bin/gui.exe $^ $(WEXLIBS)

test: $(TST)
	g++ -o ../../bin/pathtest.exe $^

.PHONY: clean

clean:
	del /q obj