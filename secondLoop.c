#include "set.h"
/*declare functions*/

/*assistFunctions*/
int currentAddressCount(int value, int dataFlag);
char **costumStrTokSplit(char *line, char symbolTok);
wordList *fullHandleLine(char **wordsArray, int lookUpLocation);
void proparlyFreeRes(char **res);

/*symbolsTable*/
int setSecondLoopFlag(int flag);
int changeAllDataAttribute();
int changeAllDataAddresses();

typedef struct
{
    char **array;
    int *addressArray;
    int counter;
} requireSecondLoop;

static requireSecondLoop secondLoopObj;

/*this function add line for secondLoop*/
int addToSecondLoopList(char *fullLine)
{
    if (secondLoopObj.array == NULL)
    {
        secondLoopObj.counter = 1;
        secondLoopObj.array = (char **)calloc(1, sizeof(char *));
        secondLoopObj.addressArray = (int *)calloc(1, sizeof(int *));

        (secondLoopObj.array)[secondLoopObj.counter - 1] = fullLine;
        (secondLoopObj.addressArray)[secondLoopObj.counter - 1] = currentAddressCount(0, 0);
    }
    else
    {
        char **temp;
        int *addressArrayTemp;

        temp = (char **)realloc(secondLoopObj.array, ++(secondLoopObj.counter) * sizeof(char *));
        addressArrayTemp = (int *)realloc(secondLoopObj.addressArray, (secondLoopObj.counter) * sizeof(int *));

        if (!temp || !addressArrayTemp)
        {
            printf("wtf?");
            return 1;
        }
        secondLoopObj.array = temp;
        secondLoopObj.addressArray = addressArrayTemp;

        (secondLoopObj.array)[secondLoopObj.counter - 1] = fullLine;
        (secondLoopObj.addressArray)[secondLoopObj.counter - 1] = currentAddressCount(0, 0);
    }
    currentAddressCount(4, 0);

    return 0;
}

/*main start for the second loop*/
int startSecondLoop()
{
    int i, checkState = secondLoopObj.counter,flag=0;
    changeAllDataAttribute();
    changeAllDataAddresses();
    setSecondLoopFlag(1);
    for (i = 0; i < checkState; i++)
    {
        if (checkState == secondLoopObj.counter)
        {
            char **res = NULL, space = ' ';
            wordList *pointerToWordFunction;
            currentAddressCount(secondLoopObj.addressArray[i] - currentAddressCount(0, 0), 0);

            res = costumStrTokSplit(secondLoopObj.array[i], space);

            pointerToWordFunction = fullHandleLine(res, 0);
            if (pointerToWordFunction->funct != -99)
            {
                flag+= (pointerToWordFunction->function)(res, pointerToWordFunction, 0);
            }
            else if (pointerToWordFunction->opcode == -101)
            {
                printf("\nOH NO ERROR CODE (%d) line(%s) array(%s)\n", pointerToWordFunction->opcode, secondLoopObj.array[i], res[0]);
                return 1;
            }
        }
        else
        {
            printf(" Variable Not Found even in second loop | %s \n", secondLoopObj.array[i - 1]);
            return 1;
        }
    }
    return flag;
}

/* void printAllSecondLoop()
 * {
 *     int i;
 *
 *     if (secondLoopObj.array != NULL)
 *     {
 *         printf("\nALL SECOND LOOP (%d):\n", secondLoopObj.counter);
 *         for (i = 0; i < secondLoopObj.counter; i++)
 *             printf("%s %d\n", secondLoopObj.array[i], secondLoopObj.addressArray[i]);
 *     }
 * }
 */

void resetSecondLoop()
{
    free(secondLoopObj.array);
    free(secondLoopObj.addressArray);
    secondLoopObj.array = NULL;
    secondLoopObj.addressArray = NULL;
    secondLoopObj.counter = 1;
}
