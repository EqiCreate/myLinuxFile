#include <stdio.h>
#include "lib.h"
#include "algorithm.h"
#include "matrix.h"
#include "malloc.h"
#include <stdio.h>
#include <sys/select.h>
#include<unistd.h>
#include "string.h"

#define add 7*5
typedef void (*eventhandler)();
int value1=22;
static int s_value3=33;
extern int test_external;
extern void dosth2();
extern void simple_sort();
static int EVENT =0;


static void sleep_ms(unsigned int secs)
{
    struct timeval tval;

    tval.tv_sec=secs/1000;

    tval.tv_usec=(secs*1000)%1000000;

    select(0,NULL,NULL,NULL,&tval);
}

void doevent()
{
    printf("Event on");
    // sleep(1);
    sleep_ms(40);
    printf("Event off");

}
static eventhandler EventsManger[16]={NULL,NULL,NULL,doevent};



int get_eventid()
{
    return 3;
    // if(!EVENT)return -1;
    // return ;

}
void pevent_main()
{
    int k[1];
    eventhandler func;
    int event_id;
    while(1)
    {
        event_id=get_eventid();
        func =EventsManger[event_id];
        if(func!=NULL)
            func();
    // sleep_ms(40);
        
    }
}

// 变量初始化的问题
void fun(int *px,int *py)
{

    int a=0,c=0;
    auto int k =0;
    scanf("%d",&k);

    while (k!=0)
    {
        if (k>0)
        {
            a++;
        }
        if(k<0)
            c++;
        scanf("%d",&k);
    }
    *px=a;*py=c;
    
}
int main()
{
    //test ref & pointer
    // int a=1,b=2;
    // swap_by_pointer(&a,&b);
    // printf("'");
    //test strlen and scanf
    // char str[]="123456";
    // char str_n[]="123";
    // char *str_p="123";
    // printf("%d\n",sizeof(str_p));

    // printf("%d\n",sizeof(str_n));
    
    // printf("%d\n",strlen(str));
    // // int aaaa,*p=&aaaa;
    // // scanf("%d",p);
    // int aa;char b=0;float c=0;char * s;char ss[20];
    // s=(char*)malloc(sizeof(char));
    // // scanf("%d,%f,%c",&aa,&c,&b);
    // // scanf("%d%s%f",&aa,s,&c);
    // scanf("%s",s);
    // int len=sizeof(s);
    // scanf("%s",ss);
    // int len_ss=strlen(ss);
    // char * str="hello";
    // int len_str=sizeof(str);
    // printf("1");
    // char aaa[]="hello";
    // printf("%d",strlen(aaa));
    // int M =0;int n=0;
    // (M)?(n++):(n--);
    // test event
    // pevent_main();

    //test variabel 
    // static int  s_value=2;
    // auto float a_value1=3.6;
    // auto z="sddf1";
    // printf("%d",value1);

    // dosth(); // extern test
    // printf("extern_num = %d \n max_range=%d\n",test_external,max_range);
    
    // test function 
    // int x,y;
    //  fun(&x,&y);
    // printf("x=%d,y=%d \n",x,y);

    // struct / class test
    // Tree tree = malloc(sizeof(TreeNode));
    // tree->id = 111;
    // printf("%d\n",tree->id);
    // dosth2();
    // printf("%d \n",add);
    // TreeNode tree ;
    // tree.id= 1;
    // tree.body.nodeDepth= 2;

    // printf("%d\t",tree.id);
    // printf("%d\n",tree.body.value);


    // struct Node node;
    // node.id=0x22; 
    // printf("%d\n",node.id);

    // struct book tech;
    //  tech.bookId=0x19;   
    //    printf("%d\n",tech.bookId);
    // printf("%d \n",sizeof(TreeNode));


    // test variale expression
    // int a=0;int b=0;
    // int x = (a=3,b=a--); // 返回逗号最后一个式子 x =b =3;
    // printf("%d %d %d",x,a,b);
    // a=2;b=5;
    // b*=(b%=a); // b=b%a=1, b=b*b=1
    // printf("%d",b); 

    // test char
    // char *str1 ="hihello12";
    // char *str2 ="hihe";
    // printf("%d \n", strlen(str1));
    // int res=strcmp(str1,str2);
    // printf("cmp =%d \n", res);

    // if( res)
    //     printf("1 \n");
    // else 
    //     printf("2 \n");

    // float div_value ;
    // div_value =10/4;
    // printf("%.3f \n",div_value);
    // double d =3%2;
    // printf("%.3f \n",d);

    // simple_sort();
    // printf("\018 \n"); // 八进制的 /618=18(十进制)
    // printf("%o \n",add);
    // printf("\x43 \n"); // x表示16进制,表示66，即B
    // // putchar('f'-'a'+'A' ); // putchar ,输入字符
    // printf("adfasdf\0DDDD \n"); //终止符\0 导致终止输出
    // putchar('');
    // int u=-1;float v=-1;
    // scanf("%4d%f",&u,&v);
    // printf("\' 11\b 22 \11692 \n");
    // int m=102;
    // int n =013;
    // printf("%5d,%2d \n",m,n);
    // printf("%8.4s \n","hi\bjklmnOPQrst");// 表示最少8位字符，且只保留其中前4位，向右对齐，左边补u
    // int x1=-1;char x2=-1;float x3=-1;
    // scanf("%d%c%f",&x1,&x2,&x3);
    // int x4,x5;
    // scanf("%2d%*1d%2d",&x4,&x5);

    // int a,*p=&a;
    // scanf("%d",p);


    // char *str3="abcdefghi";
    // char y[10],*str4;
    // stpcpy(str4=y+1,str3);
    // int M=2;int a=0;
    // (M)?(a++):(a--);
    // if (M)
    // {
    //    printf("%-06d \n",M);
    // }
    
    // int (*array[2])[3];
    // int (*p)[10];
    // printf("sizeof array=%d sizeof p =%d ,sizeof array[0] =%d \n", sizeof(array),sizeof(p),sizeof(array[0]));

    //test algrorithm
    // int array[]={1,0,3,2,6,4,3,5,8,11,1};
    // int input_dicemal;
    // int index=0;
    // int array[5]={0};
    // // int len =sizeof(array);
    // int len =sizeof(array)/sizeof(int);

    // while(1)
    // {
    //     if(index>=len)
    //         break;
        
    //     if(scanf("%d",&input_dicemal)!= EOF)
    //     {
    //         array[index]=input_dicemal;
    //         index++;
    //     }
    // }

    // int len =sizeof(array)/sizeof(int);
    // int * res;
    // res=sort_by_directinsert(array,len);
    // res=sort_by_simpleselect(array,len);
    // res=sort_by_bubble(array,len);
    // printf_array(array,len);

    //test array[][]
    // float a[3][3]={1,2,3,4,5};
    // float b[3][3]={1,2,3,4,5};
    // printf_matrix(a,b,3);
    // Mnode nodes[3][3];
    // nodes[0][0].id=100;
    // nodes[0][1].id=101;
    // nodes[1][0].id=200;
    // printf_matrix_nodes(nodes,3);
    
    //test 2012
    // cal_percentages_2012();
    // printf_flower();
    // char*s1="a12",*s2="113",*s3="111",*s4="aa";
    // char * strs[]={s1,s2,s3,s4};
    // printf_compare_strs(strs,sizeof(strs)/sizeof(char*)) ;
    // int array[]={2,4,6,8};
    // printf_different_value_by_array(array,sizeof(array)/sizeof(array[0]),3);
    // char * str,*output;
    // char * str =(char*)malloc(sizeof(char));
    // //  char str[100]={0};
    // char output[100]={0};
    // scanf("%s",str);
    // // output =malloc(sizeof(str));
    // inverse(str,output,strlen(str));
    // printf("%s\n",output);
    // free(str);

    //test 2016
    // cal_percentages_2016();
    // char str1[100]={0};
    // gets(str1);
    // int len =strlen(str1);
    // // // sort_by_bubble_with_chars(str1,100);
    // // printf("the char need to be delete ");
    // char target=getchar();
    // delete_char_in_array(str1,len,target);
    // printf("%s",str1);
    
    // sell_watermellon(1020 );

    // float array[3][3]={0};
    // for (int i = 0; i < 3; i++)
    // {
    //     for (int j = 0; j < 3; j++)
    //     {
    //         scanf("%f", &array[i][j]);
    //     }
    // }
    // int sumA=0,sumB=0;
    // for (int i = 0; i < 3; i++)
    // {
    //     sumA+=array[i][i];
    //     sumB+=array[2-i][i];
    // }
    // printf("man = %d,vice=%d \n",sumA,sumB);
    // int n=0;
    // scanf("%d",&n);
    // game_5_quite(n);
    // char * input,arr[40][40]={""};
    // input= (char*)malloc(sizeof(char));
    // gets(input);
    // int len =strlen(input);
    // char *substr = strtok(input," ");
    // int timer=0;
    // while (substr!=NULL)
    // {
    //     strcpy(arr[timer],substr);
    //     timer++;
    //     substr=strtok(NULL," ");
    // }
    
    
    // create_numbers_list((char*)arr);
    // free(input);
    int x,a,b;
    a=012;
    b=(a--);
    // x=(a=3,b=a--);
    return 0;
}


