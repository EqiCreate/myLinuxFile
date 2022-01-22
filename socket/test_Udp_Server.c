#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define N 128
#define errlog(errmsg) do{perror(errmsg); \
        printf("%s--%s--%d--\n",__FILE__,__FUNCTION__,__LINE__); \
        return -1;} while(0)
int main(int argc,const char *argv[]){

	int sockfd;
	struct sockaddr_in serveraddr,clientaddr;
	socklen_t addrlen = sizeof(serveraddr);
	char buf[N] ="";
	
	//
	if (argc < 3){
		fprintf(stderr,"usage :%s ip port \n",argv[0]);return -1;	
	}
	if ((sockfd =socket (AF_INET,SOCK_DGRAM,0)) < 0){
		errlog("socket error"); 
	}
	//
	serveraddr.sin_family =AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port =htons(atoi(argv[2]));
	
	if (bind(sockfd, (struct sockaddr *)&serveraddr,addrlen )< 0 ){
		errlog("bind error");
	}
	
	ssize_t bytes;
	while (1){
		if((bytes = recvfrom(sockfd,buf,N,0,
		(struct sockaddr *)&clientaddr,&addrlen)) < 0){
			errlog("recvfrom error");
		 }
		else{
			printf("ip :%s,port :%d\n",inet_ntoa(clientaddr.sin_addr),

				ntohs(clientaddr.sin_port));
			if(strncmp (buf,"quit",4) ==0 ){
				printf("client quit");break;
			 }
			else{
				printf("client :%s\n",buf);
				strcat(buf,"-server");
				if(sendto(sockfd,buf,N,0,
					(struct sockaddr*)&clientaddr,addrlen) < 0){
					errlog("sendto error ");
				}
			}
		}
	}
	close(sockfd);
	return 0;
}
