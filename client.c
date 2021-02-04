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

static int connect_to_server(Environment *env);
static int wait(Environment *env);
static int write_to_server(Environment *env);
static int close(Environment *env);
static int end(Environment *env);
_Noreturn static int error_exit(Environment *env);

typedef enum
{

    GAME_START = FSM_APP_STATE_START, // 2
    IDLE,       // 3
    ERROR,      // 4
    INPUT,      // 5
    RESULT,     // 6
    END,        // 7

} States;

typedef struct
{
    Environment common;
    int c;
} EchoEnvironment;

int main(int argc, char *argv[])
{
    EchoEnvironment env;
    StateTransition transitions[] =
        {
            {FSM_INIT, GAME_START, &connect_to_server},
            {GAME_START, IDLE, &wait},
            {GAME_START, INPUT, &write_to_server},
            {INPUT, IDLE, &wait},
            {INPUT, INPUT, &wait},
            {IDLE, INPUT, &write_to_server},
            {IDLE, RESULT, &close},
            {RESULT, END, &end},
            {FSM_IGNORE, FSM_IGNORE, NULL},
        };
    int code;
    int start_state;
    int end_state;
    fprintf(stderr, "Press enter key \n");
    start_state = FSM_INIT;
    end_state = GAME_START;
    code = fsm_run((Environment *)&env, &start_state, &end_state, transitions);

    if (code != 0)
    {
        fprintf(stderr, "Cannot move from %d to %d\n", start_state, end_state);

        return EXIT_FAILURE;
    }

    fprintf(stderr, "Exiting state %d\n", start_state);

    return EXIT_SUCCESS;
}

static int connect_to_server(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;


    fprintf(stderr, "You have entered the game!\n");
    
    return IDLE;
}

static int wait(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    int position = 0;
    char *buffer = malloc(sizeof(char)*1024);
    int c;

    echo_env = (EchoEnvironment *)env;
    while (1)
    {
        fprintf(stderr, "Enter input!!\n");
        c = getchar();
        if(c == EOF || c == '\n'){
            buffer[position] ='\0';
            break;
        }
        position = position + 1;
    }
    echo_env->c = buffer[0];
    if (echo_env->c == EOF)
    {
        if (ferror(stdin))
        {
            return ERROR;
        }

        return FSM_EXIT;
    }

    fprintf(stderr, "Transferring to input state!!\n");

    return INPUT;
}

static int write_to_server(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
   
    if (echo_env->c > 73 || echo_env->c < 65)
    {
        fprintf(stderr, "This is NOT a valid input! Try again!\n");
        return INPUT;
    }
    fprintf(stderr, "Input valid!\n");

    return IDLE;
}

static int close(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
   
    
    fprintf(stderr, "You win\n");

    return RESULT;
}

static int end(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    fprintf(stderr, "Bye bye!!\n");

    return FSM_EXIT;
}
_Noreturn static int error_exit(Environment *env)
{
    perror("Error! Disconnected!");

    exit(EXIT_FAILURE);
}
