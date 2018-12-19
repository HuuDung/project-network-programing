#ifndef __QUESTION__
#define __QUESTION__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "valid.h"
typedef enum
{
    A,
    B,
    C,
    D,
} LIST_ANSWER;

typedef enum
{
    EASY,
    NORMAL,
    HARD,
} LEVEL;

typedef struct
{
    char content[BUFF_SIZE];
    LIST_ANSWER answer;
} Answer;

typedef struct
{
    char question[BUFF_SIZE];
    Answer ans;
    LEVEL level;
    LIST_ANSWER true_ans;
    struct Question *next;
} Question;



#endif