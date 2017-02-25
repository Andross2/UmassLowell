all:program1 program2

program1:
	gcc Server.c -pthread -o Server

program2:
	gcc Client.c -pthread -o Client

clean:
	rm*.o Server Client
