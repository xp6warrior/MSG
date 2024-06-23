#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        fprintf(stderr, "ERROR: unable to create socked\n");
        return -2;
    }
    printf("Created socket\n");

    // Connect
    if (connect(sock, (struct sockaddr *) &addr, sizeof addr) == -1) {
        fprintf(stderr, "ERROR: unable to connect\n");
        return -3;
    }
    printf("Connected to server\n");

    while (1) {
        char buffer[32];
        scanf("%s", buffer);
        send(sock, buffer, sizeof buffer, 0);
    }

    // Close
    close(sock);

    return 0;
}