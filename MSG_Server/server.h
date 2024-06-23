struct client {
    int sockfd;
    char addr[INET6_ADDRSTRLEN];
    struct client *next;
};


void disconnectClient(struct client *c, struct client **clientList);

void establishConnections(int s_sock, struct client **clientList);

void getDefaultIP(char *ip, int size);