#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define STACK_SIZE 512

// Functions are right associative operators with high

typedef struct
{
    char op;
    char prec;
    char assoc; // 1 left 0 right
} operator;

// Type
enum
{
    NIL,
    OP,
    VAL
};

// Associativity
enum
{
    RIGHT,
    LEFT
};

operator ops[256] = { 0 };

//
//typedef struct
//{
//    char type;
//    union 
//    {
//        double val;
//        double* var;
//    };
//} value;
//
//typedef struct
//{
//    char type;
//    union
//    {
//        operator op;
//        value val;
//    };
//} token;
//

int toTokens(char* str, char* tokstack)
{
    if (!str) { return -1; }
    int lastType = NIL;
    int tindex = 0;
    do
    {
        if (*str == '.' || (*str >= '0' && *str <= '9')) // if str is a number
        {
            if (lastType == OP) // we should push a NULL
            {
                tokstack[tindex] = '\0';
                tindex++;
            }
            tokstack[tindex] = *str;
            tindex++;
            lastType = VAL;
        }
        else if (*str == '^' || (*str >= '(' && *str <= '/'))
        {
            if (*str == '-' && lastType != VAL) 
            {
                tokstack[tindex] = *str;
                tindex++;
                lastType = VAL;
                continue;
            }
            if (lastType == VAL || 1)
            {
                tokstack[tindex] = '\0';
                tindex++;
            }
            tokstack[tindex] = *str;
            tindex++;
            lastType = OP;
        }
    } while(*(str++) && (tindex < STACK_SIZE - 1));
    return tindex;
}

void addop(char op, int precedence, int associativity)
{
    operator tmp = { .op = op, .prec = (char)precedence, .assoc = associativity };
    ops[(int)tmp.op] = tmp;
}

int popTo(char* from, int* fromindex, char* to, int* toindex)
{
    if ((*toindex) < STACK_SIZE - 1) { (*toindex)++; }
    while ((*toindex) < STACK_SIZE - 1 && to[(*toindex) - 1]) { (*toindex)++; }
    int len = strnlen(from + *fromindex, STACK_SIZE - *fromindex - 1) + 1;
    memcpy(to + *toindex, from + *fromindex, len);
    memset(from + *fromindex, 0, len);
    if ((*fromindex) > 0) { (*fromindex)--; }
    else *fromindex = -1;
    while (*(fromindex) > 0 && from[(*fromindex) - 1]) { (*fromindex)--; }
}

int tokrev(char* tokstack, int tokindex)
{
    char tmp[STACK_SIZE] = { 0 };
    int newindex = 0;
    while (tokindex >= 0)
    {
        popTo(tokstack, &tokindex, tmp, &newindex);
    }
    memcpy(tokstack, tmp, STACK_SIZE);
    return newindex;
}

int pop(char* from, int* fromindex)
{
    int len = strnlen(from + *fromindex, STACK_SIZE - *fromindex - 1) + 1;
    memset(from + *fromindex, 0, len);
    if ((*fromindex) > 0) { (*fromindex)--; }
    else *fromindex = -1;
    while (*(fromindex) > 0 && from[(*fromindex) - 1]) { (*fromindex)--; }
}

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}



int main(int argc, char* argv[])
{
    char buf[STACK_SIZE / 2];
    fgets(buf, STACK_SIZE / 2, stdin);
    
    //if (argc < 1) { return -1; }
    int len = strlen(buf);

    addop('+', 2, LEFT);
    addop('-', 2, LEFT);
    addop('/', 3, LEFT);
    addop('*', 3, LEFT);
    addop('^', 4, RIGHT);
    //ops[0].prec = 100;

    char opstack[STACK_SIZE] = { 0 };
    int opindex = 0;
    int opindex_last = 0;
    char tokstack[STACK_SIZE] = { 0 };
    int tokindex = 0;
    int tokindex_last = 0;
    char polish[STACK_SIZE] = { 0 };
    int polindex = 0;
    int polindex_last = 0;

    tokindex = toTokens(buf, tokstack) - 1;
    while (tokindex > 0 && tokstack[tokindex - 1]) { tokindex--; }
    tokindex = tokrev(tokstack, tokindex);
    while (tokindex >= 0)
    {
        char c = tokstack[tokindex];
        printf("c: %c\n", c);
        int tlen = strnlen(&tokstack[tokindex], STACK_SIZE - tokindex) + 1;
        // if the current token is a number
        if (c == '.' || (c >= '0' && c <= '9'))
        {
            popTo(tokstack, &tokindex, polish, &polindex);
            continue;
        }
        // if the current token is an operator
        if (ops[c].op == c)
        {
            char topPrec = ops[opstack[opindex]].prec;
            printf("topPrec: %d\n", (int)topPrec);
            while ((opindex > 0) && (opstack[opindex] != '(') && (topPrec > ops[c].prec) || (topPrec == ops[c].prec && ops[c].assoc == LEFT))
            {
                popTo(opstack, &opindex, polish, &polindex);
                topPrec = ops[opstack[opindex]].prec;
            }
            popTo(tokstack, &tokindex, opstack, &opindex);
            continue;
        }
        if (c == '(')
        {
            popTo(tokstack, &tokindex, opstack, &opindex);
            continue;
        }
        if (c == ')')
        {
            while (opindex > 0 && opstack[opindex] != '(')
            {
                popTo(opstack, &opindex, polish, &polindex);
            }
            if (opstack[opindex] == '(')
            {
                pop(opstack, &opindex);
            }
            pop(tokstack, &tokindex);
            continue;
        }
    }
    while (opindex > 0)
    {
        printf("c: %c\n", opstack[opindex]);
        popTo(opstack, &opindex, polish, &polindex);
    }
    polindex = 0;
    do
    {
        char c = polish[polindex];
        if (c) { printf("%c", c); }
        else { printf("_"); }
    } while(polindex++ < STACK_SIZE - 1);
    exit(0);
}
