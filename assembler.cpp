// assembler - simple version
//TODO: rewrite JMP - it must have number of str, where we want to go

// useful libraries--------------
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "stack.h"
#include "commands.h"
// ------------------------------

// macro-------------------------
#define USER_COMAND_FILE "calc.txt"
#define ERROR_FILE "ERRORS_AS.log"
#define EXECUTABLE_FILE "EXECUTE.txt"
#define OPEN_FILE_FOR_REWRITING "a"
#define MIN_BUFFER_SIZE 1000
#define MAX_COMMAND_LEN 1000
#define MIN_CODE_LEN 1000
#define LABEL_CNT 20
#define NO_ARG_COMMANDS_CNT 7
// ------------------------------

// struct------------------------
struct NO_ARG_COMMANDS
    {
        int cnt;
       const char* names [NO_ARG_COMMANDS_CNT];
       PROG_CODE command_enum [NO_ARG_COMMANDS_CNT];
    } ;
// ------------------------------

// functions---------------------
void label_init (int* label);
int assembly (char* buffer_commands, long long* len_buffer_commands, int* code, int* label, FILE* error_file);
void PUSH_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem);
void POP_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem);
void JMP_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem, FILE* error_file, int* label);
void null_now_command (char* now_command, int* len_now_command);
void twice_assembly (char* buffer_commands, int* code, int len_code, int* label);
PROG_CODE decode_command (char* now_command);
void create_execute_file (int* code, int len_code, FILE* error_file);
void destroy_variables (FILE* error_file, char* buffer_commands, int user_comand_file, int* code);
// ------------------------------

