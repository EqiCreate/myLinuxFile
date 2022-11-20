#include <stdio.h>
#include <string.h>
#include "lib.h"
#include "malloc.h"

#define add 7*5

int value1=22;
static int s_value3=33;
extern int test_external;
extern void dosth2();
extern void simple_sort();




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
    int M=2;int a=0;
    (M)?(a++):(a--);
    if (M)
    {
       printf("%-06d \n",M);
    }
    

    
    return 0;
}


