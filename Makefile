CC=gcc

swtd:
	$(CC) -Wall -g -o swtd swtd.c -lncurses -lmenu -lsqlite3
	# release switches -- -O2 -s (no -g)

clean:
	rm -f swtd *.o
