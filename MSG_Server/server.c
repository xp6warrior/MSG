#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "server.h"

void addClient(struct client *c, struct client **clientList) {
    struct client **p = clientList;
    while (*p != NULL) {
        p = &((*p)->next);
    }
    *p = c;
}

int removeClient(struct client *c, struct client **clientList) {
    if (*clientList == NULL) return -1;
    if (*clientList = c) {
        *clientList = c->next;
        free(c);
        c = NULL;
        return 0;
    }
    struct client **ptr = clientList;
    while ((*ptr)->next != c) ptr = &((*ptr)->next);

    struct client *ptr2 = c;
    (*ptr)->next = c->next;
    free(c);
    c = NULL;
    return 0;
}

struct client* establishConnections(int s_sock, struct client **clientList) {
    struct sockaddr_in c_addr;
    int addrSize = sizeof c_addr;
    int c_sock = accept(s_sock, (struct sockaddr *) &c_addr, &addrSize);
    
    if (c_sock != -1) {
        struct client c = { .sockfd = c_sock, .next = NULL };
        strcpy(c.addr, inet_ntoa(c_addr.sin_addr));

        struct client *c_ptr = malloc(sizeof (struct client));
        *c_ptr = c;
        
        struct client **p = clientList;
        while (*p != NULL) {
            p = &((*p)->next);
        }
        *p = c_ptr;

        return c_ptr;
    }
    return NULL;
}

// TODO freeConnections

int recieveMessages(struct client *c, char *buff, int buff_len) {
    if (c != NULL) {
        struct pollfd recvPoll = { .fd = c->sockfd, .events = POLLIN };

        if (poll(&recvPoll, 1, 100)) {
            if (c->sockfd == 0) { //stdin
                int bytesRead = read(c->sockfd, buff, buff_len);
                if (bytesRead != -1) {
                    // \n marks the end of the input.
                    // If buffer is full and there is no \n, the stdin still has characters
                    // while loop flushes stdin
                    if (bytesRead == buff_len && buff[bytesRead-1] != '\n')
                        while (fgetc(stdin) != '\n');

                    buff[bytesRead-1] = '\0';
                    return -2;
                }
            } else {
                return recv(c->sockfd, buff, buff_len, 0);
            }
        }
    }
    return -1;
}

int getDefaultIP(char *buff, int buff_len) {
    struct ifaddrs *addrs, *ptr;
    if (getifaddrs(&addrs) == -1)
        return -1;

    for (ptr = addrs; ptr != NULL; ptr = ptr->ifa_next) {
        if (ptr->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) ptr->ifa_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), buff, buff_len);
            if (buff != NULL)
                break;
        }
    }
    freeifaddrs(addrs);
    return 0;
}