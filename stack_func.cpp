// stack_func.cpp - it includes, why our stack work safely

// include guards-------------------------------------
#ifndef STACK_FUNC_CPP
#define STACK_FUNC_CPP
// ---------------------------------------------------

// useful libraries---------------------------------------
#include "stack.h"                                        // our library for safe working with stack
#include <stdio.h>                                        // for standart input/output func.
#include <assert.h>                                       // for assert-protection
#include <stdlib.h>                                       // for calloc & realloc
#include <math.h>                                         // for fabs() for compare doubles
// -------------------------------------------------------

FILE* error_in_stack_i = NULL;


// function ----------------------------------------------
Error_Codes stack_init (struct Stack* stack_i, int stack_length IF_ON_DEBAG(,
                                                                                const char* name,
                                                                                const char* func,
                                                                                const char* file,
                                                                                int line)        )
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    if (stack_length == 0) printf (VIOLET_START "WARNING: you created 0-size stack!!!\n" COLOR_STOP);


    stack_i -> capacity = stack_length;
    if (stack_i -> capacity == 0)
        stack_i -> size = -1;
    else
        stack_i -> size = 1;

    IF_ON_DEBAG(
        stack_i -> name = name;
        stack_i -> func = func;
        stack_i -> file = file;
        stack_i -> line = line;
    )

    stack_i -> data = (TYPE_OF_STACK_ELEM*)calloc (IF_ON_CANARY(CANARIES +) stack_length, sizeof (TYPE_OF_STACK_ELEM));

    IF_ON_CANARY (
        stack_i -> data [0] = CANARY_ONE;

        if (stack_i -> capacity == 0)
            stack_i -> data [1] = CANARY_TWO;
        else
            stack_i -> data [stack_i -> capacity - 1] = CANARY_TWO;
    )

    for (int i = 0 IF_ON_CANARY(+ 1); i < stack_i -> capacity IF_ON_CANARY(- 1); i++)
    {
        stack_i -> data [i] = 0xDED;
    }

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
    )

    assert (!error);

    return error;
    // ----------------------------------------------------
}

Error_Codes stack_push (struct Stack* stack_i, TYPE_OF_STACK_ELEM var)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    if ( (stack_i -> size) + 1 == (stack_i -> capacity) || (stack_i -> capacity) == 0 )
    {
        stack_real_up_capacity (stack_i);
    }

    stack_i -> data [stack_i -> size] = var;

    stack_i -> size ++ ;

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    return error;
    // ----------------------------------------------------
}

Error_Codes stack_pop (struct Stack* stack_i, TYPE_OF_STACK_ELEM* pop_elem)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
    )

    if (stack_i -> capacity == 0)
    {
        error = CAPACITY_ZERO_IN_STACK_POP;
        STACK_DUMP(stack_i, &error);
    }

    assert (!error);

     if (stack_i -> size + 1 == (stack_i -> capacity / 4))
    {
        stack_real_down_capacity (stack_i);
    }

    *pop_elem = stack_i -> data [stack_i -> size - CORRECTION_SIZE_FOR_DATA_INDEX];
    stack_i -> size -- ;

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
    )

    assert (!error);

    return error;
    // ----------------------------------------------------
}

void decode_the_error_code_enum (Error_Codes* error_returned, FILE* error_in_stack_i)
{
    // TODO: добавить вывод в файлик
    // working area----------------------------------------
    switch (*error_returned)
    {
        case NO_ERROR : fprintf (error_in_stack_i,  "NO ERROR: %d\n"  , *error_returned);
        break;

        case NULL_POINTER_STACK_I : fprintf (error_in_stack_i,  "struct Stack* stack_i == NULL, error code : %d\n"  , *error_returned);
        break;

        case NULL_POINTER_DATA : fprintf (error_in_stack_i,  "stack_i -> data == NULL, error code : %d\n"  , *error_returned);
        break;

        case CAPACITY_SMALLER_THAN_ZERO : fprintf (error_in_stack_i,  "capacity < 0 (null), error code : %d\n"  , *error_returned);
        break;

        case SIZE_SMALLER_THAN_ZERO : fprintf (error_in_stack_i,  "size < 0 (null), error code : %d\n"  , *error_returned);
        break;

        case CAPACITY_SMALLER_THAN_SIZE : fprintf (error_in_stack_i,  "capacity < size, error code : %d\n"  , *error_returned);
        break;

        case CANARY_ONE_WAS_DEAD : fprintf (error_in_stack_i,  "CANARY_ONE WAS DEAD (stack_i.data[0] attacked), error code : %d\n"  , *error_returned);
        break;

        case CANARY_TWO_WAS_DEAD : fprintf (error_in_stack_i,  "CANARY_TWO WAS DEAD (stack_i.data[stack.capacity - 1] attacked), error code : %d\n"  , *error_returned);
        break;

        case CAPACITY_ZERO_IN_STACK_POP : fprintf (error_in_stack_i,  "capacity == 0 (null), error code : %d\n"  , *error_returned);
        break;

        default : fprintf (error_in_stack_i, "ERROR CODE: %d\n"  , *error_returned);
    }
    // ----------------------------------------------------
}

