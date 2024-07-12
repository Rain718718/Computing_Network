#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

// 错误日志
void ErrorLog(char *, struct sockaddr_in, int);
// 访问日志
void AccessLog(char *, struct sockaddr_in, int);