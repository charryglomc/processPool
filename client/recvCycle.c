#include "head.h"

int recvCycle(int sockFd, void *buf, int totalSize)
{
    int recvSize = 0;
    int ret = 0;
    while(recvSize < totalSize){
        ret = recv(sockFd, (char *)buf + recvSize, totalSize - recvSize, 0);
        recvSize += ret;
    }

    return recvSize;
}
