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

void tutorial()
{
    printf("--------------------------------------\n");
    printf("\nLogin Tutorial: ");
    printf("\n\tSignin syntax: USER username");
    printf("\n\tRegister syntax: REGISTER username password");
    printf("\n\tPassword syntax: PASS password");
    printf("\n\tLogout syntax: LOGOUT username");
    printf("\n\tQuit syntax: QUIT \n");
    printf("\n--------------------------------------");
    printf("\nInput to syntax: \n");
}
int main(int argc, char const *argv[])
{
    int client_sock, servPort;
    char buff[BUFF_SIZE], username[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char code[BUFF_SIZE], data[BUFF_SIZE];

    Request_Login *request = (Request_Login *)malloc(sizeof(Request_Login));
    Response *loginResponse = (Response *)malloc(sizeof(Response));

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

                tutorial();
                memset(buff, '\0', (strlen(buff) + 1));
                fgets(buff, BUFF_SIZE, stdin);
                buff[strlen(buff) - 1] = '\0';
                if (strcmp(buff, "QUIT") == 0)
                    break;
                // send(client_sock, buff, strlen(buff), 0);
                setOpcodeRequestLogin(request, buff);
                sendRequest(client_sock, request, sizeof(Request_Login), 0);
                receiveResponse(client_sock, loginResponse, sizeof(Response), 0);
                readMessageResponse(loginResponse);
            }
            //Step 5: Close socket
            close(client_sock);
            return 0;
        }
    }
}