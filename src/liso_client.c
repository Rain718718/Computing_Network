#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define ECHO_PORT 9999
#define BUF_SIZE 8192
// #define DEBUG

int main(int argc, char *argv[])
{
#ifdef DEBUG
    printf("----- Echo Client -----\n");
#endif
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <server-ip> <port>", argv[0]);
        return EXIT_FAILURE;
    }

    char buf[BUF_SIZE];

    int status, sock;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    struct addrinfo *servinfo;       // will point to the results
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s \n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
    {
        fprintf(stderr, "Socket failed");
        return EXIT_FAILURE;
    }

    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        fprintf(stderr, "Connect");
        return EXIT_FAILURE;
    }

    char msg[BUF_SIZE];
    // fgets(msg, BUF_SIZE, stdin);

    // 从文件中读取到msg中
    // char filename[BUF_SIZE] = "request_pipeline1";
    char filename[BUF_SIZE];
    fgets(filename, BUF_SIZE, stdin);
    char path[BUF_SIZE] = "samples/";
    char temp[BUF_SIZE];
    strcat(path, filename);
    strncpy(temp, path, strlen(path) - 1);
    strcpy(path, temp);

    FILE *fp;
    if ((fp = fopen(path, "r")) == NULL)
    {
        fprintf(stderr, "Open file failed");
        return EXIT_FAILURE;
    }

    char line[BUF_SIZE];
    while (fgets(line, BUF_SIZE, fp) != NULL)
    {
        strcat(msg, line);
    }
    fclose(fp);
    int bytes_received;
    fprintf(stdout, "Sending %s", msg);
    send(sock, msg, strlen(msg), 0);

    int count = 0;
    while (1)
    {
        if ((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
        {
            buf[bytes_received] = '\0';
            fprintf(stdout, "Received %s", buf);
            ++count;
#ifdef DEBUG
            printf("[count\t]\t%d\n\n", count);
#endif
        }
        else
        {
            break;
        }
    }

    freeaddrinfo(servinfo);
    close(sock);
    return EXIT_SUCCESS;
}
