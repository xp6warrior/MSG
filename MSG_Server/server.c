#include <stdlib.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

int getDefaultIP(char *buff, int buff_len) {
    struct ifaddrs *addrs, *ptr;
    if (getifaddrs(&addrs) == -1) {
        return -1;
    }

    for (ptr = addrs; ptr != NULL; ptr = ptr->ifa_next) {
        if (ptr->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) ptr->ifa_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), buff, buff_len);
            if (buff != NULL) {
                break;
            }
        }
    }
    freeifaddrs(addrs);
    return 0;
}