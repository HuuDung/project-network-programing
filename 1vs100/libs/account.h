#ifndef __ACCOUNT__
#define __ACCOUNT__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BLOCKED 0
#define ACTIVE 1

#define ONLINE 1
#define OFFLINE 0

typedef struct
{
    char username[30];
    char password[30];
    int position;
    int status;
    int accountStatus;
    int countSignIn;
    struct Account *next;
} Account;

Account **createLinkList();
Account *findUserNameAccount(Account **head, char *username);
Account *findUserNameAccountByPosition(Account **head, int position);
Account *newAccount(char *username, char *password, int accountStatus, int position);
void addAccount(Account **head, char *username, char *password, int accountStatus, int position);
int countMemberOnline(Account **head, int *numberPLayerArray);
void printListAccount(Account **head);

void blockAccount(Account **head, char *username);
void signinAccount(Account **head, char *username);
#endif