#include <stdio.h>
#include "lib.h"
#include <math.h>
#include "malloc.h"
#include "string.h"
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

void printf_array(int * array,int len)
{
    printf("start output array");
    for (int i = 0; i < len; i++)
    {
        printf("%d\n",*(array+i));
    }
}

void printf_different_value_by_array(int *array,int len,int required_len)
{
     int temp=0;
     int total=0;
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len; j++)
        {
            if(j==i)continue;
            for (int k = 0; k < len ; k++)
            {
                if(k==j)continue;
                if(k==i)continue;
                temp= *(array+i) *100 + *(array+j) *10+*(array+k) ;
                printf("%d\t",temp);
                total++;
            }
            
        }
    }
    printf("total = %d \n",total);
}

void printf_array_with_3_dicemal(int * array)
{
    int temp=0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if(j==i)continue;
            for (int k = 0; k < 3 ; k++)
            {
                if(k==j || k==i)continue;
                temp= *(array+i) *100 + *(array+j) *10+*(array+k) ;
                printf("%d\n",temp);
            }
            
        }
        
    }
}

void inverse(char * str,char * output,int len)
{
    for (int i = 0; i < len; i++)
    {
        *(output+i) = *(str-i+len-1);
    }
    
}
void cal_percentages_2016()
{
    float total=0,temp=0;
    int child=1;
    for (int i = 0; i < 20; i++)
    {
      total+= (float)1/child;
      if(i%2==0){
        child= 2*i +1;
      }
      else{
        child= 0-(2*i +1);
      }
    }
    printf("%.2f\n",total);

}

void delete_char_in_array(char *array,int len,char need_tobe_del)
{
    int temp_index=0;
    int timer=1;
    for (int i = 0; i < len; i++)
    {
        if(*(array+i) == need_tobe_del )
        {
            temp_index=i;
            
           while (temp_index<len-1)
           {
                if(array[temp_index+timer]!=need_tobe_del){
                    array[temp_index]=array[temp_index+timer];
                    temp_index++;
                }
                else{
                    timer++;
                }
           }
        }
    }
}  
void sell_watermellon(int total)
{
    int days=0;
    while (total >0)
    {
        total =total/2-2;
        days++;
    }
    printf("days=%d \n",days);
}
void game_5_quite(int n)
{
    int left_people=n;
    int arr[n];
    int exchange_timer=5;
    int index=0;
    int order=1;
    memset(arr,0,n);
    while(left_people!=1)
    {
        if(index>=n)
        {
            index=0;
        }
        if(order%5==0){
            arr[index]=1;
            left_people--;
        }
        index++;
        while(arr[index]==1){
            index++;
            if(index>=n)
            {
                index=0;
            }
        }
        order++;
    }
    for (int i = 0; i < n; i++)
    {
        if(arr[i]==0)
            printf("the orgin no is %d\n",i+1);
    }
}

struct Number * create_numbers_list(char * a)
{
   struct Number *list,*head,*p,*q;
   list= malloc(sizeof(struct Number));
    int i=0;
   p=malloc(sizeof(struct Number));
    p->c=a[i++];
    if(p->c!=0)
    {
        head=malloc(sizeof(struct Number));
        list->next=head;
        head->next=p;
    }
    else{
        head=NULL;
        list->next=head;
        return list;
    }
    
   while(p->c!=0)
   {
        if(a[i]!=0)
        {
            q=malloc(sizeof(struct Number));
            q->c=a[i];
            p->next=q;
            p=q;
            i++;
        }
        else
        {
            break;
        }
   }
   p->next=NULL;
   return list;

}