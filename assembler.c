#include "set.h"

int firstTransition(char *file);
int startSecondLoop();
int createSymbolsFile(char *nameOfFile);
int createMemoryMapFile(char *nameOfFile);
void resetAllStaticAndFreeMemory();
char *stringCopy(char *stringToCopyFrom);

/* void printAllSymbols();
 * void printMemoryTable(int dataFlag);
 * void printAllSecondLoop();
 */

/*this is the main program*/
int main(int argc, char *argv[])
{
    int i, flag = 0;
    for (i = 1; i < argc; i++)
    {
        flag = firstTransition(argv[i]);
        if (!flag)
            flag += startSecondLoop();
        if (!flag)
        {
            char *copiedFileName;
            copiedFileName = stringCopy(argv[i]);
            createSymbolsFile(copiedFileName);
            createMemoryMapFile(copiedFileName);
            resetAllStaticAndFreeMemory();
            free(copiedFileName);
            printf("DONE %s\n", argv[i]);
        }
        else
            printf("finished with errors %s\n", argv[i]);
        /* 3 useful functions for debugging : (use before the resetAllStaticAndFreeMemory function)
             * printAllSymbols();
             * printMemoryTable(0);
             * printAllSecondLoop(); */
    }
    return 0;
}
