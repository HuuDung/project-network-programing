#include "../libs/gameplay.h"

int luckyMember(Account **head)
{
    int memberArray[BUFF_SIZE];
    int length = 0;
    Account *ptr = NULL;
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->status == ONLINE)
            memberArray[length++] = ptr->position;
    }
    return randomNumberInArray(memberArray, length);
}