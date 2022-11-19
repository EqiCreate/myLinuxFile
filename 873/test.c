#include <stdio.h>
#include <string.h>
#include "lib.h"
#include "malloc.h"

#define add 7*5

int value1=22;
static int s_value3=33;
extern int test_external;
extern void dosth2();





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
    // variabel test
    // static int  s_value=2;
    // auto float a_value1=3.6;
    // auto z="sddf1";
    // printf("%d",value1);

    // dosth(); // extern test
    // printf("extern_num = %d \n max_range=%d\n",test_external,max_range);
    
    //  function test
    // int x,y;
    //  fun(&x,&y);
    // printf("x=%d,y=%d \n",x,y);

    // struct / class test

    // Tree tree = malloc(sizeof(TreeNode));
    // tree->id = 111;
    // printf("%d\n",tree->id);
    dosth2();
    printf("%d \n",add);
    TreeNode tree ;
    tree.id= 1;
    tree.body.nodeDepth= 2;

    printf("%d\t",tree.id);
    printf("%d\n",tree.body.value);


    struct Node node;
    node.id=0x22; 
    printf("%d\n",node.id);

    struct book tech;
     tech.bookId=0x19;   
       printf("%d\n",tech.bookId);

    return 0;
}


