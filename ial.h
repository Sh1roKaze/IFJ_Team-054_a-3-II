// ial.h
// 10. 11. 2016
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

// -------------------------Hashovací tabulka symbolů--------------------------------

// Velikost tabulky
#define IAL_HTSIZE 101

// Klíč k položkám tabulky
typedef char *IAL_htKey;

// Struktura jedné položky tabulky
typedef struct IAL_htItem {
	IAL_htKey key;
	struct IAL_htItem *next;
} IAL_htItem;

// Datový typ tabulky
typedef struct IAL_htItem *IAL_HTable[IAL_HTSIZE];

// Funkce pracující s hashovací tabulkou:

// Inicializace již alokované tabulky - nutno provést před prvním použitím.
void IAL_htInit(IAL_HTable *htptr);

// Vrátí ukazatel na položku s daným klíčem. Pokud položka v tabulce není, vrací NULL.
IAL_htItem *IAL_htSearch(IAL_HTable *htptr, IAL_htKey key);

// Přidá položku do tabulky.
// Vrací ukazatel na vložený prvek nebo NULL v případě chyby
IAL_htItem *IAL_htInsert(IAL_HTable *htptr, IAL_htKey key);

// Odstraní a uvolní všechny prvky v tabulce. Neuvolňuje klíče - ty musí uvolnit, kdo je alokoval!
void IAL_htDispose(IAL_HTable *htptr);


#endif // IAL_
