#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "response.h"
#include <sys/select.h>

#define ECHO_PORT 9999
#define BUF_SIZE 81920
#define MAX_CLIENTS 1024
// #define DEBUG

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int sock;
    ssize_t readret;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    fprintf(stdout, "----- Echo Server -----\n");

    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }

    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    // 初始化客户端socket集合
    fd_set client_fds;
    FD_ZERO(&client_fds);
    FD_SET(sock, &client_fds);
    int max_fd = sock;
    int client_sockets[MAX_CLIENTS] = {0};

    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        // 客户端socket集合
        fd_set read_fds = client_fds;

        // 使用select()监听所有客户端socket
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            fprintf(stderr, "Error in select()\n");
            exit(EXIT_FAILURE);
        }

        // 检查监听socket是否有连接请求
        if (FD_ISSET(sock, &read_fds))
        {
            struct sockaddr_in client_addr = {0};
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_fd == -1)
            {
                fprintf(stderr, "Error accepting connection\n");
                continue;
            }
            printf("Accepted connection from %s:%d (fd=%d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

            // 将新的客户端套接字加入集合
            int i;
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = client_fd;
                    FD_SET(client_fd, &client_fds);
                    if (client_fd > max_fd)
                    {
                        max_fd = client_fd;
                    }
                    break;
                }
            }
            if (i == MAX_CLIENTS)
            {
                fprintf(stderr, "Too many clients\n");
                close(client_fd);
            }
        }
        // 检查客户端socket是否有数据可读
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int client_fd = client_sockets[i];
            if (client_fd > 0 && FD_ISSET(client_fd, &read_fds))
            {
                // 读取数据
                if ((readret = recv(client_fd, buf, BUF_SIZE, 0)) > 0)
                {
                    char *subs;
                    char subbuf[BUF_SIZE]; // 单条请求
                    int count = 0;
                    while (1)
                    {
                        if ((subs = strstr(buf, "\r\n\r\n")) == NULL)
                        {
                            if (count == 0)
                            {
                                strcpy(subbuf, buf);
                                memset(buf, 0, BUF_SIZE);
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            strncpy(subbuf, buf, subs - buf + 4);
                            strcpy(buf, subs + 4);
                        }
                        // 处理请求
                        int re = handle_request(client_fd, subbuf, cli_addr, strlen(subbuf));
                        ++count;
#ifdef DEBUG
                        printf("[buf\t]\t%ld\r\n", strlen(buf));
                        printf("[count\t]\t%d\n", count);
#endif
                        if (re == 0)
                        {
                            close_socket(client_fd);
                            FD_CLR(client_fd, &client_fds);
                            client_sockets[i] = 0;
                        }
                    }
                    memset(buf, 0, BUF_SIZE);
                }
                if (readret == -1)
                {
                    fprintf(stderr, "Error reading from socket\n");
                    close(client_fd);
                    FD_CLR(client_fd, &client_fds);
                    client_sockets[i] = 0;
                }
                // 客户端关闭连接
                // else if (readret == 0) {
                printf("Connection closed by client (fd=%d)\n", client_fd);
                close(client_fd);
                FD_CLR(client_fd, &client_fds);
                client_sockets[i] = 0;
                // }
            }
        }
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}
