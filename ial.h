/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/
// ial.h
// 10. 11. - 21. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL


#ifndef IAL_
#define IAL_

// Find first occur. of the search string in s string
// s - string to be searched in
// search - string to be searched for
// return index of the firs foun search string
//        0 when search string is empty
//        -1 when no search string is found in s
//        -99 when malloc error
int IFJ16_find(char *s, char *search);

// Sort characters from in to out by their char value using Shell Sort method
// in - input string
// out - output string - must be allocated with size at least strlen(in) + 1
// Returns 0 in case of success
//        -1 in case of NULL param
int IFJ16_sort(char *in, char *out);

// -------------------------Hash table--------------------------------

// One item of table
typedef struct IAL_htItem {
	char *id; // ID and key
	struct IAL_htItem *next; // Next synonyms pointer
	void *valptr; // Pointer to value of variable
	int n; // length of types array
	int index; // Index in class
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
// returns 0 in case of success
//         1 in case of NULL param
//         2 in case of malloc error
//         3 when item with the same ID alredy exists
int IAL_htInsert(IAL_HashTable *htptr, char *id, int index, char *types);

// Remove all the items and free memory allocated for items
void IAL_htDispose(IAL_HashTable *htptr);
#endif // IAL_
