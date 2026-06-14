#ifndef SOCKET_H
#define SOCKET_H


void perr_exit(const char *str);
int32_t Socket(int32_t family, int32_t type, int32_t protocol);
int32_t Bind(int32_t fd, const struct sockaddr *sa, socklen_t sa_len);
int32_t Listen(int32_t fd, int32_t backlog);
int32_t Accept(int32_t fd, struct sockaddr *sa, socklen_t *sa_lenptr);
int32_t Connect(int32_t fd, const struct sockaddr *sa, socklen_t sa_len);

int32_t Read(int32_t fd, void *buf, uint32_t buf_size);
int32_t Write(int32_t fd, const void *buf, uint32_t buf_size);
int32_t Read_one(int32_t fd, char *buf);
int32_t Read_line(int32_t fd, char *buf, uint32_t buf_size);

int32_t Recv(int32_t sockfd, void *buf, uint32_t len, int32_t flags);
int32_t Send(int32_t sockfd, const void *buf, uint32_t len, int32_t flags);
int32_t Recv_one(int32_t fd, char *buf, int32_t flags);
int32_t Recv_line(int32_t fd, char *buf, uint32_t len);

int32_t Close(int32_t fd);

int32_t initTcpSocket(const int32_t port, char *IP);
int32_t initTcpConn(const int32_t port, char *IP);

#endif