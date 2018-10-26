# allfiles = Makefile a2sdn.cc

# compile: 
# 	g++ -std=c++11 -Wall a2sdn.cc -o a2sdn
all: a2sdn fifo
a2sdn: a2sdn.o controller.o switch.o
	g++ -std=c++11 -Wall -o a2sdn a2sdn.o controller.o switch.o

a2sdn.o: a2sdn.cc variables.h controller.h switch.h
	g++ -std=c++11 -Wall -c a2sdn.cc

controller.o: controller.cc variables.h controller.h
	g++ -std=c++11 -Wall -c controller.cc

switch.o: switch.cc variables.h switch.h
	g++ -std=c++11 -g -Wall -c switch.cc

fifo:
	mkfifo fifo-1-0
	mkfifo fifo-0-1

	mkfifo fifo-2-0
	mkfifo fifo-0-2

	mkfifo fifo-3-0
	mkfifo fifo-0-3

	mkfifo fifo-4-0
	mkfifo fifo-0-4

	mkfifo fifo-5-0
	mkfifo fifo-0-5

	mkfifo fifo-6-0
	mkfifo fifo-0-6

	mkfifo fifo-7-0
	mkfifo fifo-0-7

	mkfifo fifo-1-2
	mkfifo fifo-2-1

	mkfifo fifo-2-3
	mkfifo fifo-3-2

	mkfifo fifo-3-4
	mkfifo fifo-4-3

	mkfifo fifo-4-5
	mkfifo fifo-5-4

	mkfifo fifo-5-6
	mkfifo fifo-6-5

	mkfifo fifo-6-7
	mkfifo fifo-7-6



clean:
	rm -f *.o
	rm a2sdn
	rm -rf fifo*
