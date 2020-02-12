CC=gcc

swtd:
	$(CC) -g -o swtd -lncurses -lmenu swtd.c

clean:
	rm -f swtd *.o
