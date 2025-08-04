main:
	gcc -c *.c
	gcc *.o -o dictparse
	rm -f *.o

clean:
	rm -f *.o
	rm -f dictparse