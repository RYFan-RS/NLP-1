###User Change Section################
ROOT_DIR = ../../..
include $(ROOT_DIR)/MakeVars
INCLUDE = -I/usr/include/libxml2 -I$(ROOT_DIR)/include -I$(ROOT_DIR)/src -I../Common/
LIBRARY = -lxml2 $(ROOT_DIR)/bin/libSemanticEngine.a $(ROOT_DIR)/bin/libDynamicParser.a
C_FLAGS = ${GOPTION} -fPIC
###################################

SOURCES = $(wildcard *.cpp ../*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: main

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(OBJECTS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS) $(LIBRARY)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main ../*.o ../*.d
