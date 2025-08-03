LIB=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CC=gcc
SRC=src/*.c

all: clean build run

clean:
	rm -rf build

build:
	mkdir build
	${CC} ${SRC} -o build/game -I . ${LIB}
	cp -r levels build
	cp -r audio build
	cp -r sprites build

run:
	./build/game
test:
	rm -rf build
	mkdir build
	${CC} -g ./src/*.c -o build/game -I . ${LIB}
	cp -r levels build
	cp -r sprites build
	cp -r audio build
	gdb ./build/game

lvledit:
	rm -f lvledit
	gcc src/levelEditor/main.c -o lvledit -I . ${LIB}
	./lvledit

web:
	rm -rf webbuild
	mkdir webbuild
	cp -r levels webbuild
	cp -r audio webbuild
	cp -r sprites webbuild
	emcc -o webbuild/index.html ${SRC} -Os -Wall ~/Downloads/raylib/src/libraylib.web.a -I. -I~/projects/robotGame/libs/raylib.h -L. -L~/Downloads/raylib/src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY --shell-file ~/Downloads/raylib/src/minshell.html -DPLATFORM_WEB --embed-file audio --embed-file sprites --embed-file levels
	rm -f webgame.zip
	zip webgame.zip ./webbuild/*

runWeb:
	npx http-server ./webbuild -c-1
