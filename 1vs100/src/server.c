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

#define BACKLOG 20 /* Number of allowed connections */
#define BUFF_SIZE 1024
Account **head;
Question **headQuestion;
void loadDataBase()
{
    FILE *fin;
    char username[30], password[20];
    int account;
    char c;
    head = createLinkList();
    fin = fopen("../data/account.txt", "r");
    Account *new = NULL;
    while (!feof(fin))
    {
        fscanf(fin, "%s%c%s%c%d%c", username, &c, password, &c, &account, &c);
        if (feof(fin))
            break;
        addAccount(head, username, password, account);
    }
    fclose(fin);
}

void exportDataToFile()
{
    FILE *fout;
    Account *ptr;
    fout = fopen("../data/account.txt", "w");
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        fprintf(fout, "%s%c%s%c%d%c", ptr->username, '\t', ptr->password, '\t', ptr->accountStatus, '\n');
    }
    fclose(fout);
}

int main(int argc, char const *argv[])
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t ret;
    fd_set readfds, allset;
    char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char username[30], password[30];
    int status[BACKLOG], sessionStatus;
    Account *account[BACKLOG];
    Request_Login *rcvRequest = (Request_Login *)malloc(sizeof(Request_Login));
    Response *loginResponse = (Response *)malloc(sizeof(Response));
    headQuestion= createQuestionList();

    if (argc != 2)
    {
        printf("Params invalid\n");
    }
    else
    {
        readQuestionFromFile(headQuestion);
        printListQuestion(headQuestion);
        if (checkPort(argv[1]) == 1)
        {
            loadDataBase();
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

                for (i = 0; i <= maxi; i++)
                { /* check all clients for data */
                    if ((sockfd = client[i]) < 0)
                        continue;
                    if (FD_ISSET(sockfd, &readfds))
                    {
                        ret = receiveRequestLogin(sockfd, rcvRequest, sizeof(Request_Login), 0);
                        if (ret <= 0)
                        {
                            FD_CLR(sockfd, &allset);
                            close(sockfd);
                            client[i] = -1;
                        }
                        else
                        {
                            // status_client = status[i];
                            // processData(acc, &status_client, code, data, rcvBuff, result);
                            // status[i]=status_client;

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
                                            loginResponse->code = USER_NAME_BLOCKED;
                                        }
                                        else
                                        {
                                            status[i] = UNAUTHENTICATE;
                                            loginResponse->code = USER_NAME_FOUND;
                                        }
                                    }
                                    else
                                    {
                                        loginResponse->code = USER_NAME_NOT_FOUND;
                                    }
                                    break;
                                case UNAUTHENTICATE:
                                    if (findUserNameAccount(head, rcvRequest->message) != NULL)
                                    {
                                        account[i] = findUserNameAccount(head, rcvRequest->message);
                                        if (account[i]->accountStatus == BLOCKED)
                                        {
                                            loginResponse->code = USER_NAME_BLOCKED;
                                        }
                                        else
                                        {
                                            loginResponse->code = USER_NAME_FOUND;
                                        }
                                    }
                                    else
                                    {
                                        status[i] = NOT_LOGGED_IN;
                                    }
                                    break;
                                case AUTHENTICATED:
                                    loginResponse->code = USER_NAME_IS_SIGNIN;
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
                                            loginResponse->code = PASSWORD_INCORRECT_THREE_TIMES;
                                            exportDataToFile();
                                        }
                                        else
                                        {
                                            loginResponse->code = PASSWORD_INCORRECT;
                                        }
                                    }
                                    else
                                    {
                                        if (account[i]->status == ONLINE)
                                        {
                                            loginResponse->code = PASSWORD_CORRECT_BUT_ACCOUNT_IS_SIGNINED_IN_ORTHER_CLIENT;
                                        }
                                        else
                                        {
                                            status[i] = AUTHENTICATED;
                                            signinAccount(head, account[i]->username);
                                            account[i]->countSignIn = 0;
                                            loginResponse->code = PASSWORD_CORRECT;
                                            strcpy(loginResponse->data, account[i]->username);
                                        }
                                    }
                                    break;
                                default:
                                    loginResponse->code = INVALID_OPERATION;
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
                                        account[i]->status = OFFLINE;
                                        loginResponse->code = LOGOUT_SUCCESS;
                                        strcpy(loginResponse->data, account[i]->username);
                                    }
                                    else
                                    {
                                        loginResponse->code = LOGOUT_NOT_SIGNIN;
                                    }
                                    break;
                                }
                            }
                            else if (rcvRequest->code == REGISTER)
                            {
                                switch (status[i])
                                {
                                case NOT_LOGGED_IN:
                                    if (checkSpace(rcvRequest->message))
                                    {
                                        splitMessageData(rcvRequest->message, username, password);
                                        if (findUserNameAccount(head, username) != NULL)
                                        {
                                            loginResponse->code = REGISTER_USERNAME_EXISTED;
                                        }
                                        else
                                        {
                                            addAccount(head, username, password, ACTIVE);
                                            loginResponse->code = REGISTER_SUCCESSFULL;
                                        }
                                    }
                                    else
                                    {
                                        loginResponse->code = SYNTAX_ERROR;
                                    }
                                    break;
                                default:
                                    loginResponse->code = SYNTAX_ERROR;
                                    break;
                                }
                            }
                            else
                            {
                                loginResponse->code = SYNTAX_ERROR;
                            }

                            setMessageResponse(loginResponse);
                            sendResponse(sockfd, loginResponse, sizeof(Response), 0);
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
