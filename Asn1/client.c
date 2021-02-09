/*
 * Copyright 2021 D'Arcy Smith + the BCIT CST Datacommunications Option students.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"


static int read_input(Environment *env);
static int write_output(Environment *env);
static int validate_code(Environment *env);
_Noreturn static int read_error(Environment *env);
_Noreturn static int write_error(Environment *env);


typedef enum
{
    READ = FSM_APP_STATE_START,    // 2
    WRITE,                         // 3
    VALIDATE,                      // 4
    ERROR,                         // 5
} States;


typedef struct
{
    Environment common;
    int c;
    int code;
} EchoEnvironment;

int i = 0;


int main(int argc, char *argv[])
{
    EchoEnvironment env;
    StateTransition transitions[] =
            {
                    { FSM_INIT,   READ,       &read_input   },
                    { READ,       READ,       &read_input   },
                    { READ,       ERROR,      &read_error   },
                    { READ,       VALIDATE,   &validate_code },
                    { READ,       WRITE,      &write_output },
                    { READ,       FSM_EXIT,   NULL  },
                    { WRITE,      ERROR,      &write_error  },
                    { WRITE,      READ,       &read_input   },
                    { FSM_IGNORE, FSM_IGNORE, NULL  },
            };
    int code;
    int start_state;
    int end_state;

    start_state = FSM_INIT;
    end_state   = READ;
    code = fsm_run((Environment *)&env, &start_state, &end_state, transitions);

    if(code != 0)
    {
        fprintf(stderr, "Cannot move from %d to %d\n", start_state, end_state);

        return EXIT_FAILURE;
    }

//    fprintf(stderr, "Exiting state %d\n", start_state);

    return EXIT_SUCCESS;
}

/**
 * Reads in server's response code
 */
static int read_input(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    echo_env->c = getchar();
    echo_env->code = echo_env->c;

    if(echo_env->c == EOF)
    {
        if(ferror(stdin))
        {
            return ERROR;
        }

        return FSM_EXIT;
    } else if (echo_env->c == '\n') {
        return WRITE;
    }

    // error starts with 4, by then,
    // server would have already prompted for move again
    if (echo_env->code == '4' && i == 0) {
        printf("invalid move");
        //putchar('\n'); // signal to decline invalid move, would suggest 1 digit code
        return READ;
    }
    i++;
    if (i == 3) {
        i = 0;
    }

    return WRITE;
}

/**
 * Write to the screen
 */
static int write_output(Environment *env)
{
    EchoEnvironment *echo_env;
    int              ret_val;

    echo_env = (EchoEnvironment *)env;
    ret_val = putchar(echo_env->c);

    if(ret_val == EOF)
    {
        return ERROR;
    }


    return READ;
}

/**
 * Writes move to server then idle until received appropriate
 * response code to continue move
 */
static int validate_code(Environment *env) {
    EchoEnvironment *echo_env;
    int ret_val;

    echo_env = (EchoEnvironment *) env;
    ret_val = echo_env->code;

    return WRITE;
}

_Noreturn static int read_error(Environment *env)
{
    perror("getc");

    exit(EXIT_FAILURE);
}

_Noreturn static int write_error(Environment *env)
{
    perror("putc");

    exit(EXIT_FAILURE);
}
