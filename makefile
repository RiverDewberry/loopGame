LIB=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CC=gcc
SRC=src/*.c

all: clean build run

clean:
	rm -rf build

build:
	mkdir build
	${CC} ${SRC} -o build/game -I . ${LIB}
	cp -r sprites build

run:
	./build/game
test:
	rm -rf build
	mkdir build
	${CC} -g ./src/*.c -o build/game -I . ${LIB}
	cp -r sprites build
	gdb ./build/game
