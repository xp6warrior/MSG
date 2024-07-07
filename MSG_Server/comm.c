#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "comm.h"

void addClient(struct client *c, struct client **clientList) {
    struct client **p = clientList;
    while (*p != NULL) {
        p = &((*p)->next);
    }
    *p = c;
}

void removeClient(struct client *c, struct client **clientList) {
    if (*clientList == c) {
        *clientList = c->next;
        close(c->sockfd);
        free(c);
        c = NULL;
        return;
    }
    struct client **ptr = clientList;
    while ((*ptr)->next != c) {
        ptr = &((*ptr)->next);
    }
    if (c->next == NULL) {
        (*ptr)->next = NULL;
    } else {
        (*ptr)->next = c->next;
    }
    close(c->sockfd);
    free(c);
    c = NULL;
}

struct client* establishConnections(int s_sock, struct client **clientList) {
    struct sockaddr_in c_addr;
    int addrSize = sizeof c_addr;
    int c_sock = accept(s_sock, (struct sockaddr *) &c_addr, &addrSize);
    
    if (c_sock != -1) {
        struct client c = { .sockfd = c_sock, .admin = 0, .next = NULL };
        strcpy(c.addr, inet_ntoa(c_addr.sin_addr));

        struct client *c_ptr = malloc(sizeof (struct client));
        if (c_ptr == NULL) return NULL;
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

void freeConnections(struct client *clientList) {
    while (clientList != NULL) {
        struct client *c;
        c = clientList;
        clientList = clientList->next;
        close(c->sockfd);
        free(c);
        c = NULL;
    }
}

int recieveMessage(struct client *c, char *buff, int buff_len) {
    if (c == NULL) {
        return -1;
    }
    struct pollfd recvPoll = { .fd = c->sockfd, .events = POLLIN };
    if (poll(&recvPoll, 1, 100)) {
        if (c->sockfd == 0) { //stdin
            int bytesRead = read(c->sockfd, buff, buff_len);
            if (bytesRead != -1) {
                // \n marks the end of the input.
                // If buffer is full and there is no \n, the stdin still has characters
                // while loop flushes stdin
                if (bytesRead == buff_len && buff[bytesRead-1] != '\n') {
                    while (fgetc(stdin) != '\n');
                }

                buff[bytesRead-1] = '\0';
                return 1;
            }
        } else {
            return recv(c->sockfd, buff, buff_len, 0);
        }
    }
    return -1;
}
