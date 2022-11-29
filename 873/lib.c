#include <stdio.h>
#include "lib.h"
#include <math.h>

static int test_external;
void dosth ()
{
     int test_external=9;
    printf("asdf \n");
    
}

void dosth2()
{
    printf("dosth2 \n");
}

void simple_sort()
{
    int array[]={1,3,24,4,5,2,11},temp;
    int index,i,j;
    int n =sizeof(array)/sizeof(int);
    for (i = 0; i < n; i++)
    {
        index=i;
        for ( j = i+1; j < n; j++)
        {
            if(array[j] > array[index])
                index=j;
        }
        if(index==i)
            continue;
        temp=array[index];
        array[index]=array[i];
        array[i]=temp;
    }
    for ( i = 0; i < n; i++)
    {
        printf("%d \n",*(array+i));
    }
    
}

void cal_percentages_2012()
{
    float total=0,temp=0;
    int initA=1,initB=2;
    for (int i = 0; i < 20; i++)
    {
        total+=(float)initB/initA;
        temp=initA;
        initA=initB;
        initB=temp+initB;
    }
    printf("%.2f\n",total);

}
void printf_flower()
{
    char b=0,s=0,g=0;
    for (int i = 0; i < 999; i++)
    {
        g=i %10;
        s= i/10 %10;
        b=i/100 %10;
        // if(i== pow(b,3)+pow(s,3)+pow(g,3))
        if(i== b*b*b+g*g*g+s*s*s)
        {
            printf("%d \n",i);
        }
    }
}

void printf_compare_strs(char* p[],int len)
{
    char * temp;
    for (int i = 1; i < len; i++)
    {
        if( strcmp(p[i-1],p[i]) >0)
        {
            temp=p[i-1];
            p[i-1]=p[i];
            p[i]=temp;
        }
    }
    for (int i = 0; i < len; i++)
    {
        printf("%s \n",*(p+i));
    }
}