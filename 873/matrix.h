
typedef struct matrinode    
{
    int id;
}Mnode,*MnodeTree;

void printf_matrix(void **a,float (*b)[3],int len);
void printf_matrix_nodes(void ** nodes,int size);