#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define errlog(errmsg) do{perror(errmsg); \
	printf("%s--%s--%d--\n",__FILE__,__FUNCTION__,__LINE__); \
	return -1;} while(0)

void handler(int arg){
	if(arg == SIGINT){
		puts("catch the SIGINT");
	}
}

int main (int argc,const char* argv[])
{
	printf("-------begin--------");
	if(signal(SIGINT,handler) ==SIG_ERR)
	{ errlog("signal error");}
	if(signal(SIGTSTP,SIG_IGN) ==SIG_ERR)
	{ errlog("signal SIGTSTP error");}
	while(1){
		sleep(2);
		printf("testsignal\n");
	}
	return 0;
}
