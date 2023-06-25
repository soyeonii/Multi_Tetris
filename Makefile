CC = gcc
TARGETS = tetris server client
OBJS = 
all: $(TARGETS)
tetris: tetris.c netprog.c
	$(CC) -o tetris tetris.c netprog.c -lncursesw
server: server.c netprog.c
	$(CC) -o server server.c netprog.c -lpthread -D_REENTRANT
client: client.c netprog.c
	$(CC) -o client client.c netprog.c
clean:
	rm -f $(TARGETS) $(OBJS)
