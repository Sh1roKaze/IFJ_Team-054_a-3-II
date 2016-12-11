/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/
// ifj16.c
// 23. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ifj16.h"

// String list struct and it's methods (for readString)
typedef struct StringListItem
{
	int length;
	struct StringListItem *next;
	char string[101];
} StringListItem;
StringListItem *stringListNew();
int stringListAddChar(StringListItem *first, char c);
char *stringListToString(StringListItem *first);
void stringListDelete(StringListItem **first);


// Read integer from standard input
// iptr - output parameter, input value will be given here
// returns 0 in case of success
//         7 in case of incorrect input
int IFJ16_readInt(int *iptr)
{
	*iptr = 0;
	int res = 0;
	int base = 10;

	char c = getchar();
	if (c == '0')
		base = 8;
	else if (c > '0' && c <= '9')
		res = c - '0';
	else
		goto chybaVstupuInt;

	while ((c = getchar()) != EOF && c != '\n')
		if (c >= '0' && c <= '9')
			res = res * base + (c - '0');
		else
			goto chybaVstupuInt;
	
	*iptr = res;
	return 0;

chybaVstupuInt:
	while ((c = getchar()) != EOF && c != '\n');
	return 7;
}

// Read real number from standard input
// dptr - output parameter, input value will be given here
// returns 0 in case of success
//         7 in case of incorrect input
int IFJ16_readDouble(double *dptr)
{
	*dptr = 0;
	double inp = 0;
	double dec = 0;
	double shift = 1;
	int exp = 0;
	int expSign = 1;
	char c;
	while ((c = getchar()) != EOF && c != '\n')
		if (c >= '0' && c <= '9')
			inp = inp * 10 + (c - '0');
		else if (c == '.')
			goto decimal;
		else if (c == 'e' || c == 'E')
			goto exponent;
		else
			goto chybaVstupuDouble;

	*dptr = inp;
	return 0;

decimal:
	while ((c = getchar()) != EOF && c != '\n')
		if (c >= '0' && c <= '9')
			dec += (c - '0') * (shift /= 10);
		else if (c == 'e' || c == 'E')
			goto exponent;
		else
			goto chybaVstupuDouble;

	*dptr = inp + dec;
	return 0;

exponent:
	inp += dec;
	c = getchar();
	if (c == '-')
	{
		expSign = -1;
		c = getchar();
	}
	else if (c == '+')
		c = getchar();

	if (c >= '0' && c <= '9')
		exp = c - '0';
	else
		goto chybaVstupuDouble;

	while ((c = getchar()) != EOF && c != '\n')
		if (c >= '0' && c <= '9')
			exp = exp * 10 + (c - '0');
		else
			goto chybaVstupuDouble;

	for ( ; exp > 0; exp--)
		if (expSign == 1)
			inp *= 10;
		else
			inp /= 10;

	*dptr = inp;
	return 0;

chybaVstupuDouble:
	while ((c = getchar()) != EOF && c != '\n');
	return 7;
}


// Read string ended by EOF or \n from standard input
// sptr - pointer to string to be read 
// ATTENTION!!! this function use malloc for *sptr don't forget to free the memory !!!
// returns 0 in case of success
//        99 in case of malloc error
int IFJ16_readString(char **sptr)
{
	*sptr = NULL;
	StringListItem *strList = stringListNew();
	if (strList == NULL)
		return 99;

	char c;
	while ((c = getchar()) != '\n' && c != EOF)
		if (stringListAddChar(strList, c) == 99)
		{
			stringListDelete(&strList);
			return 99;
		}

	*sptr = stringListToString(strList);
	if (sptr == NULL)
	{
		stringListDelete(&strList);
		return 99;
	}

	stringListDelete(&strList);
	return 0;
}

// Print integer to standard output
// i - integer to be printed
void IFJ16_printInt(int i)
{
	printf("%i", i);
}

// Print real number to standard output
// d - real number to be prented
void IFJ16_printDouble(double d)
{
	printf("%g", d);
}

// Print string to standard output
// s - string to be printed
void IFJ16_printString(char *s)
{
	printf("%s", s);
}

// Returns length of string or negative number in case of error
int IFJ16_length(char *s)
{
	return strlen(s);
}

// Copy substring of in to out
// in - input string
// i - start index of subrstring
// n - nuber of chars (length of substring)
// out - output string - must be allocated at least for n+1 (ending \0 char)!!!
// Returns 0 in case of succes
//         1 in case of NULL params
//         10 if substring is out of inputs range
int IFJ16_substr(char *in, int i, int n, char *out)
{
	if (in == NULL || out == NULL)
		return 1;
	if (strlen(in) < i + n || i < 0 || n < 0)
	{
		out[0] = '\0';
		return 10;
	}

	strncpy(out, in + i, n);
	out[n] = '\0';
	return 0;
}

// Compare tvo strings (s1, s2)
// Returns 0 if strings are equal
//         1 if s1 > s2
//        -1 if s1 < s2
int IFJ16_compare(char *s1, char *s2)
{
	int cmp = strcmp(s1, s2);

	if (cmp < 0)
		return -1;
	else if (cmp > 0)
		return 1;
	else 
		return 0;
}



// Methods of StringList
StringListItem *stringListNew()
{
	StringListItem *first = malloc(sizeof(StringListItem));
	if (first == NULL)
		return NULL;

	first->length = 0;
	first->next = NULL;
	for (int i = 0; i <= 100; i++)
		first->string[i] = '\0';
	return first;
}

int stringListAddChar(StringListItem *first, char c)
{
	StringListItem *item = first;
	while (item->next != NULL)
		item = item->next;

	if (item->length < 100)
	{
		item->string[item->length] = c;
		item->length++;
		return 0;
	}
	else
	{
		item->next = stringListNew();
		if (item->next == NULL)
			return 99;

		item->next->string[0] = c;
		item->next->length = 1;
		return 0;
	}
}

char *stringListToString(StringListItem *first)
{
	StringListItem *item = first;
	int totalLength = 0;
	
	while (item != NULL)
	{
		totalLength += item->length;
		item = item->next;
	}
	
	char *string = malloc(totalLength + 1);
	if (string == NULL)
		return NULL;

	item = first;
	int listIndex = 0;
	while (item != NULL)
	{
		strcpy(string + 100*listIndex, item->string);
		item = item->next;
		listIndex++;
	}

	return string;
}

void stringListDelete(StringListItem **first)
{
	if ((*first)->next != NULL)
		stringListDelete(&((*first)->next));

	free(*first);
}
