#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <stdio.h>

#include "command.h"

#ifndef __client
#define __client
typedef struct {
    struct Client *next;
    char *hash;
    char *addr;
    int fd;
    struct sockaddr_storage *client_sock;
    Commands cmds;
} Client;

typedef struct {
    Client *first;
    Client *end;
    int client_count;
} Clients;
#endif


Client *client_exists(Client **clients, char *addr, int count);
int client_cmd_queue(Client **cts, int count, int selection, Command *cmd);
void client_read(Client c, pthread_mutex_t c_mutex);
