all: libfshack-npapi.so libfshack-ppapi.so

%.o: %.c common.h
	gcc -fPIC -c -Wall $*.c -o $*.o

libfshack-npapi.so: npapi.o common.o
	gcc -shared npapi.o common.o -ldl -o libfshack-npapi.so

libfshack-ppapi.so: ppapi.o common.o
	gcc -shared ppapi.o common.o -ldl -o libfshack-ppapi.so

clean:
	rm -f *.o *.so tester
