#include "set.h"

/*declare functions*/
/*symbolsTable*/
int addSymbol(char *symbolstr, int attribute);
int searchSymbolAddress(char *nameOfSymbol, int searchFlag);
int setSecondLoopFlag(int flag);
void resetSymbolsTable();

/*secondLoop*/
int addToSecondLoopList(char *fullLine);
void resetSecondLoop();

/*memoryMap*/
int addToMemoryTable(char *code, unsigned machineCode, int size, int dataFlag);
void resetMamoryMap();

/* array of every function and his string and type
 * This variable was supposed to be in set.h but
 * I tried to avoid the warning 'not used' so its in the function*/
wordList *getWordList()
{
    static wordList allWords[] =
    {
        {"add", typeR, 0, 1, 'R'},
        {"sub", typeR, 0, 2, 'R'},
        {"and", typeR, 0, 3, 'R'},
        {"or", typeR, 0, 4, 'R'},
        {"nor", typeR, 0, 5, 'R'},
        {"move", typeR, 1, 1, 'R'},
        {"mvhi", typeR, 1, 2, 'R'},
        {"mvlo", typeR, 1, 3, 'R'},
        {"addi", typeI, 10, 0, 'I'},
        {"subi", typeI, 11, 0, 'I'},
        {"andi", typeI, 12, 0, 'I'},
        {"ori", typeI, 13, 0, 'I'},
        {"nori", typeI, 14, 0, 'I'},
        {"bne", typeI, 15, 0, 'I'},
        {"beq", typeI, 16, 0, 'I'},
        {"blt", typeI, 17, 0, 'I'},
        {"bgt", typeI, 18, 0, 'I'},
        {"lb", typeI, 19, 0, 'I'},
        {"sb", typeI, 20, 0, 'I'},
        {"lw", typeI, 21, 0, 'I'},
        {"sw", typeI, 22, 0, 'I'},
        {"lh", typeI, 23, 0, 'I'},
        {"sh", typeI, 24, 0, 'I'},
        {"jmp", typeJ, 30, 0, 'J'},
        {"la", typeJ, 31, 0, 'J'},
        {"call", typeJ, 32, 0, 'J'},
        {"stop", typeJ, 63, 0, 'J'},
        {".dh", guideWord, -1, 2, 'G'},
        {".dw", guideWord, -1, 4, 'G'},
        {".db", guideWord, -1, 1, 'G'},
        {".asciz", guideWord, -2, 1, 'G'},
        {".entry", guideWord, -3, 7, 'G'},
        {".extern", guideWord, -4, 8, 'G'},
        {"DOTS", guideWord, -5, -98, 'G'},
        {"NULL", NULL, -100, -99, 'N'},
        {"ERROR", NULL, -101, -99, 'E'}
    };
    return allWords;
}

/*remove spaces and newLine
 *the idea is similar to strtok
 *but unlike strtok this function copies
 *the given string and puts it in array of pointers
 *=> more stable function then strtok*/
char **costumStrTokSplit(char *line, char symbolTok)
{
    int i, resCounter = 0;
    char *pointer = NULL, **res = NULL;
    res = (char **)calloc(1, sizeof(char **));
    if (!res)
        printf("\nWtf No space? -customStrTokSplit 1.0-\n");
    for (i = 0; line[i] != '\0' || pointer != 0; i++)
    {
        if (line[i] == symbolTok || line[i] == '\n' || line[i] == '\0')
        {
            if (pointer != NULL)
            {
                /*add to arr And set pointer to null*/
                char *newString, **newRes;
                newString = (char *)calloc(line + i - pointer + 1, sizeof(char));
                if (!newString)
                    printf("\nWtf No space? -customStrTokSplit 1.1-\n");
                strncat(newString, pointer, line + i - pointer);
                if (strchr(newString, '\r') != 0)
                    *(strchr(newString, '\r')) = '\0';
                res[resCounter++] = newString;
                newRes = (char **)realloc(res, (resCounter + 1) * sizeof(char **));
                if (!newRes)
                    printf("\nWtf No space? -customStrTokSplit 1.2-\n");
                else
                    res = newRes;
                pointer = NULL;
            }
        }
        else
        {
            if (pointer == NULL)
                pointer = line + i;
        }
    }

    res[resCounter] = 0;
    return res;
}
/*this function hold the current address for each data & Instructions separately
 *its also possible to change the current value by given value*/
