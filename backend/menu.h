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

int menu(Client **clients, int client_count, pthread_mutex_t *clients_mutex);
int menu_command_status(Client **clients, int client_count, int selection);
int menu_queue_command(Client **clients, int client_count, pthread_mutex_t *clients_mutex, int selection);
int menu_select_client(Client **clients, int client_count);
