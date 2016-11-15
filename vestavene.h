// vestavene.h
// 14. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL



#ifndef vestavene_
#define vestavene_

// Přečte celé číslo ze vstupu a to uloží na místo, kam ukazuje parametr.
// Vrací nulu v případě neúspěchu a nenulové celé číslo při neúspěchu.
int IFJ16_readInt(int *);

// Přečte desetinné číslo ze standardního vstupu podobně, jako u celého čísla.
int IFJ16_readDouble(double *);

// Přečte řetězec ze vstupu a alokuje pro něj novou paměť - nutno nakonec uvolnit!!!
int IFJ16_readString(char **);

// Vypíše celé číslo na standardní výstup
void IFJ16_printInt(int);

// Vypíše desetinné číslo na standardní výstup
void IFJ16_printDouble(double);

// Vypíše řetězec na standardní výstup
void IFJ16_printString(char *);

// Vrátí délku řetězce, v případě chyby vrací záporné číslo
int IFJ16_length(char *);

// Uloží podřetězec řetězce in do řetězce out -  musí být alokovaný na požadovanou velikost (n)!!!
// Vrací nulu v případě úspěchu, jinak vrací nenulové číslo.
int IFJ16_substr(char *in, int i, int n, char *out);

// Porovná dva řetězce a vrátí nulu v případě rovnosti.
// Pokud je první řetězec větší, vrací 1, pokud je první menší, vrací -1
int IFJ16_compare(char *, char *);

#endif//vestavene_
