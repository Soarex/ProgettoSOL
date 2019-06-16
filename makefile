CC = gcc
CFLAGS = -std=gnu99 -pedantic -Wall
client_objects = client.o message_processer_client.o
server_objects = message_processer_server.o hash_table.o object_store.o server.o
objects = $(client_objects) $(server_objects)
bins = libclient.a server.out test_client.out
server_path = Server/src/
client_path = Client/src/
test_client_path = TestClient/src/
client_include = -I$(client_path)
server_include = -I$(server_path)

.PHONY : all clean test

all: $(bins)

server.out: $(server_objects)
	$(CC) -o server.out $(server_objects) -lpthread

object_store.o: $(server_path)object_store.c $(server_path)object_store.h $(server_path)utils.h
	$(CC) $(CFLAGS) -c $(server_include) $(server_path)object_store.c

message_processer_server.o: $(server_path)message_processer.c $(server_path)message_processer.h $(server_path)utils.h
	$(CC) $(CFLAGS) -c $(server_include) $(server_path)message_processer.c -o message_processer_server.o

hash_table.o: $(server_path)hash_table.c $(server_path)hash_table.h
	$(CC) $(CFLAGS) -c $(server_include) $(server_path)hash_table.c

server.o: $(server_path)server.c $(server_path)server.h $(server_path)utils.h
	$(CC) $(CFLAGS) -c $(server_include) $(server_path)server.c

libclient.a: $(client_objects)
	ar rvs libclient.a $(client_objects)

message_processer_client.o: $(client_path)message_processer.c $(client_path)message_processer.h $(client_path)utils.h
	$(CC) $(CFLAGS) $(client_include) $(client_path)message_processer.c -c -o message_processer_client.o

client.o:$(client_path)client.c $(client_path)client.h $(client_path)utils.h
	$(CC) $(CFLAGS) $(client_include) $(client_path)client.c -c -o client.o

test_client.out: $(test_client_path)main.c $(client_path)client.h
	$(CC) $(CFLAGS) $(client_include) $(test_client_path)main.c -o test_client.out -L. -lclient

clean:
	-rm -f $(objects) server.out libclient.a test_client.out

test:
	./test.sh

.ONESHELL: