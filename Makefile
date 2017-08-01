CC=g++
CPPFLAGS=-std=c++11 -Wall -Wpedantic -Wextra
MKDIR_P=mkdir -p

all: directories bin/midas

bin/midas: obj/midas.o
	$(CC) $(CPPFLAGS) $^ -o $@

obj/%.o : src/%.cpp
	$(CC) $(CPPFLAGS) $< -c -o $@

.PHONY: clean directories

directories:
	${MKDIR_P} bin obj

clean:
	$(RM) midas bin/midas
	$(RM) obj/*.o
