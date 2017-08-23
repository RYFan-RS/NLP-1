ROOT_DIR = ../../..
include $(ROOT_DIR)/MakeVars

INCLUDE = -I$(ROOT_DIR)/include -I/usr/include/mysql -I/usr/include/mysql++
LFLAG = -L/usr/local/lib -lmysqlpp

GOOPTION = -g
ifeq (${GOPTION}, -g)
	GOOPTION = $(GOPTION)
endif

ifeq (${OOPTION}, -O3)
	GOOPTION = $(OOPTION)
endif

C_FLAGS = $(GOOPTION) -fPIC


main = main
main_obj =  main.o


all: obj $(main)

obj:
	$(CC) -c *.cpp $(C_FLAGS) $(INCLUDE)
	
$(main):
	@echo
	@echo start create $(main)
	$(CC) $(main_obj) -o $(main) $(INCLUDE) $(C_FLAGS) $(LFLAG)
	@echo create $(main) finish
	
clean:
	rm $(main) *.o -f
	@echo clean finish
