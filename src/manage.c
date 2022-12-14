#include "manage.h"

/* ---------------------------------------------------------------------------------
 * function name : _input_grade
 * return value
 *  - num : input number in range
--------------------------------------------------------------------------------- */
float _input_grade()
{
    float num;

    while(1)
    {
        scanf("%f",&num);
        _IO_getc(stdin);

        if(num>=0.0 && num<=100.0)
            return num;
        else
            printf("범위를 벗어난 숫자 입니다. 다시 입력하세요 :");
    }

}



/* ---------------------------------------------------------------------------------
 * function name : _file_read
 * return value
 *  - ERROR : file open fail
 *  - NO_ERROR : NO ERROR
--------------------------------------------------------------------------------- */
int _file_read(STUDENT** file_mem,char* filename, __uint64* num)
{
    FILE* fp;
    STUDENT* tmp=NULL;

    if((fp = fopen(filename, "r")) == NULL)
        return ERROR;
    
    fscanf(fp,"%lld%*c",num);

    tmp = (STUDENT*)malloc(sizeof(STUDENT)*(*num));

    for(int i=0; i<(*num); i++)
    {
        fscanf(fp,"%lld%%*c",&tmp[i].id);
        fscanf(fp,"%[^|]%*c", tmp[i].name);
    
        for(int j=0; j<MAX_GRADE_NUM; j++)
        {
            fscanf(fp,"%f", &tmp[i].grade[j]);
        }
        fscanf(fp,"%f",&tmp[i].average);
    }

    *file_mem = tmp;

    fclose(fp);
    return NO_ERROR;
}



/* ---------------------------------------------------------------------------------
 * function name : swap
 * arguments
 *  - stu1 : STUDENT member to swap stu2
 *  - stu2 : STUDENT member to swap stu1
--------------------------------------------------------------------------------- */
void swap(STUDENT* stu1, STUDENT* stu2)
{
    STUDENT tmp = *stu2;
    *stu2 = *stu1;
    *stu1 = tmp;
}



/* ---------------------------------------------------------------------------------
 * function name : mystrcpy
 * arguments
 *  - str1 : string 1 to paste
 *  - str2 : string 2 to copy
 * retrun vale
 *  str1 : copied string address
--------------------------------------------------------------------------------- */
char* mystrcpy(char *str1, const char *str2)
{
	char* tmp = str1;

    while(*str2)
    {
        *tmp++ = *str2++;
    }

    *tmp = '\0';
    return str1;
}



/* ---------------------------------------------------------------------------------
 * function name : mystrcmp
 * arguments
 *  - str1 : string 1 to compare
 *  - str2 : string 2 to comppare
 * retrun vale
 *  - str1 > str2  => + , Positive
 *  - str1 == str2 => 0 , Zero
 *  - str1 < str2  => - , Negative
--------------------------------------------------------------------------------- */
char mystrcmp(const char* str1, const char* str2)
{
    while(*str1 && (*str1 == *str2))
    {
        str1++; 
        str2++;
    }
    
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}




/* ---------------------------------------------------------------------------------
 * function name : init
 * arguments
 *  - htbl : hash table struct pointer for initialization
 *  - pfilename : filename to open
 * return value
 *  - ERROR : Error signal to terminate program [file open fail || hash table initialize fail || data insert fail]
 *  - NO_ERROR : Continuation siganl to execute program
--------------------------------------------------------------------------------- */
int init(OHTBL* htbl, char* pfilename)
{
    STUDENT* member=NULL;
    __uint64 num,pos=0;

     
    if(_file_read(&member, pfilename, &num) == ERROR)
    {
        printf("%s 파일을 열 수 없습니다.\n",pfilename);
        return ERROR;
    }


    if(ohtbl_init(htbl, num) == ERROR)
    {
        printf("해시 테이블 초기화에 실패했습니다.\n");
        return ERROR;
    }
        
    for(__uint64 i=0; i<num; i++)
    {
        pos = ohtbl_insert(htbl,&member[i]);
        if( pos == ERROR )
        {    
            printf("파일 데이터를 해시 테이블에 삽입하는데 실패했습니다.\n");
            return ERROR;
        }
    }

    free(member);
    return NO_ERROR;
}



