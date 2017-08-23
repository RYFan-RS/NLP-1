###User Change Section################
ROOT_DIR = ../../..
include $(ROOT_DIR)/MakeVars
INCLUDE = -I/usr/include/libxml2 -I$(ROOT_DIR)/include -I$(ROOT_DIR)/src
DEST_DIR = $(ROOT_DIR)/bin
C_FLAGS = ${GOPTION} ${OOPTION} -fPIC #-DMEAN_DEBUG
###################################


SOURCES = $(wildcard *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)
ENGINE_OBJS= AnalyzeEngine.o BidirectionalMap.o CellCache.o Cell.o CommonParser.o DataManager.o DataStorage.o DefineRule.o DequeWord.o EntryManager.o ErrorData.o ExecutePhase.o ExecuteRule.o FileManager.o IndexRule.o KeyPart.o KeyRange.o LineIndexManager.o MatchingMatrix.o md5.o mean.o MeanResult.o MeanWord.o NormalRule.o Position.o Range.o ScriptLimit.o SearchMode.o TagLimit.o TagManager.o TagRule.o Template.o Utility.o VCellCache.o VCell.o VersionManager.o WordLimit.o WordManager.o KeyWordManager.o SmtcKeyWordMngr.o 
SE_LIB =  $(DEST_DIR)/libSemanticEngine.a
DYPSR_LIB = $(DEST_DIR)/libDynamicParser.a

all: main semanticCheck $(SE_LIB)

%.d: %.cpp
	@set -e; rm -f $@;
	$(CC) $(INCLUDE) -MM $< | sed -e 's,.*\.o[ :]*,$*.o : ,g' > $@;

sinclude $(SOURCES:.cpp=.d)

%.o: %.cpp
	$(CC) $(INCLUDE) $(C_FLAGS) -c $< -o $@

main : $(ENGINE_OBJS) main.o
	$(CC) $(C_FLAGS) $^ -o $@ $(DYPSR_LIB)
	
semanticCheck : $(ENGINE_OBJS) SemanticCheck.o
	$(CC) $(C_FLAGS) $^ -o $@ $(DYPSR_LIB)

$(SE_LIB) : $(ENGINE_OBJS)
	@echo
	@echo creating Semantic Engine library ...
	@echo
	$(AR) rcs $(SE_LIB) $^

.PHONY: clean

clean:
	rm -f core *.o *.d *.BAK main $(SE_LIB)
