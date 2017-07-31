###User Change Section################
ROOT_DIR = ../../..
include $(ROOT_DIR)/MakeVars
INCLUDE = -I$(ROOT_DIR)/include
LIBRARY = -lxml2
DEST_DIR = $(ROOT_DIR)/bin
ifeq (${GOPTION}, -g)
#	OPTION_DEFINATIONS = -D_DYNAMIC_PARSER_DEBUG
endif
ifeq (${OOPTION}, -O3)
	OPTION_DEFINATIONS = -DUSE_ENCODE_DECODE
endif
C_FLAGS = ${GOPTION} ${OOPTION} ${OPTION_DEFINATIONS} -DTIXML_USE_STL -DOS_LINUX -fPIC
###################################

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)
ENGINE_OBJS =Dictionary.o Parser.o Range.o Result.o RuleOfParse.o Node.o

DYPSR_LIB = $(DEST_DIR)/libDynamicParser.a

all: main $(DYPSR_LIB)

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(OBJECTS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS)

$(DYPSR_LIB) : $(ENGINE_OBJS)
	@echo
	@echo creating Dynamic Parser library ...
	@echo
	$(AR) rcs $(DYPSR_LIB) $^

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main $(DYPSR_LIB)
