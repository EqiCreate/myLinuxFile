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
void cal_percentages_2012();
void printf_flower();
void printf_compare_strs(char* p[],int len);