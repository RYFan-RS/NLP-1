MAKE = make
C = gcc -g 
CC = g++ -g

ROOT_DIR = .

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDE = -I./cppjieba/include -I./cppjieba/deps -I../../include
C_FLAGS = -fPIC
L_FLAGS = 
LIBS = 

all: main

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(OBJECTS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS) $(L_FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main
