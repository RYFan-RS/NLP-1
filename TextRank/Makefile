ROOT_DIR = ../..
include $(ROOT_DIR)/MakeVars

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDE = -I$(ROOT_DIR)/include
C_FLAGS = -fPIC

all: main

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(OBJECTS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main