int currentAddressCount(int value, int dataFlag)
{
    if (dataFlag)
    {
        static int currentAddressCountData = 0;
        currentAddressCountData += value;
        return currentAddressCountData - value;
    }
    else
    {
        static int currentAddressCount = 100;
        currentAddressCount += value;
        return currentAddressCount - value;
    }
}
/* The purpose of this function to free the array from the customstrtok*/
void proparlyFreeRes(char **res)
{
    int i;
    for (i = 0; res[i] != NULL; i++)
    {
        free(res[i]);
    }
    free(res);
}
/*this function reverse the bits in a given unsigned int*/
unsigned int reverseBits(unsigned int num, unsigned int NO_OF_BITS)
{
    unsigned int reverse_num = 0;
    int i;
    for (i = 0; i < NO_OF_BITS; i++)
    {
        if ((num & (1 << i)))
            reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
    }
    return reverse_num;
}

/*this function return a copy of a given 'string' = array of char*/
char *stringCopy(char *stringToCopyFrom)
{
    char *newString;
    newString = (char *)calloc(strlen(stringToCopyFrom), sizeof(char));
    if (newString)
        strcpy(newString, stringToCopyFrom);
    return newString;
}

/*This function sorts each command into its handler function */
wordList *fullHandleLine(char **wordsArray, int lookUpLocation)
{
    int i;
    for (i = 0; i <= wordListSize; i++)
        if (!strcmp(wordsArray[lookUpLocation], getWordList()[i].word))
            return getWordList() + i;

    /*CHECK FOR GUIDEWORDS */
    if (wordsArray[lookUpLocation][strlen(wordsArray[lookUpLocation]) - 1] == ':')
    {
        return getWordList() + DOTSFUNCTION;
    }
    else if (*wordsArray[lookUpLocation] == ';')
    {
        return getWordList() + NULLFUNCTION;
    }

    return getWordList() + ERRORFUNCTION;
}

/*this function united the given array to one long string*/
char *stringArrToString(char **string)
{
    int i, totalLength = 1;
    char *completeLine;

    for (i = 0; string[i] != NULL; i++)
        totalLength += strlen(string[i]);
    completeLine = (char *)calloc(totalLength + i, sizeof(char));

    if (!completeLine)
        return NULL;
    strcpy(completeLine, string[0]);
    for (i = 1; string[i] != NULL; i++)
    {
        strcat(completeLine, " ");
        strcat(completeLine, string[i]);
    }
    return completeLine;
}

