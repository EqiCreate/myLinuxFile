#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define N 128
#define SIZE sizeof(struct msgbuf) -sizeof(long)///?
#define TYPE1 100
#define TYPE2 200
struct msgbuf{
	long mtype;
	char buf[N];
};
int main(int argc,const char *argv[]){
	key_t key;
	if((key = ftok(".", 'a') )< 0 ){
		perror("ftok error");
	}
	//打开消息队列
	int msqid;
	struct msgbuf msg_snd,msg_rcv;
	if( (msqid = msgget (key,IPC_CREAT|IPC_EXCL|0664)) < 0){
		if(errno != EEXIST){
			perror("msgget error");
			return -1;
		}
		else{
			msqid = msgget(key,0664);
		}
	}
	
	pid_t pid;
	pid =fork();
	if (pid < 0){
		perror("fork error");return -1;
	}
	else if (pid ==0){
		while(1){
			msg_snd.mtype =TYPE2;
			fgets(msg_snd.buf,N,stdin);
			msg_snd.buf[strlen(msg_snd.buf)-1]= '\0';
			msgsnd(msqid,&msg_snd,SIZE,0);
			
			if (strncmp (msg_snd.buf,"quit",4)== 0){
				kill(getppid(),SIGKILL);break;
			}
		}
	}
	else{
		while(1){
			msgrcv(msqid,&msg_rcv,SIZE,TYPE1,0);
			if (strncmp (msg_rcv.buf,"quit",4)== 0){
				kill(pid,SIGKILL);goto err;
			}

			printf("msg_a:%s \n",msg_rcv.buf);
		}
	}

	return 0;
err:
	msgctl(msqid,IPC_RMID,NULL);
}
