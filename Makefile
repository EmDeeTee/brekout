all:
	gcc -Wall -Wextra main.c -o brekout `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
	./brekout
