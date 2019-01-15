#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "../libs/account.h"
#include "../libs/tool.h"
#include "../libs/protocol.h"
#include "../libs/valid.h"
#include "../libs/question.h"
#include "../libs/gameplay.h"

#define BACKLOG 3 /* Number of allowed connections */

#define TRUE 1
#define FALSE 0

#define BUFF_SIZE 1024
Account **head;
Question **headQuestion;
int loadDataBase()
{
    FILE *fin;
    char username[30], password[20];
    int account, members = 0;
    char position[30];
    char c;
    head = createLinkList();
    fin = fopen("../data/account.txt", "r");
    Account *new = NULL;
    while (!feof(fin))
    {
        fscanf(fin, "%s%c%s%c%s%c%d%c", position, &c, username, &c, password, &c, &account, &c);
        members++;
        if (feof(fin))
            break;
        addAccount(head, username, password, account, atoi(position));
    }
    fclose(fin);
    return members;
}

void exportDataToFile()
{
    FILE *fout;
    Account *ptr;
    fout = fopen("../data/account.txt", "w");
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        fprintf(fout, "%d%c%s%c%s%c%d%c", ptr->position, '\t', ptr->username, '\t', ptr->password, '\t', ptr->accountStatus, '\n');
    }
    fclose(fout);
}

