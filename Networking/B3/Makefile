all:
	gcc -o server server.c -lcrypto
	gcc -o client client.c -lcrypto
client:
	gcc -o client client.c
server:
	gcc -o server server.c
clean:
	rm -rf client server