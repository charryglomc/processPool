#include "head.h"

int recvCycle(int sockFd, void *buf, int totalSize);

int main(int argc, char **argv)
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    //保存本机的ip和端口
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(atoi(argv[2]));
    
    //连接服务器
    int ret = 0;
    ret = connect(sfd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    ERROR_CHECK(ret, -1, "connect");

    int rcvbuf = 65536;
    int len = sizeof(rcvbuf);
    setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, len);

    char buf[1000] = {0};
    //接收文件名
        
    //先接收长度
    int dataLen = 0;

    off_t fileSize = 0;

    off_t recvLen = 0;

    float rate = 0;


    recv(sfd, &dataLen, sizeof(int), 0);

    //根据长度来接收数据
    recv(sfd, buf, dataLen, 0);


    int fd = open(buf, O_CREAT|O_RDWR, 0666);
    ERROR_CHECK(fd, -1, "open");

    //接收文件总长度
    recv(sfd, &dataLen, 4, 0);
    recv(sfd, &fileSize, dataLen, 0);
    printf("fileSize = %ld\n", fileSize);

    struct timeval beg, end;
    memset(&beg, 0, sizeof(beg));
    memset(&end, 0, sizeof(end));
    gettimeofday(&beg, NULL);

    //接收文件内容
    /* while(1){ */
    /*     recv(sfd, &dataLen, sizeof(int), 0); */
    /*     /1* if(1000 != dataLen){ *1/ */
    /*     /1*     printf("dataLen = %d\n", dataLen); *1/ */
    /*     /1* } *1/ */
    /*     if(0 == dataLen){ */
    /*         break; */
    /*     } */
    /*     //让recv完整接收datalen长度的数据 */
    /*     /1* ret = recv(sfd, buf, dataLen, MSG_WAITALL); *1/ */
    /*     ret = recvCycle(sfd, buf, dataLen); */
    /*     recvLen += ret; */
    /*     rate = (float)recvLen / fileSize * 100; */
    /*     /1* printf("rate = %5.2f%%\r", rate); *1/ */
    /*     /1* fflush(stdout); *1/ */

    /*     write(fd, buf, ret); */
    /* } */


    //获取传输文件前的时间
    //给文件开辟空间
    ftruncate(fd, fileSize);

    char *pMap = (char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(pMap, (char*)-1, "mmap");

    recvCycle(sfd, pMap, fileSize);

    munmap(pMap, fileSize);
    
    gettimeofday(&end, NULL);
    printf("cost time = %ld\n", (end.tv_sec - beg.tv_sec)*1000000  + (end.tv_usec - beg.tv_usec));

    return 0;
}
