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
#define MAX_MSG_LEN 64
#define MAX_RECORD_LEN INET6_ADDRSTRLEN + MAX_MSG_LEN + 3

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

    // Initialising address struct
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
    printf("Bound socket\n");
    
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
    

    // Server loop
    // TODO fix linked list problem (disconnecting last joined client dosent show disconnected message)
    struct client *clientList = NULL;
    struct client serverClient = { 0, "Server", NULL };
    addClient(&serverClient, &clientList);

    while (1) {
        // Establish new connections
        struct client *connected = establishConnections(s_sock, &clientList);
        if (connected != NULL)
            printf("%s has connected\n", connected->addr);

        // Recieve messeges from clients
        struct client *ptr = clientList;
        char record[MAX_RECORD_LEN];
        char text[MAX_MSG_LEN];
        int recvStatus = recieveMessages(ptr, text, sizeof text);
        
        while (ptr != NULL && recvStatus == -1) {
            ptr = ptr->next;
            recvStatus = recieveMessages(ptr, text, sizeof text);
        }
        if (recvStatus == 0) { // disconnected
            snprintf(record, sizeof record, "%s has disconnected\n", ptr->addr);
            printf("%s", record);
            removeClient(ptr, &clientList);
        } else if (recvStatus != -1) { // message
            snprintf(record, sizeof record, "%s: %s\n", ptr->addr, text);
            printf("%s", record);
        }

        // TODO Server commands

        // Replicate messages to all clients
        ptr = clientList;
        while (ptr != NULL && recvStatus != -1) {
            send(ptr->sockfd, record, sizeof record, 0);
            ptr = ptr->next;
        }
    }

    // Close
    close(s_sock);

    return 0;
}