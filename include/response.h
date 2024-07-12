#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/socket.h>
#include "parse.h"
#include "logger.h"

// 处理请求
int handle_request(int, char *, struct sockaddr_in, int);

// 处理get请求
char *handle_get_request(char *, struct sockaddr_in);

// 处理post请求
char *handle_post_request(char *, struct sockaddr_in);

// 处理head请求
char *handle_head_request(char *, struct sockaddr_in);