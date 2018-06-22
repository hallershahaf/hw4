OUTPUT= cacheSim

all: $(OUTPUT)

OBJ= cacheSim.cpp memory.c memory.h Cache.c \
     Cache.h

CFLAGS= -std=c++11 -g -Werror -Wall

$(OUTPUT): cacheSim.cpp $(OBJ)
	g++ $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o
	rm -f cacheSim
