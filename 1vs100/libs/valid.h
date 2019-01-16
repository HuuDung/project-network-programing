#ifndef __VALID__
#define __VALID__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 2014

#define TRUE 1
#define FALSE 0

int checkValue(int value); 
int checkDigit(char character);
int checkDotInString(char *string);
int checkIP(char *string);
int checkPort(char *port);
int checkSpace(char *input);
int checkIPAndPort(char *ipaddr, char *port);
#endif