#include "set.h"

/*declare functions*/

/*currentFile*/
int handleLine(char *line);

/*assistFunctions*/
char **costumStrTokSplit(char *line, char symbolTok);
wordList *fullHandleLine(char **wordsArray, int lookUpLocation);
void proparlyFreeRes(char **res);

/* reads each line from the file and
* send the line to handleLine */
int firstTransition(char *fileName)
{
    char buff[LINESZ];
    int flag = 0;
    FILE *fin = fopen(fileName, "r");
    printf("starting firstTransition: FILE NAME %s\n", fileName);
    if (fin != NULL)
    {
        while (fgets(buff, LINESZ, fin))
        {
            /* Process buff here. */
                flag += handleLine(buff);
        }
        fclose(fin);
    }
    else
    {
        printf("file not found\n");
        return 1;
    }
    return flag;
}

/* handle each Line from the file
   Sends each line to its handler function*/
int handleLine(char *line)
{
    char **res = NULL,
           space = ' ';
    wordList *pointerToWordFunction;
    int answer=0;

    res = costumStrTokSplit(line, space);

    if (res[0] == 0 || strlen(res[0]) <= 1)
        return 0;
    pointerToWordFunction = fullHandleLine(res, 0);
    if (pointerToWordFunction->funct != -99)
        answer=  ((pointerToWordFunction->function)(res, pointerToWordFunction, 0));
    if(answer==1)
    {
        printf("error in line %s",line);
        return 1;
    }
    else if (pointerToWordFunction->opcode == -101)
    {
        printf("\nOH NO ERROR CODE (%d) line(%s) array(%s)\n", pointerToWordFunction->opcode, line, res[0]);
        return 1;
    }

    /* free the memory allocated */
    proparlyFreeRes(res);

    return 0;
}