void stack_destroy (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
    )

    assert (!error);

    for (int i = 0; i < stack_i -> capacity; i ++)
    {
        stack_i -> data [i] = 0;
    }
    free (stack_i -> data);
    stack_i -> data = NULL;
    stack_i -> capacity = 0;
    stack_i -> size = 0;
    // ----------------------------------------------------
}

Error_Codes stack_clear (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    for (int i = stack_i -> capacity -1 IF_ON_CANARY(- 1); i > stack_i -> capacity - 3 IF_ON_CANARY(- 1); i --)
    {
        stack_i -> data [i] = 0xDED;
    }

    stack_i -> size = stack_i -> size - 2;

     IF_ON_CANARY (
        stack_i -> data [0] = CANARY_ONE;

        if (stack_i -> capacity == 0)
            stack_i -> data [1] = CANARY_TWO;
        else
            stack_i -> data [stack_i -> capacity - 1] = CANARY_TWO;
    )

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    return error;
    // ----------------------------------------------------
}

int sum_first_stack_elem (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    int index_sum = 0;
    // ----------------------------------------------------

    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    IF_ON_CANARY (
        index_sum ++ ;
    )

    for (int i = 0 IF_ON_CANARY (+ 1);
         i < stack_i -> capacity &&
         stack_i -> data [i] != 0xDED
         IF_ON_CANARY (&& stack_i -> data [i] != CANARY_TWO);
         i ++)
    {
        index_sum = i;
    }

    return stack_i -> data [index_sum] + stack_i -> data [index_sum - 1];
    // ----------------------------------------------------
}

int unsum_first_stack_elem (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    int index_sum = 0;
    // ----------------------------------------------------

    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    IF_ON_CANARY (
        index_sum ++ ;
    )

    for (int i = 0 IF_ON_CANARY (+ 1);
         i < stack_i -> capacity &&
         stack_i -> data [i] != 0xDED
         IF_ON_CANARY (&& stack_i -> data [i] != CANARY_TWO);
         i ++)
    {
        index_sum = i;
    }

    return abs(stack_i -> data [index_sum] - stack_i -> data [index_sum - 1]);
    // ----------------------------------------------------
}

int div_first_stack_elem (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    int index_sum = 0;
    // ----------------------------------------------------

    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    IF_ON_CANARY (
        index_sum ++ ;
    )

    for (int i = 0 IF_ON_CANARY (+ 1);
         i < stack_i -> capacity &&
         stack_i -> data [i] != 0xDED
         IF_ON_CANARY (&& stack_i -> data [i] != CANARY_TWO);
         i ++)
    {
        index_sum = i;
    }

    return stack_i -> data [index_sum - 1] / stack_i -> data [index_sum];
    // ----------------------------------------------------
}

