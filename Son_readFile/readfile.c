
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFF_SIZE 1000
typedef enum
{
    A,
    B,
    C,
    D,
} LIST_ANSWER;

typedef enum
{
    EASY,
    NORMAL,
    HARD,
} LEVEL;

typedef struct
{
    char content[BUFF_SIZE];
    LIST_ANSWER answer;
} Answer;

typedef struct
{
    char question[BUFF_SIZE];
    Answer ans;
    char level[30];
    LIST_ANSWER true_ans;
    struct Question *next;
} Question;

void printQuestion(Question *i){
    printf("%s\t\t%s\t\t%s\n\n",i->level,i->question,(i->ans).content);
    if(i->next != NULL){
        printQuestion(i->next);
    }
}
Question* fileReader(FILE *file){
    Question *i = malloc(sizeof(Question));
    char *lv;
    char *qs;
    char *as;
    int fileRead1 = fgets(lv,'\t\t',file);
    int fileRead2 = fgets(qs,'\t\t',file);
    int fileRead3 = fgets(as,'\0',file);
    if((fileRead1 != EOF) && (fileRead2 != EOF) && (fileRead3 != EOF)){
        strcpy(i->level,lv);
        strcpy(i->question,qs);
        strcpy((i->ans).content,as);
        i->next = fileReader(file);
    }
    if((fileRead1 == EOF)|| (fileRead2 == EOF) || (fileRead3 == EOF)) {
        return NULL;
    }
    return i;
}
int main(){
    FILE *x = fopen("question.txt","r");
    Question *q = fileReader(x);
    printQuestion(q);
    fclose(x);
}