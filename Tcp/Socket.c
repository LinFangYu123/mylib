#include "Socket.h"
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>

void perr_exit(const char *str)
{
    perror(str);
    exit(1);
}

int32_t Socket(int32_t family, int32_t type, int32_t protocol)
{
    int32_t n;

    do
    {
        n = socket(family, type, protocol);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        perr_exit("socket");
    }
    return n;
}

int32_t Bind(int32_t fd, const struct sockaddr *sa, socklen_t sa_len)
{
    int32_t n;

    do
    {
        n = bind(fd, sa, sa_len);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        perr_exit("bind");
    }
    return n;
}

int32_t Listen(int32_t fd, int32_t backlog)
{
    int32_t n;

    do
    {
        n = listen(fd, backlog);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        perr_exit("listen");
    }
    return n;
}

int32_t Accept(int32_t fd, struct sockaddr *sa, socklen_t *sa_lenptr)
{
    int32_t n;

    do
    {
        n = accept(fd, sa, sa_lenptr);
    } while ((n < 0) && ((errno == ECONNABORTED) || (errno == EINTR)));

    if (n < 0)
    {
        perr_exit("accept");
    }
    return n;
}

int32_t Connect(int32_t fd, const struct sockaddr *sa, socklen_t sa_len)
{
    int32_t n;

    do
    {
        n = connect(fd, sa, sa_len);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        perr_exit("bind");
    }
    return n;
}

int32_t Read(int32_t fd, void *buf, uint32_t buf_size)
{
    int32_t n;

    do
    {
        n = read(fd, buf, buf_size);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        return -1;
    }
    return n;
}

int32_t Write(int32_t fd, const void *buf, uint32_t buf_size)
{
    int32_t n;

    do
    {
        n = write(fd, buf, buf_size);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        return -1;
    }
    return n;
}

int32_t Read_one(int32_t fd, char *buf)
{
    int32_t n;

    n = Read(fd, buf, 1);

    if (n > 0)
    {
        buf[1] = '\0';
    }
    else
    {
        buf[0] = '\0';
    }

    return n;
}

int32_t Read_line(int32_t fd, char *buf, uint32_t buf_size)
{
    int32_t n;
    int32_t i = 0;
    char ch = '\0';

    while ((i < buf_size - 1) && (ch != '\n'))
    {
        n = Read(fd, &ch, 1);
        if (n > 0)
        {
            buf[i] = ch;
            i++;
        }
        else
        {
            ch = '\n';
        }
    }
    buf[i] = '\0';
    return i;
}

int32_t Recv(int32_t sockfd, void *buf, uint32_t len, int32_t flags)
{
    int32_t n;

    do
    {
        n = recv(sockfd, buf, len, flags);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        return -1;
    }
    return n;
}

int32_t Send(int32_t sockfd, const void *buf, uint32_t len, int32_t flags)
{
    int32_t n;

    do
    {
        n = send(sockfd, buf, len, flags);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        return -1;
    }
    return n;
}

int32_t Recv_one(int32_t fd, char *buf, int32_t flags)
{
    int32_t n;
    n = Recv(fd, buf, 1, flags);
    if (n > 0)
    {
        buf[1] = '\0';
    }
    else
    {
        buf[0] = '\0';
    }
    return n;
}

int32_t Recv_line(int32_t fd, char *buf, uint32_t len, int32_t flags)
{
    int32_t n;
    int32_t i = 0;
    char ch = '\0';

    while ((i < len - 1) && (ch != '\n'))
    {
        n = Recv(fd, &ch, 1, flags);
        if (n > 0)
        {
            buf[i] = ch;
            i++;
        }
        else
        {
            ch = '\n';
        }
    }
    buf[i] = '\0';
    return i;
}

int32_t Close(int32_t fd)
{
    int32_t n;

    do
    {
        n = close(fd);
    } while ((-1 == n) && (errno == EINTR));

    if (n == -1)
    {
        return -1;
    }
    return n;
}

int32_t initTcpSocket(const int32_t port, char *IP)
{
    int32_t fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;

    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);

    if (NULL == IP)
    {
        serv.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if (inet_pton(AF_INET, IP, &serv.sin_addr.s_addr) <= 0)
        {
            perr_exit("inet_pton");
        }
    }

    int32_t opt = 1;
    int32_t ret;

    do
    {
        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    } while ((0 != ret) && (errno == EINTR));

    if (0 != ret)
    {
        perr_exit("setsockopt");
    }

    Bind(fd, (struct sockaddr *)&serv, sizeof(serv));
    Listen(fd, 128);

    return fd;
}

int32_t initTcpConn(const int32_t port, char *IP)
{
    int32_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        perr_exit("socket");
    }

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_port = htons(port);
    serv.sin_family = AF_INET;

    if (IP != NULL)
    {
        if (inet_pton(AF_INET, IP, &serv.sin_addr.s_addr) <= 0)
        {
            close(fd);
            perr_exit("inet_pton");
        }
    }
    else
    {
        if (inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr) <= 0)
        {
            close(fd);
            perr_exit("inet_pton");
        }
    }

    socklen_t servlen = sizeof(serv);

    int32_t ret = Connect(fd, (struct sockaddr *)&serv, servlen);
    if (-1 == ret)
    {
        close(fd);
        return -1;
    }

    return fd;
}