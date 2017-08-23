MAKE = make
C = gcc -g 
CC = g++ -g

ROOT_DIR = .

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDE = -I$(ROOT_DIR)/../include
C_FLAGS = -fPIC
L_FLAGS = 
LIBS = ../lib/libDynamicParser.a

all: main

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(OBJECTS) $(LIBS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS) $(L_FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main
