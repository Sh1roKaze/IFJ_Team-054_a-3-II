/*
Projekt IFJ16

xvlach16 - Antonin Vlach
*/
// ial.c
// 15. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#include "ial.h"
#include <string.h>
#include <stdlib.h>

// Find first occur. of the search string in s string
// s - string to be searched in
// search - string to be searched for
// return index of the firs foun search string
//        0 when search string is empty
//        -1 when no search string is found in s
//        -99 when malloc error
int IFJ16_find(char *s, char *search)
{
	int l1 = strlen(s);
	int l2 = strlen(search);
	if (l2 == 0)
		return 0;

	int *fail = malloc(l2 * sizeof(int));
	if (fail == NULL)
		return -99;

	fail[0] = -1;
	int r;
	for (int i = 1; i < l2; i++)
	{
		r = fail[i-1];
		while (r >= 0 && search[r] != search[i-1])
			r = fail[r];
		fail[i] = r+1;
	}

	int i1 = 0;
	int i2 = 0;
	while (i1 < l1 && i2 < l2)
		if (i2 == -1 || s[i1] == search[i2])
		{
			i1++;
			i2++;
		}
		else
			i2 = fail[i2];

	free(fail);

	if (i2 == l2)
		return i1-l2;
	else
		return -1;
}

#define SWAP(a,b) do{(a)=(a)^(b);(b)=(a)^(b);(a)=(a)^(b);} while(0)

// Sort characters from in to out by their char value using Shell Sort method
// in - input string
// out - output string - must be allocated with size at least strlen(in) + 1
// Returns 0 in case of success
//        -1 in case of NULL param
int IFJ16_sort(char *in, char *out)
{
	if (in == NULL || out == NULL)
		return -1;

	int l = strlen(in);

	for (int i = 0; i <= l; i++)
		out[i] = in[i];
	out[l] = '\0';

	for (int step = l / 2; step > 0; step /= 2)
		for (int i = step; i < l; i++)
			for (int j = i - step; j >= 0 && out[j] > out[j+step]; j -= step)
				SWAP(out[j],out[j+step]);

	return 0;
}

// ---------------------------------- Hash table -------------------------------

// Vygenerování hashkódu podle klíče
int IAL_hashCode(char *key)
{
	int hash = 0;
	int l = strlen(key);

	for (int i = 0; i < l; i++)
		hash += key[i];

	return hash % IAL_HTSIZE;
}

// Initialisation of empty values in hash table
// htptr - must be alredy allocated
void IAL_htInit(IAL_HashTable *htptr)
{
	if (htptr == NULL)
		return;

	for (int i = 0; i < IAL_HTSIZE; i++)
		(*htptr)[i] = NULL;
}

// Search item with the same id and the same types
// htptr - pointer to table
// id - id to be search for
// types - specific types combination ("types" value of item)
// returns pointer to the found item or NULL
IAL_htItem *IAL_htSearch(IAL_HashTable *htptr, char *id)
{
	if (htptr == NULL || id == NULL)
		return NULL;

	IAL_htItem *itemptr = (*htptr)[IAL_hashCode(id)];

	while (itemptr != NULL)
		if (strcmp(id, itemptr->id))
			itemptr = itemptr->next;
		else break;

	return itemptr;
}

// Insert item to table. Call the search
// htptr - pointer to table
// id - id of inserter item
// types - data of inserter item
// returns 0 in case of success
//         1 in case of NULL param
//         2 in case of malloc error
//         3 when item with the same ID alredy exists
int IAL_htInsert(IAL_HashTable *htptr, char *id, int index, char *types)
{
	if (htptr == NULL || id == NULL || types == NULL)
		return 1;
	if (IAL_htSearch(htptr, id) != NULL)
		return 3;

	int hash = IAL_hashCode(id);
	int idLength = strlen(id);
	int typesNum = strlen(types);
	IAL_htItem *itemptr = malloc(
		sizeof(char *) + //char *id
		sizeof(IAL_htItem *) + //IAL_htItem *next
		sizeof(void *) + //void *valptr
		sizeof(int) + //int n
		sizeof(int) + //int index
		typesNum + 1 + //char types[]
		idLength + 1 //ID
		);
	if (itemptr == NULL)
		return 2;

	// Initialise item's values
	itemptr->id = itemptr->types + typesNum + 1;
	strcpy(itemptr->id, id);
	itemptr->next = (*htptr)[hash];
	itemptr->valptr = NULL;
	itemptr->n = typesNum;
	itemptr->index = index;
	strcpy(itemptr->types, types);

	// Add item to list
	(*htptr)[hash] = itemptr;
	return 0;
}

// Remove all the items and free memory allocated for items
void IAL_htDispose(IAL_HashTable *htptr)
{
	if (htptr == NULL)
		return;

	IAL_htItem *itemptr;
	for (int i = 0; i < IAL_HTSIZE; i++)
		while ((*htptr)[i] != NULL)
		{
			itemptr = (*htptr)[i];
			(*htptr)[i] = (*htptr)[i]->next;
			free(itemptr);
		}
}
