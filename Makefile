BITS=64

all: tester libfullscreenhack.so

tester: tester.c
	gcc -m$(BITS) -o tester tester.c -lX11 -lXrandr

libfullscreenhack.so: fullscreenhack.c
	gcc -m$(BITS) -fPIC -c -Wall fullscreenhack.c -o fullscreenhack.o
	gcc -m$(BITS) -shared fullscreenhack.o -ldl -o libfullscreenhack.so

test: all
	echo running without the hack...
	./tester
	echo running with the hack...
	LD_PRELOAD=./libfullscreenhack.so ./tester

fftest: all
	LD_PRELOAD=`pwd`/libfullscreenhack.so firefox 'http://www.youtube.com'

clean:
	rm -f *.o *.so tester

install:
	cp libfullscreenhack.so /usr/lib/nspluginwrapper/i386/linux/
	cp npviewer /usr/lib/nspluginwrapper/noarch/
