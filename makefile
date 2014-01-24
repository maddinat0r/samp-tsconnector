GPP=g++ -m32
GCC=gcc -m32

OUTFILE="bin/TSConnector.so"

COMPILE_FLAGS = -c -O3 -fPIC -w -DLINUX -Wall -Isrc/
LIBRARIES = -pthread -lrt -Wl,-Bstatic -lboost_thread -lboost_chrono -lboost_date_time -lboost_system -lboost_atomic -Wl,-Bdynamic

all: compile clean
	
compile:
	@mkdir -p bin
	@echo Compiling plugin...
	@ $(GCC) $(COMPILE_FLAGS) src/SDK/amx/*.c
	@ $(GPP) $(COMPILE_FLAGS) src/SDK/*.cpp
	@ $(GPP) $(COMPILE_FLAGS) src/*.cpp
	@echo Linking...
	@ $(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o $(LIBRARIES)
	
clean:
	@echo Cleaning...
	@ rm -f *.o
	@echo Done.
