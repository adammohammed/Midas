CC=g++

all: bin/midas

bin/midas: obj/midas.o
	$(CC) $^ -o $@

obj/%.o : src/%.cpp
	$(CC) $< -c -o $@

.PHONY: clean

clean:
	$(RM) midas bin/midas
	$(RM) obj/*.o
