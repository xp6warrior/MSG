#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "server.h"

void kickCmd(char *cmd, void *clientList, char *record, int sizeRecord) {
    if (*(cmd += 5) == ' ') {
        struct client *ptr = clientList;
        while (ptr != NULL) {
            if (strstr(cmd, ptr->addr) != NULL) {
                snprintf(record, sizeRecord, "%s has been kicked\n", ptr->addr);
                removeClient(ptr, (struct client **) &clientList);
                return;
            }
                
            ptr = ptr->next;
        }
    }
}