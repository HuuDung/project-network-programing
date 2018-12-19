#ifndef __QUESTION__
#define __QUESTION__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "valid.h"
#include "tool.h"
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
    char question[BUFF_SIZE];
    char answer1[BUFF_SIZE];
    char answer2[BUFF_SIZE];
    char answer3[BUFF_SIZE];
    char answer4[BUFF_SIZE];
    LEVEL level;
    LIST_ANSWER true_ans;
    struct Question *next;
} Question;

Question **createQuestionList();
void readQuestionFromFile(Question **head);
Question *newQuestion(char *level, char *question, char *answer1, char *answer2, char *answer3, char *answer4, char *answerTrue);
void addQuestion(Question **head, char *level, char *question, char *answer1, char *answer2, char *answer3, char *answer4, char *answerTrue);
void printListQuestion(Question **head);

LIST_ANSWER convertListAnswer(char *input);
LEVEL convertLevel(char *input);
#endif