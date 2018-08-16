CC = gcc

all: main filter_generator clean
	#./filter_generator 3
	./main 20 dragon.ascii.pgm filter20.txt result.ascii.pgm

main: main.o
	gcc -o main main.o -lpthread

main.o: main.c
	gcc -c main.c

filter_generator: filter_generator.o
	gcc -o filter_generator filter_generator.o

filter_generator.o: filter_generator.c
	gcc -c filter_generator.c

clean:
	rm -f *.o

cleanAll:
	rm -f *.o
	rm main
	rm filter_generator