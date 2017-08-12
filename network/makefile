CFLAG=-lcjson -lmysqlclient -lpthread

result:myserver.o myclient.o
	gcc -o server myserver.o $(CFLAG) 
	gcc -o client myclient.o $(CFLAG)
myserver.o:myserver.c
	gcc -c myserver.c $(CFLAG)
myclient.o:myclient.c
	gcc -c myclient.c $(CFLAG)
clean:
	rm client server
	rm myclient.o myserver.o