/*this function handle each command arguments*/
int handleArguments(int mode, int *arrayNumbers, char *arguments)
{
    char *variablePointer = NULL;
    int i = 0, situation = 0, arrayNumberCounter = 0, buildNumber = 0, nextNumMinus = 0;

    if (mode == 5)
        situation = 3;

    while (arguments[i] != '\0')
    {
        if (!isspace(arguments[i]))
            switch (situation)
            {
            case 0:
                /*            wait for $*/
                if (arguments[i] == '$')
                {
                    situation++;
                }
                else
                {
                    printf("error $\n");
                    return 1;
                }

                break;
            case 1:
                /*            wait for numbers*/
                if (isdigit(arguments[i]))
                {
                    situation++;
                    buildNumber *= 10;
                    buildNumber += arguments[i] - '0';
                    if (nextNumMinus)
                        buildNumber *= -1;
                    nextNumMinus = 0;
                }
                else
                {
                    printf("error number case 1\n");
                    return 1;
                }
                break;
            case 2:
                /*wait for numbers or comma*/
                if (arguments[i] == ',')
                {
                    /*check number of arguments for possible error*/
                    if ((arrayNumberCounter >= 2 && mode != 5) || (arrayNumberCounter >= 1 && mode == 2))
                    {
                        printf("FULL ERROR,to many numbers\n");
                        return 1;
                    }
                    else
                    {
                        /*send modes to their cases*/
                        if ((mode == 5) || (buildNumber >= 0 && buildNumber <= 31) || (mode == 3 && arrayNumberCounter == 1 && buildNumber >= -32767 && buildNumber <= 32767))
                        {
                            arrayNumbers[arrayNumberCounter++] = buildNumber;
                            buildNumber = 0;
                            if (mode == 3 && arrayNumberCounter == 1)
                                situation = 3;
                            else if (mode == 4 && arrayNumberCounter == 2)
                                situation = 4;
                            else if (mode == 5)
                                situation = 3;
                            else
                                situation = 0;
                        }
                        else
                        {
                            printf("ERROR number not in range\n");
                            return 1;
                        }
                    }
                }
                else if (isdigit(arguments[i]))
                {
                    buildNumber *= 10;
                    if (buildNumber < 0)
                        buildNumber -= arguments[i] - '0';
                    else
                        buildNumber += arguments[i] - '0';
                }
                else
                {
                    printf("error number case 2 (%d) (%d)\n", arguments[i], i);
                    return 1;
                }
                break;
            case 3:
                /*wait for numbers or minus*/
                if (isdigit(arguments[i]))
                {
                    situation = 2;
                    buildNumber *= 10;
                    buildNumber += arguments[i] - '0';
                }
                else if (arguments[i] == '-')
                {
                    nextNumMinus = 1;
                    situation = 1;
                }
                else if (arguments[i] != '+')
                {
                    situation = 1;
                }
                else
                {
                    printf("error number case 3\n");
                    return 1;
                }
                break;
            case 4:
                /*variable*/
                if (!isalpha(arguments[i]) && !isdigit(arguments[i]))
                {
                    printf("Illegel variable\n");
                    return 1;
                }
                if (variablePointer == NULL)
                    variablePointer = arguments + i;

                break;
            }
        i++;
    }

    if (situation == 2)
    {
        if ((buildNumber >= 0 && buildNumber <= 31) || (mode == 5))
            arrayNumbers[arrayNumberCounter++] = buildNumber;
        else
        {
            printf("Error number not in range\n");
            return 1;
        }
    }
    else if (situation == 4)
    {
        int variableAddress = searchSymbolAddress(variablePointer, setSecondLoopFlag(0));
        if (variableAddress != -1)
            arrayNumbers[arrayNumberCounter++] = variableAddress;
        else
            return 2;
    }
    else
    {
        printf("Error illegel end\n");
        return 1;
    }
    return 0;
}

