// parsHT.h
// 16. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#ifndef parsHT_
#define parsHT_

// One item fo table
typedef struct parsHT_Item {
	char *id; // ID and key
	struct parsHT_Item *next; // Next synonyms pointer
	int n; // length of types array
	char []types; // variable: V[IDS]0, function F[VIDS][IDS]*0
} parsHT_Item;

#define PARSHT_SIZE 503 // Hash table size
typedef parsHT_Item *parsHT_Table[PARSHT_SIZE]; // Hash htable

// Initialisation of empty values in hash table
// htptr - must be alredy allocated
void parsHT_Init(parsHT_Table *htptr);

// Search item with the same id (key)
// htptr - pointer to table
// id - id to be search for
// returns pointer to the found item or NULL
patsHT_Item *parsHT_Search(parsHT_Table *htptr, char *id);

// Insert item to table. Call the search
// htptr - pointer to table
// id - id of inserter item
// types - data of inserter item
// returns pointer to inserted item of NULL in case of malloc error
parsHT_Item *parsHT_Insert(parsHT_Table *htptr, char *id, char *types);

// Remove all the items and free memory allocated for items
parsHT_Dispose(parsHT_Table *htptr);

#endif // parsHT_
