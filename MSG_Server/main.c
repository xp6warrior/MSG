#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
#define DEFAULT_IP "0.0.0.0"

void getDefaultIP(char *ip, int size) {
    struct ifaddrs *addrs, *ptr;
    if (getifaddrs(&addrs) == -1) {
        fprintf(stderr, "ERROR: unable to retrieve default IP\n");
        return;
    }

    for (ptr = addrs; ptr != NULL; ptr = ptr->ifa_next) {
        if (ptr->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) ptr->ifa_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip, size);
            if (ip != NULL)
                break;
        }
    }
    freeifaddrs(addrs);
}


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
    int s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == 0) {
        fprintf(stderr, "ERROR: unable to create socked\n");
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
    

    // System loop
    while (1) {
        struct sockaddr_in c_addr;
        int size = sizeof c_addr;

        accept(s_sock, (struct sockaddr *) &c_addr, &size);

        char *c_ip;
        inet_ntop(c_addr.sin_family, &(c_addr.sin_addr), c_ip, INET6_ADDRSTRLEN);
        printf("%s has connected\n", c_ip);
    }

    // Close
    close(s_sock);

    return 0;
}