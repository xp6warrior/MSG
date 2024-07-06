/*
    Client linked list (contains socket and ip address)
    @param sockfd file descriptor of socket
    @param addr IP address
    @param admin has access to commands
    @param *next ptr to next item in list
*/
struct client {
    int sockfd;
    char addr[46];
    char admin;
    struct client *next;
};

/* 
    Adds client to linked list
    @param *c ptr to client struct
    @param **clientList ptr to client linked list
*/
void addClient(struct client *c, struct client **clientList);
/*
    Removes (and frees) client from linked list
    @param *c ptr to client struct
    @param **clientList ptr to client linked list
    @return -1 if fail
*/
int removeClient(struct client *c, struct client **clientList);
/*
    Accepts incoming connect requests, adds client data to linked list
    @param s_sock server socket file descriptor
    @param **clientList ptr to client linked list
    @return A ptr to client struct containing connected client, NULL if no connections were established
*/
struct client* establishConnections(int s_sock, struct client **clientList);
/* Frees allocated memory for client linked list*/
void freeConnections();
/*
    Recieves any incoming data from client, places it in buffer
    @param *c ptr to client struct
    @param *buff ptr to buffer
    @param buff_len size of buffer
    @return Recieve status (-1 if fail)
*/
int recieveMessage(struct client *c, char *buff, int buff_len);
/*
    Fetches private IP address of local machine, places it in buffer
    @param *buff ptr to buffer
    @param buff_len size of buffer
    @return -1 if fail
*/
int getDefaultIP(char *buff, int buff_len);