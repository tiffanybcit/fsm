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
#include <unistd.h>
#include <dcfsm/fsm.h>

static int await_connection();
static int read_input(Environment *env);
static int write_code(Environment *env);
static int prompt(Environment *env);
static int read_error(Environment *env);
_Noreturn static int write_error(Environment *env);

int i = 0;
int moves = 0;

typedef enum
{
    PROMPT = FSM_APP_STATE_START,  // 2
    AWAIT_CONNECTION,              // 3
    READ,                          // 4
    WRITE,                         // 5
    WIN,                           // 6
    LOSE,                          // 7
    TIE,                           // 8
    ERROR,                         // 9
} States;


typedef struct
{
    Environment common;
    int c;
    int code;
} EchoEnvironment;


int main(int argc, char *argv[])
{
    EchoEnvironment env;
    StateTransition transitions[] =
    {
            { FSM_INIT,   AWAIT_CONNECTION,     &await_connection},
            { AWAIT_CONNECTION,   PROMPT,     &prompt},
            { PROMPT,     READ,       &read_input   },
            { READ,       ERROR,      &read_error   },
            { READ,       WRITE,      &write_code   },
            { READ,       TIE,        &prompt       },
            { TIE,       FSM_EXIT,NULL      },
            { READ,       FSM_EXIT,   NULL  },
            { WRITE,      ERROR,      &write_error  },
            { WRITE,      READ,       &read_input   },
            {ERROR,       PROMPT,     &prompt       },
            { FSM_IGNORE, FSM_IGNORE, NULL          },
    };
    int code;
    int start_state;
    int end_state;

    start_state = FSM_INIT; // 0
    end_state   = AWAIT_CONNECTION;
    code = fsm_run((Environment *)&env, &start_state, &end_state, transitions);

    if(code != 0)
    {
        fprintf(stderr, "Cannot move from %d to %d\n", start_state, end_state);

        return EXIT_FAILURE;
    }

//    fprintf(stderr, "Exiting state %d\n", start_state);

    return EXIT_SUCCESS;
}

static int await_connection()
{
    printf("Awaiting connection...\n");
    sleep(3);
    printf("Client 1 connected\n");
    sleep(3);
    printf("Client 2 connected\n");
    return PROMPT;
}

static int read_input(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    echo_env->c = getchar();

    if(echo_env->c == EOF)
    {

        if(ferror(stdin))
        {
            return ERROR;
        }
        printf("EOF\n");
        return FSM_EXIT;
    } else if (echo_env->c == '\n') {
        return WRITE;
    }

    switch (i) {
        case 0:
            if (echo_env->c == '1' || echo_env->c == '2') {
                printf("validating player...\n");
                echo_env->code = 200;
            } else  {
                i = 0;
                echo_env->code = 400;
                return ERROR;
            }
            break;
        case 1:
            if ((echo_env->c - '0') >= 0 && (echo_env->c - '0') <= 2 && echo_env->code == 200) {
                printf("validating row...\n");
                echo_env->code = 201;
            } else {
                i = 0;
                echo_env->code = 401;
                return ERROR;
            }
            break;
        case 2:
            if ((echo_env->c - '0') >= 0 && (echo_env->c - '0') <= 2 && echo_env->code == 201) {
                printf("validating column...\n");
                echo_env->code = 202;
            } else {
                echo_env->code = 402;
                i = 0;
                return ERROR;
            }
            break;
        default:
            i = 0;
            return ERROR;
    }

    i++;
    if (i == 3) {
        i = 0;
        moves++;
    }
    if (moves == 5) {
        return TIE;
    }
    return WRITE;
}

static int write_code(Environment *env)
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

static int prompt(Environment *env)
{
    char* prompt = "Prompt: Place a move\n";
    EchoEnvironment *echo_env;
    int              ret_val;

    echo_env = (EchoEnvironment *)env;
    ret_val = echo_env->code;
    if (moves == 5) {
        printf("==========TIE==========\n");
        return FSM_EXIT;
    }
    switch (ret_val) {
        case 400:
            printf("Invalid player\n");
            break;
        case 401:
            printf("Renter your move\n");
            break;
        case 402:
            printf("Renter your move\n");
            break;
        default:
            printf("%s", prompt);
            break;
    }
    echo_env->code = 0;
    return READ;
}

static int read_error(Environment *env)
{
    EchoEnvironment *echo_env;
    int              ret_val;

    echo_env = (EchoEnvironment *)env;
    ret_val = echo_env->code;
    if (ret_val == 401) {
        printf("invalid row\n");
        return PROMPT;
    } else if (ret_val == 402) {
        printf("invalid column\n");
        return PROMPT;
    } else {
        printf("invalid moves\n");
        return PROMPT;
    }
}

_Noreturn static int write_error(Environment *env)
{
    perror("putc");

    exit(EXIT_FAILURE);
}
