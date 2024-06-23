#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "server.h"

#define DEFAULT_PORT 8000
#define DEFAULT_IP "0.0.0.0"

int main(int argc, char **argv) {
    char *ipAddress = DEFAULT_IP;
    unsigned short int port = DEFAULT_PORT;

    // Initialising IP and port
    switch (argc) {
        case 1: // default ip default port
            printf("No IP address specified, default to system IP\n");
            printf("No port number specified, default to %d\n\n", DEFAULT_PORT);
            break;
        case 2: // default port
            printf("No port number specified, default to %d\n\n", DEFAULT_PORT);
            ipAddress = argv[1];
            break;
        case 3:
            ipAddress = argv[1];
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "usage: MSG_Server [ipaddress] [port]\n");
            return -1;
    }

    // Initialising address
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress, &(s_addr.sin_addr));
    printf("Initialied address\n");

    // Server socket
    int s_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // NONBLOCK so accepting dosent block the loop
    if (s_sock == 0) {
        fprintf(stderr, "ERROR: unable to create socket\n");
        return -2;
    }
    printf("Created socket\n");

    // Binding socket
    if (bind(s_sock, (struct sockaddr *) &s_addr, sizeof s_addr) == -1) {
        fprintf(stderr, "ERROR: unable to bind socket\n");
        return -3;
    }
    printf("Bound socked\n");
    
    // Listen for connections
    if (listen(s_sock, 5) == -1) {
        fprintf(stderr, "ERROR: unable to listen\n");
        return -4;
    }

    if (ipAddress == DEFAULT_IP) {
        char displayip[INET_ADDRSTRLEN];
        getDefaultIP(displayip, sizeof displayip);
        printf("\nListening at %s on port %u\n", displayip, port);
    } else {
        printf("\nListening at %s on port %u\n", ipAddress, port);
    }
    

    struct client *clientList = NULL;

    // Server loop
    while (1) {
        // Establish new connections
        establishConnections(s_sock, &clientList);

        // Recieve messeges from clients
        struct client *ptr = clientList;
        while (ptr != NULL) {
            char buffer[32];

            struct pollfd recvPoll;
            recvPoll.fd = ptr->sockfd;
            recvPoll.events = POLLIN;

            if (poll(&recvPoll, 1, 100)) {
                int recieveStatus = recv(ptr->sockfd, buffer, sizeof buffer, 0);

                if (recieveStatus == -1) continue;
                if (recieveStatus == 0) {
                    printf("%s has disconnected\n", ptr->addr);
                    disconnectClient(ptr, &clientList);
                    continue;
                }
                printf("%s: %s\n", ptr->addr, buffer);
            }

            ptr = ptr->next;
        }
    }

    // Close
    close(s_sock);

    return 0;
}