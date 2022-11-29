
#include "matrix.h"
void printf_matrix(void *(*a),float (*b)[3],int size)
{
    
    float c[size][size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // c[i][j]=a[i][j]+b[i][j];
            c[i][j]= *((float*)a + i*size+j) +b[i][j];
            
            printf("%-0.2f\t",c[i][j]);
            if(j==2)
                printf("\n");
        }
        
    }

}
void printf_matrix_nodes(void ** nodes,int size)
{
    struct matrinode node;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)  
        {
           node= *((Mnode*)nodes+ i*size +j);
            printf("%2d\t",node.id);
        }
    }
   

}