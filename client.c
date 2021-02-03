#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"

static int printRandoms(int lower, int upper);
static int write(Environment *env);
static int onPlayerTurn(Environment *env);
static int onPlayer2Turn(Environment *env);
static int connect(Environment *env);
static int validMove(Environment *env);

_Noreturn static int error_exit(Environment *env);

typedef enum
{

    GAME_START = FSM_APP_STATE_START, // 2
    IDLE,                             // 3
    ERROR,                            // 4
    PLAYERTURN,                       //5
    PLAYER2TURN,                      //6
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
            {GAME_START, IDLE, &onPlayer2Turn},
            {GAME_START, PLAYERTURN, &onPlayerTurn},
            {PLAYERTURN, PLAYERTURN, &onPlayerTurn},
            {PLAYERTURN, IDLE, &write},
            {IDLE, PLAYERTURN, &validMove},
            {PLAYERTURN, ERROR, &error_exit},
            {FSM_IGNORE, FSM_IGNORE, NULL},
        };
    int code;
    int start_state;
    int end_state;
    fprintf(stderr, "Press enter key\n");
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

static int printRandoms(int lower, int upper)
{

    int num = (rand() %
               (upper - lower + 1)) +
              lower;
    printf("%d ", num);

    return num;
}

static int connect(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;

    fprintf(stderr, "You have entered the game!\n");
    int random1 = printRandoms(0, 3);
    if (random1 == 1)
    {
        return PLAYERTURN;
    }

    return IDLE;
}

static int write(Environment *env)
{
    EchoEnvironment *echo_env;

    int position = 0;

    echo_env = (EchoEnvironment *)env;
    while (position < 1)
    {
        echo_env->c = getchar();
        position = position+1;
    }

    if (echo_env->c == EOF)
    {
        if (ferror(stdin))
        {
            return ERROR;
        }

        return FSM_EXIT;
    }

    if (echo_env->c > 73 || echo_env->c < 65)
    {
        return PLAYERTURN;
    }

    return IDLE;
}

static int onPlayerTurn(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;

    return IDLE;
}

static int onPlayer2Turn(Environment *env)
{
    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;

    return IDLE;
}

static int validMove(Environment *env)
{

    EchoEnvironment *echo_env;

    echo_env = (EchoEnvironment *)env;
    return PLAYERTURN;
}

_Noreturn static int error_exit(Environment *env)
{
    perror("Error! Disconnected!");

    exit(EXIT_FAILURE);
}
