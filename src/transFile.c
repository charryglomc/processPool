#define _GNU_SOURCE
#include "head.h"

void sigFunc(int sigNum)
{
    printf("sig is comming\n");
}

int transFile(int clientFd)
{   
    signal(SIGPIPE, sigFunc);
    int ret = 0;
    int fd = open("file", O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    struct stat fileInfo;
    memset(&fileInfo, 0, sizeof(fileInfo));

    //获取当前文件的状态
    fstat(fd, &fileInfo);

    Train_t train;
    memset(&train, 0, sizeof(train));

    train.dataLen = 4;
    strcpy(train.buf, "file");

    //发送文件名字
    send(clientFd, &train, 4 + train.dataLen, 0);

    train.dataLen = sizeof(fileInfo.st_size);
    memcpy(train.buf, &fileInfo.st_size, sizeof(off_t));

    send(clientFd, &train, 4 + train.dataLen, 0);
    printf("fileSize = %ld\n", fileInfo.st_size);

    /* while(1){ */
    /*     train.dataLen = read(fd, train.buf, sizeof(train.buf)); */

    /*     //发送文件内容 */
    /*     ret = send(clientFd, &train, 4 + train.dataLen, 0); */
    /*     if(-1 == ret){ */
    /*         return -1; */
    /*     } */

    /*     if(0 == train.dataLen){ */
    /*         break; */
    /*     } */
    /* } */

    //零拷贝1
    /* char *pMap = (char*)mmap(NULL, fileInfo.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); */
    /* ERROR_CHECK(pMap, (char*)-1, "mmap"); */

    /* send(clientFd, pMap, fileInfo.st_size, 0); */

    /* munmap(pMap, fileInfo.st_size); */

    //零拷贝2
   /* sendfile(clientFd, fd, 0, fileInfo.st_size); */

    //零拷贝3
    
    int fds[2];
    pipe(fds);

    int recvLen = 0;
    while(recvLen < fileInfo.st_size){
        ret = splice(fd, 0, fds[1], 0, 128, 0);
        ret = splice(fds[0], 0, clientFd, 0, ret, 0);
        recvLen += ret;
    }

    return 0;
}
