CC = gcc
CFLAGS=-I.

jsh: jsh.c
	$(CC) -o jsh jsh.c command.c

clean:
	rm jsh
