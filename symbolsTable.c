#include "set.h"

/*declare functions*/

/*assistFunctions*/
int currentAddressCount(int value, int dataFlag);

typedef struct symbolNode
{
    char *symbol;
    int value;
    int attribute;
    struct symbolNode *next;
} symbolNode;

static symbolNode *symNode;

typedef struct externObj
{
    symbolNode *mapObj;
    int address;
    struct externObj *next;
} externObj;

static externObj *externArray;

/*attribute values meaning*/
/* code = 1
 * data = 2
 * entry = 3
 * entry & code = 4
 * entry & data = 5
 * external = 6
 */

int addSymbol(char *symbolstr, int attribute)
{
    symbolNode *newNode = symNode;
    int address = currentAddressCount(0, (attribute % 3) - 1);
    if (attribute % 3 == 0)
        address = 0;

    if (symNode != NULL)
    {
        while (newNode->next != NULL && strcmp(newNode->symbol, symbolstr) != 0)
            newNode = newNode->next;
        if (strcmp(newNode->symbol, symbolstr) == 0)
        {
            if (newNode->attribute <= 3 && attribute + newNode->attribute % 3 != 0)
            {
                newNode->attribute += attribute;
                newNode->value += address;
                return 0;
            }
            else
            {
                printf("\n ERROR: the Symbol Already exists And can't be modify with the current attribute\n");
                return 1;
            }
        }
        newNode->next = (symbolNode *)malloc(sizeof(symbolNode));

        newNode->next->symbol = (symbolstr);
        newNode->next->value = address;
        newNode->next->attribute = attribute;
        newNode->next->next = NULL;
        return 0;
    }
    else
    {

        newNode = (symbolNode *)malloc(sizeof(symbolNode));
        symNode = newNode;

        newNode->symbol = symbolstr;
        newNode->value = address;
        newNode->attribute = attribute;
        newNode->next = NULL;
        return 0;
    }
    return 1;
}
/*add to table for externs attribute*/
int addToExternArray(symbolNode *mapObj)
{

    if (externArray == NULL)
    {
        externArray = (externObj *)malloc(sizeof(externObj));
        externArray->next = NULL;
    }
    else
    {
        externObj *temp;
        temp = (externObj *)malloc(sizeof(externObj));
        temp->next = externArray;
        externArray = temp;
    }
    externArray->mapObj = mapObj;
    externArray->address = currentAddressCount(0, 0);
    return 0;
}
/*search and return symbols address if none found return -1*/
int searchSymbolAddress(char *nameOfSymbol, int searchFlag)
{
    symbolNode *pointer = symNode;

    while (pointer)
    {
        if (strcmp(nameOfSymbol, pointer->symbol) == 0 && ((searchFlag) || (!searchFlag && pointer->attribute % 4 == 1)))
        {
            if (pointer->attribute == 6)
                addToExternArray(pointer);
            return pointer->value;
        }
        pointer = pointer->next;
    }
    return -1;
}
/*change all data values in table after first loop*/
int changeAllDataAttribute()
{
    symbolNode *pointer = symNode;
    int address = currentAddressCount(0, 0);
    while (pointer)
    {
        if (pointer->attribute % 3 == 2)
            pointer->value += address;
        pointer = pointer->next;
    }
    return 0;
}

/*second Loop static manage*/
int setSecondLoopFlag(int flag)
{
    static int secondLoopFlag;
    if (flag)
        secondLoopFlag = !secondLoopFlag;
    return secondLoopFlag;
}

/* void printAllSymbols()
 * {
 *     symbolNode *pointer = symNode;
 *     printf("\nSTART PRINTING ALL SYMBOLS FROM SYMBOLSTABLE\n");
 *     while (pointer)
 *     {
 *         printf("symbol->%s Address->%d attribute->%d\n", pointer->symbol, pointer->value, pointer->attribute);
 *         pointer = pointer->next;
 *     }
 * }
 */

/* main function for print into ext file*/
int createSymbolsFile(char *nameOfFile)
{
    symbolNode *pointer = symNode;
    externObj *externPointer = externArray;
    FILE *externFile = NULL, *entryFile = NULL;
    char *pointerForDot = strchr(nameOfFile, '.');
    while (pointer)
    {
        if (pointer->attribute >= 3 && pointer->attribute < 6)
        {

            /*ENTRY*/
            if (entryFile == NULL)
            {
                strcpy(pointerForDot, ".ent");
                entryFile = fopen(nameOfFile, "w+");
            }
            fprintf(entryFile, "%s %.4d\n", pointer->symbol, pointer->value);
        }
        pointer = pointer->next;
    }

    while (externPointer)
    {

        if (externFile == NULL)
        {
            strcpy(pointerForDot, ".ext");
            externFile = fopen(nameOfFile, "w+");
        }
        fprintf(externFile, "%s %.4d\n", externPointer->mapObj->symbol, externPointer->address);
        externPointer = externPointer->next;
    }
    fclose(externFile);
    fclose(entryFile);

    strcpy(pointerForDot, ".pas");
    return 0;
}

/*reset and free the table after secondLoop over*/
void resetSymbolsTable()
{
    symbolNode *temp;
    externObj *temp2;
    while (symNode != NULL)
    {
        temp = symNode;
        symNode = symNode->next;
        free(temp);
    }

    while (externArray != NULL)
    {
        temp2 = externArray;
        externArray = externArray->next;
        free(temp2);
    }
    externArray = NULL;
    symNode = NULL;
}
