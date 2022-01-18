#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>

#define N 128
#define errlog(errmsg) do{perror(errmsg); \
	printf("%s--%s--%d--\n",__FILE__,__FUNCTION__,__LINE__); \
	return -1;} while(0)

int main(int argc ,const char *argv[])
{
	pid_t pid;
	int fdr,fdw;
	ssize_t nbyte;
	int fd[2];
	char buf[N] ="";
	if((fdr =open("test.txt",O_RDONLY)) <0 )
	{ errlog("open test.txt error");}
	if ((fdw =open("read.txt",O_CREAT | O_WRONLY|O_TRUNC,0664)) < 0)
	{errlog("open readtxt error"); }

	if(pipe(fd) < 0)
	{errlog("pipe error"); }
	pid =fork();
	
	if(pid <0 )
	{ errlog("fork error");}
	else if (pid == 0)
	{
		while((nbyte = read(fd[0],buf,N)) > 0)
		{write(fdw,buf,nbyte); }
	 }
	else
	{
		while((nbyte = read(fdr,buf,N)) > 0)
		{write(fd[1],buf,nbyte); }
	}


	return 0;
}
