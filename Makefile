all: tester libfullscreenhack.so libfullscreenhackppapi.so

tester: tester.c
	gcc -o tester tester.c -lX11 -lXrandr

libfullscreenhack.so: fullscreenhack.c
	gcc -fPIC -c -Wall fullscreenhack.c -o fullscreenhack.o
	gcc -shared fullscreenhack.o -ldl -o libfullscreenhack.so

libfullscreenhackppapi.so: fullscreenhackppapi.c
	gcc -fPIC -c -Wall fullscreenhackppapi.c -o fullscreenhackppapi.o
	gcc -shared fullscreenhackppapi.o -ldl -o libfullscreenhackppapi.so

test: all
	echo running without the hack...
	./tester
	echo running with the hack...
	LD_PRELOAD=./libfullscreenhack.so ./tester

clean:
	rm -f *.o *.so tester
