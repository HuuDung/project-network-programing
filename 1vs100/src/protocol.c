#include "../libs/protocol.h"

int receiveRequest(int socket, Request *buff, int size, int flags)
{
  int n;

  n = recv(socket, buff, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}

int sendRequest(int socket, Request *buff, int size, int flags)
{
  int n;

  n = send(socket, buff, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}

int sendResponse(int socket, Response *msg, int size, int flags)
{
  int n;
  n = send(socket, msg, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}

int receiveResponse(int socket, Response *msg, int size, int flags)
{
  int n;
  n = recv(socket, msg, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}

void setMessageResponse(Response *msg)
{
  if (msg->code != NULL)
  {
    switch (msg->code)
    {
    case SYNTAX_ERROR:
      strcpy(msg->message, "Syntax error ");
      break;
    case INVALID_OPERATION:
      strcpy(msg->message, "Invalid operation ");
      break;
    case USER_NAME_FOUND:
      strcpy(msg->message, "Username is correct ");
      break;
    case USER_NAME_NOT_FOUND:
      strcpy(msg->message, "Cannot find account ");
      break;
    case USER_NAME_BLOCKED:
      strcpy(msg->message, "Account is blocked ");
      break;
    case USER_NAME_IS_SIGNIN:
      strcpy(msg->message, "Login only one account ");
      break;
    case PASSWORD_CORRECT:
      strcpy(msg->message, "Login successful ");
      break;
    case PASSWORD_INCORRECT:
      strcpy(msg->message, "Password incorrect ");
      break;
    case PASSWORD_INCORRECT_THREE_TIMES:
      strcpy(msg->message, "Password is incorrect. Account is blocked ");
      break;
    case LOGOUT_SUCCESS:
      strcpy(msg->message, "Logout successful ");
      break;
    case LOGOUT_NOT_SIGNIN:
      strcpy(msg->message, "Account is not sign in ");
      break;
    case REGISTER_SUCCESSFULL:
      strcpy(msg->message, "Register successfull ");
      break;
    case REGISTER_USERNAME_EXISTED:
      strcpy(msg->message, "Username is existed ");
      break;
    case PASSWORD_CORRECT_BUT_ACCOUNT_IS_SIGNINED_IN_ORTHER_CLIENT:
      strcpy(msg->message, "Account is signin in orhter client ");
      break;
    case ANSWER_IS_CORRECT:
      strcpy(msg->message, "The answer is correct ");
      break;
    case ANSWER_IS_INCORRECT:
      strcpy(msg->message, "The answer is incorrect ");
      break;
    case USER_USED_HINT:
      strcpy(msg->message, "User used hint ");
      break;
    case USER_CHOOSE_TOPIC_LEVEL:
      strcpy(msg->message, "User choose level ");
      break;
    case LEAVE_ROOM:
      strcpy(msg->message, "Leave room ");
      break;
    case GAME_READY:
      strcpy(msg->message, "Game ready ");
      break;
    case GAME_NOT_READY:
      strcpy(msg->message, "Waiting orther player... ");
      break;
    case GAME_HAVE_QUESTION:
      strcpy(msg->message, "");
      break;
    case GAME_NO_QUESTION:
      strcpy(msg->message, "");
      break;
    default:
      strcpy(msg->message, "Exception ");
      break;
    }
  }
}

void readMessageResponse(Response *msg)
{
  if (msg->code != NULL)
  {
    printf("%s\n", msg->message);
    switch (msg->code)
    {
    case PASSWORD_CORRECT:
      printf("Hello %s\n", msg->data);
      break;
    case LOGOUT_SUCCESS:
      printf("\nGoodbye %s\n", msg->data);
      break;
    default:
      break;
    }
  }
}

void setOpcodeRequest(Request *request, char *input)
{
  char code[BUFF_SIZE], data[BUFF_SIZE];
  splitMessage(input, code, data);
  strcpy(request->message, data);
  if (strcmp(code, "USER") == 0)
    request->code = USER;
  else if (strcmp(code, "PASS") == 0)
    request->code = PASS;
  else if (strcmp(code, "REGISTER") == 0)
    request->code = REGISTER;
  else if (strcmp(code, "LOGOUT") == 0)
    request->code = LOGOUT;
  else if (strcmp(code, "CHECK") == 0)
    request->code = CHECK;
  else if (strcmp(code, "ANSWER") == 0)
    request->code = CHOOSE_ANWSER;
  else if (strcmp(code, "TOPIC") == 0)
    request->code = TOPIC_LEVEL;
  else if (strcmp(code, "HELP") == 0)
    request->code = HELP;
  else if (strcmp(code, "LEAVE") == 0)
    request->code = LEAVE;
  else
    request->code = INPUT_SYNTAX_ERROR;
}

int sendQuestion(int socket, Question *question, int size, int flags)
{
  int n;
  n = send(socket, question, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}
int receiveQuestion(int socket, Question *question, int size, int flags)
{
  int n;
  n = recv(socket, question, size, flags);
  if (n < 0)
    perror("Error: ");
  return n;
}

void requestGet(int socket)
{
  Request *request = (Request *)malloc(sizeof(Request));
  setOpcodeRequest(request, "CHECK check");
  sendRequest(socket, request, sizeof(Request), 0);
}
