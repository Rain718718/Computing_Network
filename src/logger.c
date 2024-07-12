#include "logger.h"

// 错误日志
void ErrorLog(char *msg, struct sockaddr_in cli_addr, int fd)
{
    // 创建日志文件
    FILE *fp = fopen("error.log", "a+");
    if (fp == NULL)
    {
        printf("Open log file failed");
        return;
    }
    // 获取当前时间
    time_t now;
    struct tm *timenow;
    time(&now); // 将当前时间存储在now变量中
    timenow = localtime(&now);

    // 获取当前时间的字符串表示形式
    char *time = asctime(timenow);
    // 去除字符串末尾的换行符。
    time[strlen(time) - 1] = '\0';
    fprintf(fp, "[%s] %s %s:%d %d\n", time, msg, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd);

    fclose(fp);
}

// 访问日志
void AccessLog(char *msg, struct sockaddr_in cli_addr, int fd)
{
    // 创建日志文件
    FILE *fp = fopen("error.log", "a+");
    if (fp == NULL)
    {
        printf("Open log file failed");
        return;
    }
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    char *time = asctime(timenow);
    time[strlen(time) - 1] = '\0';
    fprintf(fp, "[%s] %s %s:%d %d\n", time, msg, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd);
    fclose(fp);
}