
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

void printQuestion(Question **head){
    Question *p = NULL;
    for (p = *head; p != NULL; p = p->next)
    printf("%s%s%s\n\n",p->level,p->question,(p->ans).content);
}
void addQuestion(Question **head, Question *new)
{
    Question *current = (*head);

    if (*head == NULL)
    {
        (*head) = new;
    }
    else
    {
        while (current->next != NULL)
            current = current->next;
        current->next = new;
    }
}
Question* fileReader(FILE *file){
    Question *i = malloc(sizeof(Question));
    int numberQ;
    int tmp;
    file = fopen("question.txt","r");
    fscanf(file,"%d",&numberQ);
    fgetc(file);
    for(int j=0;j<numberQ;j++){
        Question *new = malloc(sizeof(Question));
        fgets(new->level,1000,file);
        fgets(new->question,1000,file);
        fgets((new->ans).content,1000,file);
        addQuestion(i,new);  
    }
    return i;
}
int main(){

    FILE *x = fopen("question.txt","r");
    Question *q = fileReader(x);
    printQuestion(q);
    fclose(x);
}