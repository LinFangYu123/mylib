#include "Socket.h"
#include <stdio.h>
#include <string.h>

int main()
{
    int32_t sfd = initTcpConn(8888, NULL);

    while (1)
    {
        char buf[1024];
        memset(buf, 0, 1024);
        fgets(buf, 1024, stdin);
        int32_t buf_size = Write(sfd, buf, 7);
    }

    return 0;
}