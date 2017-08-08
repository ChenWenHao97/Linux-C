result:epollserver.o epollclient.o
	gcc -o epollserver epollserver.o
	gcc -o epollclient epollclient.o
epollserver.o:
	gcc -c epollserver.c
epollclient.o:
	gcc -c epollclient.c
clean:
	rm epollclient epollserver
	rm epollclient.o epollserver.o
