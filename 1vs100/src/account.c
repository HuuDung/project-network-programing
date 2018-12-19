#include "../libs/account.h"

Account **createLinkList()
{
    Account **head = (Account **)malloc(sizeof(Account *));
    *head = NULL;

    return head;
}

Account *findUserNameAccount(Account **head, char *username)
{
    Account *ptr = NULL;
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp(ptr->username, username) == 0)
            return ptr;
    }
    return NULL;
}

Account *newAccount(char *username, char *password, int accountStatus)
{
    Account *new = (Account *)malloc(sizeof(Account));
    strcpy(new->username, username);
    strcpy(new->password, password);
    new->accountStatus = accountStatus;
    new->status = OFFLINE;
    new->countSignIn = 0;
    new->next = NULL;
    return new;
}

void addAccount(Account **head, char *username, char *password, int accountStatus)
{
    Account *new = newAccount(username, password, accountStatus);
    Account *current = (*head);

    if (*head == NULL)
    {
        (*head) = new;
    }
    else
    {
        while (current->next != NULL)
            current = current->next;
        current->next = new;
    }
}

void printListAccount(Account **head)
{
    Account *ptr = NULL;
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        printf("Username: %s\n", ptr->username);
        printf("Pass: %s\n", ptr->password);
        printf("Account: %s\n", ptr->accountStatus == ACTIVE ? "Active" : "Blocked");
        printf("Status : %s\n", ptr->status == ONLINE ? "Online" : "Offline");
        printf("Count wrong password: %d\n", ptr->countSignIn);
        printf("\n");
    }
}

void blockAccount(Account **head, char *username)
{
    Account *ptr = findUserNameAccount(head, username);
    ptr->accountStatus = BLOCKED;
}
void signinAccount(Account **head, char *username)
{
    Account *ptr = findUserNameAccount(head, username);
    ptr->status = ONLINE;
}
