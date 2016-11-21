// ial.c
// 15. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#include "ial.h"
#include <string.h>
#include <stdlib.h>

// Najde řetězec search uvnitř řetězce s a vrátí jeho pozici.
// V řípadě neúspěchu vrací -1.
int IFJ16_find(char *s, char *search)
{
	if (s == NULL || search == NULL)
		return -1;

	// TODO

	return -1; // TODO
}

// Seřadí znaky z řetězce in do řetězce out.
// Řetězec out musí být alokován na stejnou velikost, jakou má řetězec in.
// Vrací 0 v případě úspěchu a nenulové číslo v případě chyby.
int IFJ16_sort(char *in, char *out)
{
	if (in == NULL || out == NULL)
		return -1; // TODO

	int l = strlen(in);

	for (int i = 0; i <= l; i++)
		out[i] = in[i]; // TODO

	// TODO

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
// returns pointer to inserted item of NULL in case of malloc error
IAL_htItem *IAL_htInsert(IAL_HashTable *htptr, char *id, char *types)
{
	if (htptr == NULL || id == NULL || types == NULL)
		return NULL;

	int hash = IAL_hashCode(id);
	int idLength = strlen(id);
	int typesNum = strlen(types);
	IAL_htItem *itemptr = malloc(
		sizeof(char *) + //char *id
		sizeof(IAL_htItem *) + //IAL_htItem *next
		sizeof(void *) + //void *valptr
		sizeof(int) + //int n
		typesNum + 1 + //char types[]
		idLength + 1 //ID
		);
	if (itemptr == NULL)
		return NULL;

	// Initialise item's values
	itemptr->id = itemptr->types + typesNum + 1;
	strcpy(itemptr->id, id);
	itemptr->next = (*htptr)[hash];
	itemptr->valptr = NULL;
	itemptr->n = typesNum;
	strcpy(itemptr->types, types);

	// Add item to list
	(*htptr)[hash] = itemptr;
	return itemptr;
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
