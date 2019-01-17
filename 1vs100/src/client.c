#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "../libs/protocol.h"
#include "../libs/tool.h"
#include "../libs/valid.h"

#define BUFF_SIZE 1024

void loginTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nLogin Tutorial: ");
    printf("\n\tSignin syntax: USER username");
    printf("\n\tRegister syntax: REGISTER username password");
    printf("\n\tPassword syntax: PASS password");
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nInput to syntax: \n");
}

void gamePlayForNormalTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tAnswer syntax: ANSWER answer");
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nInput to syntax: \n");
}
void chooseTopicLevel()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial (Choose level): ");
    printf("\n\tChoose Topic Level syntax: TOPIC level (level: EASY, NORMAL, HARD)");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
void gamePlayForSpecialTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tChoose Answer syntax: ANSWER answer");
    printf("\n\tUse Help syntax: HELP");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
void showQuestion(Question *question)
{
    printf("\n%s", question->question);
    printf("\n%s", question->answer1);
    printf("\n%s", question->answer2);
    printf("\n%s", question->answer3);
    printf("\n%s", question->answer4);
}
void waitingPlayer()
{
    printf("Wating orther player...\n");
}
int main(int argc, char const *argv[])
{
    int client_sock, servPort;
    char buff[BUFF_SIZE], username[BUFF_SIZE], luckyPlayer[BUFF_SIZE], topic[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char code[BUFF_SIZE], data[BUFF_SIZE];
    GAMEPLAY_STATUS status = UNAUTH;
    Question *ques = (Question *)malloc(sizeof(Question));
    Request *request = (Request *)malloc(sizeof(Request));
    Response *response = (Response *)malloc(sizeof(Response));
    Information *infor = (Information *)malloc(sizeof(Information));
    int lucky = FALSE, existQuestion = FALSE, help = FALSE;
    int questionNumber = 0;
    float score = 0;
    int inforamation = TRUE;
    int gameStatus = GAME_PLAYING;
    if (argc != 3)
    {
        printf("\nParams incorrect\n");
    }
    else
    {
        //Check input : IP address & Port
        if (checkIPAndPort(argv[1], argv[2]) != 0)
        {
            //Step 1: Construct socket
            client_sock = socket(AF_INET, SOCK_STREAM, 0);

            //Step 2: Specify server address
            servPort = atoi(argv[2]);

            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(servPort);
            server_addr.sin_addr.s_addr = inet_addr(argv[1]);
            //Step 3: Request to connect server
            if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
            {
                printf("\nError!Can not connect to sever! Client exit imediately! ");
                return 0;
            }
            while (1)
            {
                switch (status)
                {
                case UNAUTH:
                    loginTutorial();
                    memset(buff, '\0', (strlen(buff) + 1));
                    fgets(buff, BUFF_SIZE, stdin);
                    buff[strlen(buff) - 1] = '\0';
                    setOpcodeRequest(request, buff);
                    sendRequest(client_sock, request, sizeof(Request), 0);
                    receiveResponse(client_sock, response, sizeof(Response), 0);
                    readMessageResponse(response);
                    status = response->status;
                    if (response->data != NULL)
                    {
                        memset(username, '\0', (strlen(username) + 1));
                        strcpy(username, response->data);
                    }
                    break;
                case WAITING_PLAYER:
                    requestGet(client_sock);
                    receiveResponse(client_sock, response, sizeof(Response), 0);
                    status = response->status;
                    if (status == WAITING_QUESTION)
                    {
                        readMessageResponse(response);
                        memset(luckyPlayer, '\0', (strlen(luckyPlayer) + 1));
                        strcpy(luckyPlayer, response->data);
                        printf("Lucky player: %s\n", luckyPlayer);

                        if (strcmp(luckyPlayer, username) == 0)
                            lucky = TRUE;
                        else
                            lucky = FALSE;
                    }
                    break;
                case WAITING_QUESTION:
                    if (lucky == TRUE)
                    {
                        if (inforamation == FALSE)
                        {
                            requestCheckInformation(client_sock);
                            receiveInformation(client_sock, infor, sizeof(Information), 0);
                            if (infor->status == TRUE)
                            {
                                inforamation = TRUE;
                                if (help == FALSE)
                                {
                                    score = score + infor->score;
                                    printf("Số người trả lời sai câu hỏi trên: %d\n", infor->playerAnswerWrong);
                                    printf("Số điểm bạn nhận được: %.1f\n", infor->score);
                                    printf("Số người cùng chơi: %d\n", infor->playerPlaying);
                                    printf("Số điểm của bạn hiện tại: %.1f\n", score);
                                }
                                else
                                {
                                    score = score - infor->score;
                                    printf("Số điểm bạn nhận bị trừ: %.1f\n", infor->score);
                                    printf("Số người trả lời sai câu hỏi trên: %d\n", infor->playerAnswerWrong);
                                    printf("Số người cùng chơi: %d\n", infor->playerPlaying);
                                    printf("Số điểm của bạn hiện tại: %.1f\n", score);
                                    help = FALSE;
                                }
                            }
                        }
                        else
                        {
                            requestGet(client_sock);
                            receiveResponse(client_sock, response, sizeof(Response), 0);
                            inforamation = FALSE;
                            if (response->status == END_GAME)
                            {
                                status = response->status;
                                readMessageResponse(response);
                            }
                            else
                            {
                                chooseTopicLevel();
                                memset(buff, '\0', (strlen(buff) + 1));
                                fgets(buff, BUFF_SIZE, stdin);
                                buff[strlen(buff) - 1] = '\0';
                                setOpcodeRequest(request, buff);
                                sendRequest(client_sock, request, sizeof(Request), 0);
                                receiveResponse(client_sock, response, sizeof(Response), 0);
                                status = response->status;
                                if (status == PLAYING)
                                {
                                    strcpy(topic, response->data);
                                    readMessageResponse(response);
                                }
                                if (status == WAITING_QUESTION)
                                {
                                    readMessageResponse(response);
                                    inforamation = TRUE;
                                }
                            }
                        }
                    }
                    else
                    {
                        requestGet(client_sock);
                        receiveResponse(client_sock, response, sizeof(Response), 0);
                        if (response->status == END_GAME)
                        {
                            status = response->status;
                            readMessageResponse(response);
                        }
                        else
                        {
                            requestGet(client_sock);
                            receiveResponse(client_sock, response, sizeof(Response), 0);
                            status = response->status;
                            if (status == PLAYING)
                            {
                                strcpy(topic, response->data);
                                readMessageResponse(response);
                            }
                        }
                    }
                    break;
                case PLAYING:
                    if (lucky == TRUE)
                    {
                        if (existQuestion == TRUE)
                        {
                            printf("\nCâu hỏi số %d\n", questionNumber);
                            printf("Chủ đề: %s", topic);
                            showQuestion(ques);
                            printf("\nCâu trả lời: \n");
                            gamePlayForSpecialTutorial();
                            if (strcmp(buff, "HELP") == 0)
                            {
                                requestGetHelp(client_sock);
                            }
                            else
                            {
                                memset(buff, '\0', (strlen(buff) + 1));
                                fgets(buff, BUFF_SIZE, stdin);
                                buff[strlen(buff) - 1] = '\0';

                                setOpcodeRequest(request, buff);
                                sendRequest(client_sock, request, sizeof(Request), 0);
                            }
                            //
                            receiveResponse(client_sock, response, sizeof(Response), 0);

                            status = response->status;
                            readMessageResponse(response);
                            if (status == WAITING_QUESTION)
                            {
                                existQuestion = FALSE;
                            }
                            if (response->code == USER_USED_HINT_SUCCESS)
                                help = TRUE;
                        }
                        else
                        {
                            requestGet(client_sock);
                            receiveQuestion(client_sock, ques, sizeof(Question), 0);
                            existQuestion = TRUE;
                            questionNumber++;
                        }
                    }
                    else
                    {
                        if (existQuestion == TRUE)
                        {
                            printf("\nCâu hỏi số %d\n", questionNumber);
                            printf("Chủ đề: %s", topic);
                            showQuestion(ques);
                            printf("\nCâu trả lời: \n");
                            gamePlayForNormalTutorial();

                            memset(buff, '\0', (strlen(buff) + 1));
                            fgets(buff, BUFF_SIZE, stdin);
                            buff[strlen(buff) - 1] = '\0';
                            setOpcodeRequest(request, buff);
                            sendRequest(client_sock, request, sizeof(Request), 0);

                            receiveResponse(client_sock, response, sizeof(Response), 0);
                            status = response->status;
                            if (status == WAITING_QUESTION)
                            {
                                readMessageResponse(response);
                                existQuestion = FALSE;
                            }
                            else if (status == PLAYING)
                                readMessageResponse(response);
                        }
                        else
                        {
                            requestGet(client_sock);
                            receiveQuestion(client_sock, ques, sizeof(Question), 0);
                            existQuestion = TRUE;
                            questionNumber++;
                        }
                    }
                    break;
                case END_GAME:
                    if (lucky == TRUE)
                    {
                        if (inforamation == FALSE)
                        {
                            requestCheckInformation(client_sock);
                            receiveInformation(client_sock, infor, sizeof(Information), 0);
                            if (infor->status == TRUE)
                            {
                                printf("Số điểm bạn nhận được là: %1.f\n", infor->score);
                                inforamation = TRUE;
                            }
                        }
                        else
                        {
                            requestLogout(client_sock, username);
                            receiveResponse(client_sock, response, sizeof(Response), 0);
                            status = response->status;
                            readMessageResponse(response);
                            gameStatus = GAME_END;
                        }
                    }
                    else
                    {
                        if (inforamation == TRUE)
                        {
                            inforamation = FALSE;
                            requestCheckInformation(client_sock);
                            receiveInformation(client_sock, infor, sizeof(Information), 0);
                            printf("Số điểm bạn nhận được là: %1.f\n", infor->score);
                        }
                        else
                        {
                            requestLogout(client_sock, username);
                            receiveResponse(client_sock, response, sizeof(Response), 0);
                            status = response->status;
                            readMessageResponse(response);
                            gameStatus = GAME_END;
                        }
                    }
                    break;
                }
                if (gameStatus == GAME_END)
                    break;
            }
            //Step 5: Close socket
            close(client_sock);
            return 0;
        }
    }
}