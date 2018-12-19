#ifndef __PROTOCOL__
#define __PROTOCOL__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../libs/tool.h"
#include "../libs/valid.h"

typedef enum
{
    USER,
    PASS,
    REGISTER,
    LOGOUT,
    INPUT_SYNTAX_ERROR,
} Login_Opcode;

typedef enum
{
    CHOOSE_ANWSER,
    TOPIC,
    HELP,
    GAMEPLAY_SYNTAX_ERROR,
} Game_Client_Opcode;

typedef enum
{
    LUCKY_USER,
    QUESTION,
    ANSWER,
    USER_STILL,
    USER_WRONG,
    BONUS,
    LOSE,
    WIN,
} Game_Server_Opcode;

typedef enum
{
    NOT_LOGGED_IN,
    UNAUTHENTICATE,
    AUTHENTICATED,
} Phase;

typedef enum
{
    SYNTAX_ERROR = 02,
    INVALID_OPERATION = 03,

    USER_NAME_FOUND = 11,
    USER_NAME_NOT_FOUND = 12,
    USER_NAME_BLOCKED = 13,
    USER_NAME_IS_SIGNIN = 14,

    PASSWORD_CORRECT = 21,
    PASSWORD_INCORRECT = 22,
    PASSWORD_INCORRECT_THREE_TIMES = 23,
    PASSWORD_CORRECT_BUT_ACCOUNT_IS_SIGNINED_IN_ORTHER_CLIENT = 24,

    LOGOUT_SUCCESS = 31,
    LOGOUT_NOT_SIGNIN = 32,

    REGISTER_SUCCESSFULL = 41,
    REGISTER_USERNAME_EXISTED = 42,

} Message;

typedef struct
{
    Login_Opcode code;
    char message[50];
} Request_Login;

typedef struct
{
    Message code;
    char message[50];
    char data[50];
} Response;
//core function
int receiveRequestLogin(int socket, Request_Login *buff, int size, int flags);
int sendRequestLogin(int socket, Request_Login *buff, int size, int flags);

int sendMessage(int socket, Response *msg, int size, int flags);
int receiveMessage(int socket, Response *msg, int size, int flags);

// set message response
void setMessageResponse(Response *msg);
void readMessageResponse(Response *msg);

//set opcode resquest
void setLoginOpcodeRequest(Request_Login *request, char *input);

#endif