all: client server

client: client.c ../../hf.h hf_defs.h
	cc client.c -o client

server: server.c ../../hf.h hf_defs.h
	cc server.c -o server

.PHONY: clean
clean:
	rm -f client server