// main--------------------------
int main ()
{
    // variables && etc.---------
    FILE* error_file = NULL;
    int user_comand_file = 0;

    struct stat file_size = { .st_size = 0 } ;
    long long len_buffer_commands = 0;
    // char* buffer_commands = NULL;
    char buffer_commands [1000] = { } ;

    int* code = NULL;
    int len_code = 0;
    int label [LABEL_CNT] = { } ;
    // --------------------------

    // working area--------------
    if ((error_file = fopen (ERROR_FILE, OPEN_FILE_FOR_REWRITING)) == NULL)
    {
        error_file = stderr;
        printf (ERROR_FILE " wasn't open -> errors'll printf to stderr");
        fprintf (error_file, ERROR_FILE " wasn't open, sorry.\nfile: %s, func: main.cpp: %d\ndate: %s time: %s\n",
                                                                         __FILE__, __LINE__, __DATE__, __TIME__);
    }

    if ((user_comand_file = open (USER_COMAND_FILE, O_RDONLY)) == 0)
    {
        fprintf (error_file, USER_COMAND_FILE " wasn't open, sorry.\nfile: %s, func: main.cpp: %d\ndate: %s time: %s\n",
                                                                         __FILE__, __LINE__, __DATE__, __TIME__);
        abort();
    }

    // if ((buffer_commands = (char*)calloc(MIN_BUFFER_SIZE, sizeof(char))) == NULL)
    // {
    //     fprintf (error_file, "buffer for commands didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
    //     abort ();
    // }

    stat (USER_COMAND_FILE, &file_size);

    if ((len_buffer_commands = read (user_comand_file, buffer_commands, file_size.st_size)) == 0)
    {
        fprintf (error_file, "commands array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    buffer_commands [len_buffer_commands - 1] = '\0';

    if ((code = (int*)calloc (MIN_CODE_LEN, sizeof(int))) == NULL)
    {
        fprintf (error_file, "code_array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }


    label_init (label);

    len_code = assembly (buffer_commands, &len_buffer_commands, code, label, error_file);

// // -===-
//     for (int  i = 0; i < len_code; i ++)
//     {
//         printf ("[%d] == " "%d" "\n", i, code [i]);
//     } printf ("\n-------------------------------------------------------------------------------------\n");
// // -===-
    twice_assembly (buffer_commands, code, len_code, label);
// // -===-
//     for (int  i = 0; i < len_code; i ++)
//     {
//         printf ("[%d] == " "%d" "\n", i, code [i]);
//     } printf ("\n");
// //-===-

    create_execute_file (code, len_code, error_file);

    destroy_variables (error_file, buffer_commands, user_comand_file, code);
    // --------------------------

}
// ------------------------------

// functions---------------------
void label_init (int* label)
{
    assert (label);

    for (int i = 0; i < LABEL_CNT; i ++)
    {
        label [i] = -100;
    }

}

int assembly (char* buffer_commands, long long* len_buffer_commands, int* code, int* label, FILE* error_file)
{
    assert (buffer_commands);
    assert (code);
    assert (label);

    // variables && etc.---------
    // long long len_realloc_code = MIN_CODE_LEN;

    char now_command [MAX_COMMAND_LEN] = {0} ;
    int len_now_command = 0;

    int code_elem = 0;
    // --------------------------

    // working area--------------
    for (long long i = 0; i <= *len_buffer_commands; i ++)
    {
            printf ("buffer[%lld] == <%c>, now_command == <%s>\n", i, buffer_commands [i], now_command);

        if (buffer_commands [i] == ';')
        {
            while (buffer_commands [i] != '\n')
            {
              // printf ("in: - buffer_commands [%lld] == <%c>", i,
              // buffer_commands [i]);
              i++;
            }
            continue;
        }

        if (buffer_commands [i] == ':')
        {
            int temp_label = 0;
            i ++ ;

            if (!isdigit(buffer_commands [i]))
            {
                fprintf (error_file, "SORRY, BUT YOU USE NO_EXISTENT LABEL (must 0 < label < %d)\nREAD documentation, plz :)\n", LABEL_CNT);
                fprintf (error_file, "YOU LABEL START WITH: %c\n", buffer_commands [i]);
                exit (-1);
            }

            while (buffer_commands [i] != '\n')
            {
                temp_label = temp_label * 10 + (buffer_commands[i] - '0');
                i ++ ;
            }

            label [temp_label] = code_elem + 1;
        }

        if (isalpha (buffer_commands [i]))
        {
            now_command [len_now_command] = buffer_commands [i];
            len_now_command ++ ;
            continue;
        }

        if (buffer_commands [i] == ' ')
        {

            if (!strcmp (now_command, "PUSH"))
            {
                PUSH_decode (buffer_commands, &i, now_command, &len_now_command, code, &code_elem);
            // printf ("after PUSH_decode: i == %lld]\n", i);

                continue ;
            }

            if (!strcmp (now_command, "POP"))
            {
                POP_decode (buffer_commands, &i, now_command, &len_now_command, code, &code_elem);
                continue ;
            }

            if (!strcmp (now_command, "JMP"))
            {
                JMP_decode (buffer_commands, &i, now_command, &len_now_command, code, &code_elem, error_file, label);
                continue;
            }

            fprintf (error_file, "SORRY, extra space after [now_command]: %s\nIt killed me :(\n", now_command);
            exit (-1);
        }

        if (buffer_commands [i] == '\n')
        {
            if (fabs(code [code_elem - 2] - (TYPE_OF_STACK_ELEM)PUSH) < 0.0000001 && len_now_command == 0)
                continue;

            PROG_CODE temp_code = NO;
            if ((temp_code = decode_command (now_command)) == NO )
            {
                printf (VIOLET_START "WARNING! (check " ERROR_FILE ")\n" COLOR_STOP);
                fprintf (error_file, "WARNING!!! command is NO\n");
            }

            code [code_elem] = (int)temp_code;
            code_elem ++ ;
            null_now_command (now_command, &len_now_command);
            continue;
        }

        if (buffer_commands [i] == '\0')
        {
            PROG_CODE temp_code = NO;
            if ((temp_code = decode_command (now_command)) == NO )
            {
                printf (VIOLET_START "WARNING! (check " ERROR_FILE ")\n" COLOR_STOP);
                fprintf (error_file, "WARNING!!! command is NO\n");
            }

            code [code_elem] = (int)temp_code;
            code_elem ++ ;
            null_now_command (now_command, &len_now_command);
            *len_buffer_commands = i + 1;
            break;
        }
    }

    return code_elem;
    // --------------------------
}

void PUSH_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem)
{
    assert (buffer_commands);
    assert (i);
    assert (now_command);
    assert (code);
    assert (code_elem);

    char minus ='\0';
    int temp_arg = 0;

    (*i) ++ ;

    if (buffer_commands [*i] == '-')
    {
        minus = buffer_commands [*i];
        (*i) ++ ;
    }

    while (buffer_commands [*i] != '\n' && buffer_commands [*i] != ';')
    {
        // printf ("in PUSH_decode: buffer_commands[%lld] == <%c>\n", *i, buffer_commands [*i]);

        temp_arg = 10 * temp_arg + (buffer_commands [*i] - '0');
        (*i) ++ ;
    }

    (*i) -- ;

    if (minus == '-')
    {
        temp_arg = (-1) * temp_arg;
    }

    code [*code_elem] = (int)PUSH;
    (*code_elem) ++ ;
    code [*code_elem] = temp_arg;
    (*code_elem) ++ ;

    null_now_command (now_command, len_now_command);
}

void POP_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem)
{
    assert (buffer_commands);
    assert (i);
    assert (now_command);
    assert (code);
    assert (code_elem);

    char minus ='\0';
    int temp_arg = 0;

    (*i) ++ ;

    if (buffer_commands [*i] == '-')
    {
        minus = buffer_commands [*i];
        (*i) ++ ;
    }

    while (buffer_commands [*i] != '\n')
    {
        temp_arg = 10 * temp_arg + (buffer_commands [*i] - '0');
        (*i) ++ ;
    }

    if (minus == '-')
    {
        temp_arg = (-1) * temp_arg;
    }

    code [*code_elem] = (int)POP;
    (*code_elem) ++ ;
    code [*code_elem] = temp_arg;
    (*code_elem) ++ ;

    null_now_command (now_command, len_now_command);
}

void JMP_decode (char *buffer_commands, long long* i, char* now_command, int* len_now_command, int* code, int* code_elem, FILE* error_file, int* label)
{
    assert (buffer_commands);
    assert (i);
    assert (now_command);
    assert (code);
    assert (code_elem);

    int temp_arg = 0;

    (*i) ++ ;

    code [*code_elem] = (int)JMP;
    (*code_elem) ++ ;

    if (buffer_commands [*i] == ':')
    {
        (*i) ++ ;
        while (buffer_commands [*i] != '\n')
        {
            temp_arg = 10 * temp_arg + (buffer_commands [*i] - '0');
            (*i) ++ ;
        }

        if (temp_arg < 0 || temp_arg > LABEL_CNT)
        {
            fprintf (error_file, "SORRY, but JMP use strange label - :%d", temp_arg);
            exit (-1);
        }

        code [*code_elem] = label [temp_arg];
        (*code_elem) ++ ;

         null_now_command (now_command, len_now_command);
    } else
    {
        while (buffer_commands [*i] != '\n')
        {
            temp_arg = 10 * temp_arg + (buffer_commands [*i] - '0');
            (*i) ++ ;
        }

        code [*code_elem] = temp_arg;
        (*code_elem) ++ ;

        null_now_command (now_command, len_now_command);
    }
}

void null_now_command (char* now_command, int* len_now_command)
{
    assert (now_command);
    assert (len_now_command);

    for (int i = 0; i < *len_now_command; i++)
    {
        now_command [i] = '\0';
    }

    *len_now_command = 0;
}

void twice_assembly (char* buffer_commands, int* code, int len_code, int* label)
{
    assert (code);
    assert (label);

    // variables && etc.-------------------
    int temp_arg = 0;
    int seek_jmp = 0;
    long long seek_j = 0;
    // ------------------------------------

    while (buffer_commands [seek_j] != '\0')
    {
        long long seek_label = 0;
        temp_arg = 0;

        while (buffer_commands [seek_j] != 'J' && buffer_commands [seek_j] != '\0')
            seek_j ++;

        if (buffer_commands [seek_j] == '\0')
            break;

        seek_label = seek_j;

        for (int j = seek_jmp; j < len_code; j ++)
        {
            if (fabs(code [j] - (int)JMP) < 0.000001)
            {
                seek_jmp = j;
                break;
            }
        }
        seek_jmp ++ ;

        while (buffer_commands [seek_label] != ':' && buffer_commands [seek_label] != '\n')
            seek_label ++ ;

        if (buffer_commands [seek_label] == '\n')
            continue;
        else
            seek_label ++ ;

        while (buffer_commands [seek_label] != '\n')
        {
            temp_arg = 10 * temp_arg + (buffer_commands [seek_label] - '0');
            seek_label ++ ;
        }

        if (fabs(code [seek_jmp] - label [temp_arg]) > 0.00001)
            code [seek_jmp] = label [temp_arg];

    seek_j ++ ;
    seek_jmp ++ ;

    }

}

PROG_CODE decode_command (char* now_command) //TODO - мини структурки для каждой команды (имя + enum) - в основной структуре массив из маленьких
{
    struct NO_ARG_COMMANDS commands =
    {
        .cnt = 7,
        .names =
                {
                    "ADD",
                    "SUB",
                    "MULL",
                    "DIV",
                    "OUT",
                    "HLT",
                    "DUMP"
                } ,
        .command_enum =
                {
                    ADD,
                    SUB,
                    MULL,
                    DIV,
                    OUT,
                    HLT,
                    DUMP
                }
    } ;


    for (int i = 0; i < commands.cnt; i ++)
    {
        if (!strcmp (now_command, commands.names [i])) return commands.command_enum [i];
    }

    return NO;
}

void create_execute_file (int* code, int len_code, FILE* error_file)
{
    assert (code);

    FILE* execute_file = NULL;

    if ((execute_file = fopen(EXECUTABLE_FILE, "w")) == 0)
    {
        fprintf (error_file, EXECUTABLE_FILE " wasn't open -> programme stoped, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort();
    }

    for (int i = 0; i < len_code; i ++)
    {
        fprintf (execute_file, "%d", code [i]);
        if (code [i] == PUSH || code [i] == JMP)
        {
            i ++ ;
            fprintf (execute_file, " %d\n", code [i]);
        } else
            fprintf (execute_file, "\n");
    }

    fclose (execute_file);
}


void destroy_variables (FILE* error_file, char* buffer_commands, int user_comand_file, int* code)
{
    assert (error_file);
    assert (buffer_commands);

    fclose (error_file);
    close (user_comand_file);
    free (buffer_commands);
    free (code);
}
// ------------------------------

// THE END-----------------------
