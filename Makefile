# allfiles = Makefile a2sdn.cc

# compile: 
# 	g++ -std=c++11 -Wall a2sdn.cc -o a2sdn

a2sdn: a2sdn.o controller.o switch.o
	g++ -std=c++11 -Wall -o a2sdn a2sdn.o controller.o switch.o

a2sdn.o: a2sdn.cc variables.h controller.h switch.h
	g++ -std=c++11 -Wall -c a2sdn.cc

controller.o: controller.cc variables.h controller.h
	g++ -std=c++11 -Wall -c controller.cc

switch.o: switch.cc variables.h switch.h
	g++ -std=c++11 -Wall -c switch.cc



clean:
	rm -f *.o
	rm a2sdn
