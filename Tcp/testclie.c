#include "Socket.h"
#include <stdio.h>
#include <string.h>

int main(){
	int sfd = initTcpConn(8888,NULL);
	while(1){
		char buf[1024];
		memset(buf,0,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);
		int buf_size = Write(sfd,buf,7);
	}
	return 0;
}
