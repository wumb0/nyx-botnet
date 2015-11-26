#include "beacon_server.h"
#include "menu.h"

Client **clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int client_count=0;
int running = 1;

int main( int argc, char *argv[] )
{
    clients = (Client **)malloc(sizeof(Client *) * 255);
    pthread_t thread1;
    int port = 8080;
    int *port_ptr = &port;

    int ret = pthread_create( &thread1, NULL, (void *(*)(void *))listen_for_connections, (void *)port_ptr);
    printf("ret: %d\n",ret);
    
    while (menu(clients, client_count, &clients_mutex) >= 0);
    running = 0;
}

void listen_for_connections(void *port){
    int portno = *((int *)port);
    //printf("Listening on port: %d", portno);
    int sockfd, newsockfd;
    unsigned int clilen;
    struct sockaddr_storage serv_addr, cli_addr;
    
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    struct sockaddr_in *serv_addr_in = (struct sockaddr_in*)&serv_addr;
    (*serv_addr_in).sin_family = AF_INET;
    (*serv_addr_in).sin_addr.s_addr = INADDR_ANY;
    (*serv_addr_in).sin_port = htons( portno );
    unsigned int len = sizeof (struct sockaddr_in);
    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, len)) {
        perror("\n Critical: ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here
     * process will go in sleep mode and will wait
     * for the incoming connection
     */

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while (running) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        struct sockaddr_in *client_addr_in_ptr = (struct sockaddr_in *)&cli_addr;
        struct sockaddr_in client_addr_in = *client_addr_in_ptr;

        Client *cli;
        //struct sockaddr_storage *new_sock = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));
        //memcpy(new_sock,&cli_addr,sizeof(struct sockaddr_storage));
        char *addr = inet_ntoa(client_addr_in.sin_addr);
        char *new_addr = (char *)malloc(sizeof(char)*(strlen(addr)+1));
        strncpy(new_addr,addr,strlen(addr));
        printf("\nClient %s connected.. Checking if they have connected before... \n",new_addr);
        if ( ( cli = client_exists(clients, new_addr,  client_count ) ) == NULL ) {
            printf("\nClient %s doesn't exist...",new_addr);
            cli = (Client *)malloc(sizeof(Client));
            (*cli).addr = new_addr;
            (*cli).client_sock = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));
            memcpy((*cli).client_sock,&cli_addr,sizeof(struct sockaddr_storage));
            (*cli).fd = newsockfd;
            pthread_mutex_lock( &clients_mutex );
            clients[client_count] = cli;
            printf("Added [index: %d]\n",client_count);
            client_count++;
            pthread_mutex_unlock( &clients_mutex );
        } else {
            printf("\nClient %s already exists... Not adding. \n",new_addr);
        }
        /* Create child process */
        int pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            /* This is the client process */
            close(sockfd);
            client_read(*cli,clients_mutex);
            printf("Done with client.\n");
            close(newsockfd);
            //close(sockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
        bzero((char *) &cli_addr, sizeof(cli_addr));
    } /* end of while */
    close(sockfd);
}
