ROOT_DIR = ../../..
include $(ROOT_DIR)/MakeVars

L_FLAGS = -L/usr/local/lib -lxml2 -lcurl -lpcre -lpthread

ifeq (${GOPTION}, -g)
	DEBUG_OPTION = -D_CHAT_ENGINE_DEBUG
	ENCODE_OPTION = -DUSE_ENCODE_DECODE
else
	LOG_OPTION = -D_CHAT_LOG
endif
ifeq (${OOPTION}, -O3)
	ENCODE_OPTION = -DUSE_ENCODE_DECODE
endif
C_FLAGS = ${GOPTION} ${ENCODE_OPTION} -fPIC

SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

INCLUDE = -I$(ROOT_DIR)/include/Common -Iinclude

RObOSAY_LIBS = $(ROOT_DIR)/bin/libDynamicParser.a 
HTTP_LIBS = $(ROOT_DIR)/bin/libsimpleserver.a $(ROOT_DIR)/bin/libhttp_parser.a $(ROOT_DIR)/bin/libjson_libmt.a
LIBS = $(RObOSAY_LIBS) $(HTTP_LIBS)
HTTP_MODULE = ehttp

all: main

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

$(HTTP_LIBS):
	@echo "-> Compiling HTTP_MODULE ..."
	$(MAKE) -C $(HTTP_MODULE)
	cp $(HTTP_MODULE)/output/lib/libsimpleserver.a $(ROOT_DIR)/bin/.
	cp $(HTTP_MODULE)/deps/http-parser/libhttp_parser.a $(ROOT_DIR)/bin/.
	cp $(HTTP_MODULE)/deps/json-cpp/output/lib/libjson_libmt.a $(ROOT_DIR)/bin/.
		
compile:
	tools/compiler -c data/semantic.txt -o data/semantic.dat
	
main : $(OBJECTS) $(LIBS)
	$(CC) $(C_FLAGS) -o $@ $(OBJECTS) $(LIBS) $(L_FLAGS)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK $(ROOT_DIR)/bin/libsimpleserver.a $(ROOT_DIR)/bin/libhttp_parser.a $(ROOT_DIR)/bin/libjson_libmt.a main
	$(MAKE) -C $(HTTP_MODULE) clean
