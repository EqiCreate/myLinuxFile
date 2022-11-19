#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define N 128
#define errlog(errmsg) do{perror(errmsg); \
        printf("%s--%s--%d--\n",__FILE__,__FUNCTION__,__LINE__); \
        return -1;} while(0)
int main(int argc,const char *argv[]){

	int sockfd,aceptfd;
	struct sockaddr_in serveraddr;
	socklen_t addrlen = sizeof(serveraddr);
	char buf[N] ="";
	
	//
	if (argc < 3){
		fprintf(stderr,"usage :%s ip port \n",argv[0]);return -1;	
	}
	if ((sockfd =socket (AF_INET,SOCK_STREAM,0)) < 0){
		errlog("socket error"); 
	}
	//
	serveraddr.sin_family =AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port =htons(atoi(argv[2]));
#if 0	
	struct sockaddr_in clientaddr;
        clientaddr.sin_family =AF_INET;
	clientaddr.sin_addr.s_addr = inet_addr(argv[3]);
	clientaddr.sin_port =htons(atoi(argv[4]));

	if (bind(sockfd, (struct sockaddr *)&clientaddr,addrlen )< 0 ){
		errlog("bind error");
	}
	
#endif	
	if(connect (sockfd,(struct sockaddr *)&serveraddr,addrlen) < 0){
		errlog("connect error");
	}
	printf("host info :ip:%s port:%d\n ",inet_ntoa(serveraddr.sin_addr),ntohs(serveraddr.sin_port));	
	
	while (1){
		fgets(buf,N,stdin);
		buf[strlen(buf)-1] = '\0';
		if(send (sockfd,buf,N,0)< 0){errlog("send error");}
		else{
			if(strncmp (buf,"quit",4) ==0 ){
				printf("client quit\n"); goto err;
			 }
			if(recv(sockfd,buf,N,0) <0 ){errlog("recv error");}
			printf ("server :%s \n",buf);
		    }
		}

	return 0;
err:
	close(sockfd);
	return 0;
}
