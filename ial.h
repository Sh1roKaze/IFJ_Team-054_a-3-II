// ial.h
// 10. 11. - 21. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#ifndef IAL_
#define IAL_

// Najde řetězec search uvnitř řetězce s a vrátí jeho pozici.
// V řípadě neúspěchu vrací -1.
int IFJ16_find(char *s, char *search);

// Seřadí znaky z řetězce in do řetězce out.
// Řetězec out musí být alokován na stejnou velikost, jakou má řetězec in.
// Vrací 0 v případě úspěchu a nenulové číslo v případě chyby.
int IFJ16_sort(char *in, char *out);

// -------------------------Hash table--------------------------------

// One item of table
typedef struct IAL_htItem {
	char *id; // ID and key
	struct IAL_htItem *next; // Next synonyms pointer
	void *valptr; // Pointer to value of variable
	int n; // length of types array
	char types[];
	// types[0] = V/F(variable/function)
	// types[1] = V/I/D/S(void/int/double/string) type of variable or return type of function
	// types[2..n-1] = I/D/S(int/double/string) parameters' types of function
} IAL_htItem;

// Hash table size
#define IAL_HTSIZE 503

// Hash htable
typedef IAL_htItem *IAL_HashTable[IAL_HTSIZE];

// Initialisation of empty values in hash table
// htptr - must be alredy allocated
void IAL_htInit(IAL_HashTable *htptr);

// Search item with the same id and the same types
// htptr - pointer to table
// id - id to be search for
// types - specific types combination ("types" value of item)
// returns pointer to the found item or NULL
IAL_htItem *IAL_htSearch(IAL_HashTable *htptr, char *id);

// Insert item to table. Call the search
// htptr - pointer to table
// id - id of inserter item
// types - data of inserter item
// returns pointer to inserted item of NULL in case of malloc error
IAL_htItem *IAL_htInsert(IAL_HashTable *htptr, char *id, char *types);

// Remove all the items and free memory allocated for items
void IAL_htDispose(IAL_HashTable *htptr);
#endif // IAL_
