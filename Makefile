allfiles = Makefile a2sdn.cc

compile: 
	g++ -std=c++11 -Wall a2sdn.cc -o a2sdn

clean:
	rm a2sdn
