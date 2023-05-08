#include "head.h"

int makeChild(pProcess_data_t pData, int processNum)
{
    int fds[2];
    pid_t pid;
    for(int i = 0; i < processNum; ++i){
        //创建一对描述符，用于父子进程传输数据
       socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
       pid = fork();
       if(0 == pid){
            close(fds[1]);
            childFunc(fds[0]);
            exit(0);
       }
       close(fds[0]);
       pData[i].pid = pid;
       pData[i].pipeFd = fds[1];
       //如果flag为0，表示子进程非忙碌，1忙碌。
       pData[i].flag = 0;
    }
    return 0;
}

int childFunc(int pipeFd)
{
    int clientFd = 0;
    int exitFlag = 0;
    while(1){
        recvFd(pipeFd, &clientFd, &exitFlag);
        if(1 == exitFlag){
            printf("child exit\n");
            exit(0);
        }

        transFile(clientFd);
        //通知父进程，我已工作完成
        write(pipeFd, "a", 1);

        close(clientFd);
    }
    return 0;
}