int main(int argc, char const *argv[])
{
    int i, maxi, maxfd, listenfd, connfd, sockfd, j;
    int nready, client[FD_SETSIZE];
    ssize_t ret;
    fd_set readfds, allset;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE], topic[BUFF_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char username[30], password[30];
    int numberQuestionArray[BUFF_SIZE], numberPlayerArray[BACKLOG];
    int positionLuckyPlayer = 0;
    PHASE status[BACKLOG];
    GAMEPLAY_STATUS gamePlayStatus[BACKLOG];

    int numberArrayLength, number;
    int existQuestion = FALSE;

    Account *account[BACKLOG];
    Account *luckyPlayer = (Account *)malloc(sizeof(Account));
    Request *rcvRequest = (Request *)malloc(sizeof(Request));
    Response *response = (Response *)malloc(sizeof(Response));
    headQuestion = createQuestionList();
    Question *ques = (Question *)malloc(sizeof(Question));
    int members;
    if (argc != 2)
    {
        printf("Params invalid\n");
    }
    else
    {
        if (checkPort(argv[1]) == 1)
        {
            members = loadDataBase();
            readQuestionFromFile(headQuestion);
            //Step 1: Construct a TCP socket to listen connection request
            if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            { /* calls socket() */
                perror("\nError: ");
                return 0;
            }

            //Step 2: Bind address to socket
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(atoi(argv[1]));

            if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
            { /* calls bind() */
                perror("\nError: ");
                return 0;
            }

            //Step 3: Listen request from client
            if (listen(listenfd, BACKLOG) == -1)
            { /* calls listen() */
                perror("\nError: ");
                return 0;
            }

            maxfd = listenfd; /* initialize */
            maxi = -1;        /* index into client[] array */
            for (i = 0; i < FD_SETSIZE; i++)
            {
                client[i] = -1; /* -1 indicates available entry */
                status[i] = NOT_LOGGED_IN;
                gamePlayStatus[i] = UNAUTH;
                account[i] = NULL;
            }

            FD_ZERO(&allset);
            FD_SET(listenfd, &allset);

            //Step 4: Communicate with clients
            while (1)
            {
                readfds = allset; /* structure assignment */
                nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
                if (nready < 0)
                {
                    perror("\nError: ");
                    return 0;
                }

                if (FD_ISSET(listenfd, &readfds))
                { /* new client connection */
                    clilen = sizeof(cliaddr);
                    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
                        perror("\nError: ");
                    else
                    {
                        printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
                        for (i = 0; i < FD_SETSIZE; i++)
                            if (client[i] < 0)
                            {
                                client[i] = connfd; /* save descriptor */
                                break;
                            }
                        if (i == FD_SETSIZE)
                        {
                            printf("\nToo many clients");
                            close(connfd);
                        }

                        FD_SET(connfd, &allset); /* add new descriptor to set */
                        if (connfd > maxfd)
                            maxfd = connfd; /* for select */
                        if (i > maxi)
                            maxi = i; /* max index in client[] array */

                        if (--nready <= 0)
                            continue; /* no more readable descriptors */
                    }
                }
                if (countMemberOnline(head, numberPlayerArray) == BACKLOG && positionLuckyPlayer == 0)
                {
                    positionLuckyPlayer = randomNumberInArray(numberPlayerArray, BACKLOG);
                    luckyPlayer = findUserNameAccountByPosition(head, positionLuckyPlayer);
                }
                for (i = 0; i <= maxi; i++)
                { /* check all clients for data */
                    if ((sockfd = client[i]) < 0)
                        continue;
                    if (FD_ISSET(sockfd, &readfds))
                    {
                        ret = receiveRequest(sockfd, rcvRequest, sizeof(Request), 0);
                        if (ret <= 0)
                        {
                            FD_CLR(sockfd, &allset);
                            close(sockfd);
                            client[i] = -1;
                        }
                        else
                        {
                            switch (gamePlayStatus[i])
                            {
                            case UNAUTH:

                                if (rcvRequest->code == USER) // code = "USER"
                                {
                                    switch (status[i])
                                    {
                                    case NOT_LOGGED_IN:
                                        if (findUserNameAccount(head, rcvRequest->message) != NULL)
                                        {
                                            account[i] = findUserNameAccount(head, rcvRequest->message);
                                            if (account[i]->accountStatus == BLOCKED)
                                            {
                                                response->code = USER_NAME_BLOCKED;
                                            }
                                            else
                                            {
                                                status[i] = UNAUTHENTICATE;
                                                response->code = USER_NAME_FOUND;
                                            }
                                        }
                                        else
                                        {
                                            response->code = USER_NAME_NOT_FOUND;
                                        }
                                        break;
                                    case UNAUTHENTICATE:
                                        if (findUserNameAccount(head, rcvRequest->message) != NULL)
                                        {
                                            account[i] = findUserNameAccount(head, rcvRequest->message);
                                            if (account[i]->accountStatus == BLOCKED)
                                            {
                                                response->code = USER_NAME_BLOCKED;
                                            }
                                            else
                                            {
                                                response->code = USER_NAME_FOUND;
                                            }
                                        }
                                        else
                                        {
                                            status[i] = NOT_LOGGED_IN;
                                        }
                                        break;
                                    case AUTHENTICATED:
                                        response->code = USER_NAME_IS_SIGNIN;
                                        break;
                                    default:
                                        response->code = INVALID_OPERATION;
                                        break;
                                    }
                                }
                                else if (rcvRequest->code == PASS) //CODE = PASS
                                {
                                    switch (status[i])
                                    {
                                    case UNAUTHENTICATE:
                                        if (strcmp(rcvRequest->message, account[i]->password) != 0)
                                        {
                                            account[i]->countSignIn++;
                                            if (account[i]->countSignIn >= 3)
                                            {
                                                status[i] = NOT_LOGGED_IN;
                                                blockAccount(head, account[i]->username);
                                                response->code = PASSWORD_INCORRECT_THREE_TIMES;
                                                exportDataToFile();
                                            }
                                            else
                                            {
                                                response->code = PASSWORD_INCORRECT;
                                            }
                                        }
                                        else
                                        {
                                            if (account[i]->status == ONLINE)
                                            {
                                                response->code = PASSWORD_CORRECT_BUT_ACCOUNT_IS_SIGNINED_IN_ORTHER_CLIENT;
                                            }
                                            else
                                            {
                                                status[i] = AUTHENTICATED;
                                                signinAccount(head, account[i]->username);
                                                account[i]->countSignIn = 0;
                                                response->code = PASSWORD_CORRECT;
                                                strcpy(response->data, account[i]->username);
                                                gamePlayStatus[i] = WAITING_PLAYER;
                                            }
                                        }
                                        break;
                                    default:
                                        response->code = INVALID_OPERATION;
                                        break;
                                    }
                                }
                                else if (rcvRequest->code == LOGOUT) // CODE = LOGOUT
                                {
                                    switch (status[i])
                                    {
                                    case AUTHENTICATED:
                                        if (strcmp(rcvRequest->message, account[i]->username) == 0)
                                        {
                                            status[i] = NOT_LOGGED_IN;
                                            gamePlayStatus[i] = UNAUTH;
                                            account[i]->status = OFFLINE;
                                            response->code = LOGOUT_SUCCESS;
                                            strcpy(response->data, account[i]->username);
                                        }
                                        else
                                        {
                                            response->code = LOGOUT_NOT_SIGNIN;
                                        }
                                        break;
                                    default:
                                        response->code = INVALID_OPERATION;
                                        break;
                                    }
                                }
                                else if (rcvRequest->code == REGISTER) //CODE = REGISTER
                                {
                                    switch (status[i])
                                    {
                                    case NOT_LOGGED_IN:
                                        if (checkSpace(rcvRequest->message))
                                        {
                                            splitMessageData(rcvRequest->message, username, password);
                                            if (findUserNameAccount(head, username) != NULL)
                                            {
                                                response->code = REGISTER_USERNAME_EXISTED;
                                            }
                                            else
                                            {
                                                addAccount(head, username, password, ACTIVE, members);
                                                response->code = REGISTER_SUCCESSFULL;
                                                exportDataToFile();
                                            }
                                        }
                                        else
                                        {
                                            response->code = SYNTAX_ERROR;
                                        }
                                        break;
                                    default:
                                        response->code = SYNTAX_ERROR;
                                        break;
                                    }
                                }
                                else
                                {
                                    response->code = SYNTAX_ERROR;
                                }
                                response->status = gamePlayStatus[i];
                                setMessageResponse(response);
                                sendResponse(sockfd, response, sizeof(Response), 0);
                                break;
                            case WAITING_PLAYER:
                                if (countMemberOnline(head, numberPlayerArray) == BACKLOG)
                                {
                                    gamePlayStatus[i] = WAITING_QUESTION;
                                    response->code = GAME_READY;
                                    response->status = gamePlayStatus[i];
                                    strcpy(response->data, luckyPlayer->username);
                                    setMessageResponse(response);
                                    sendResponse(sockfd, response, sizeof(Response), 0);
                                }
                                else
                                {
                                    response->code = GAME_NOT_READY;
                                    response->status = gamePlayStatus[i];
                                    setMessageResponse(response);
                                    sendResponse(sockfd, response, sizeof(Response), 0);
                                }
                                break;
                            case WAITING_QUESTION:
                                if (strcmp(account[i]->username, luckyPlayer->username) == 0)
                                {
                                    if (rcvRequest->code == TOPIC_LEVEL)
                                    {
                                        printf("\n%s\n", rcvRequest->message);
                                        strcpy(topic, rcvRequest->message);
                                        numberArrayLength = searchQuestionByLevel(headQuestion, convertLevel(topic), numberQuestionArray);
                                        ques = searchQuestionByStt(headQuestion, randomNumberInArray(numberQuestionArray, numberArrayLength));
                                        printf("\n%s\n", ques->question);
                                        existQuestion = TRUE;
                                        gamePlayStatus[i] = PLAYING;
                                        response->code = GAME_HAVE_QUESTION;
                                        response->status = gamePlayStatus[i];
                                        strcpy(response->data, topic);
                                        setMessageResponse(response);
                                        sendResponse(sockfd, response, sizeof(Response), 0);
                                    }
                                }
                                else
                                {
                                    if (existQuestion == TRUE)
                                    {
                                        response->code = GAME_HAVE_QUESTION;
                                        gamePlayStatus[i] = PLAYING;
                                        response->status = gamePlayStatus[i];
                                        strcpy(response->data, topic);
                                        setMessageResponse(response);
                                        sendResponse(sockfd, response, sizeof(Response), 0);
                                    }
                                    else
                                    {
                                        response->code = GAME_NO_QUESTION;
                                        response->status = gamePlayStatus[i];
                                        setMessageResponse(response);
                                        sendResponse(sockfd, response, sizeof(Response), 0);
                                    }
                                }
                                break;
                            case PLAYING:
                                // sendQuestion(sockfd, ques, sizeof(Question), 0);
                                if (strcmp(account[i]->username, luckyPlayer->username) == 0)
                                {
                                    if (rcvRequest->code == CHOOSE_ANWSER)
                                    {
                                        printf("\n1\n");
                                    }
                                    else if (rcvRequest->code == HELP)
                                    {
                                        printf("\n2\n");
                                    }
                                    else if (rcvRequest->code == CHECK)
                                    {
                                        sendQuestion(sockfd, ques, sizeof(Question), 0);
                                    }
                                    else
                                    {
                                        printf("\n6\n");
                                    }
                                }
                                else
                                {
                                    if (rcvRequest->code == CHOOSE_ANWSER)
                                    {
                                        printf("\n5\n");
                                    }
                                    else if (rcvRequest->code == CHECK)
                                    {
                                        sendQuestion(sockfd, ques, sizeof(Question), 0);
                                    }
                                    else
                                    {
                                        printf("\n6\n");
                                    }
                                }

                                break;
                            case END_GAME:
                                break;
                            default:
                                break;
                            }
                        }

                        if (--nready <= 0)
                            break; /* no more readable descriptors */
                    }
                }
            }

            return 0;
        }
    }
}
