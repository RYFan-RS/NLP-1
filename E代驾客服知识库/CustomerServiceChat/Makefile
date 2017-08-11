###User Change Section################
ROOT_DIR = ../..
include $(ROOT_DIR)/MakeVars
INCLUDE =  -I/usr/include/mysql -I/usr/include/mysql++ -I/usr/include/libxml2 -I$(ROOT_DIR)/include -I$(ROOT_DIR)/src -ICommon/ -I/usr/local/include/thrift/
LIBRARY = $(ROOT_DIR)/bin/libMixParser.a $(ROOT_DIR)/bin/libSemanticEngine.a $(ROOT_DIR)/bin/libDynamicParser.a $(ROOT_DIR)/bin/libPcre++.a $(ROOT_DIR)/bin/libWordNetwork.a $(ROOT_DIR)/bin/libStaticParser.a $(ROOT_DIR)/bin/libExAPI.a $(ROOT_DIR)/bin/libBasicSearch.a /usr/local/lib/libthrift.a
LFLAG = -L/usr/local/lib -lxml2 -lcurl -lpcre -lpthread -lopencc -lmysqlpp

ifeq (${GOPTION}, -g)
	DEBUG_OPTION = -D_CHAT_ENGINE_DEBUG
	ENCODE_OPTION = -DUSE_ENCODE_DECODE
else
	LOG_OPTION = -D_CHAT_LOG
endif
ifeq (${OOPTION}, -O3)
	ENCODE_OPTION = -DUSE_ENCODE_DECODE
endif
C_FLAGS = ${GOPTION} ${ENCODE_OPTION} -DFEATURE_TF_ONE_TIME -fPIC

SOURCE = $(wildcard *.cpp FaqChat/*.cpp *.hpp)
OBJECT = $(SOURCES:.cpp=.o)

ENGINE_OBJ = FaqChat/KnowledgeBase.o FaqChat/Path.o Answer.o ChatEngine.o Input.o KnowledgeBase.o Path.o QA.o Result.o Target.o Entry.o VKnowledgeBase.o GeneralChat/KnowledgeBase.o GeneralChat/Path.o GeneralChat/QA.o GeneralChat/Sentence.o GeneralChat/WordSubdivisionDic.o GeneralChat/TfIdfRemend.o GeneralChat/WordCombination.o ChatResult.o KeyWordScoreRecord.o DicAnalysis.o ErrorCorrectionDic.o ErrorCorrectionHandle.o Filter.o GeneralChat/WordNearMeaning.o word2vec/distance.o
SERVER_OBJ = SemanticSearch_constants.o SemanticSearchService.o SemanticSearch_types.o

all: main server client

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

server: $(ENGINE_OBJ) $(SERVER_OBJ) $(LIBRARY) Server.o
	$(CC) $(C_FLAGS) -o $@ Server.o $(ENGINE_OBJ) $(SERVER_OBJ) $(LIBRARY) $(LFLAG)

main: $(ENGINE_OBJ) $(LIBRARY) main.o
	$(CC) $(C_FLAGS) -o $@ main.o $(ENGINE_OBJ) $(LIBRARY) $(LFLAG)

client: $(SERVER_OBJ) Client.o
	$(CC) $(C_FLAGS) -o $@ Client.o $(SERVER_OBJ) $(LIBRARY) $(LFLAG)

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main server client $(ENGINE_OBJ) $(SERVER_OBJ) main.o server.o client.o CustomerServiceChat.tar.gz 

#Sub directories
BIN_DIR = $(ROOT_DIR)/bin
TOOL_DIR = $(ROOT_DIR)/tool
RELEASE_DIR=$(ROOT_DIR)/release

top-release:release
	mv CustomerServiceChat.tar.gz $(RELEASE_DIR)

release: server
	-ln -s $(ROOT_DIR)/data .
	@echo "#####################################################"
	@echo "# Copy CustomerServiceChat 		 ...                       #"
	@echo "#####################################################"
	-rm ChatControler_release_cache -r
	mkdir ChatControler_release_cache
	mkdir ChatControler_release_cache/CustomerServiceChat/
	cp server ChatControler_release_cache/CustomerServiceChat/
	cp server.sh ChatControler_release_cache/CustomerServiceChat/
	@echo "#####################################################"
	@echo "# Copy data ... 				                       #"
	@echo "#####################################################"
	@echo "-> Encrypting Files ..."
	$(MAKE) -C $(TOOL_DIR)/EncryptFile
	$(TOOL_DIR)/EncryptFile/main data
	@echo "-> Copy Files ..."
	$(MAKE) -C $(TOOL_DIR)/CopyFile
	$(TOOL_DIR)/CopyFile/main ./data dat,xml,usr,conf ChatControler_release_cache/CustomerServiceChat
	#cp data/0012 ChatControler_release_cache/ChatControler/data -r
	#cp data/InterfaceXml ChatControler_release_cache/ChatControler/data -r
	#cp data/com ChatControler_release_cache/ChatControler/data -r
	@echo "#####################################################"
	@echo "# Create release package ...		                   #"
	@echo "#####################################################"
	rm -f CustomerServiceChat.tar.gz
	#7z a -t7z -r ChatControler.7z ChatControler_release_cache/ChatControler
	cd ChatControler_release_cache;tar -zcvf ../CustomerServiceChat.tar.gz CustomerServiceChat;cd ..
	rm -rf ChatControler_release_cache

