#include "menu.h"
#include "client.h"

int menu(Client **clients, int client_count, pthread_mutex_t *clients_mutex){
    puts("1\tSelect Clients");
    puts("2\tQueue Commands");
    puts("3\tCommand status");
    puts("9\tExit");
    printf("[menu]: ");
    fflush(stdout);
    char buff[255];
    int BUFSIZE = 255;
    int n;
    int selected = 0;
    while((n = read(STDIN_FILENO, buff, BUFSIZE)) > 0) {
        printf("\n");
        for(int i = 0; i < n-1; ++i ) {
            if ( !isdigit( buff[i] ) ) {
                puts("Invalid input.");
                puts("1\tSelect Clients");
                puts("2\tQueue Commands");
                puts("3\tCommand status");
                puts("9\tExit");
                printf("[menu]: ");
                fflush(stdout);
            }
        }

        int ch = atoi(buff);
        printf("[menu] input: %d\n",ch);
        switch (ch) {
            case 1:
                selected = menu_select_client(clients, client_count);
                printf("[menu] Client Selected: %d\n",selected);
                break;
            case 2:
                menu_queue_command(clients, client_count, clients_mutex, selected);
                break; 
            case 3:
                menu_command_status(clients, client_count, selected);
                break;
            case 9:
                return -1;
                break;
            default:
                break;
        }
        puts("1\tSelect Clients");
        puts("2\tQueue Commands");
        puts("3\tCommand status");
        puts("9\tExit");
        printf("[menu]: ");
        fflush(stdout);
    }
    return -1;
}

int menu_command_status(Client **clients, int client_count, int selection) {
    if ( client_count < 1 ) return -1;

    for (int i = 0; i < client_count; i++){
        if (selection == 0 || selection == i+1) {
            Client c = (*clients[i]);
            printf("[menu/command_status]: Client %s has %d commands.\n", c.addr, c.cmds.queue_len);
            Command *cur = c.cmds.cmd_start;
            while ( cur != (void *)0 ) {
                if ((*cur).persist) {
                    printf("[menu/command_status]: Client %s \tCommand: %s\tPersistence: Yes\n",c.addr,(*cur).cmd);
                } else {
                    printf("[menu/command_status]: Client %s \tCommand: %s\tPersistence: No\n",c.addr,(*cur).cmd);
                }
            }   
        }
    }
    return 0;
}

int menu_queue_command(Client **clients, int client_count, pthread_mutex_t *clients_mutex, int selection){
    
    char buff[1024];
    int BUFSIZE = 1023;
    int n;
    printf("[menu/queue_command]: Command: ");
    fflush(stdout);
    if((n = read(STDIN_FILENO, buff, BUFSIZE)) > 0) {
        char buff2[64];
        int BUFF2SIZE = 63;
        Command *c = (Command *)malloc(sizeof(Command));
        printf("[menu/queue_command]: Persist the command? (y/n): ");
        fflush(stdout);
        if((n = read(STDIN_FILENO, buff2, BUFF2SIZE)) > 0) {
            if (buff2[0] == 'y' || buff2[0] == 'Y') {
                c->persist=1;
            }
            else {
                c->persist=0;
            }
        } else {
            free(c);
            return -1;
        }
        c->complete=0;
        pthread_mutex_lock( clients_mutex );
        int count = client_cmd_queue(clients,client_count,selection,c);
        pthread_mutex_unlock( clients_mutex );
        printf("[menu/queue_command]: Command Added for %d clients.\n",count);
    }
    return 0;
}


int menu_select_client(Client **clients, int client_count){
    for (int i = 0; i < client_count; i++) {
        Client c = *(clients[i]);
        printf("\t%d\t%s\n",i+1,inet_ntoa((*(struct sockaddr_in *)c.client_sock).sin_addr));
    }
    printf("\t0\tALL CLIENTS\n");
    printf("[menu/select_client]: ");
    fflush(stdout);
    char buff[255];
    int BUFSIZE = 255;
    int n;
    while((n = read(STDIN_FILENO, buff, BUFSIZE)) > 0) {
        for(int i = 0; i < n-1; ++i ) {
            if ( !isdigit( buff[i] ) ) {
                puts("Invalid input.");
                puts("1\tSelect Clients");
                puts("2\tQueue Commands");
                puts("3\tCommand status");
                puts("9\tExit");
            }
        }

        int ch = atoi(buff);
        if ( ch >= 0 & ch <= client_count ) {
            return ch;
        }
        for (int i = 0; i < client_count; i++) {
            Client c = *(clients[i]);
            printf("\t%d\t%s\n",i+1,inet_ntoa((*(struct sockaddr_in *)c.client_sock).sin_addr));
        }
        printf("\t0\tALL CLIENTS\n");
        printf("[menu/select_client]: ");
        fflush(stdout);
    }
    return -1;
}
