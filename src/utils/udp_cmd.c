#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 8192

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    int                len = sizeof(struct sockaddr_in);
    char               buf[BUF_SIZE];
    struct hostent *   host;
    int                n, s, port;
    struct timeval     tv;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <host> <port> <message>\n", argv[0]);
        return 1;
    }

    host = gethostbyname(argv[1]);
    if (host == NULL) {
        perror("gethostbyname");
        return 1;
    }

    port = atoi(argv[2]);

    /* initialize socket */
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1;
    }

    /* initialize server addr */
    memset((char *)&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr   = *((struct in_addr *)host->h_addr);

    tv.tv_sec  = 0;
    tv.tv_usec = 500000;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt");
    }

    /* send message */
    if (sendto(s, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&server, len) == -1) {
        perror("sendto()");
        return 1;
    }

    while ((n = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *)&server, &len)) != -1) {
        if (n >= 3 && !strncasecmp(buf, "END", sizeof("END") - 1)) {
            break;
        }
        printf("%.*s", n, buf);
    }

    close(s);
    return 0;
}
