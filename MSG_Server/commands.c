#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "commands.h"
#include "comm.h"

void kickCmd(char *cmd, void *clientList, char *record, int record_len) {
    if (*(cmd += 5) == ' ') {
        struct client *ptr = clientList;
        while (ptr != NULL) {
            if (strstr(cmd, ptr->addr) != NULL) {
                snprintf(record, record_len, "%s has been kicked\n", ptr->addr);
                send(ptr->sockfd, "You have been kicked from the server!\n", 39, 0);
                removeClient(ptr, (struct client **) &clientList);
                return;
            }
            ptr = ptr->next;
        }
    }
}