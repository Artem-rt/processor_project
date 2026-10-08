// stack.h - work safely with stack!-----------------

// include guards-------------------------------------
#ifndef STACK_H
#define STACK_H
// ---------------------------------------------------

#include <stdio.h> // for init. FILE*

// important define for create debag-mode-------------
#ifdef VERIFICATION_OFF
#define NDEBAG
#define IF_ON_DEBAG(...)
#else
#define IF_ON_DEBAG(...) __VA_ARGS__
#endif

#ifdef CANARY_OFF
#define IF_ON_CANARY(...)
#else
#define IF_ON_CANARY(...) __VA_ARGS__
#endif

#ifdef HESH_OFF
#define IF_ON_HESH(...)
#else
#define IF_ON_HESH(...) __VA_ARGS__
#endif
// ---------------------------------------------------

// useful macro---------------------------------------
#define VIOLET_START "\e[35m"
#define RED_START "\e[31m"
#define GREEN_START "\e[32m"
#define COLOR_STOP "\e[0m"
#define ERROR_STACK_FILE "ERROR_IN_STACK_I.log"
#define STACK_PRINTED "STACK_PRINTED.txt"
#define OPEN_FILE_FOR_WRITING "a+"
#define OPEN_FILE_FOR_READING "r"
#define CANARIES 2
#define CORRECTION_SIZE_FOR_DATA_INDEX 1
#define STACK_INIT(_stack_, _len_) stack_init (_stack_, _len_ IF_ON_DEBAG(, \
                                                                                #_stack_,\
                                                                                "",\
                                                                                __FILE__,\
                                                                                __LINE__) )


#define STACK_DUMP_FILE(_stack_, _error_, _file_) stack_dump (_stack_, _error_, _file_)

#define STACK_DUMP_LOG(_stack_, _error_)         if ((error_in_stack_i = fopen (ERROR_STACK_FILE, OPEN_FILE_FOR_WRITING)) == NULL)          \
                                                 {                                                                                          \
                                                     printf ("You have errors with stack, but " ERROR_STACK_FILE " didn't open. Sorry..."); \
                                                     error_in_stack_i = stderr;                                                             \
                                                 }                                                                                          \
                                                 stack_dump (_stack_, _error_, error_in_stack_i);                                           \
                                                 if (fclose(error_in_stack_i)) printf ("ERROR WITH CLOSING ERROR_FILE");
#define STACK_DUMP_SELECT(_1, _2, _3, macro, ...) macro
#define STACK_DUMP(...) STACK_DUMP_SELECT(__VA_ARGS__, STACK_DUMP_FILE, STACK_DUMP_LOG)(__VA_ARGS__)
// ---------------------------------------------------

// determining the data type in the stack-------------
typedef int TYPE_OF_STACK_ELEM;
#define SPECIFICATOR_TYPE "%d"
// ---------------------------------------------------

// canary const---------------------------------------
const TYPE_OF_STACK_ELEM CANARY_ONE = 0xDEADC0DE;
const TYPE_OF_STACK_ELEM CANARY_TWO = 0xDEADBABE;
// ---------------------------------------------------

// structure == our stack-----------------------------
struct Stack
{
    IF_ON_DEBAG(
        const char* name;
        const char* func;
        const char* file;
        int line;
    ) ;

    TYPE_OF_STACK_ELEM* data;
    int size;
    int capacity;

} ;
// ---------------------------------------------------

// error's codes enum---------------------------------
enum Error_Codes
{
    NO_ERROR = 0,
    NULL_POINTER_STACK_I = -1,
    NULL_POINTER_DATA = -2,
    CAPACITY_SMALLER_THAN_ZERO = -3,
    SIZE_SMALLER_THAN_ZERO = -4,
    CAPACITY_SMALLER_THAN_SIZE = -5,
    CANARY_ONE_WAS_DEAD = -6,
    CANARY_TWO_WAS_DEAD = -7,
    CAPACITY_ZERO_IN_STACK_POP = -8
} ;
// ---------------------------------------------------

// function prototypes--------------------------------
Error_Codes stack_init (struct Stack* stack_i, int stack_length IF_ON_DEBAG(,
                                                                                const char* name,
                                                                                const char* func,
                                                                                const char* file,
                                                                                int line)        );
Error_Codes stack_push (struct Stack* stack_i, TYPE_OF_STACK_ELEM var);
Error_Codes stack_pop (struct Stack* stack_i, TYPE_OF_STACK_ELEM* pop_elem);
void decode_the_error_code_enum (Error_Codes* error_returned, FILE* error_in_stack_i);
void stack_destroy (struct Stack* stack_i);
Error_Codes stack_clear (struct Stack* stack_i);
int sum_first_stack_elem (struct Stack* stack_i);
int unsum_first_stack_elem (struct Stack* stack_i);
int div_first_stack_elem (struct Stack* stack_i);

IF_ON_DEBAG(
Error_Codes stack_verifier (struct Stack* stack_i);
void stack_dump (struct Stack* stack_i, Error_Codes* error, FILE* output_file);
)



void stack_real_up_capacity (struct Stack* stack_i);
void stack_real_down_capacity (struct Stack* stack_i);
// void stack_printf (struct Stack* stack_i);
// ---------------------------------------------------

#endif  // #ifndef STACK_H





























































































// generic.h
