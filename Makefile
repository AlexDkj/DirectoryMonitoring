all: main.o queue.o list.o
	gcc main.o queue.o list.o -o sniffer
main.o: main.c
	gcc -c main.c
queue.o: queue.c
	gcc -c queue.c
list.o: list.c
	gcc -c list.c
clean:
	rm *.o
	rm sniffer
	rm ../out/*.out
