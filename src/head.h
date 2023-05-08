#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#define ARGS_CHECK(argc, val) \
    if(argc != val) { \
        printf("error args\n"); \
        return -1; \
    } 


#define ERROR_CHECK(ret, retVal, funcName) { \
    if(ret == retVal) { \
        perror(funcName); \
        return -1; \
    } \
}

#define THREAD_ERROR_CHECK(ret, funcName) \
    do { \
        if (0 != ret) { \
            printf("%s : %s\n", funcName, strerror(ret)); \
        } \
    }while(0)

//保存子进程相关信息
typedef struct data{
    int flag;
    pid_t pid;
    int pipeFd;
}process_data_t, *pProcess_data_t;

//私有协议-小火车
typedef struct{
    int dataLen;
    char buf[1000];
}Train_t;

int makeChild(pProcess_data_t pData, int processNum);
int tcpInit(char *ip, char *port, int *sockFd);
int epollAddFd(int epfd, int fd);
int sendFd(int pipeFd, int fd, int exitFlag);
int recvFd(int pipeFd, int *fd, int *exitFlag);
int transFile(int clientFd);
int childFunc(int pipeFd);

