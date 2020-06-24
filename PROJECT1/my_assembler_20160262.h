/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_SECTION 3       //section�� �ִ�
#define MAX_REG 9           //register�� �ִ�
#define MAX_BUFFER 60       //T record BUFFER�� �ִ�

/*
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
    char name[7];       //inst ��ɾ�
    int format;         //inst ��ɾ� ����
    int Opcode;         //inst ��ɾ� 16���� OPCODE
    int Operands;       //inst ��ɾ� ���� ����
};

// instruction�� ������ ���� ����ü�� �����ϴ� ���̺� ����
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit
{
	char *label;				//��ɾ� ���� �� label
	char *operator;				//��ɾ� ���� �� operator
	char *operand[MAX_OPERAND]; //��ɾ� ���� �� operand
	char *comment;				//��ɾ� ���� �� comment
	char nixbpe;				// ���� ������Ʈ���� ���ȴ�.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 */
struct symbol_unit
{
	char symbol[10];
	int addr;
};

/*
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ���ȴ�.
*/
struct literal_unit
{
	char literal[10];
	int addr;
};
/*
* ���Ǻ� Modification�� �����ϱ� ���� ����ü�̴�.
* Modification�� location�� operand�� symbol�� �����Ѵ�.
*/
struct extended
{
    short location;
    short length;
    char Flag;
    char* operand;
};
/*
* register�� �����ϱ� ���� ����ü�̴�.
* register�� name�� num�� �����ȴ�.
*/
struct registers
{
    char R[2];
    char num;
};
/*
* ���Ǻ� T record�� �����ϱ� ���� ����ü�̴�.
* start �ּҿ� record ���� �����Ѵ�.
*/
struct T_record
{
    short start;
    char record[MAX_BUFFER];
};

typedef struct symbol_unit symbol;
typedef struct literal_unit literal;
typedef struct extended ex;
typedef struct registers registers;
typedef struct T_record T_record;

//register���� const�� ����
const registers reg[MAX_REG] = { {"A",0},{"X",1},{"L",2},{"B",3},{"S",4},{"T",5},{"F",6},{"PC",8},{"SW",9} };
static int locctr;
char tempstr[10];           //String���� ó���ϱ� ���� �ӽ� ����

/*
* ���Ǻ� ���α׷��� �����ϱ� ���� ����ü
* ���α׷� �̸�, �����ּ�, ����, EXTDEF, EXTREF,
* SYMTAB, LITTAB, OBJECT_CODE, T_RECORD, M_RECORD �� �����Ѵ�.
*/
struct program 
{
    char name[10];                      //program name
    short start;                        //start addr
    short length;                       //program length
    char D[MAX_OPERAND][10];            //EXTDEF D record
    char R[MAX_OPERAND][10];            //EXTREF R record
    symbol sym_table[MAX_LINES];        //SYMTAB
    int sym_num;                        //SYMTAB�� �����ϱ� ���� ����
    literal lit_table[MAX_LINES];       //LITTAB
    int lit_num;                        //LITTAB�� �����ϱ� ���� ����
    int ob_code[MAX_LINES];             //OBJECT CODE
    int ob_num;                         //OBJECT CODE�� �����ϱ� ���� ����
    T_record T[10];                     //T Record
    int t_num;                          //T Record�� �����ϱ� ���� ����
    ex M_table[MAX_LINES];              //M Record
    int M_num;                          //M Record�� �����ϱ� ���� ����
};
typedef struct program program;
program section[MAX_SECTION];
static int sect_num;
//--------------
static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
//Make new function
int search_symtab(char* str,int sess);
int search_littab(char* str, int sess);
int search_regist(char* str);
void hextoString(char* temp, int value, short size);