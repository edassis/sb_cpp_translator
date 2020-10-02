############################# Makefile ##########################
all: main
main:
	g++ -std=c++11 -Wall -g -I header -o montador src/main.cpp

clean:
	rm -rf *.o montador

exec:
	./montador -p equ.asm
