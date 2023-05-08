#include "head.h"

int exitPipe[2];
void sigFunc1(int sigNum)
{
    printf("sig is comming\n");

    //异步拉起同步
    write(exitPipe[1], &sigNum, 4);
}

int main(int argc, char **argv)
{
    int processNum = atoi(argv[3]);

    pProcess_data_t pData = (pProcess_data_t)calloc(processNum, sizeof(process_data_t));
    //1.创建子进程
    makeChild(pData, processNum);
    signal(SIGUSR1, sigFunc1);
    pipe(exitPipe);

    //创建tcp监听套接字
    int sfd = 0;
    tcpInit(argv[1], argv[2], &sfd);

    //创建epoll,把需要监听的描述符加入到epoll中，
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    epollAddFd(epfd, sfd);
    epollAddFd(epfd, exitPipe[0]);

    for(int i = 0; i < processNum; ++i){
        epollAddFd(epfd, pData[i].pipeFd);
    }

    struct epoll_event evs[2];
    memset(evs, 0, sizeof(evs));

    int newFd = 0;

    int readyNum = 0;
    while(1){
        readyNum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < readyNum; ++i){
            if(evs[i].data.fd == sfd){
                newFd = accept(sfd, NULL, NULL);
                for(int j = 0; j < processNum; ++j){
                    if(0 == pData[j].flag){
                        sendFd(pData[j].pipeFd, newFd, 0);
                        pData[j].flag = 1;
                        printf("child %d is busy\n", pData[j].pid);
                        break;
                    }
                }
                close(newFd);
            }

            else if(evs[i].data.fd == exitPipe[0]){
                //第一种退出方式，比较暴力
                /* for(int j = 0; j < processNum; ++j){ */
                /*     kill(pData[j].pid, SIGUSR1); */
                /* } */

                //第二种退出方式
                for(int j = 0; j < processNum; ++j){
                    //发送退出标记，标记为1表示退出，0，表示不退出
                    sendFd(pData[j].pipeFd, 0, 1);
                }

                for(int j = 0; j < processNum; ++j){
                   wait(NULL);
                }
                printf("main process exit\n");
                exit(0);
            }
            else{
                for(int j = 0; j < processNum; ++j){
                    if(evs[i].data.fd == pData[j].pipeFd){
                        char buf[2] = {0};
                        //读管道内容，否则管道一直就绪
                        read(pData[j].pipeFd, buf, sizeof(buf));

                        printf("child %d is not busy\n", pData[j].pid);

                        pData[j].flag = 0;
                        break;
                    }
                }
            }

            //查找就绪的管道属于哪个进程
        }
    }

    return 0;
}

