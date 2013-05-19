GPP=g++
GCC=gcc

OUTFILE="bin/TSConnector.so"

COMPILE_FLAGS=-c -m32 -O3 -fPIC -w -DLINUX -Wall -Isrc/

all:
	$(GCC) $(COMPILE_FLAGS) src/SDK/amx/*.c
	$(GPP) $(COMPILE_FLAGS) src/SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) src/*.cpp
	$(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o lib/libboost_regex.a
	rm -f *.o