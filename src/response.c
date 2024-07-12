#include "response.h"

#ifndef RESPONSE_C
#define RESPONSE_C 0
#define BUF_SIZE 81920
// #define DEBUG
// MIME类型映射表
char *MIME_TYPES[][2] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".gif", "image/gif"},
    {NULL, NULL}};
// 响应消息
char *RESPONSE_400 = "HTTP/1.1 400 Bad request\r\n\r\n";
char *RESPONSE_404 = "HTTP/1.1 404 Not Found\r\n\r\n";
char *RESPONSE_501 = "HTTP/1.1 501 Not Implemented\r\n\r\n";
char *RESPONSE_505 = "HTTP/1.1 505 HTTP Version not supported\r\n\r\n";

// 读取静态文件
char *BASE_DIR = ".";
char *STATIC_DIR = "/static_site";
#endif

// 处理请求
int handle_request(int client_socket, char *request_msg, struct sockaddr_in cli_addr, int msg_len)
{
    // 解析请求
    Request *request = parse(request_msg, msg_len, 0);
    // 如果解析失败，则返回400错误响应消息
    if (request == NULL)
    {
        char *response = "HTTP/1.1 400 Bad request\r\n\r\n";
        if (send(client_socket, response, strlen(response), 0) < 0)
        {
            fprintf(stderr, "send error1\n"); // add
            return 0;
        }
        return 1;
    }
    // 消息中的方法、路径和协议版本号
    char method[BUF_SIZE], path[BUF_SIZE], protocol[BUF_SIZE];
    strcpy(method, request->http_method);
    strcpy(path, request->http_uri);
    strcpy(protocol, request->http_version);
#ifdef DEBUG
    printf("[request]\tmethod: %s, path: %s, protocol: %s\n", method, path, protocol);
#endif
    // 如果协议版本号不是HTTP/1.1，则返回505错误响应消息
    if (strcmp(protocol, "HTTP/1.1") != 0)
    {
        char *response = "HTTP/1.1 505 HTTP Version not supported\r\n\r\n";
        if (send(client_socket, response, strlen(response), 0) < 0)
        {
            fprintf(stderr, "send error2\n"); // add
            return 0;
        }
        return 1;
    }
    // 如果方法不为GET、HEAD或POST，则返回501错误响应消息
    else if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0 && strcmp(method, "POST") != 0)
    {
        char *response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
        if (send(client_socket, response, strlen(response), 0) < 0)
        {
            fprintf(stderr, "send error3\n"); // add
            return 0;
        }
        return 1;
    }
    // 方法为GET、HEAD或POST，处理请求
    else
    {
        char *response;
        // 处理POST请求
        if (strcmp(method, "POST") == 0)
        {
            response = request_msg;
            AccessLog("POST", cli_addr, 1);
        }
        // 处理GET请求
        else if (strcmp(method, "GET") == 0)
        {
            response = handle_get_request(path, cli_addr);
            AccessLog("GET", cli_addr, 1);
        }
        // 处理HEAD请求
        else if (strcmp(method, "HEAD") == 0)
        {
            response = handle_head_request(path, cli_addr);
            AccessLog("HEAD", cli_addr, 1);
        }
        // 发回响应消息
        if (send(client_socket, response, strlen(response), 0) < 0)
        {
            fprintf(stderr, "send error4\n"); // add
            return 0;
        }
        return 1;
    }
}

