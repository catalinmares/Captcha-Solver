CC=gcc
CFLAGS=-Wall
EXE=captcha_editor

build: captcha_editor

captcha_editor: captcha_editor.c bmp_header.h
	$(CC) captcha_editor.c -o $(EXE)

run:
	./$(EXE)

clean:
	rm -rf $(EXE) 
