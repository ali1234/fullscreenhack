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

clean:
	rm -f *.o *.so tester

install:
	@echo "'install' is not a valid option. Read the README file!"

simpleinstall:
ifeq ($(BITS),64)
	cp libfullscreenhack.so /usr/lib64/libfullscreenhack.so
else
	cp libfullscreenhack.so /usr/lib/libfullscreenhack.so
endif


firefox:
ifeq ($(wildcard /usr/bin/firefox.main),) 
	cp /usr/bin/firefox /usr/bin/firefox.main
endif
ifeq ($(BITS),64)
	cp libfullscreenhack.so /usr/lib64/libfullscreenhack.so
	echo "export LD_PRELOAD=/usr/lib64/libfullscreenhack.so" > /usr/bin/firefox
else
	cp libfullscreenhack.so /usr/lib/libfullscreenhack.so
	echo "export LD_PRELOAD=/usr/lib/libfullscreenhack.so" > /usr/bin/firefox
endif
	echo 'firefox.main "$$@"' >> /usr/bin/firefox
