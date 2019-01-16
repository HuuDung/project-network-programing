#ifndef __GAMEPLAY__
#define __GAMEPLAY__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "account.h"
#include "tool.h"
#include "valid.h"

#define GAME_PLAYING 1
#define GAME_END 0

typedef struct
{
    float score;
    int playerAnswerWrong;
    int playerPlaying;
    int status;
} Information;
typedef struct
{
    int stt;
    float score;
    char key[BUFF_SIZE];
    float value;
    struct Help *next;
} Help;
typedef enum
{
    UNAUTH,
    WAITING_PLAYER,
    WAITING_QUESTION,
    PLAYING,
    END_GAME,
} GAMEPLAY_STATUS;

int luckyMember(Account **head);
float calculateScore(int countPlayerPlaying, int countPlayerLoser);

Help **createHelpList();
void readHelpFromFile(Help **head);
Help *newHelp(char *stt, char *key, char *value);
void addHelp(Help **head, char *stt, char *key, char *value);
void deleteHelp(Help **head, int stt);
Help *searchHelpByStt(Help **head, int stt);
#endif