#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#define N 128
#define errlog(errmsg) do{perror(errmsg); \
	printf("%s--%s--%d--\n",__FILE__,__FUNCTION__,__LINE__); \
	return -1;} while(0)
struct shmbuf{
	char buf[N];
};
union semun{
	int val;
};

int main(int argc,const char *argv[])
{
	key_t key;
	if(( key = ftok (".",'a')) < 0){
		errlog("ftok error");
	}	
	int shmid;
	struct shmbuf *shm;
	// 第二个参数是设置共享内存的大小空间 应该是KB
	if((shmid = shmget(key,512,IPC_CREAT|IPC_EXCL|0664)) < 0 ){
		if(errno !=EEXIST){
			errlog("shmget error");
		}
		else{shmid =shmget(key,512,0664); }
	}
	//shmdt主要是将内存映射到虚拟地址 
	if((shm =shmat(shmid,NULL,0)) > 0){printf("shm:%p \n",shm); }
	
	//sem表示信号量的集和
	int semid;
	union semun semun;
	struct sembuf sem;
	semid = semget(key,2,IPC_CREAT|IPC_EXCL|0664);
	
	if(semid < 0){
		if(errno != EEXIST){errlog("semget error");return -1; }
	
		else{semid =semget (key,2,0664); }
	}
	else{
		//初始化信号量的值
		semun.val = 0;
		semctl(semid,0,SETVAL,semun); //第二个参数是信号量初始,第四个参数是共用体
		semun.val = 1;
		semctl (semid,1,SETVAL,semun);
	}
	while(1){
		sem.sem_num = 1;
		sem.sem_op = -1;
		sem.sem_flg = 0;
		semop (semid,&sem,1);//
		fgets (shm -> buf,N,stdin);
		shm->buf[strlen(shm ->buf)-1] = '\0';

		sem.sem_num =0;
		sem.sem_op =1;
		sem.sem_flg = 0;
		semop (semid,&sem,1);
		
		if(strncmp(shm ->buf,"quit",4)== 0){
			goto ERR;
		}
	}
	return 0;
ERR:
	shmdt(shm);//断开与共享内存的连接
}
