// VM - base version

// useful libraries---------------------
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "stack.h"
#include "stack_func.cpp"
#include "commands.h"
// -------------------------------------

// useful macro-------------------------
#define EXECUTABLE_FILE "EXECUTE.txt"
#define ERROR_FILE "ERRORS_VM.log"
// #define START_COMMAND_SIZE 5
#define INCREASE_COMMAND 2
#define MIN_MEMORY_SIZE 20
#define MIN_BUFFER_SIZE 1000
#define OPEN_FILE_FOR_REWRITING "a"
#define MIN_CODE_LEN 1000
// -------------------------------------

// SPU_struct---------------------------
struct SPU
{
    double AX; //number 1
    double BX; //number 2
    double CX; //number 3
    double DX; //number 4
    int code [MIN_CODE_LEN];
    int len_code;
    int IP;
    // ram;
    // ret_call_cnt;
} ;
// -------------------------------------


// functions----------------------------
void get_commands_from_file (char* buffer_commands, struct SPU*  VM);
void run_programme (struct SPU* VM, struct Stack* vm_memory, FILE* error_file);
void PUSH_decode (struct SPU* VM, struct Stack* vm_memory);
void ADD_decode (struct SPU* VM, struct Stack* vm_memory);
void SUB_decode (struct SPU* VM, struct Stack* vm_memory);
void MULL_decode (struct SPU* VM, struct Stack* vm_memory);
void DIV_decode (struct SPU* VM, struct Stack* vm_memory);
void OUT_decode (struct SPU* VM, struct Stack* vm_memory);
void DUMP_decode (struct SPU* VM, struct Stack* vm_memory, FILE* error_file);
void JMP_decode (struct SPU* VM);
void destroy_variables (FILE* error_file, int* execute_file, Stack* vm_memory);
// -------------------------------------

// main()-------------------------------
int main()
{
    // variables && etc.----------------
    struct stat file_size = { .st_size = 0 } ;
    struct SPU VM = { } ;
    Stack vm_memory = { } ;
    FILE* error_file = NULL;
    int execute_file = 0;
    char buffer_commands [MIN_BUFFER_SIZE] = { };
    int len_buffer_commands = 0;
    // ---------------------------------

    // working area---------------------
    STACK_INIT(&vm_memory, MIN_MEMORY_SIZE);

    if ((error_file = fopen (ERROR_FILE, OPEN_FILE_FOR_REWRITING)) == NULL)
    {
        error_file = stderr;
        printf (ERROR_FILE " wasn't open -> errors'll printf to stderr");
        fprintf (error_file, ERROR_FILE " wasn't open, sorry.\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
    }

    if ((execute_file = open(EXECUTABLE_FILE, O_RDONLY)) == 0)
    {
        fprintf (error_file, EXECUTABLE_FILE " wasn't open -> programme stoped, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort();
    }

    stat (EXECUTABLE_FILE, &file_size);

    if ((len_buffer_commands = read (execute_file, buffer_commands, file_size.st_size)) == 0)
    {
        fprintf (error_file, "commands array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    buffer_commands [len_buffer_commands - 1] = '\0';

    get_commands_from_file (buffer_commands, &VM);

    run_programme (&VM, &vm_memory, error_file);

    destroy_variables (error_file, &execute_file, &vm_memory);
}
// -------------------------------------

// functions----------------------------
void get_commands_from_file (char* buffer_commands, struct SPU* VM)
{
    for (int i = 0; buffer_commands [i] != '\0'; i++)
    {
        if (isdigit (buffer_commands [i]) || buffer_commands [i] == '-')
        {
            int temp_arg = 0;
            int minus = 0;

            if (buffer_commands [i] == '-')
            {
                i ++ ;
                minus = 1;
            }

            while (buffer_commands [i] != '\n' && buffer_commands [i] != ';' && buffer_commands [i] != ' ')
            {
                temp_arg = 10 * temp_arg + (int)(buffer_commands [i] - '0');
                (i) ++ ;
            }

            if (minus)
                temp_arg = -1 * temp_arg;
            VM -> code [VM -> len_code] = temp_arg;
            VM -> len_code ++ ;
        }
    }
}

void run_programme (struct SPU* VM, struct Stack* vm_memory, FILE* error_file)
{
    int hlt_flag = 1;

    while (VM -> IP < VM -> len_code && hlt_flag)
    {
        switch (VM -> code [VM -> IP])
        {
            case PUSH : PUSH_decode (VM, vm_memory);
            break;

            case ADD : ADD_decode (VM, vm_memory);
            break;

            case SUB : SUB_decode (VM, vm_memory);
            break;

            case MULL : MULL_decode (VM, vm_memory);
            break;

            case DIV : DIV_decode (VM, vm_memory);
            break;

            case OUT : OUT_decode (VM, vm_memory);
            break;

            case DUMP : DUMP_decode (VM, vm_memory, error_file);
            break;

            case JMP : JMP_decode (VM);
            break;

            case HLT : hlt_flag = 0;
            break;

            default : hlt_flag = 0;
        }
    }
}

void PUSH_decode (struct SPU* VM, struct Stack* vm_memory)
{
    VM -> IP ++ ;
    stack_push (vm_memory, VM -> code [VM -> IP]);
    VM -> IP ++ ;
}

void ADD_decode (struct SPU* VM, struct Stack* vm_memory)
{
    int temp = sum_first_stack_elem (vm_memory);
    printf ("add temp == %d", temp);
    stack_clear (vm_memory);
    stack_push (vm_memory, temp);

    VM -> IP ++ ;
}

void SUB_decode (struct SPU* VM, struct Stack* vm_memory)
{
    int temp = unsum_first_stack_elem (vm_memory);
    stack_clear (vm_memory);
    stack_push (vm_memory, temp);

    VM -> IP ++ ;
}

void MULL_decode (struct SPU* VM, struct Stack* vm_memory)
{
    int temp = mull_first_stack_elem (vm_memory);
    stack_clear (vm_memory);
    stack_push (vm_memory, temp);

    VM -> IP ++ ;
}

void DIV_decode (struct SPU* VM, struct Stack* vm_memory)
{
    int temp = div_first_stack_elem (vm_memory);
    stack_clear (vm_memory);
    stack_push (vm_memory, temp);

    VM -> IP ++ ;
}

void OUT_decode (struct SPU* VM, struct Stack* vm_memory)
{
    TYPE_OF_STACK_ELEM temp = 0xBEDA;
    stack_pop (vm_memory, &temp);
    printf (GREEN_START "result of OUT: " COLOR_STOP SPECIFICATOR_TYPE, temp);

    VM -> IP ++ ;
}

void DUMP_decode (struct SPU* VM, struct Stack* vm_memory, FILE* error_file)
{
    Error_Codes error_unused = NO_ERROR;
    STACK_DUMP (vm_memory, &error_unused, error_file);

    VM -> IP ++ ;
}

void JMP_decode (struct SPU* VM)
{
    VM -> IP = VM -> code [VM -> IP + 1];
}

void destroy_variables (FILE* error_file, int* execute_file, Stack* vm_memory)
{
    assert (error_file);
    assert (execute_file);
    assert (vm_memory);

    fclose (error_file);
    close (*execute_file);
    stack_destroy (vm_memory);
}
// -------------------------------------

// THE END------------------------------
