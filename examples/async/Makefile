all: client server

client: client.c ../../hf.h
	cc client.c -o client

server: server.c ../../hf.h
	cc server.c -o server

.PHONY: clean
clean:
	rm -f client server
