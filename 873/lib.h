#define max_range 100

typedef union 
{
    int nodeDepth;
    char value;
}NodeBody;

typedef struct Node
{
    int id;
    NodeBody body;
    
}TreeNode,*Tree;

struct  book
{
    int bookId;
}TechDigtal;


void dosth();