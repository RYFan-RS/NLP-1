###User Change Section################
ROOT_DIR = ../..
include $(ROOT_DIR)/MakeVars
###################################

#Source directories
DynamicParser = DynamicParser
StaticParser  = StaticParser
MixParser     = MixParser

all : ALL_LIBS

ALL_LIBS :
	@echo "-> -> Compiling DynamicParser ..."
	$(MAKE) -C $(DynamicParser)
	@echo "-> -> Compiling StaticParser ..."
	$(MAKE) -C $(StaticParser)
	@echo "-> -> Compiling MixParser ..."
	$(MAKE) -C $(MixParser)
	
clean:
	$(MAKE) -C $(DynamicParser) clean
	$(MAKE) -C $(StaticParser) clean
	$(MAKE) -C $(MixParser) clean
