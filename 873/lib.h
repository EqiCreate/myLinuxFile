#define max_range 100

typedef union 
{
    int nodeDepth;
    double value;
}NodeBody;

typedef struct Node
{
    int id;
    NodeBody body;
    char tag;
    
}TreeNode,*Tree;

struct  book
{
    int bookId;
}TechDigtal;


void dosth();