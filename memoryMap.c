#include "set.h"

/*declare functions*/

/*assistFunctions*/
int currentAddressCount(int value, int dataFlag);
unsigned int reverseBits(unsigned int num, unsigned int NO_OF_BITS);

typedef struct memoryMapStruct
{
    int address;
    char *sourceCode;
    unsigned machineCode : 32;
    int bitsSize;
    struct memoryMapStruct *next;
} memoryMapStruct;

static memoryMapStruct *memoryObj;
static memoryMapStruct *dataObj;
/*there are 2 pointers of type memoryMapStruct memoryObj point to all the memory rules in memory map
 *and dataObj points to all data rules in memory map Which is at the end of memoryObj */

/*addToMemoryTable Manages all options for inserting into the memory map table
 *And maintains the legitimacy of the pointers*/
int addToMemoryTable(char *code, unsigned machineCode, int size, int dataFlag)
{
    memoryMapStruct *newOne, *pointer = memoryObj;

    if (memoryObj == NULL && !dataFlag)
    {
        memoryObj = (memoryMapStruct *)malloc(sizeof(memoryMapStruct));
        newOne = memoryObj;
        if (dataObj == NULL)
            newOne->next = NULL;
        else
            newOne->next = dataObj;
    }
    else if (dataObj == NULL && dataFlag)
    {
        if (memoryObj == NULL)
        {
            dataObj = (memoryMapStruct *)malloc(sizeof(memoryMapStruct));
            newOne = dataObj;
            newOne->next = NULL;
        }
        else
        {
            newOne = memoryObj;
            while (newOne->next)
                newOne = newOne->next;
            dataObj = (memoryMapStruct *)malloc(sizeof(memoryMapStruct));
            newOne->next = dataObj;
            newOne = newOne->next;
            newOne->next = NULL;
        }
    }
    else
    {
        memoryMapStruct *temp;
        int currentAddress = currentAddressCount(0, dataFlag);
        if (dataFlag)
            pointer = dataObj;

        while (pointer->next != NULL && pointer->next != dataObj && ((temp = pointer->next)->address) < currentAddress)
            pointer = pointer->next;
        if (pointer->next != NULL)
        {

            temp = pointer->next;
            newOne = (memoryMapStruct *)malloc(sizeof(memoryMapStruct));
            pointer->next = newOne;
            newOne->next = temp;
        }
        else
        {
            pointer->next = (memoryMapStruct *)malloc(sizeof(memoryMapStruct));
            newOne = pointer->next;
            newOne->next = NULL;
        }
    }
    /*create the new memory obj */
    newOne->address = currentAddressCount(size, dataFlag);

    newOne->sourceCode = code;
    newOne->bitsSize = size * 8;
    newOne->machineCode = machineCode;

    return 0;
}

/*Print functions for debugging */
/* void printBin(int x, int size)
 * {
 *     unsigned int mask = 1 << (size - 1);
 *     int i;
 *     char string[size + 1];
 *     for (i = 0; i < size; i++)
 *     {
 *         ((x & mask) == 0) ? strncpy(string + (size - 1 - i), "0", 1) : strncpy(string + (size - 1 - i), "1", 1);
 *         mask >>= 1;
 *     }
 *     string[size] = '\0';
 *     printf("%s", string);
 * }
 * void printMemoryTable(int dataFlag)
 * {
 *     memoryMapStruct *pointer = memoryObj;
 *     if (dataFlag)
 *         pointer = dataObj;
 *     printf("\nMEMORY MAP (%d)", dataFlag);
 *     while (pointer)
 *     {
 *         printf("\nADDRESS->%d sourceCode->%s machineCode->", pointer->address, pointer->sourceCode);
 *         printBin(pointer->machineCode, pointer->bitsSize);
 *
 *         pointer = pointer->next;
 *     }
 *     putchar('\n');
 * }
 */

/*after first Loop change all data rules address*/
int changeAllDataAddresses()
{
    memoryMapStruct *pointer = dataObj;

    int address = currentAddressCount(0, 0);
    while (pointer)
    {
        pointer->address += address;
        pointer = pointer->next;
    }

    return 0;
}

/*binaryToEx prints the machine code into the ob/pas File*/
int binaryToEx(unsigned buildNumber, FILE *memoryFile, int newLineLocation, int addressForNewLine, int maxSize)
{
    int count = 0;
    unsigned int mask = 15;
    unsigned int arr[8];
    buildNumber = reverseBits(buildNumber, 32);
    while (count != maxSize / 4)
    {
        if ((++count) % 2 == 0)
            arr[count - 2] = mask & buildNumber;
        else
            arr[count] = mask & buildNumber;
        buildNumber /= 16;
    }

    for (count = 0; count < maxSize / 8; count++)
    {
        if (newLineLocation == count)
            fprintf(memoryFile, "\n%.4d", addressForNewLine);
        fprintf(memoryFile, " %X%X", arr[count * 2], arr[count * 2 + 1]);
    }
    if (newLineLocation < maxSize / 8 && newLineLocation >= 0)
        return 1;
    return 0;
}
/*main function for print into ob/pas files*/
int createMemoryMapFile(char *nameOfFile)
{

    memoryMapStruct *pointer = memoryObj;
    FILE *memoryFile = NULL;
    char *pointerForDot = strchr(nameOfFile, '.');
    int bitsCounter = 32;
    int addressCounter = 100;
    int newLineValue;

    strcpy(pointerForDot, ".ob");
    memoryFile = fopen(nameOfFile, "w+");
    fprintf(memoryFile, "\t %d %d", dataObj->address - 100, currentAddressCount(0, 1));
    while (pointer != NULL)
    {
        newLineValue = 32 - bitsCounter;
        if (binaryToEx(pointer->machineCode, memoryFile, newLineValue / 8, addressCounter, pointer->bitsSize))
        {
            addressCounter += 4;
            bitsCounter -= 32;
        }

        bitsCounter += pointer->bitsSize;

        pointer = pointer->next;
    }

    strcpy(pointerForDot, ".pas");
    return 0;
}

/*reset and free after the second Loop*/
void resetMamoryMap()
{
    memoryMapStruct *temp;
    while (memoryObj != NULL)
    {
        temp = memoryObj;
        memoryObj = memoryObj->next;
        free(temp);
    }
    memoryObj = NULL;
    dataObj = NULL;
}