// 处理get请求
char *handle_get_request(char *uri, struct sockaddr_in cli_addr)
{
    char *response_buffer;
    int buffer_len;
    int file_fd;
    struct stat file_stat;
    char file_path[BUF_SIZE], *mime_type;

    // 构造请求的文件路径
    sprintf(file_path, "%s%s%s", BASE_DIR, STATIC_DIR, uri);
    if (file_path[strlen(file_path) - 1] == '/')
    {
        strcat(file_path, "index.html");
    }
    // 打开文件，获取文件大小和文件类型
    file_fd = open(file_path, O_RDONLY);
    // 如果文件不存在，则返回404错误响应消息
    if (file_fd == -1)
    {
        ErrorLog("Failed to open file", cli_addr, 0);
        response_buffer = RESPONSE_404;
        return response_buffer;
    }
    // 如果文件状态获取失败，则返回404错误响应消息
    if (fstat(file_fd, &file_stat) == -1)
    {
        ErrorLog("Failed to get file stat", cli_addr, 0);
        response_buffer = RESPONSE_404;
        close(file_fd);
        return response_buffer;
    }
    // 判断文件类型
    for (int i = 0; MIME_TYPES[i][0] != NULL; i++)
    {
        if (strstr(file_path, MIME_TYPES[i][0]))
        {
            mime_type = MIME_TYPES[i][1];
            break;
        }
    }

    // 构造HTTP响应消息
    char buffer[BUF_SIZE]; // add
    buffer_len = sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %lu\r\n\r\n", mime_type, file_stat.st_size);
    response_buffer = malloc(buffer_len + file_stat.st_size);
    // 如果内存分配失败，则返回404错误响应消息
    if (response_buffer == NULL)
    {
        ErrorLog("Failed to allocate memory for response buffer", cli_addr, 0);
        response_buffer = RESPONSE_404;
        close(file_fd);
        return response_buffer;
    }
    memcpy(response_buffer, buffer, buffer_len);
    // 如果文件读取失败，则返回404错误响应消息
    if (read(file_fd, response_buffer + buffer_len, file_stat.st_size) != file_stat.st_size)
    {
        ErrorLog("Failed to read file", cli_addr, 0);
        response_buffer = RESPONSE_404;
        close(file_fd);
        return response_buffer;
    }
    close(file_fd);

    return response_buffer;
}

// 处理head请求
char *handle_head_request(char *uri, struct sockaddr_in cli_addr)
{
    char *response_buffer;
    int buffer_len;
    int file_fd;
    struct stat file_stat;
    char file_path[BUF_SIZE], *mime_type;

    // 构造请求的文件路径
    sprintf(file_path, "%s%s%s", BASE_DIR, STATIC_DIR, uri);
    if (file_path[strlen(file_path) - 1] == '/')
    {
        strcat(file_path, "index.html");
    }
    // 打开文件，获取文件大小和文件类型
    file_fd = open(file_path, O_RDONLY);
    // 如果文件不存在，则返回404错误响应消息
    if (file_fd == -1)
    {
        ErrorLog("Failed to open file", cli_addr, 0);
        response_buffer = RESPONSE_404;
        return response_buffer;
    }
    // 如果文件状态获取失败，则返回404错误响应消息
    if (fstat(file_fd, &file_stat) == -1)
    {
        ErrorLog("Failed to get file stat", cli_addr, 0);
        response_buffer = RESPONSE_404;
        close(file_fd);
        return response_buffer;
    }
    // 判断文件类型
    for (int i = 0; MIME_TYPES[i][0] != NULL; i++)
    {
        if (strstr(file_path, MIME_TYPES[i][0]))
        {
            mime_type = MIME_TYPES[i][1];
            break;
        }
    }

    // 构造HTTP响应消息
    char buffer[BUF_SIZE]; // add
    buffer_len = sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %lu\r\n\r\n", mime_type, file_stat.st_size);
    response_buffer = malloc(buffer_len + file_stat.st_size);
    // 如果内存分配失败，则返回404错误响应消息
    if (response_buffer == NULL)
    {
        ErrorLog("Failed to allocate memory for response buffer", cli_addr, 0);
        response_buffer = RESPONSE_404;
        close(file_fd);
        return response_buffer;
    }
    memcpy(response_buffer, buffer, buffer_len);
    return response_buffer;
}

// 处理post请求
char *handle_post_request(char *path, struct sockaddr_in cli_addr)
{
    return NULL;
}