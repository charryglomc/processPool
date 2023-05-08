#include "head.h"

int sendFd(int pipeFd, int fd, int exitFlag)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));

    struct iovec iov;
    memset(&iov, 0, sizeof(iov));

    iov.iov_base = &exitFlag;
    iov.iov_len = 4;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    size_t len = CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg = (struct cmsghdr *)calloc(1, len);
    cmsg->cmsg_len = len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

   *(int*)CMSG_DATA(cmsg) = fd;

   msg.msg_control = cmsg;
   msg.msg_controllen = len;

   sendmsg(pipeFd, &msg, 0);

   return 0;
}

