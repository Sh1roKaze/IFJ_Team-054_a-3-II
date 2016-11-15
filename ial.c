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

// Funkce pracující s hashovací tabulkou:

// Vygenerování hashkódu podle klíče
int IAL_hashCode(IAL_htKey key)
{
	int hash = 0;
	int l = strlen(key);

	for (int i = 0; i < l; i++)
		hash += key[i];

	return hash % IAL_HTSIZE;
}

// Inicializace již alokované tabulky - nutno provést před prvním použitím.
void IAL_htInit(IAL_HTable *htptr)
{
	if (htptr == NULL)
		return;	

	for (int i = 0; i < IAL_HTSIZE; i++)
		(*htptr)[i] = NULL;
}

// Vrátí ukazatel na položku s daným klíčem. Pokud položka v tabulce není, vrací NULL.
IAL_htItem *IAL_htSearch(IAL_HTable *htptr, IAL_htKey key)
{
	if (htptr == NULL || key == NULL)
		return NULL;

	IAL_htItem *itemptr = (*htptr)[IAL_hashCode(key)];

	while (itemptr != NULL)
		if (strcmp(key, itemptr->key))
			itemptr = itemptr->next;
		else break;

	return itemptr;
}

// Přidá položku do tabulky.
// Vrací ukazatel na vložený prvek nebo NULL v případě chyby
IAL_htItem *IAL_htInsert(IAL_HTable *htptr, IAL_htKey key)
{
	if (htptr == NULL || key == NULL)
		return NULL;

	int hash = IAL_hashCode(key);
	IAL_htItem *itemptr = malloc(sizeof(IAL_htItem));
	if (itemptr == NULL)
		return NULL;
	itemptr->key = key;
	itemptr->next = (*htptr)[hash];
	(*htptr)[hash] = itemptr;
	return itemptr;
}

// Odstraní a uvolní všechny prvky v tabulce. Neuvolňuje klíče - ty musí uvolnit, kdo je alokoval!
void IAL_htDispose(IAL_HTable *htptr)
{
	if (htptr == NULL)
		return;

	IAL_htItem *itemptr;
	for (int i = 0; i < IAL_HTSIZE; i++)
		while (itemptr != NULL)
		{
			itemptr = (*htptr)[i];
			(*htptr)[i] = (*htptr)[i]->next;
			free(itemptr);
		}
}
