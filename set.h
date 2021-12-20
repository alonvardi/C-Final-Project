#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/*Maximum size assumption */
#define LINESZ 1024
/*The maximum relevant place in the array*/
#define wordListSize 32
/*the : place place in the array*/
#define DOTSFUNCTION 33
/*empty line place in the array*/
#define NULLFUNCTION 34
/*error place in the array*/
#define ERRORFUNCTION 35

typedef struct wordList
{
    char *word;
    int (*function)(char **line, struct wordList *wordfunc, int lookUpLocation);
    int opcode;
    int funct;
    char type;
} wordList;

int typeR(char **arrLine, wordList *wordfunc, int lookUpLocation);
int typeI(char **arrLine, wordList *wordfunc, int lookUpLocation);
int typeJ(char **arrLine, wordList *wordfunc, int lookUpLocation);
int guideWord(char **arrLine, wordList *wordfunc, int lookUpLocation);

/*those struct use To simplify the array Insert
 *The idea is to divide the machineCode by 32 bits according to the types (type R,i,j) */
typedef struct
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int rd : 5;
    unsigned int funct : 5;
    unsigned int null : 6;
} instructTypeR;
typedef struct
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int immed : 16;
} instructTypeI;
typedef struct
{
    unsigned int opcode : 6;
    unsigned int reg : 1;
    unsigned int address : 25;
} instructTypeJ;
typedef union
{
    unsigned int codeblocks : 32;
    instructTypeR instructR;
    instructTypeI instructI;
    instructTypeJ instructJ;
} instructions;
