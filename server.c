#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"

static int annouce_winner(Environment *env);
static int waitForMove(Environment *env);
static int read(Environment *env);
static int write(Environment *env);
static int connect(Environment *env);
_Noreturn static int error_exit(Environment *env);

typedef enum
{

    GAME_START, // 2
    IDLE,       // 3
    ERROR,      // 4
    VALIDATE,   // 5
    RESULT,     // 6
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

            {VALIDATE, IDLE, &write},
            {IDLE, VALIDATE, &read},
            {VALIDATE, RESULT, &annouce_winner},
            {VALIDATE, ERROR, &error_exit},
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

    int position = 0;

    echo_env = (EchoEnvironment *)env;

    while (position < 1)
    {
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

    fprintf(stderr, "Two players have entered the game!\n");
    return GAME_START;
}

static int waitForMove(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    int position = 0;

    echo_env = (EchoEnvironment *)env;
    while (position < 1)
    {
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

    

    return IDLE;
}

static int read(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;

    return VALIDATE;
}


static int write(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    if (echo_env->c > 73 || echo_env->c < 65)
    {
        return VALIDATE;
    }

    return IDLE;
}

static int annouce_winner(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;

    return RESULT;
}

_Noreturn static int error_exit(Environment *env)
{
    perror("Error! Disconnected!");

    exit(EXIT_FAILURE);
}