/*this function hundle only type R cases*/
int typeR(char **arrLine, wordList *wordfunc, int lookUpLocation)
{
    char *arguments;
    int arrayNumbers[3] = {-1, -1, -1},functionResualt=0;
    instructions *instructPointer;
    instructPointer = (instructions *)malloc(sizeof(instructions));

    arguments = stringArrToString(arrLine + lookUpLocation + 1);

    if (!arguments)
        return 1;

    functionResualt=handleArguments((*wordfunc).opcode + 1, arrayNumbers, arguments);

    if (functionResualt)
        return 1;
    if ((arrayNumbers[0] != -1 && arrayNumbers[1] != -1) && (((*wordfunc).opcode == 1 && arrayNumbers[2] == -1) || ((*wordfunc).opcode == 0 && arrayNumbers[2] != -1)))
    {
        if ((*wordfunc).opcode == 1)
        {
            if (arrayNumbers[2] != -1)
            {
                printf("\nTo many numbers\n");
                return 1;
            }
            arrayNumbers[2] = arrayNumbers[1];
            arrayNumbers[1] = 0;
        }
        instructPointer->instructR.opcode = reverseBits((*wordfunc).opcode, 6);
        instructPointer->instructR.rs = reverseBits(arrayNumbers[0], 5);
        instructPointer->instructR.rt = reverseBits(arrayNumbers[1], 5);
        instructPointer->instructR.rd = reverseBits(arrayNumbers[2], 5);
        instructPointer->instructR.funct = reverseBits((*wordfunc).funct, 5);
        instructPointer->instructR.null = 0;
        addToMemoryTable(stringArrToString(arrLine), instructPointer->codeblocks, 4, 0);
        return 0;
    }
    else
    {
        printf("error missing arguments\n");
        return 1;
    }
    return 0;
}
/*this function hundle only typeI cases*/
int typeI(char **arrLine, wordList *wordfunc, int lookUpLocation)
{
    char *arguments, mode = 3;
    int arrayNumbers[3] = {-1, -1, -1}, functionResult = 0;
    instructions *instructPointer;
    instructPointer = (instructions *)malloc(sizeof(instructions));

    arguments = stringArrToString(arrLine + lookUpLocation + 1);

    if (!arguments)
        return 1;
    if ((*wordfunc).opcode >= 15 && (*wordfunc).opcode <= 18)
        mode++;
    functionResult = handleArguments(mode, arrayNumbers, arguments);
    if (functionResult)
    {
        if (functionResult == 2)
            addToSecondLoopList(stringArrToString(arrLine));
            return functionResult;
    }

    if (arrayNumbers[0] != -1 && arrayNumbers[1] != -1 && arrayNumbers[2] != -1)
    {
        if (mode == 4)
        {
            int temp = arrayNumbers[2];
            arrayNumbers[2] = arrayNumbers[1];
            if (temp)
                arrayNumbers[1] = temp - currentAddressCount(0, 0);
            else
                arrayNumbers[1] = temp;
        }
        instructPointer->instructI.opcode = reverseBits((*wordfunc).opcode, 6);
        instructPointer->instructI.rs = reverseBits(arrayNumbers[0], 5);
        instructPointer->instructI.rt = reverseBits(arrayNumbers[2], 5);
        instructPointer->instructI.immed = reverseBits(arrayNumbers[1], 16);
        addToMemoryTable(stringArrToString(arrLine), instructPointer->codeblocks, 4, 0);
    }
    return 0;
}
/*this function hundle only typeJ cases*/
int typeJ(char **arrLine, wordList *wordfunc, int lookUpLocation)
{
    int address = 0, reg = 0, location = 0;
    instructions *instructPointer;
    instructPointer = (instructions *)malloc(sizeof(instructions));

    /*    check what is this*/
    if (arrLine[lookUpLocation + 1] != NULL)
    {
        while (arrLine[lookUpLocation + 1][location] == ' ')
            location++;
        if (*arrLine[lookUpLocation + 1] == '$')
        {
            reg = 1;
            address = atoi(arrLine[lookUpLocation + 1] + 1);
        }
        else
            address = searchSymbolAddress(arrLine[lookUpLocation + 1], setSecondLoopFlag(0));
    }

    if (address == -1)
    {
        addToSecondLoopList(stringArrToString(arrLine));
        if(!setSecondLoopFlag(0))
            return 2;
        return 1;
    }
    instructPointer->instructJ.opcode = reverseBits((*wordfunc).opcode, 6);
    instructPointer->instructJ.reg = reg;
    instructPointer->instructJ.address = reverseBits(address, 25);
    addToMemoryTable(stringArrToString(arrLine), instructPointer->codeblocks, 4, 0);
    return 0;
}
/*same as math.pow but more reliable*/
long power(int base, int n)
{
    int i;
    long p;

    p = 1;
    for (i = 1; i <= n; ++i)
        p *= base;
    return p;
}
/*this function hundle all guide & data cases*/
int guideWord(char **arrLine, wordList *wordfunc, int lookUpLocation)
{
    wordList *pointerToWordFunction;
    char **arrSplitByColon, *arguments, *pointer;
    int arrayNumbers[255] = {0}, functionResult, i;

    switch ((*wordfunc).opcode)
    {
    case -1:
        arguments = stringArrToString(arrLine + lookUpLocation + 1);
        functionResult = handleArguments(5, arrayNumbers, arguments);
        if (functionResult)
        {
            printf("ERROR GUIDEWORD\n");
            if(functionResult==2 && setSecondLoopFlag(0))
                return 2;
            return 1;
        }

        for (i = 0; arrayNumbers[i] != 0; i++)
        {
            if (arrayNumbers[i] < power(2, (*wordfunc).funct * 8 - 1) - 1 && arrayNumbers[i] > power(2, (*wordfunc).funct * 8 - 1) * -1)
            {
                if (i)
                    addToMemoryTable("", reverseBits(arrayNumbers[i], (*wordfunc).funct * 8) << (32 - (*wordfunc).funct * 8), (*wordfunc).funct, 1);
                else
                    addToMemoryTable(stringArrToString(arrLine), reverseBits(arrayNumbers[i], (*wordfunc).funct * 8) << (32 - (*wordfunc).funct * 8), (*wordfunc).funct, 1);
            }
            else
            {
                printf("Numbers Not in range\n");
                return 1;
            }
        }

        break;
    case -2:
        pointer = arrLine[lookUpLocation + 1];

        while (*pointer == ' ')
            pointer++;
        if (*pointer == '"')
        {
            pointer++;
            addToMemoryTable(stringArrToString(arrLine), reverseBits(*pointer, (*wordfunc).funct * 8) << 24, (*wordfunc).funct, 1);
            pointer++;
            while (*pointer != '"')
            {
                addToMemoryTable("", reverseBits(*pointer, (*wordfunc).funct * 8) << 24, (*wordfunc).funct, 1);
                pointer++;
            }
            addToMemoryTable("", 0, (*wordfunc).funct, 1);
            if (strlen(pointer) > 2)
            {
                printf("Input Error -The Current Line Must End After commas(%s)\n", pointer);
                return 1;
            }
        }
        break;
    case -3:
        pointer = arrLine[lookUpLocation + 1];

        while (*pointer == ' ')
            pointer++;

        if (addSymbol(stringCopy(pointer), 3))
        {
            printf("Error -3\n");
            return 1;
        }

        break;
    case -4:
        pointer = arrLine[lookUpLocation + 1];

        while (*pointer == ' ')
            pointer++;

        if (addSymbol(stringCopy(pointer), 6))
        {
            printf("Error -4\n");
            return 1;
        }
        break;
    case -5:
        arrSplitByColon = costumStrTokSplit(arrLine[lookUpLocation], ':');
        pointerToWordFunction = fullHandleLine(arrLine, lookUpLocation + 1);
        if (arrSplitByColon[1] == NULL)
        {
            if (pointerToWordFunction->opcode > -1)
                addSymbol(stringCopy(arrSplitByColon[0]), 1);
            else
                addSymbol(stringCopy(arrSplitByColon[0]), 2);
        }
        /*Check For More Functions */

        if (pointerToWordFunction->funct != -99)
            return (pointerToWordFunction->function)(arrLine, pointerToWordFunction, lookUpLocation + 1);
        break;
    }
    return 0;
}

/*reset every static and free any calloc/malloc/realloc*/
void resetAllStaticAndFreeMemory()
{
    currentAddressCount(-currentAddressCount(0, 1), 1);
    currentAddressCount(-currentAddressCount(0, 0), 0);
    currentAddressCount(100, 0);
    resetMamoryMap();
    resetSymbolsTable();
    resetSecondLoop();
    if (setSecondLoopFlag(0))
        setSecondLoopFlag(1);
}
