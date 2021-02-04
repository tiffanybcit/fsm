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

static int announceWinner(Environment *env);
static int nextPlayer(Environment *env);
static int check(Environment *env);
static int waitForMove(Environment *env);
static int read(Environment *env);
static int write(Environment *env);
static int connect(Environment *env);
_Noreturn static int errorExit(Environment *env);

typedef enum
{

    GAME_START = FSM_APP_STATE_START, // 2
    IDLE,       // 3
    ERROR,      // 4
    VALIDATE,   // 5
    RESULT,     // 6
    CHECKSTATE,     // 7
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
            {FSM_INIT, GAME_START, &connect},
            {GAME_START, IDLE, &waitForMove},
            {IDLE, VALIDATE, &read},
            {VALIDATE, IDLE, &waitForMove},
            
            {VALIDATE, CHECKSTATE, &check},
            {CHECKSTATE, RESULT, &announceWinner},
            {CHECKSTATE, IDLE, &nextPlayer},
            {VALIDATE, ERROR, &errorExit},
    
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

static int connect(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;


    fprintf(stderr, "Two players have entered the game!\n");
    return IDLE;
}

static int waitForMove(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    int position = 0;

    echo_env = (EchoEnvironment *)env;
    while (position < 1)
    {
        fprintf(stderr, "Enter input!!\n");
        echo_env->c = getchar();
        position = position + 1;
    }

    if (echo_env->c == EOF)
    {
        if (ferror(stdin))
        {
            return ERROR;
        }

        return FSM_EXIT;
    }

    position = 0;
    fprintf(stderr, "Time to validate the input!!\n");

    return VALIDATE;
}

static int read(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
   
    if (echo_env->c > 73 || echo_env->c < 65)
    {
        fprintf(stderr, "This is NOT a valid input! Try again!\n");
        return IDLE;
    }
    fprintf(stderr, "Input valid!\n");

    return CHECKSTATE;
}

static int nextPlayer(Environment *env){
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    fprintf(stderr, "Next player's turn!\n");

    return IDLE;

}

static int check(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    fprintf(stderr, "Time to announce the winner!\n");

    return RESULT;
}


static int announceWinner(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    fprintf(stderr, "Player 1 wins!!\n");

    return FSM_EXIT;
}

_Noreturn static int errorExit(Environment *env)
{
    perror("Error! Disconnected!");

    exit(EXIT_FAILURE);
}
