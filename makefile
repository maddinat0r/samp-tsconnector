GPP=g++ -m32
GCC=gcc -m32

OUTFILE="bin/TSConnector.so"

COMPILE_FLAGS = -c -std=c++11 -O3 -fPIC -w -DLINUX -Wall -I lib/
LIBRARIES = -pthread -lrt -Wl,-Bstatic -lboost_regex -lboost_thread -lboost_chrono -lboost_date_time -lboost_system -lboost_atomic -Wl,-Bdynamic

all: compile clean
	
compile:
	@mkdir -p bin
	@echo Compiling plugin...
	@ $(GPP) $(COMPILE_FLAGS) src/*.cpp
	@ $(GPP) $(COMPILE_FLAGS) src/*.cc
	@echo Compiling SDK...
	@ $(GCC) $(COMPILE_FLAGS) lib/sdk/amx/*.c
	@ $(GPP) $(COMPILE_FLAGS) lib/sdk/*.cpp
	@echo Linking...
	@ $(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o $(LIBRARIES)
	
clean:
	@echo Cleaning...
	@ rm -f *.o
	@echo Done.
