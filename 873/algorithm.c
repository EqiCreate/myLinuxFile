// 直接插入 排序
int* sort_by_directinsert(int *array,int len )
{
    int is_needed_sort=1;
    int k=0;
    int temp=0;
    for (int i = 1; i < len; i++)
    {
        temp=array[i];
        for (int j = 0; j < i; j++)
        {
            if(temp<array[j])
            {
                for (k=i; k>j; k--)
                {
                    array[k]=array[k-1];
                }
                array[j]=temp;
                break;
            }
        }
    }
    
}   

// 简单选择
int* sort_by_simpleselect(int *array,int len)
{
    int temp=0;
    int index=0;
    for (int i = 0; i < len; i++)
    {
        index=i;
        temp=array[i];
        for (int j = i+1; j < len; j++)
        {
            if(array[j]> array[index])
            {
               index=j;
            }
        }
        if(index==i)
        {
            continue;
        }
        array[i]=array[index];
        array[index]=temp;
    }
}  

// 冒泡优化
int* sort_by_bubble(int *array,int len)
{
    int temp=0;
    int index=0;
    int flag=0;
    for (int i = len-1; i >0; i--)
    {
        flag=1;
        for (int j = 1; j <= i; j++)
        {
            if(array[j] < array[j-1])
            {
                temp=array[j];
                array[j]=array[j-1];
                array[j-1]=temp;
                flag=0;
            }
        }

        if(flag==1)
            break;
    }
}  


