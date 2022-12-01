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
void printf_array(int * array,int len);
// void printf_different_value_by_array(int *array,int len,int required_len);
void inverse(char * str,char * output,int len);

void cal_percentages_2016();
void delete_char_in_array(char *array,int len,char need_tobe_del);
void sell_watermellon(int total);
void game_5_quite(int n);
struct Number{int c; struct Number *next;};
struct Number * create_numbers_list(char * a);
 
