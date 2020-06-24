/*
 * ȭ�ϸ� : my_assembler_2016062.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler_20160262.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	//make_opcode_output("opcode_20160262.txt");
    make_symtab_output("symtab_20160262.txt");
	make_literaltab_output("literaltab_20160262.txt");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
    make_objectcode_output("output_20160262.txt");

	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
    FILE* file;

    int errno;
    fopen_s(&file, inst_file, "r");
    if (file != NULL)              //Success Open File             
    {
        for (inst_index = 0; inst_index < MAX_INST; inst_index++)
        {
            inst_table[inst_index] = (inst*)malloc(sizeof(inst));
            fscanf_s(file, "%s %d %02X %d\n", inst_table[inst_index]->name, 7,
                &inst_table[inst_index]->format,
                &inst_table[inst_index]->Opcode,
                &inst_table[inst_index]->Operands);
        }
        errno = 0;
    }
    else
        errno = -1;                 //file IOException


    fclose(file);                   //file close
    return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
    FILE* file;

    int errno;
    char buffer[256]; //�ӽ� BUFFER ����
    fopen_s(&file, input_file, "r");

    if (file == NULL) {                                             //file IOException
        errno = -1;
        return errno;
    }
    else
    {
        while (fgets(buffer, 256, file) != NULL)                    //file���� �о �� ���� buffer�� ����
        {
            input_data[line_num] = (char*)malloc(sizeof(buffer));
            strcpy_s(input_data[line_num++], 256, buffer);          //input_data�� buffer���� ���� String���� ����
        }
    }


    fclose(file);                                                   //file close

    return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
    if (str == NULL)                                                    //str IOException
        return -1;
    else
    {
        char* tok, * context, * pos;
        int cnt = 0;
        int index = 0;
        if (str[0] == '\t')                                             //Haven't Label
        {
            token_table[token_line]->label = NULL;                      //set label null

            tok = strtok_s(str, "\t\n", &context);                      //Read operator
            token_table[token_line]->operator = tok;

            if ((token_table[token_line]->operator != NULL)
                && (!strcmp(token_table[token_line]->operator,"RSUB")       //Have RSUB
                    || !strcmp(token_table[token_line]->operator,"LTORG")   //Have LTORG
                    || !strcmp(token_table[token_line]->operator,"CSECT"))) //Have CSECT
            {
                token_table[token_line]->operand[0] = NULL;             //SET operand null;
                tok = strtok_s(NULL, "\n", &context);                   //comment
                token_table[token_line]->comment = tok;
            }
            else                                                        //Haven't RSUB
            {
                tok = strtok_s(NULL, "\n\t", &context);                 //operand
                pos = tok;
                if (pos != NULL)
                {
                    for (int i = 0; i < strlen(tok); i++)               //operand ���� Ȯ��
                        if (tok[i] == ',')                            
                            cnt++;

                    if (cnt == 0)                                       //operand ���� 1��
                    {
                        token_table[token_line]->operand[0] = pos;
                        token_table[token_line]->operand[1] = NULL;
                        token_table[token_line]->operand[2] = NULL;
                    }
                    else if (cnt == 1)                                  //operand ���� 2��
                    {
                        token_table[token_line]->operand[0] = strtok_s(pos, ",\0", &token_table[token_line]->operand[1]);
                        token_table[token_line]->operand[2] = NULL;
                    }
                    else if (cnt > 1)                                   //operand ���� 3��
                    {
                        token_table[token_line]->operand[0] = strtok_s(pos, ",\0", &token_table[token_line]->operand[1]);
                        strtok_s(token_table[token_line]->operand[1], ",", &token_table[token_line]->operand[2]);
                    }
                }

                tok = strtok_s(NULL, "\n", &context);                   //comment
                token_table[token_line]->comment = tok;
            }
        }
        else if (str[0] == '.')                                         //remark line
        {
            tok = strtok_s(input_data[token_line], "\n", &context);     //label
            token_table[token_line]->label = tok;
            token_table[token_line]->operator = NULL;
            token_table[token_line]->operand[0] = NULL;
            token_table[token_line]->operand[1] = NULL;
            token_table[token_line]->operand[2] = NULL;
            token_table[token_line]->comment = NULL;
        }
        else                                                            //Have Label
        {
            tok = strtok_s(input_data[token_line], "\t", &context);     //label
            token_table[token_line]->label = tok;

            tok = strtok_s(NULL, "\t\n", &context);                     //operator
            token_table[token_line]->operator = tok;

            if ((token_table[token_line]->operator != NULL)
                && (!strcmp(token_table[token_line]->operator,"RSUB")       //Have RSUB
                    || !strcmp(token_table[token_line]->operator,"LTORG")   //Have LTORG
                    || !strcmp(token_table[token_line]->operator,"CSECT"))) //Have CSECT
            {
                token_table[token_line]->operand[0] = NULL;                 //SET operand null;
                tok = strtok_s(NULL, "\n", &context);                       //comment
                token_table[token_line]->comment = tok;
            }
            else
            {
                tok = strtok_s(NULL, "\n\t", &context);                     //operand
                pos = tok;

                if (pos != NULL)
                {
                    for (int i = 0; i < strlen(tok); i++)                   //operand ���� Ȯ��
                        if (tok[i] == ',')
                            cnt++;
                    if (cnt == 0)                                           //operand ���� 1��
                    {
                        token_table[token_line]->operand[0] = pos;
                        token_table[token_line]->operand[1] = NULL;
                        token_table[token_line]->operand[2] = NULL;
                    }
                    else if (cnt == 1)                                      //operand ���� 2��
                    {
                        token_table[token_line]->operand[0] = strtok_s(pos, ",\0", &token_table[token_line]->operand[1]);
                        token_table[token_line]->operand[2] = NULL;
                    }
                    else if (cnt > 1)                                       //operand ���� 3��
                    {
                        token_table[token_line]->operand[0] = strtok_s(pos, ",\0", &token_table[token_line]->operand[1]);
                        strtok_s(token_table[token_line]->operand[1], ",", &token_table[token_line]->operand[2]);
                    }
                }

                tok = strtok_s(NULL, "\n", &context);                       //comment
                token_table[token_line]->comment = tok;
            }
        }

        //nixbpe
        token_table[token_line]->nixbpe = 0;                                //reset
        if (token_table[token_line]->operator != NULL)                      //check remark line
        {
            if ((index = search_opcode(token_table[token_line]->operator)) >= 0)            //It's INST
            {
                if (token_table[token_line]->operator[0] == '+')                            //extended
                    token_table[token_line]->nixbpe |= (1 << 0);
                else if (inst_table[index]->format == 3)
                    token_table[token_line]->nixbpe |= (1 << 1);                            //PC Flag

                if (token_table[token_line]->operand[0] != NULL)
                {
                    if (token_table[token_line]->operand[0][0] == '#')                      //immediate
                    {
                        token_table[token_line]->nixbpe ^= (1 << 1);
                        token_table[token_line]->nixbpe |= (1 << 4);
                    }
                    else if (token_table[token_line]->operand[0][0] == '@')                 //indirect
                    {
                        token_table[token_line]->nixbpe |= (1 << 5);
                    }
                    else if (inst_table[index]->format == 3)                                //SIC/XE basic
                    {
                        token_table[token_line]->nixbpe |= (1 << 4);
                        token_table[token_line]->nixbpe |= (1 << 5);
                    }

                    if (token_table[token_line]->operand[1] != NULL && token_table[token_line]->operand[1][0] == 'X')      //check X 
                        token_table[token_line]->nixbpe |= (1 << 3);

                }
                else if (!strcmp(token_table[token_line]->operator,"RSUB"))                 //RSUB ����ó��
                {
                    token_table[token_line]->nixbpe ^= (1 << 1);
                    token_table[token_line]->nixbpe |= (1 << 4);
                    token_table[token_line]->nixbpe |= (1 << 5);
                }
            }
        }
        return 0;                                                                           //��������
    }
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
    if (str == NULL)                                                //str IOException
        return -1;

    for (inst_index = 0; inst_index < MAX_INST; inst_index++)
    {
        if (str[0] == '+')                                          //Extended Format
            str = &str[1];
        if (strcmp(str, inst_table[inst_index]->name) == 0)         //find Opcode
            return inst_index;

    }
    return -1;                                                      //Can't Find
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
    int index = -1 ;
    char* tok1, * tok2;
    for (token_line = 0; token_line < line_num; token_line++)
    {
        if (input_data[token_line] != NULL)
        {
            token_table[token_line] = (token*)malloc(sizeof(token));
            token_parsing(input_data[token_line]);  //CALL TOKEN_PARSING
        }
        else                                        //input_data IOException
            return -1;
    }
    //Pass1 START 
    for (token_line = 0; token_line < line_num; token_line++)                               //sym_table, literal_table, locctr ó��
    {
        if (token_table[token_line]->operator != NULL &&!strcmp(token_table[token_line]->operator,"START")) //START
        {
            section[sect_num].start = atoi(token_table[token_line]->operand[0]);            //SAVE #OPERAND as Starting address
            locctr = section[sect_num].start;                                               //initialize LOCCTR to starting address
        }

        if (token_table[token_line]->label != NULL && !(token_table[token_line]->label[0] == '.'))
        {
            if (!strcmp(token_table[token_line]->operator,"CSECT"))                                         //CSECT
            {
                locctr = 0;
                sect_num++, section[sect_num].sym_num = 0;
            }                                                                           //INSERT SYMTAB(LABEL, LOCCTR)
            strcpy_s(section[sect_num].sym_table[section[sect_num].sym_num].symbol, 10, token_table[token_line]->label);
            section[sect_num].sym_table[section[sect_num].sym_num++].addr = locctr;
        }
        if (token_table[token_line]->operator != NULL)
        {
            if (!strcmp(token_table[token_line]->operator,"START"))                                         //START
            {
                ;
            }
            else if (!strcmp(token_table[token_line]->operator,"EXTDEF"))                                   //EXTDEF
            {
                ;
            }
            else if (!strcmp(token_table[token_line]->operator,"EXTREF"))                                   //EXTREF
            {
                ;
            }           
            else if (!strcmp(token_table[token_line]->operator,"RESW"))                                     //RESW
            {
                locctr += 3 * atoi(token_table[token_line]->operand[0]);
            }
            else if (!strcmp(token_table[token_line]->operator,"RESB"))                                     //RESB
            {
                locctr += atoi(token_table[token_line]->operand[0]);
            }
            else if (!strcmp(token_table[token_line]->operator,"WORD"))                                     //WORD
            {
                locctr += 3;
            }
            else if (!strcmp(token_table[token_line]->operator,"BYTE"))                                     //BYTE
            {
                if (token_table[token_line]->operand[0] == 'X')                                             
                {
                    locctr += 1;
                }
                else                                                                                        
                    locctr += 1;
            }
            else if (!strcmp(token_table[token_line]->operator,"EQU"))                                      //EQU
            {
                if (token_table[token_line]->operand[0][0] == '*')
                    ;
                else                                                                            
                {
                    for (int i = 0; i < strlen(token_table[token_line]->operand[0]); i++)
                    {
                        if (token_table[token_line]->operand[0][i] == '-')                                  //MAXLEN process
                        {
                            tok1 = strtok_s(token_table[token_line]->operand[0], "-", &tok2);
                            section[sect_num].sym_table[section[sect_num].sym_num - 1].addr 
                                = search_symtab(tok1, sect_num) - search_symtab(tok2, sect_num);
                        }
                    }
                }
            }
            else if (!strcmp(token_table[token_line]->operator,"LTORG") 
                    || !strcmp(token_table[token_line]->operator,"END"))                                //LTORG, END process
            {
                for (int i = 0; i < section[sect_num].lit_num; i++)
                {
                    section[sect_num].lit_table[i].addr = locctr;
                    if (section[sect_num].lit_table[i].literal[1] == 'C')               //Type C
                    {
                        strncpy_s(tempstr, 10, &section[sect_num].lit_table[i].literal[3],
                            strlen(section[sect_num].lit_table[i].literal) - 4);
                        locctr += strlen(tempstr);
                    }
                    else if (section[sect_num].lit_table[i].literal[1] == 'X')          //Type X
                    {
                        strncpy_s(tempstr, 10, &section[sect_num].lit_table[i].literal[3],
                            strlen(section[sect_num].lit_table[i].literal) - 4);
                        if (strlen(tempstr) % 2)
                            locctr += (int)strlen(tempstr) / 2 + 1;
                        else
                            locctr += (int)strlen(tempstr) / 2;
                    }
                    else
                    {
                        locctr += 3;
                    }
                }
            }
            else if ((index = search_opcode(token_table[token_line]->operator)) >= 0)                       //INST
            {
                if (token_table[token_line]->operator[0] == '+')                                            //extended
                    locctr += 4;
                else                                                                                        //normal
                    locctr += inst_table[index]->format;
            }
        }
        if (token_table[token_line]->operand[0] != NULL && token_table[token_line]->operand[0][0] == '=')   //INSERT LITTAB
        {
            if (token_table[token_line]->operand[0][1] == 'C')
            {
                strcpy_s(section[sect_num].lit_table[section[sect_num].lit_num++].literal,10,token_table[token_line]->operand[0]);
            }
            else if (token_table[token_line]->operand[0][1] == 'X')
            {
                if(search_littab(token_table[token_line]->operand[0], sect_num) < 0)
                    strcpy_s(section[sect_num].lit_table[section[sect_num].lit_num++].literal, 10, token_table[token_line]->operand[0]);

            }
            else
            {
                strcpy_s(section[sect_num].lit_table[section[sect_num].lit_num++].literal, 10, token_table[token_line]->operand[0]);
            }
        }
        section[sect_num].length = locctr - section[sect_num].start;                                        //section LENGTH
    }
    return 0;                                       //Success Program
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 5��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char* file_name)
{
    FILE* file;
    short index = -1;
    if (file_name == NULL)                                      //Fail
        printf("Exception! Please Check Your File Name.\n");
    else                                                        //Success
    {
        fopen_s(&file, file_name, "w");
        for (token_line = 0; token_line < line_num; token_line++)
        {
            if (token_table[token_line]->label != NULL)                                 //print label
                fprintf_s(file, "%s\t", token_table[token_line]->label);
            else
                fprintf_s(file, "\t");

            if (token_table[token_line]->operator != NULL)                              //print operator
            {
                fprintf_s(file, "%s\t", token_table[token_line]->operator);
                index = search_opcode(token_table[token_line]->operator);
            }
            else
                fprintf_s(file, "\t");

            if (token_table[token_line]->operand[0] != NULL)                            //print operand
            {
                fprintf_s(file, "%s\t", token_table[token_line]->operand[0]);
                if (strlen(token_table[token_line]->operand[0]) < 7
                    || (strstr(token_table[token_line]->operand[0], "'") != NULL)
                    || (strstr(token_table[token_line]->operand[0], ",") != NULL))
                    fprintf_s(file, "\t");
            }
            else
                fprintf_s(file, "\t\t");

            if (index < 0)                                                              //print Opcode
                fprintf_s(file, "\n");
            else
                fprintf_s(file, "%02X\n", inst_table[index]->Opcode);
        }
        fclose(file);
    }
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
    if (file_name == NULL)                                                          //Exception process
    {
        printf("Exception! Please check your file name!\n");
        return;
    }
    FILE* file;
    fopen_s(&file, file_name, "w");
    if (file == NULL)                                                               //file open Exception process
        return;
    else                                                                            //print SYMTAB
    {
        for (int i = 0; i < MAX_SECTION; i++)
        {
            for (int j = 0; j < section[i].sym_num; j++)
            {
                fprintf_s(file, "%s\t\t%04X\n",section[i].sym_table[j].symbol, section[i].sym_table[j].addr);
            }
            fprintf_s(file, "\n");
        }
    }
    fclose(file);
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *file_name)
{
    if (file_name == NULL)                                                          //Exception process
    {
        printf("Exception! Please check your file name!\n");
        return;
    }
    FILE* file;
    fopen_s(&file, file_name, "w");
    if (file == NULL)                                                               //file open Exception process
        return;
    else                                                                            //print LITTAB
    {
        for (int i = 0; i < MAX_SECTION; i++) 
        {
            for (int j = 0; j < section[i].lit_num; j++)
            {
                if (section[i].lit_table[j].addr != NULL)
                {
                    if (section[i].lit_table[j].literal[1] == 'C' || section[i].lit_table[j].literal[1] == 'X')
                    {
                        strncpy_s(tempstr, 10, &section[i].lit_table[j].literal[3], strlen(section[i].lit_table[j].literal) - 4);
                        fprintf_s(file, "%s\t\t%04X\n", tempstr, section[i].lit_table[j].addr);
                    }
                    else
                    {
                        strncpy_s(tempstr, 10, &section[i].lit_table[j].literal[1], strlen(section[i].lit_table[j].literal));
                        fprintf_s(file, "%s\t\t%04X\n", tempstr, section[i].lit_table[j].addr);
                    }
                }
                    
            }
         }
    }
    fclose(file);
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    int index = -1;                                             //SEARCH_OPCODE �Ű�����
    int targetAddr = 0;                                         //Target Address ����
    sect_num = 0;                                               //section ���� ����
    locctr = 0;                                                 //LOCATION COUNTER
    int num = 0, size = 0;                                      //T_RECORD ��ȯ�� ���� ���� num, size ����
    int preval = -1, preloc = 0;                                //pre_value, pre_location_counter ����
    for (token_line = 0; token_line < line_num; token_line++, section[sect_num].ob_num++, targetAddr = 0)   //TOKEN_TABLE �ݺ���
    {
        if (token_table[token_line]->label != NULL && token_table[token_line]->label[0] == '.')             //remark TA < 0
            section[sect_num].ob_code[section[sect_num].ob_num] = -1;

        if (token_table[token_line]->operator != NULL)
        {
            if (!strcmp(token_table[token_line]->operator,"START"))                                         //START
            {
                strcpy_s(section[sect_num].name, 10, section[sect_num].sym_table[0].symbol);                //write Header Program Name
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if (!strcmp(token_table[token_line]->operator,"EXTDEF"))                                   //EXTDEF
            {
                for (int i = 0; i < MAX_OPERAND; i++)
                {
                    if (token_table[token_line]->operand[i] != NULL)
                        strcpy_s(section[sect_num].D[i], 10, token_table[token_line]->operand[i]);          //section.D ������ OPERAND �߰�
                }
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;                                   
            }
            else if (!strcmp(token_table[token_line]->operator,"EXTREF"))                                   //EXTREF
            {
                for (int i = 0; i < MAX_OPERAND; i++)
                {
                    if (token_table[token_line]->operand[i] != NULL)
                        strcpy_s(section[sect_num].R[i], 10, token_table[token_line]->operand[i]);          //section.R ������ OPERAND �߰�
                }
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if (!strcmp(token_table[token_line]->operator,"RESW"))                                     //RESW
            {
                locctr += 3 * atoi(token_table[token_line]->operand[0]);
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if (!strcmp(token_table[token_line]->operator,"RESB"))                                     //RESB
            {
                locctr += atoi(token_table[token_line]->operand[0]);
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if (!strcmp(token_table[token_line]->operator,"WORD"))                                     //WORD
            {
                if (token_table[token_line]->operand[0] >= '0' && token_table[token_line]->operand[0] <= '9')
                {
                    section[sect_num].ob_code[section[sect_num].ob_num] |= atoi(token_table[token_line]->operand[0]);
                }
                else
                {
                    section[sect_num].ob_code[section[sect_num].ob_num] = 0;
                    if (strchr(token_table[token_line]->operand[0], '-'))                                    //section 2 MAXLEN process
                    {
                        char* context;
                        section[sect_num].M_table[section[sect_num].M_num].location = locctr;
                        section[sect_num].M_table[section[sect_num].M_num].length = 6;
                        section[sect_num].M_table[section[sect_num].M_num].Flag = '+';
                        section[sect_num].M_table[section[sect_num].M_num].operand = strtok_s(token_table[token_line]->operand[0], "-", &context);
                        section[sect_num].M_table[++section[sect_num].M_num].operand = context;
                        section[sect_num].M_table[section[sect_num].M_num].location = locctr;
                        section[sect_num].M_table[section[sect_num].M_num].length = 6;
                        section[sect_num].M_table[section[sect_num].M_num].Flag = '-';
                    }
                    section[sect_num].M_num++;
                }
                locctr += 3;
            }
            else if (!strcmp(token_table[token_line]->operator,"BYTE"))                                     //BYTE
            {
                if (token_table[token_line]->operand[0][0] == 'X')
                {
                    locctr += 1;
                    strncpy_s(tempstr, 10, &token_table[token_line]->operand[0][2], 2);
                    section[sect_num].ob_code[section[sect_num].ob_num] = strtol(tempstr, NULL, 16);        //STRING to HEX convert
                }
                else
                    locctr += 1;
            }
            else if (!strcmp(token_table[token_line]->operator,"EQU"))                                      //EQU
            {
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if (!strcmp(token_table[token_line]->operator,"LTORG") 
                    || !strcmp(token_table[token_line]->operator,"END"))                                    //LTORG, END process
            {
                for (int i = 0; i < section[sect_num].lit_num; i++)
                {
                    if (section[sect_num].lit_table[i].literal[1] == 'C')                                   //Type C
                    {
                        strncpy_s(tempstr, 10, &section[sect_num].lit_table[i].literal[3], strlen(section[sect_num].lit_table[i].literal) - 4);
                        for (int n = 0; n < strlen(tempstr); n++)
                        {
                            section[sect_num].ob_code[section[sect_num].ob_num] |= (int)tempstr[n] << (strlen(tempstr) - n - 1) * 8;
                        }
                        size = 3;
                        hextoString(&section[sect_num].T[section[sect_num].t_num].record[num], section[sect_num].ob_code[section[sect_num].ob_num], 2 * size);
                        section[sect_num].ob_num++;
                        num += 2 * size;
                    }
                    else if (section[sect_num].lit_table[i].literal[1] == 'X')                              //Type X
                    {
                        strncpy_s(tempstr, 10, &section[sect_num].lit_table[i].literal[3], strlen(section[sect_num].lit_table[i].literal) - 4);
                        section[sect_num].ob_code[section[sect_num].ob_num] |= strtol(tempstr, NULL, 16);
                    }
                    else
                    {
                        strncpy_s(tempstr, 10, &section[sect_num].lit_table[i].literal[1], strlen(section[sect_num].lit_table[i].literal)-1);
                        section[sect_num].ob_code[section[sect_num].ob_num] |= (int)(tempstr[0] - '0');
                    }
                }
            }
            else if (!strcmp(token_table[token_line]->operator,"CSECT"))                                   //CSECT
            {
                locctr = 0;
                sect_num++;
                strcpy_s(section[sect_num].name, 10, section[sect_num].sym_table[0].symbol);
                section[sect_num].ob_num = 0;
                section[sect_num].ob_code[section[sect_num].ob_num] = -1;
            }
            else if ((index = search_opcode(token_table[token_line]->operator)) >= 0)                      //INST
            {
                if (token_table[token_line]->operator[0] == '+')                                           //extended type
                {
                    section[sect_num].ob_code[section[sect_num].ob_num] |= inst_table[index]->Opcode << 24;           //ADD opcode
                    section[sect_num].ob_code[section[sect_num].ob_num] |= token_table[token_line]->nixbpe << 20;     //ADD nixbpe
                    //ADD Modified table
                    section[sect_num].M_table[section[sect_num].M_num].operand = token_table[token_line]->operand[0];
                    section[sect_num].M_table[section[sect_num].M_num].location = locctr + 1;
                    section[sect_num].M_table[section[sect_num].M_num].length = 5;
                    section[sect_num].M_table[section[sect_num].M_num].Flag = '+';
                    section[sect_num].M_num++;
                    locctr += 4;
                }
                else if (inst_table[index]->format == 2)
                {
                    section[sect_num].ob_code[section[sect_num].ob_num] |= inst_table[index]->Opcode << 8;            //ADD opcode
                    section[sect_num].ob_code[section[sect_num].ob_num] |= search_regist(token_table[token_line]->operand[0]) << 4; //ADD Register
                    locctr += 2;

                    if (token_table[token_line]->operand[1] != NULL)
                        section[sect_num].ob_code[section[sect_num].ob_num] |= search_regist(token_table[token_line]->operand[1]);  //ADD Register
                }
                else if (inst_table[index]->format == 3)
                {
                    section[sect_num].ob_code[section[sect_num].ob_num] |= inst_table[index]->Opcode << 16;                         //ADD opcode
                    section[sect_num].ob_code[section[sect_num].ob_num] |= token_table[token_line]->nixbpe << 12;                   //ADD nixbpe
                    locctr += 3;

                    if (token_table[token_line]->nixbpe & (1 << 5) && token_table[token_line]->nixbpe & (1 << 4))                 //check Normal
                    {
                        if (token_table[token_line]->operand[0] != NULL)
                            targetAddr = search_symtab(token_table[token_line]->operand[0], sect_num) - locctr;
                    }
                    else if (token_table[token_line]->nixbpe & (1 << 5))                                                        //check indirect
                    {
                        targetAddr = search_symtab(&token_table[token_line]->operand[0][1], sect_num) - locctr;
                    }
                    else if (token_table[token_line]->nixbpe & (1 << 4))                                                       //check immediate
                    {
                        targetAddr = atoi(&token_table[token_line]->operand[0][1]);
                    }
                    else                                                                                                                //error
                        return -1;
                }
            }
            else                                                                                                                        //error
                return -1;
        }
        if (token_table[token_line]->operand[0] != NULL && token_table[token_line]->operand[0][0] == '=')                   //CALCULATE LITERAL
        {
            targetAddr = search_littab(token_table[token_line]->operand[0], sect_num) - locctr;
        }
        if (targetAddr < 0)                                                                                //If TA is nagative, Than MAKE 12bit
        {
            targetAddr &= 4095;
        }
        section[sect_num].ob_code[section[sect_num].ob_num] |= targetAddr;                                 //ADD TARGET_ADDRESS

        //INSERT T RECORD
        int value = section[sect_num].ob_code[section[sect_num].ob_num];
        if (section[sect_num].t_num == 0)                                                                  //SET T_RECORD START
            section[sect_num].T[section[sect_num].t_num].start = 0;
        if (value >= 0)                                                                                                                     
        {
            if (token_table[token_line]->operator != NULL)
            {
                if (value > 0 && value <= 0xff)                                                            //SET OBJECT_CODE SIZE
                {
                    if (!strcmp(token_table[token_line]->operator,"LTORG")) {
                        size = 3;
                    }
                    else
                        size = 1;
                }
                else if (value > 0xff && value <= 0xffff)
                    size = 2;
                else if (value > 0xffff && value <= 0xffffff || value == 0)
                    size = 3;
                else if (value > 0xffffff)
                    size = 4;

                if (strlen(section[sect_num].T[section[sect_num].t_num].record) + size > 60)                 //IF LENGTH > 0X1E, THAN LINEBREAK
                {
                    section[sect_num].t_num++;
                    section[sect_num].T[section[sect_num].t_num].start = preloc;
                    num = 0;
                }
                if (preval < 0)
                    section[sect_num].T[section[sect_num].t_num].start = preloc;

                hextoString(&section[sect_num].T[section[sect_num].t_num].record[num], value, 2 * size);   //CONVERT HEX TO STRING
                num += 2 * size;
            }
        }
        else                                                                                                            //OBJECT_CODE < 0
        {
            if (token_table[token_line]->operator != NULL && !strcmp(token_table[token_line]->operator,"CSECT"))        //IF MET CSECT, 
            {                                                                                                           //THAN MEMSET T_RECORD
                section[sect_num].t_num = 0;
                num = 0;
            }
            else if (preval >= 0)
            {
                section[sect_num].t_num++;
                num = 0;
            }
        }
        preloc = locctr;                                                                                                 //STORE PRE LOCATION
        preval = value;                                                                                                  //STORE PRE VALUE
    }
    return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
    FILE* file;
    if (file_name == NULL)                                      //Exception ����ó��
    {
        printf("Exception! Please check your file name.\n");
        return;
    }
    fopen_s(&file, file_name, "w");
    for (int i = 0; i <= sect_num; i++)
    {
        //WRITE HRECORD
        fprintf(file,"H%s\t%06X%06X\n", section[i].name, section[i].start, section[i].length);
        
        //WRITE EXTDEF
        if (section[i].D[0][0] != NULL)
        {
            fprintf(file, "D");
            for (int j = 0; j < MAX_OPERAND; j++)
                if (section[i].D[j] != NULL)
                    fprintf(file, "%s%06X", section[i].D[j], search_symtab(section[i].D[j], i));
            fprintf(file, "\n");
        }
        //WRITE EXTREF
        if (section[i].R[0][0] != NULL)
        {
            fprintf(file, "R");
            for (int j = 0; j < MAX_OPERAND; j++)
                if (section[i].R[j] != NULL)
                    fprintf(file, "%s", section[i].R[j]);
            fprintf(file, "\n");
        }
        //WRITE TRECORD
        for (int j = 0; j <= section[i].t_num; j++)
        {
            if (section[i].T[j].record[0] != NULL)
            {
                fprintf(file, "T");
                fprintf(file, "%06X%02X", section[i].T[j].start,strlen(section[i].T[j].record)/2);
                fprintf(file, "%s\n", section[i].T[j].record);
            }
            
        }

        //WRITE MRECORD
        for (int j = 0; j < section[i].M_num; j++)
        {
            fprintf(file, "M%06X%02X%c%s\n", section[i].M_table[j].location, section[i].M_table[j].length, 
                                                section[i].M_table[j].Flag, section[i].M_table[j].operand);
        }
        //WRITE ERECORD
        fprintf(file, "E");
        if (i == 0)
            fprintf(file, "%06X\n", section[i].start);
        else
            fprintf(file, "\n");

        fprintf(file, "\n");
    }
    fclose(file);
}
/* ----------------------------------------------------------------------------------
* ���� : ���� ���� SYMTAB�� �ִ� symbol�� �ּҰ��� ã���ش�.
* �Ű� : SYMTAB�� symbol��, ���� ��
* ��ȯ : ���� = symbol�� �ּ� ��, ���� < 0
* -----------------------------------------------------------------------------------
*/
int search_symtab(char* str, int sess)
{
    for (int i = 0; i < section[sess].sym_num; i++)
    {
        if (!strcmp(str, section[sess].sym_table[i].symbol))
        {
            return section[sess].sym_table[i].addr;
        }
    }

    return -1;
}
/* ----------------------------------------------------------------------------------
* ���� : ���� ���� LITERAL TABLE�� �ִ� literal�� �ּҰ��� ã���ش�.
* �Ű� : LITTAB�� literal��, ���� ��
* ��ȯ : ���� = literal�� �ּ� ��, ���� < 0
* -----------------------------------------------------------------------------------
*/
int search_littab(char* str, int sess)
{
    for (int i = 0; i < section[sess].lit_num; i++)
    {
        if (!strcmp(str, section[sess].lit_table[i].literal))
        {
            return section[sess].lit_table[i].addr;
        }
    }
    return -1;
}
int search_regist(char* str)
{
    for (int i = 0; i < MAX_REG; i++)
        if (!strcmp(str, reg[i].R))
            return reg[i].num;

    return -1;
}
/* ----------------------------------------------------------------------------------
* ���� : INTEGER ���� 16������ �޾Ƽ� String ������ ��ȯ.
* �Ű� : String���� ���� ������, String������ �ٲ� INTEGER, INTEGER�� size
* ��ȯ : ����
* -----------------------------------------------------------------------------------
*/
void hextoString(char* temp, int value ,short size) 
{
    for (int i = 0; i < size; i++)
    {
        *(temp + i) = (value >> 4 * (size - 1 - i) & 0xf);
        if (*(temp + i) >= 10)
            *(temp + i) = *(temp + i) - 10 + 'A';
        else
            *(temp + i) = *(temp + i) + '0';
    }

    return;
}