IF_ON_DEBAG(
Error_Codes stack_verifier (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    if (stack_i == NULL)
    {
        error = NULL_POINTER_STACK_I;
        STACK_DUMP(stack_i, &error);
        return error;
    }

    if (stack_i -> data == NULL)
    {
        error = NULL_POINTER_DATA;
        STACK_DUMP(stack_i, &error);
        return error;
    }

    if ((stack_i -> capacity) < 0)
    {
        error = CAPACITY_SMALLER_THAN_ZERO;
        STACK_DUMP(stack_i, &error);
        return error;
    }

    if ((stack_i -> size) < 0 && (stack_i -> capacity) != 0)
    {
        error = SIZE_SMALLER_THAN_ZERO;
        STACK_DUMP(stack_i, &error);
        return error;
    }

    if ((stack_i -> capacity) < (stack_i -> size))
    {
        error = CAPACITY_SMALLER_THAN_SIZE;
        STACK_DUMP(stack_i, &error);
        return error;
    }

    IF_ON_CANARY(
        if (fabs(stack_i -> data [0] - CANARY_ONE) > 0.00001)
        {
            error = CANARY_ONE_WAS_DEAD;
            STACK_DUMP(stack_i, &error);
            return error;
        }

        if (fabs (stack_i -> data [stack_i -> capacity - 1] - CANARY_TWO) > 0.00000001 && stack_i -> capacity != 0)
        {
            error = CANARY_TWO_WAS_DEAD;
            STACK_DUMP(stack_i, &error);
            return error;
        }
    )
    return error;
    // ----------------------------------------------------
}

void stack_dump(struct Stack* stack_i, Error_Codes* error, FILE* output_file)
{
    assert (stack_i);
    assert (output_file);

    fprintf(output_file, "\n\n-------------------------------------------------------------------------------------------------------------------------\n");

    if (stack_i == NULL)
    {
        fprintf (output_file, "SORRY, BUT Stack* stack_i == NULL");
    } else
    {
        fprintf (output_file, "date:%s, time:%s\nstruct Stack \"%s\" [0x%p] created by \"%s\" at \"%s\" : %d\n\n{\n", __DATE__, __TIME__,
                                                                                                                                      stack_i -> name, stack_i,
                                                                                                                                      stack_i -> func, stack_i -> file,
                                                                                                                                      stack_i -> line);

        if (stack_i -> capacity == 0)
        {
            fprintf (output_file, "\n\nMAYBE PROBLEM IN stack_i.capacity == 0 in stack_pop(--//--)\n\n");
        }
        fprintf (output_file, "\tcapacity = %d\n", stack_i -> capacity);
        fprintf (output_file, "\tsize = %d\n", stack_i -> size);
        fprintf (output_file, "\tdata [0x%p]\n\t{\n", stack_i -> data);

        if (stack_i -> size > 0 && stack_i -> capacity > 0 && stack_i -> size <= stack_i -> capacity && stack_i -> data != NULL)
        {
            IF_ON_CANARY(
                fprintf (output_file, "\t\tCANARY_ONE: data[0] = " SPECIFICATOR_TYPE " \n", stack_i -> data [0]);
            )

            int cnt_print_dump = 0 IF_ON_CANARY(+1);

            for (; cnt_print_dump < stack_i -> capacity IF_ON_CANARY(- 1); cnt_print_dump  ++)
            {
                if (fabs(stack_i -> data [cnt_print_dump ] - 0xDED) < 0.0001)
                    fprintf (output_file, "\t\t[%d] = " SPECIFICATOR_TYPE " (MAYBE POIZEN)\n", cnt_print_dump , stack_i -> data [cnt_print_dump ]);
                else
                    fprintf (output_file, "\t\t*[%d] = " SPECIFICATOR_TYPE "\n", cnt_print_dump , stack_i -> data [cnt_print_dump ]);
            }

            IF_ON_CANARY(
                fprintf (output_file, "\t\tCANARY_ONE: data[%d] = " SPECIFICATOR_TYPE " \n", cnt_print_dump, stack_i -> data [cnt_print_dump]);
            )
        }
        fprintf (output_file, "\t}\n");
        fprintf (output_file, "}\n");

        decode_the_error_code_enum (error, output_file);

        fprintf(output_file, "-------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    // if (fclose(error_in_stack_i)) printf ("ERROR WITH CLOSING ERROR_FILE");
    // ----------------------------------------------------
}
)


void stack_real_up_capacity (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    if (stack_i -> capacity == 0)
    {
        stack_i -> data = (TYPE_OF_STACK_ELEM*)realloc (stack_i -> data, (size_t)((5 IF_ON_CANARY(+ CANARIES) )* sizeof (TYPE_OF_STACK_ELEM)));
        stack_i -> capacity = 5;
        stack_i -> size = 1;

    } else
    {
        stack_i -> data = (TYPE_OF_STACK_ELEM*)realloc (stack_i -> data, 2*(stack_i -> capacity * sizeof (TYPE_OF_STACK_ELEM)));
        stack_i -> capacity = 2*(stack_i -> capacity);
    }

    IF_ON_CANARY(
        stack_i -> data [stack_i -> capacity - 1] = CANARY_TWO;
    )

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )
    // ---------------------------------------------------
}

void stack_real_down_capacity (struct Stack* stack_i)
{
    // variables and etc.----------------------------------
    Error_Codes error = NO_ERROR;
    // ----------------------------------------------------


    // working area----------------------------------------
    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    if (stack_i -> capacity == 0)
    {
        error = CAPACITY_ZERO_IN_STACK_POP;
        STACK_DUMP(stack_i, &error);
    }

    assert (!error);

    stack_i -> data = (TYPE_OF_STACK_ELEM*)realloc (stack_i -> data, (stack_i -> capacity/4 * sizeof (TYPE_OF_STACK_ELEM)));
    stack_i -> capacity = (stack_i -> capacity) / 4;
    IF_ON_CANARY(
        stack_i -> data [stack_i -> capacity - 1] = CANARY_TWO;
    )

    IF_ON_DEBAG(
        error = stack_verifier (stack_i);
        assert (!error);
    )

    // ----------------------------------------------------
}

// void stack_printf (struct Stack* stack_i)
// {
//     // variables and etc.----------------------------------
//     Error_Codes error = NO_ERROR;
//     // ----------------------------------------------------
//
//     // working area----------------------------------------
//     IF_ON_DEBAG(
//         error = stack_verifier (stack_i);
//         assert (!error);
//     )
//
//     // variable--------------------------------------------
//     FILE* stack_printed = NULL;
//     // ----------------------------------------------------
//
//     // working area----------------------------------------
//
//     if ((stack_printed = fopen (STACK_PRINTED, OPEN_FILE_FOR_WRITING)) == NULL)
//     {
//         printf ("You have errors with stack, but " ERROR_STACK_FILE " didn't open. Sorry...");
//         stack_printed = stdout;
//     }
//
//     fprintf (stack_printed, "\n\nstruct Stack \"%s\" [0x%p] created by \"%s\" at \"%s\" : %d\n\n{\n", stack_i -> name, stack_i,
//                                                                                                                                       stack_i -> func, stack_i -> file,
//                                                                                                                                       stack_i -> line);
//     fprintf (stack_printed, "\tcapacity = %d\n", stack_i -> capacity);
//     fprintf (stack_printed, "\tsize = %d\n", stack_i -> size);
//     fprintf (stack_printed, "\tdata [0x%p]\n\t{\n", stack_i -> data);
//
//     IF_ON_CANARY(
//         fprintf (stack_printed, "\t\tCANARY_ONE: data[0] = " SPECIFICATOR_TYPE " \n", stack_i -> data [0]);
//     )
//
//     int cnt_print_dump = 0 IF_ON_CANARY(+1);
//
//     for (; cnt_print_dump < stack_i -> capacity IF_ON_CANARY(- 1); cnt_print_dump  ++)
//         {
//             if (fabs(stack_i -> data [cnt_print_dump ] - 0xDED) < 0.0001)
//                 fprintf (stack_printed, "\t\t[%d] = " SPECIFICATOR_TYPE " (MAYBE POIZEN)\n", cnt_print_dump , stack_i -> data [cnt_print_dump ]);
//             else
//                 fprintf (stack_printed, "\t\t*[%d] = " SPECIFICATOR_TYPE "\n", cnt_print_dump , stack_i -> data [cnt_print_dump ]);
//         }
//
//     IF_ON_CANARY(
//         fprintf (stack_printed, "\t\tCANARY_TWO: data[%d] = " SPECIFICATOR_TYPE " \n", cnt_print_dump, stack_i -> data [cnt_print_dump]);
//     )
//
//     fprintf (stack_printed, "\t}\n");
//
//     fprintf (stack_printed, "}\n");
//
//     if (fclose(stack_printed)) printf ("ERROR WITH CLOSING ERROR_FILE");
//     // ----------------------------------------------------
// }
// -------------------------------------------------------

#endif

// THE END------------------------------------------------


// TODO: перегрузка макроса по кол-ву аргументов
