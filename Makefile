# Makefile

main: main.c raycast.h raycast.c
	clang -o raycaster.exe -W -Wall -Wextra -pedantic-errors -Iinclude -Llib/x64 -lSDL3 *.c
	