/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_SECTION 3       //section의 최댓값
#define MAX_REG 9           //register의 최댓값
#define MAX_BUFFER 60       //T record BUFFER의 최댓값

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
    char name[7];       //inst 명령어
    int format;         //inst 명령어 형식
    int Opcode;         //inst 명령어 16진수 OPCODE
    int Operands;       //inst 명령어 인자 갯수
};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
	char *label;				//명령어 라인 중 label
	char *operator;				//명령어 라인 중 operator
	char *operand[MAX_OPERAND]; //명령어 라인 중 operand
	char *comment;				//명령어 라인 중 comment
	char nixbpe;				// 추후 프로젝트에서 사용된다.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 */
struct symbol_unit
{
	char symbol[10];
	int addr;
};

/*
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
*/
struct literal_unit
{
	char literal[10];
	int addr;
};
/*
* 세션별 Modification을 관리하기 위한 구조체이다.
* Modification의 location과 operand의 symbol을 저장한다.
*/
struct extended
{
    short location;
    short length;
    char Flag;
    char* operand;
};
/*
* register를 관리하기 위한 구조체이다.
* register의 name과 num로 구성된다.
*/
struct registers
{
    char R[2];
    char num;
};
/*
* 세션별 T record를 저장하기 위한 구조체이다.
* start 주소와 record 값을 저장한다.
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

//register값을 const로 저장
const registers reg[MAX_REG] = { {"A",0},{"X",1},{"L",2},{"B",3},{"S",4},{"T",5},{"F",6},{"PC",8},{"SW",9} };
static int locctr;
char tempstr[10];           //String값을 처리하기 위한 임시 변수

/*
* 세션별 프로그램을 관리하기 위한 구조체
* 프로그램 이름, 시작주소, 길이, EXTDEF, EXTREF,
* SYMTAB, LITTAB, OBJECT_CODE, T_RECORD, M_RECORD 를 저장한다.
*/
struct program 
{
    char name[10];                      //program name
    short start;                        //start addr
    short length;                       //program length
    char D[MAX_OPERAND][10];            //EXTDEF D record
    char R[MAX_OPERAND][10];            //EXTREF R record
    symbol sym_table[MAX_LINES];        //SYMTAB
    int sym_num;                        //SYMTAB을 관리하기 위한 변수
    literal lit_table[MAX_LINES];       //LITTAB
    int lit_num;                        //LITTAB을 관리하기 위한 변수
    int ob_code[MAX_LINES];             //OBJECT CODE
    int ob_num;                         //OBJECT CODE를 관리하기 위한 변수
    T_record T[10];                     //T Record
    int t_num;                          //T Record를 관리하기 위한 변수
    ex M_table[MAX_LINES];              //M Record
    int M_num;                          //M Record를 관리하기 위한 변수
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