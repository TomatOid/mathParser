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
            if (lastType == VAL)
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
    //from[*fromindex] = '\0';
    int len = *fromindex;

    // go to either the first character or the previous null
    while (fromindex > 0 && from[(*fromindex)--]);
    // make it so that no matter what, we are pointing at the first char of the last element
    len -= *fromindex;
    //if (fromindex > 0) fromindex++;
    printf("%d\n", len);
    memcpy(to + *toindex, from + *fromindex, len);
    *toindex += len;
    memset(from + *fromindex, 0, len);
    return 1;
}

int pop(char* from, int* fromindex)
{
    //from[*fromindex] = '\0';
    int len = *fromindex;

    // go to either the first character or the previous null
    while (fromindex > 0 && from[*fromindex--]);
    // make it so that no matter what, we are pointing at the first char of the last element
    len -= *fromindex;
    //if (fromindex > 0) fromindex++;
    memset(from + *fromindex, 0, len);
    return 1;
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

    char opstack[STACK_SIZE] = { 0 };
    int opindex = 0;
    int opindex_last = 0;
    char tokstack[STACK_SIZE] = { 0 };
    int tokindex = 0;
    int tokindex_last = 0;
    char polish[STACK_SIZE] = { 0 };
    int polindex = 0;
    int polindex_last = 0;

    tokindex = toTokens(buf, tokstack);
    tokindex_last = tokindex;
    while (tokindex >= 0)
    {
        char c = tokstack[tokindex - 1];
        printf("c: %c\n", c);
        int tlen = strnlen(&tokstack[tokindex - 1], STACK_SIZE - tokindex) + 1;
        // if the current token is a number
        if (c == '.' || (c >= '0' && c <= '9'))
        {
            popTo(tokstack, &tokindex, polish, &polindex);
            continue;
        }
        // if the current token is an operator
        if (ops[c].op == c)
        {
            char topPrec = ops[opstack[opindex - 1]].prec;
            while (opindex >= 0 && opstack[opindex - 1] != '(' && (topPrec > ops[c].prec) || (topPrec == ops[c].prec && ops[c].assoc == LEFT))
            {
                popTo(opstack, &opindex, polish, &polindex);
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
            while (opindex >= 0 && opstack[opindex - 1] != '(')
            {
                popTo(opstack, &opindex, polish, &polindex);
            }
            if (opindex >= 0 && opstack[opindex - 1] == ')')
            {
                pop(opstack, &opindex);
            }
            continue;
        }
    }
    while (opindex >= 0)
    {
        popTo(opstack, &opindex, polish, &polindex);
    }
    polindex = 0;
    do
    {
        printf("%s ", polish + polindex);
        polindex += strlen(polish + polindex) + 1;
    } while(polish[polindex]);
    exit(0);
}
