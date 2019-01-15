#ifndef __GAMEPLAY__
#define __GAMEPLAY__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "account.h"
#include "tool.h"
#include "valid.h"
typedef enum{
    UNAUTH,
    WAITING_PLAYER,
    WAITING_QUESTION,
    PLAYING,
    END_GAME,
}GAMEPLAY_STATUS;

int luckyMember(Account **head);

#endif