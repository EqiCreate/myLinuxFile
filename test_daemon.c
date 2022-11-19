#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>


int main(int argc, const char *argv[])
{
	pid_t pid;
	int i,fd;
	char *buf = "this is daemon \n";
	pid =fork();
	if(pid <0){
	printf("fork error");return -1;}
	else if(pid == 0)//父进程退出
	{exit(0); }
	else
	{
		/*
		** 1创建会话2更改目录 3 释放权限包括文件描述符
		**/
		setsid();//创建新会话
		chdir ("/tmp");//更改当前父进程所在的目录为/tmp
		umask(0);//更改文件权限掩码,这样不会因为父进程的权限影响,0取反为1,再异或
		for( i =0 ;i < getdtablesize(); i++)//释放父进程给的资源
		{close(i); }
		/**创建守护进程完毕 ***/
		
		if( (fd =open("daemon.log",O_CREAT | O_WRONLY|O_TRUNC ,0600)) < 0){
		perror("open error line");
		return -1;
	}

		while(1)
		{
			write(fd,buf,strlen(buf));
			sleep(5);
		}
		close(fd);
	}
	return 0;
}
