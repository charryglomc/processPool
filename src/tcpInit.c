#include "head.h"

int tcpInit(char *ip, char *port, int *sockFd)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    //保存本机的ip和端口
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(atoi(port));

    int reuse = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    int buf = 65536;
    setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &buf, sizeof(buf));

    //绑定本机的IP和端口，绑定到sfd上
    int ret = 0;
    ret = bind(sfd, (struct sockaddr*)&serAddr, sizeof(serAddr));

    //监听的最大连接数是10
    ret = listen(sfd, 10);
    ERROR_CHECK(ret, -1, "listen");

    *sockFd = sfd;
    return 0;
}
