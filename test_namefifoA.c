#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#define N 128
#define errlog(errmsg)do{perror(errmsg); \
	printf("--%s--%s--%d-- \n",__FILE__,__FUNCTION__,__LINE__); \
	return -1;}while(0)

int main(int argc,const char *argv[])
{	
	int fd;
	char buf[N] ="";
	if(mkfifo("fifo",0664) <0 )
	{
		if(errno == EEXIST)
		{
			fd =open("fifo",O_RDWR);
			printf("fd = %d\n",fd);
		}
		else 
		{errlog("mkfifo error"); } 
	}
	else
	{
		fd =open("fifo",O_RDWR);
		printf("fd = %d\n",fd);
	}	
	
	while (1){
		fgets(buf,N,stdin);
		buf[strlen(buf)-1] = '\0';
		write( fd, buf, strlen(buf));
		if(strncmp (buf , "quit",4) == 0)
		{break; }
	}
	return 0;
}
