CC = gcc
CFLAGS = -g -std=c99 -Wall -fsanitize=address,undefined

words: words.o lines.o
	$(CC) $(CFLAGS) -o $@ $^

lines.o: lines.h

clean:
	rm -f -r *.o *.dSYM words
