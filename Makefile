CC=gcc

swtd:
	$(CC) -g -o swtd -lncurses -lmenu swtd.c
	# release switches -- -O2 -s (no -g)

clean:
	rm -f swtd *.o
