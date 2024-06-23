struct client {
    int sockfd;
    char addr[INET6_ADDRSTRLEN];
    struct client *next;
};

void disconnectClient(struct client *c, struct client **clientList);

struct client* establishConnections(int s_sock, struct client **clientList);

int recieveMessages(struct client *c, char *buffer, int buff_len);

void getDefaultIP(char *ip, int size);