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

void splitQuestionFromFile(char *input, char *level, char *question, char *answer1, char *answer2, char *answer3, char *answer4, char *answerTrue)
{
	int levelLength = 0;
	int questionLength = 0;
	int answer1Length = 0;
	int answer2Length = 0;
	int answer3Length = 0;
	int answer4Length = 0;
	int answerTrueLength = 0;
	int i;
	//split level
	for (i = 0; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		level[levelLength++] = input[i];
	}
	i++;
	level[levelLength] = '\0';
	//split question
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		question[questionLength++] = input[i];
	}
	i++;
	question[questionLength] = '\0';
	//split answer1
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		answer1[answer1Length++] = input[i];
	}
	i++;
	answer1[answer1Length] = '\0';
	//split answer2
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		answer2[answer2Length++] = input[i];
	}
	i++;
	answer2[answer2Length] = '\0';
	//split answer3
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		answer3[answer3Length++] = input[i];
	}
	i++;
	answer3[answer3Length] = '\0';
	//split answer4
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '|')
			break;
		answer4[answer4Length++] = input[i];
	}
	i++;
	answer4[answer4Length] = '\0';
	//split answer true
	for (i; i < strlen(input); i++)
	{
		if (input[i] == '\n')
			break;
		answerTrue[answerTrueLength++] = input[i];
	}
	answerTrue[answerTrueLength] = '\0';
}
