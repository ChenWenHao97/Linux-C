CFLAG=-lcjson -lmysqlclient -lpthread -I /usr/include/mysql


result:myserver.o myclient.o
	gcc -o server myserver.o $(CFLAG) -g
	gcc -o client myclient.o $(CFLAG) -g
myserver.o:myserver.c
	gcc -c myserver.c $(CFLAG) -g
myclient.o:myclient.c
	gcc -c myclient.c $(CFLAG) -g
clean:
	rm client server
	rm myclient.o myserver.o
