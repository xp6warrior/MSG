#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "comm.h"
#include "server.h"
#include "commands.h"

#define DEFAULT_PORT 8000
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_SERVER_NAME "untitled_server"

#define MAX_MSG_LEN 64
#define MAX_RECORD_LEN INET_ADDRSTRLEN + MAX_MSG_LEN + 1
#define MAX_SERVER_NAME 30

int main(int argc, char **argv) {
    char *ipAddress = DEFAULT_IP;
    u_int16_t port = DEFAULT_PORT;
    char *serverName = DEFAULT_SERVER_NAME;

    // Initialising IP and port from arguments
    // TODO improve arguments
    switch (argc) {
        case 1: // no params
            printf("No IP address specified, default to system IP\n");
            printf("No port number specified, default to %d\n\n", DEFAULT_PORT);
            break;
        case 2: // ip
            printf("No port number specified, default to %d\n\n", DEFAULT_PORT);
            ipAddress = argv[1];
            break;
        case 3: // ip and port
            ipAddress = argv[1];
            port = atoi(argv[2]);
            break;
        case 4: // ip port and name
            ipAddress = argv[1];
            port = atoi(argv[2]);
            serverName = argv[3];
            if (strlen(serverName) > MAX_SERVER_NAME) {
                fprintf(stderr, "invalid servername: must be less than %d characters\n", MAX_SERVER_NAME);
                return -5;
            }
            break;
        default:
            fprintf(stderr, "usage: MSG_Server [ipaddress] [port] [servername]\n");
            return -1;
    }

    // Initialising address struct
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress, &(s_addr.sin_addr));
    printf("Initialied address\n");

    // Creates server socket
    int s_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // NONBLOCK so accepting dosent block the loop
    if (s_sock == 0) {
        fprintf(stderr, "ERROR: unable to create socket\n");
        return -2;
    }
    printf("Created socket\n");

    // Binds server socket to IP and port (addr struct)
    if (bind(s_sock, (struct sockaddr *) &s_addr, sizeof s_addr) == -1) {
        fprintf(stderr, "ERROR: unable to bind socket\n");
        return -3;
    }
    printf("Bound socket\n");
    
    // Listens for connections
    if (listen(s_sock, 5) == -1) {
        fprintf(stderr, "ERROR: unable to listen\n");
        return -4;
    }

    // Printing stuff
    printf("\nServer name: %s\n", serverName);
    if (ipAddress == DEFAULT_IP) {
        char displayip[INET_ADDRSTRLEN];
        getDefaultIP(displayip, sizeof displayip);
        printf("Listening at %s on port %u\n", displayip, port);
    } else {
        printf("Listening at %s on port %u\n", ipAddress, port);
    }
    

    // Server loop
    // Every loop cycle has 1 record string
    // First, the record is filled via actions from the clients, such as connecting or sending a message
    // Next, if the recored is filled, the program jumps to processing the record (running commands, replication to clients)
    
    // Adds server stdin as a 'client'
    struct client *clientList = NULL;
    struct client serverClient = { 0, "Server", 1, NULL };
    addClient(&serverClient, &clientList);

    char serverClosed = 0;
    while (!serverClosed) {
        char record[MAX_RECORD_LEN] = {'\0'};
        struct client *cPtr;
        int ignorefd = -1;

        /* CHECKING FOR CLIENT ACTIONS, FILLING RECORD */
        // Establish new connections
        cPtr = establishConnections(s_sock, &clientList);
        if (cPtr != NULL) {
            snprintf(record, sizeof record, "%s has connected\n", cPtr->addr);
            ignorefd = cPtr->sockfd;
            goto record_filled;
        }

        // Recieve messeges from clients
        cPtr = clientList;
        char text[MAX_MSG_LEN];
        int recvStatus = recieveMessage(cPtr, text, sizeof text);
        while (cPtr != NULL && recvStatus == -1) {
            cPtr = cPtr->next;
            recvStatus = recieveMessage(cPtr, text, sizeof text);
        }
        if (recvStatus == 0) { // disconnected
            snprintf(record, sizeof record, "%s has disconnected\n", cPtr->addr);
            removeClient(cPtr, &clientList);
            goto record_filled;
        } else if (recvStatus != -1) { // message
            snprintf(record, sizeof record, "%s: %s\n", cPtr->addr, text);
            goto record_filled;
        }
        
        continue;

        /* PROCESSING RECORD */
        record_filled:
        
        // Server commands
        if (cPtr != NULL && cPtr->admin == 1) {
            char *cmd;
            if ((cmd = strstr(record, "/close")) != NULL) {
                serverClosed = 1;
                snprintf(record, sizeof record, "The server has been closed!\n");
                ignorefd = 0;
            } else
            if ((cmd = strstr(record, "/kick")) != NULL) {
                kickCmd(cmd, clientList, record, sizeof record);
            }
        }

        // Replicate messages to all clients
        if (clientList->sockfd != ignorefd) {
            printf("%s", record);
        }
        cPtr = clientList->next;
        while (cPtr != NULL) {
            if (cPtr->sockfd != ignorefd) {
                send(cPtr->sockfd, record, sizeof record, 0);
            }
            cPtr = cPtr->next;
        }
    }

    printf("Closing server\n");

    // Close
    freeConnections(clientList->next); // skip 'server' client
    close(s_sock);

    return 0;
}