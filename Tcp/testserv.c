#include "Socket.h"
#include <string.h>
int main(){
	int fd;
	fd = initTcpSocket(8888,NULL);
	struct sockaddr_in clie;
	socklen_t cli_len = sizeof(clie);
	int cfd = Accept(fd,(struct sockaddr *)&clie,&cli_len);
	while(1){
		int size;
		//char buf[1024];
		char ch;
		memset(&ch,0,1);
		size = Read_one(cfd,&ch);
		if(size > 0) printf("%c",ch);
	}
}
