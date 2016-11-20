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
	void *valptr; // Pointer to value of variable
	int n; // length of types array
	char []types;
	// types[0] = V/F(variable/function)
	// types[1] = V/I/D/S(void/int/double/string) type of variable or return type of function
	// types[2..n-1] = I/D/S(int/double/string) parameters' types of function
} parsHT_Item;

#define PARSHT_SIZE 503 // Hash table size
typedef parsHT_Item *parsHT_Table[PARSHT_SIZE]; // Hash htable

// Initialisation of empty values in hash table
// htptr - must be alredy allocated
void parsHT_Init(parsHT_Table *htptr);

// Search item with the same id and the same types
// htptr - pointer to table
// id - id to be search for
// types - specific types combination ("types" value of item)
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
