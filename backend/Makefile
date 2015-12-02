all: client server

client:
	gcc -g -Wall -Wextra -std=c99 beacon_client.c -o beacon_client

server:
	gcc -Wall -Wextra -pthread -std=c99 menu.c command.c client.c beacon_server.c -o beacon_server

debug_client:
	gcc -Wall -DCLIENT_DEBUG -Wextra -ggdb -std=c99 beacon_client.c -o beacon_client
