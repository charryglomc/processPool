#include "head.h"

int recvFd(int pipeFd, int *fd, int *exitFlag)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));

    struct iovec iov;
    memset(&iov, 0, sizeof(iov));

    iov.iov_base = exitFlag;
    iov.iov_len = 4;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    size_t len = CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg = (struct cmsghdr *)calloc(1, len);
    cmsg->cmsg_len = len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

   msg.msg_control = cmsg;
   msg.msg_controllen = len;

   recvmsg(pipeFd, &msg, 0);
   *fd = *(int*)CMSG_DATA(cmsg);

   return 0;
}

