#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>
#include <time.h>
#include <locale.h>
#include <fcntl.h>

void errquit(char *msg);
int tcp_connect(int af, char *servip, unsigned short port);
int tcp_listen(int host, int port, int backlog);
void INTHandler(int sig);
