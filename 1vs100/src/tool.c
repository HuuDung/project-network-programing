#include "../libs/tool.h"

void splitMessage(char *input, char *code, char *data)
{
	int i, codeLength = 0, dataLength = 0;
	for (i = 0; input[i] != ' '; i++)
	{
		code[codeLength++] = input[i];
	}
	code[codeLength] = '\0';
	i++;
	for (i; i < strlen(input); i++)
	{
		data[dataLength++] = input[i];
	}
	data[dataLength] = '\0';
}

void splitMessageData(char *input, char *username, char *password)
{
	int i, usernameLength = 0, passwordLength = 0;
	for (i = 0; input[i] != ' '; i++)
	{
		username[usernameLength++] = input[i];
	}
	username[usernameLength] = '\0';
	i++;
	for (i; i < strlen(input); i++)
	{
		password[passwordLength++] = input[i];
	}
	password[passwordLength] = '\0';
}