// assembler - simple version

// useful libraries--------------
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// ------------------------------

// macro-------------------------
#define USER_COMAND_FILE "calc.txt"
#define ERROR_FILE "ERRORS_AS.log"
#define EXECUTABLE_FILE "EXECUTE.txt"
#define OPEN_FILE_FOR_REWRITING "a"
#define OPEN_FILE_FOR_WRITING "w"
#define MIN_BUFFER_SIZE 1000
#define MAX_COMMAND_LEN 20
// ------------------------------

// functions---------------------
void decode_comand_file (char* buffer_commands, long long len_commands_buffer, FILE* execute_file);
void destroy_variables (FILE* error_file, FILE* execute_file, char* buffer_commands, int user_comand_file);
// ------------------------------

// main--------------------------
int main ()
{
    // variables && etc.---------
    FILE* error_file = NULL;
    FILE* execute_file = NULL;
    int user_comand_file = 0;
    struct stat file_size = { .st_size = 0 } ;
    long long len_commands_buffer = 0;
    char* buffer_commands = NULL;
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

    if ((execute_file = fopen(EXECUTABLE_FILE, OPEN_FILE_FOR_WRITING)) == 0)
    {
        fprintf (error_file, EXECUTABLE_FILE " wasn't open -> programme stoped, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort();
    }

    if ((buffer_commands = (char*)calloc(MIN_BUFFER_SIZE, sizeof(char))) == NULL)
    {
        fprintf (error_file, "buffer for commands didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    stat (USER_COMAND_FILE, &file_size);

    if ((len_commands_buffer = read (user_comand_file, buffer_commands, file_size.st_size)) == 0)
    {
        fprintf (error_file, "commands array didn't create, sorry...\nfile: %s, func: main.cpp: %d\n", __FILE__, __LINE__);
        abort ();
    }

    buffer_commands [len_commands_buffer - 1] = '\0';

    decode_comand_file (buffer_commands, len_commands_buffer, execute_file);

    destroy_variables (error_file, execute_file, buffer_commands, user_comand_file);
    // --------------------------

}
// ------------------------------

// functions---------------------
void decode_comand_file (char* buffer_commands, long long len_commands_buffer, FILE* execute_file)
{
    // variables && etc.---------
    char now_command [MAX_COMMAND_LEN] = {0} ;
    int cnt_now_command = 0;
    // --------------------------
        for (int i = 0; i < len_commands_buffer; i ++)
    {
        printf ("nachalo: %s, cnt: %d\n", now_command, cnt_now_command);
        if (buffer_commands [i] == '\r' || buffer_commands [i] == ' ')
            continue;

        if (isalpha (buffer_commands [i]) && buffer_commands [i] != '\n' && buffer_commands [i] != ' ')
        {
            printf ("alpha: %c\n", buffer_commands [i]);

            now_command [cnt_now_command] = buffer_commands [i];
            cnt_now_command ++ ;
        } else if (buffer_commands [i] == '\n' || buffer_commands [i] == '\0')
        {
            printf ("<%s>", now_command);
            if (!strcmp (now_command, "ADD"))
            {
                fprintf (execute_file, "2\n");
                printf ("2\n");
            } else if (!strcmp (now_command, "SUB"))
            {
                fprintf (execute_file, "3\n");
                printf ("3\n");
            } else if (!strcmp (now_command, "DIV"))
            {
                fprintf (execute_file, "4\n");
                printf ("4\n");
            } else if (!strcmp (now_command, "OUT"))
            {
                fprintf (execute_file, "5\n");
                printf ("5\n");
            } else if (!strcmp (now_command, "HLT"))
            {
                fprintf (execute_file, "6\n");
                printf ("6\n");
            } else if (!strcmp (now_command, "DUMP"))
            {
                fprintf (execute_file, "7\n");
                printf ("7\n");
            }

            for (int j = 0; j < cnt_now_command; j ++)
            {
                now_command [j] = '\0';
            }

            for (int j = 0; j < cnt_now_command; j ++)
            {
                printf ("%d ", now_command [j]);
            }
            printf ("\n");
            cnt_now_command = 0;

        } else if (buffer_commands [i] == '\0')
        {
            break;
        } else
        {

            if (!strcmp (now_command, "PUSH\0"))
            {
                printf ("%s\n", now_command);
                fprintf (execute_file, "1 ");
                while (isdigit (buffer_commands [i]))
                {
                    fprintf (execute_file, "%c", buffer_commands [i]);
                    i ++ ;
                }
                fprintf (execute_file, "\n");

            }

            for (int j = 0; j < cnt_now_command; j ++)

                now_command [j] = '\0';

            cnt_now_command = 0;

        }
    }
}


void destroy_variables (FILE* error_file, FILE* execute_file, char* buffer_commands, int user_comand_file)
{
    fclose (error_file);
    fclose (execute_file);
    close (user_comand_file);
    free (buffer_commands);
}
// ------------------------------


//     for (int i = 0; i < len_commands_buffer; i ++)
//     {
//         printf ("nachalo: %s, cnt: %d\n", now_command, cnt_now_command);
//         if (buffer_commands [i] == '\r' || buffer_commands [i] == ' ')
//             continue;
//
//         if (isalpha (buffer_commands [i]) && buffer_commands [i] != '\n')
//         {
//             printf ("alpha: %c\n", buffer_commands [i]);
//
//             now_command [cnt_now_command] = buffer_commands [i];
//             cnt_now_command ++ ;
//         } else if (buffer_commands [i] == '\n')
//         {
//             printf ("%s\n", now_command);
//             if (strcmp (now_command, "ADD"))
//             {
//                 fprintf (execute_file, "2\n");
//             } else if (strcmp (now_command, "SUB"))
//             {
//                 fprintf (execute_file, "3\n");
//             } else if (strcmp (now_command, "DIV"))
//             {
//                 fprintf (execute_file, "4\n");
//             } else if (strcmp (now_command, "OUT"))
//             {
//                 fprintf (execute_file, "5\n");
//             } else if (strcmp (now_command, "HLT"))
//             {
//                 fprintf (execute_file, "6\n");
//             } else if (strcmp (now_command, "DUMP"))
//             {
//                 fprintf (execute_file, "7\n");
//             }
//
//             for (int j = 0; j < cnt_now_command; j ++)
//             {
//                 now_command [j] = '\0';
//             }
//
//             cnt_now_command = 0;
//
//         } else if (buffer_commands [i] == '\0')
//         {
//             break;
//         } else
//         {
//
//             if (strcmp (now_command, "PUSH\0"))
//             {
//                 printf ("%s\n", now_command);
//                 fprintf (execute_file, "1 ");
//                 while (isdigit (buffer_commands [i]))
//                 {
//                     fprintf (execute_file, "%c", buffer_commands [i]);
//                     i ++ ;
//                 }
//                 fprintf (execute_file, "\n");
//
//             }
//
//             for (int j = 0; j < cnt_now_command; j ++)
//
//                 now_command [j] = ' ';
//
//             cnt_now_command = 0;
//
//         }
//     }
// }
