#include <sys/socket.h> //sockets
#include <netinet/in.h> //
#include <arpa/inet.h> //inet_ntoa
#include <unistd.h> //read/write


#include <stdio.h> // printf
#include <string.h> // strlen, bzero
#include <pthread.h> //pthread
#include <stdlib.h> //malloc
#include <ctype.h> //isdigit

#include "command.h"
#include "client.h"


void listen_for_connections(void *port);
