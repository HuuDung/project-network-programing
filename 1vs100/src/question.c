#include "../libs/question.h"
Question **createQuestionList()
{
    Question **head = (Question **)malloc(sizeof(Question *));
    *head = NULL;

    return head;
}
LIST_ANSWER convertListAnswer(char *input)
{
    if (strcmp(input, "A") == 0)
        return A;
    else if (strcmp(input, "B") == 0)
        return B;
    else if (strcmp(input, "C") == 0)
        return C;
    else if (strcmp(input, "D") == 0)
        return D;
}

LEVEL convertLevel(char *input)
{
    if (strcmp(input, "Easy") == 0)
        return EASY;
    else if (strcmp(input, "Normal") == 0)
        return NORMAL;
    else if (strcmp(input, "Hard") == 0)
        return HARD;
}
Question *newQuestion(char *level, char *question, char *answer1, char *answer2, char *answer3, char *answer4, char *answerTrue)
{
    Question *new = (Question *)malloc(sizeof(Question));
    new->level = convertLevel(level);
    strcpy(new->question, question);
    strcpy(new->answer1, answer1);
    strcpy(new->answer2, answer2);
    strcpy(new->answer3, answer3);
    strcpy(new->answer4, answer4);
    new->true_ans = convertListAnswer(answerTrue);
    return new;
}

void addQuestion(Question **head, char *level, char *question, char *answer1, char *answer2, char *answer3, char *answer4, char *answerTrue)
{
    Question *new = newQuestion(level, question, answer1, answer2, answer3, answer4, answerTrue);
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

void readQuestionFromFile(Question **head)
{
    FILE *fin;
    fin = fopen("../data/question.txt", "r");
    char input[BUFF_SIZE];
    char level[BUFF_SIZE];
    char question[BUFF_SIZE];
    char answer1[BUFF_SIZE];
    char answer2[BUFF_SIZE];
    char answer3[BUFF_SIZE];
    char answer4[BUFF_SIZE];
    char answerTrue[BUFF_SIZE];

    while (1)
    {
        if (feof(fin))
            break;
        fgets(input, BUFF_SIZE, fin);
        splitQuestionFromFile(input, level, question, answer1, answer2, answer3, answer4, answerTrue);
        addQuestion(head, level, question, answer1, answer2, answer3, answer4, answerTrue);
    }
    fclose(fin);
}

void printListQuestion(Question **head)
{
    Question *ptr = NULL;
    for (ptr = *head; ptr != NULL; ptr = ptr->next)
    {
        printf("Level: %d\n", ptr->level);
        printf("Question: %s\n", ptr->question);
        printf("A: %s\n", ptr->answer1);
        printf("B: %s\n", ptr->answer2);
        printf("C: %s\n", ptr->answer3);
        printf("D: %s\n", ptr->answer4);
        printf("True ans: %d\n", ptr->true_ans);
    }
}
