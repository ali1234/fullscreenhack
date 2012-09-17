BITS=64

all: tester libfullscreenhack.so libfullscreenhackppapi.so

tester: tester.c
	gcc -m$(BITS) -o tester tester.c -lX11 -lXrandr

libfullscreenhack.so: fullscreenhack.c
	gcc -m$(BITS) -fPIC -c -Wall fullscreenhack.c -o fullscreenhack.o
	gcc -m$(BITS) -shared fullscreenhack.o -ldl -o libfullscreenhack.so

libfullscreenhackppapi.so: fullscreenhackppapi.c
	gcc -m$(BITS) -fPIC -c -Wall fullscreenhackppapi.c -o fullscreenhackppapi.o
	gcc -m$(BITS) -shared fullscreenhackppapi.o -ldl -o libfullscreenhackppapi.so

test: all
	echo running without the hack...
	./tester
	echo running with the hack...
	LD_PRELOAD=./libfullscreenhack.so ./tester

clean:
	rm -f *.o *.so tester
