#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "server.h"

void disconnectClient(struct client *c, struct client **clientList) {
    if (*clientList == NULL) return;
    if (*clientList = c) {
        *clientList = c->next;
        free(c);
        c = NULL;
        return;
    }

    struct client **ptr = clientList;
    while ((*ptr)->next != c) ptr = &((*ptr)->next);
    struct client *ptr2 = c;
    (*ptr)->next = c->next;
    free(c);
}

void establishConnections(int s_sock, struct client **clientList) {
    struct sockaddr_in c_addr;
    int addrSize = sizeof c_addr;
    int c_sock = accept(s_sock, (struct sockaddr *) &c_addr, &addrSize);
    
    if (c_sock != -1) {
        struct client c;
        c.sockfd = c_sock;
        strcpy(c.addr, inet_ntoa(c_addr.sin_addr));
        c.next = NULL;

        struct client *c_ptr = malloc(sizeof (struct client));
        *c_ptr = c;
        
        struct client **p = clientList;
        while (*p != NULL) p = &((*p)->next);
        *p = c_ptr;

        printf("%s has connected\n", c_ptr->addr);
    }
}

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