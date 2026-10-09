// VM - base version

// useful libraries---------------------
#include <stdio.h>
#include "stack.h"
#include "stack_func.cpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
// -------------------------------------

// useful macro-------------------------
#define EXECUTABLE_FILE "EXECUTE.txt"
#define ERROR_FILE "ERRORS_VM.log"
#define START_COMMAND_SIZE 5
#define INCREASE_COMMAND 2
#define MIN_MEMORY_SIZE 5
#define MIN_BUFFER_SIZE 1000
#define OPEN_FILE_FOR_REWRITING "a"
// -------------------------------------

// enums--------------------------------
enum PROG_CODE
{
    NO = 0,
    PUSH = 1,
    ADD = 2,
    SUB= 3,
    DIV = 4,
    OUT = 5,
    HLT = 6,
    DUMP = 7,
} ;
// -------------------------------------


// functions----------------------------
void destroy_variables (FILE* error_file, int* execute_file, int* commands, Stack* memory);
// -------------------------------------
// main()-------------------------------
int main() //close files!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
    // variables && etc.----------------
    FILE* error_file = NULL;
    int execute_file = 0;
    char* buffer_commands = NULL;
    int* commands = NULL;
    int ID = 0;
    PROG_CODE operation_code = NO;
    Stack memory = { } ;
    int temp = 0xBEDA;
    struct stat file_size = { .st_size = 0 } ;
    int len_commands_array = START_COMMAND_SIZE;
    Error_Codes er = NO_ERROR;
    // ---------------------------------

    // working area---------------------
    STACK_INIT(&memory, MIN_MEMORY_SIZE);

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

    if ((buffer_commands = (char*)calloc(MIN_BUFFER_SIZE, sizeof(char))) == NULL)
    {
        fprintf (error_file, "buffer for commands didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    stat (EXECUTABLE_FILE, &file_size);

    if ((len_commands_array = read (execute_file, buffer_commands, file_size.st_size)) == 0)
    {
        fprintf (error_file, "commands array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    buffer_commands [len_commands_array - 1] = '\0';

    if ((commands = (int*)calloc(len_commands_array, sizeof(int))) == NULL)
    {
        fprintf (error_file, "commands array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }
char* point_on_now_elem = buffer_commands;
int cnt_scanf = 0;

    for (int i = 0; i < len_commands_array - 1;)
    {
        if (*point_on_now_elem == '\n' || *point_on_now_elem == '\r' || *point_on_now_elem == ' ')
        {
            point_on_now_elem ++;
            continue;
        } else if (*point_on_now_elem == '\0')
        {
            len_commands_array = i;
            break;
        } else if (*point_on_now_elem < '9' && *point_on_now_elem > '0')
        {
            sscanf (point_on_now_elem, "%d%n", &commands [i], &cnt_scanf);
            if (cnt_scanf == 0)
            {
                // printf ("%d\n", i);
                len_commands_array = i;
                break;
            }
            // printf ("commands[%d] == %d\n", i, commands [i]);
            point_on_now_elem += cnt_scanf;
            i++;
            continue;
        } else
        {
            break;
        }

    }

    while (ID < len_commands_array)
    {
        operation_code = (PROG_CODE)commands[ID];   //7 бед - один ответ: костыль и велосипед :)
        switch (operation_code)
        {
            case PUSH :
                // printf ("push\n");
                stack_push (&memory, commands [ID + 1]);
                // STACK_DUMP(&memory, &er, error_file);
                ID += 2;
                break;

            case ADD :
                // printf ("add\n");
                temp = sum_first_stack_elem (&memory);
                // printf ("add_temp = %d\n", temp);
                stack_clear (&memory);
                stack_push (&memory, temp);
                // STACK_DUMP(&memory, &er, error_file);
                ID ++ ;
                break;

            case SUB :
                // printf ("sub\n");
                temp = unsum_first_stack_elem (&memory);
                // printf ("sub_temp = %d\n", temp);
                stack_clear (&memory);
                stack_push (&memory, temp);
                // STACK_DUMP(&memory, &er, error_file);
                ID ++ ;
                break;

            case DIV :
                // printf ("div\n");
                temp = div_first_stack_elem (&memory);
                // printf ("div_temp = %d\n", temp);
                stack_clear (&memory);
                stack_push (&memory, temp);
                // STACK_DUMP(&memory, &er, error_file);
                ID ++ ;
                break;

            case OUT :
                // STACK_DUMP(&memory, &er, error_file);
                stack_pop (&memory, &temp);
                // printf ("out_temp = %d\n", temp);
                printf ("result: %d\n", temp);
                ID ++ ;
                break;

            case HLT :
                destroy_variables (error_file, &execute_file, commands, &memory);
                return 0;

            case DUMP :
                // printf ("dump\n");
                STACK_DUMP(&memory, &er, error_file);
                ID ++ ;
                break;



            default : printf ("default"); ID ++;
                break;
        }
        // printf ("next ID == %d\n", ID);
    }

    destroy_variables (error_file, &execute_file, commands, &memory);

    return 0;
    // ----------------------------------
}
// --------------------------------------

// functions-----------------------------
void destroy_variables (FILE* error_file, int* execute_file, int* commands, Stack* memory)
{
    fclose (error_file);
    close (*execute_file);
    free (commands);
    stack_destroy (memory);
}
// -------------------------------------

// THE END------------------------------
