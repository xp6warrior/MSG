#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char **argv) {
    char *ipAddress;
    unsigned short int port;

    // Initialising IP and port
    switch (argc) {
        case 2:
            ipAddress = "127.0.0.1";
            port = atoi(argv[1]);
            break;
        case 3:
            ipAddress = argv[1];
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "usage: MSG_Client [ipaddress] [port]\n");
            return -1;
    }

    // Initialising address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress, &(addr.sin_addr));
    printf("Initialied address\n");

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == 0) {
        fprintf(stderr, "ERROR: unable to create socket\n");
        return -2;
    }
    printf("Created socket\n");

    // Connect
    if (connect(sock, (struct sockaddr *) &addr, sizeof addr) == -1) {
        fprintf(stderr, "ERROR: unable to connect\n");
        return -3;
    }
    printf("Connected to server\n");

    // Polling
    struct pollfd sendPoll;
    sendPoll.fd = 0;
    sendPoll.events = POLLIN;
    struct pollfd recvPoll;
    recvPoll.fd = sock;
    recvPoll.events = POLLIN;

    while (1) {
        char buffer[63];

        // TODO Fix messages with spaces sent from clients
        // TODO Fix issues when buffer is larger than MAX_MSG_LENGTH

        // Send
        if (poll(&sendPoll, 1, 100) == 1) {
            scanf("%s", buffer);
            send(sock, buffer, sizeof buffer, 0);
        }

        // Recieve
        if (poll(&recvPoll, 1, 100) == 1) {
            char allMsgs[256];
            if (recv(sock, allMsgs, 256, 0) == 0) {
                break;
            }
            printf("%s", allMsgs);
        }
    }

    // Close
    close(sock);

    return 0;
}