#include "head.h"

int epollAddFd(int epfd, int fd)
{
    int ret = 0;
    struct epoll_event events;
    memset(&events, 0, sizeof(events));
    events.events = EPOLLIN;
    events.data.fd = fd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &events);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    return 0;
}
