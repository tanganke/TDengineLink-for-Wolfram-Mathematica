build:
	mprep TDengineLink.tm -o TDengineLink.tm.c
	gcc -c TDengineLink.tm.c
	g++ -c TDengineLink.cpp
	mcc -g -Wall -o TDengineLink *.o -ltaos

clean:
	rm -f *.o

install:
	cp TDengineLink ~/.Mathematica/Applications/
