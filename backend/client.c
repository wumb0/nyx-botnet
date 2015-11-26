#include "client.h"


Client *client_exists(Client **cts, char *addr, int count) {
    for (int i = 0; i < count; i++) {
        Client cli = *cts[i];
        if ( strcmp(inet_ntoa((*(struct sockaddr_in *)cli.client_sock).sin_addr),addr) == 0 ) {
            printf("[client_exists]: Client %s matches %s... \n",inet_ntoa(( *(struct sockaddr_in *)cli.client_sock).sin_addr), addr);
            return cts[i];
        }
    }
    return (void *)0;
}

int client_cmd_queue(Client **cts, int count, int selection, Command *cmd) {
    int addcount = 0;
    for (int i = 0; i < count; i++) {
        if (selection == 0 || selection == i+1) {
            cmd_queue(cmd, (*cts[i]).cmds);
            addcount++;
        }
    }
    return addcount;
}


void client_read(Client c, pthread_mutex_t c_mutex) {
    int n;
    char buffer[256];
    bzero(buffer,256);
    n = read(c.fd,buffer,255);
    //while ( (n = read(c.fd,buffer,255)) > 0 )  {

        if (n < 0) {
            //perror("ERROR reading from socket");
            //printf("ERROR reading from socket\n");
            //exit(1);
        }
        
        //printf("Check-in %s\n",buffer);
        char *msg = "NOOP";
        pthread_mutex_lock(&c_mutex);
        if (c.cmds.queue_len < 1) {

        }
        else {
            Command *cur = cmd_dequeue(c.cmds);
            while ( cur != NULL ) {
                Command cur_cmd = (*cur);
                msg = cur_cmd.cmd;
                write(c.fd,msg,strlen(msg));
                cur = cmd_dequeue(c.cmds);
            }
            cmd_reset_complete(c.cmds);
        }
        pthread_mutex_unlock(&c_mutex);
        n = write(c.fd,msg,strlen(msg));

        if (n < 0) {
            //perror("ERROR writing to socket");
            //printf("ERROR writing to socket\n");
            //exit(1);
        }
        bzero(buffer,256);
    //}
    close(c.fd);
    //printf("Finished reading.\n");
}
