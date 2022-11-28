#include <stdio.h>
#include "lib.h"

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