/* ---------------------------------------------------------------------------------
 * function name : insert_student
 * arguments
 *  - htbl : hash table struct pointer for data insert
 * return value
 *  - ERROR : Error signal to terminate program [data insert fail]
 *  - NO ERROR : NO ERROR
--------------------------------------------------------------------------------- */
int insert_student(OHTBL* htbl)
{
    STUDENT mem_tmp;
    __uint64 pos=0;
    int input_num=0;

    printf("\n-----------------------------학생 정보 입력------------------------------------------\n");
    printf("[예시. 학번 : 20220001 | 이름 : Hong gil dong | 국어 : 100 | 수학 : 100 | 영어 : 100]\n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("입력할 학생 수를 입력해주세요 : ");
    scanf("%d", &input_num);    _IO_getc(stdin);

    for(int i=0; i<input_num; i++)
    {
        printf("\n남은 입력 횟수 : %d\n", input_num-i);
        printf("********************\n");
        printf("> 학번 : ");
        scanf("%lld",&mem_tmp.id);    _IO_getc(stdin);

        printf("> 이름 : ");  
        scanf("%[^\n]%*c",mem_tmp.name); 

        printf("\n과목별 성적\n");
        printf("> 국어 : ");
        mem_tmp.grade[0] = _input_grade();       

        printf("> 수학 : ");
        mem_tmp.grade[1] = _input_grade();      

        printf("> 영어 : ");
        mem_tmp.grade[2] = _input_grade();       
        printf("*********************\n\n");

        // Calculate Avergage
        mem_tmp.average = (( mem_tmp.grade[0] + mem_tmp.grade[1] + mem_tmp.grade[2] ) / (float)MAX_GRADE_NUM );

        pos = ohtbl_insert(htbl,&mem_tmp);
        if( pos == ERROR )
        {    
            printf("해시 테이블에 삽입하는데 실패했습니다.\n");
            return ERROR;
        }
        else if(pos == FOUND)
        {
            printf("ID = %lld는 이미 존재합니다.\n\n",mem_tmp.id);
        }
    }
    return NO_ERROR;
}



/* ---------------------------------------------------------------------------------
 * function name : remove_student
 * arguments
 *  - htbl : hash table struct pointer for data remove
 * return value
 *  - ERROR : Error signal to terminate program [data remove fail]
 *  - NO_ERROR : NO DATA
--------------------------------------------------------------------------------- */
int remove_student(OHTBL* htbl)
{
    if(htbl->size)
    {
        __uint64 remove_id;
        __uint64 pos=0;

        printf("\n-----------------------------학생 정보 삭제------------------------------------\n");
        printf("삭제할 학생의 학번을 입력하세요.\n");

        printf("> 학번 : ");
        scanf("%lld",&remove_id); _IO_getc(stdin);

        pos = obtbl_remove(htbl,remove_id);
        if(pos == ERROR)
        {    
            printf("\nREMOVE ERROR");
            return ERROR;
        }
        else if(pos == NO_DATA)
        {
            printf("\n=> ID = %lld는 존재하지 않습니다.\n\n", remove_id);
        }
        else
        {
            printf("\n=> 삭제했습니다. ID = %lld, name = %s\n\n", remove_id, htbl->table[pos].name);
        }
    }
    else
        printf("파일에 정보가 없습니다.\n");
    
    return NO_ERROR;
    
}



/* ---------------------------------------------------------------------------------
 * function name : exit_program
 * arguments
 *  - htbl : hash table struct pointer for data write
 *  - pfilename : file name to write
 * retrun vale
 *  - ERROR : Error signal to terminate program [file open fail]
 *  - NO_ERROR : NO ERROR
--------------------------------------------------------------------------------- */
int exit_program(OHTBL* htbl, char* pfilename)
{
    if(update_flag)
    {
        FILE* fp;
        __uint64 pos=0;
        if ((fp = fopen(pfilename, "w")) == NULL)
            return ERROR;  

        fprintf(fp,"%lld",htbl->size);
        fprintf(fp,"\n");

        for(__uint64 i=0; i<htbl->positions; i++)
        {
            pos = ohtbl_lookup(htbl,htbl->table[i].id);
            if(pos != NOT_FOUND && pos != NO_DATA)
            {
                fprintf(fp,"%08lld",htbl->table[i].id);
                fprintf(fp," %-30s|",htbl->table[i].name);

                for(int j=0; j<MAX_GRADE_NUM; j++)
                {
                    fprintf(fp,"%.2f ", htbl->table[i].grade[j]);
                }
                fprintf(fp,"%.2f ", htbl->table[i].average);
                fprintf(fp,"\n");
            }
        }

        fclose(fp);
    }
    
    free(htbl->table);
    return NO_ERROR;
}