result:my_server.o my_client.o
	gcc -o server my_server.o -lcjson -lmysqlclient
	gcc -o client my_client.o -lcjson -lmysqlclient
my_server.o:
	gcc -c my_server.c -lcjson -lmysqlclient
my_client.o:
	gcc -c my_client.c -lcjson -lmysqlclient
clean:
	rm client server
	rm my_client.o my_server.o
