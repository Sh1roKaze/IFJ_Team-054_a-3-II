// vestavene.c
// 15. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL

#include <stdio.h>
#include <string.h>

// Přečte celé číslo ze vstupu a to uloží na místo, kam ukazuje parametr.
// Vrací nulu v případě neúspěchu a nenulové celé číslo při neúspěchu.
int IFJ16_readInt(int *iptr)
{
	// TODO
	return 0;
}

// Přečte desetinné číslo ze standardního vstupu podobně, jako u celého čísla.
int IFJ16_readDouble(double *dptr)
{
	// TODO
	return 0;
}

// Přečte řetězec ze vstupu a alokuje pro něj novou paměť - nutno nakonec uvolnit!!!
int IFJ16_readString(char **sptr)
{
	// TODO
	return 0;
}

// Vypíše celé číslo na standardní výstup
void IFJ16_printInt(int i)
{
	printf(%i, i);
}

// Vypíše desetinné číslo na standardní výstup
void IFJ16_printDouble(double d)
{
	printf(%g, d);
}

// Vypíše řetězec na standardní výstup
void IFJ16_printString(char *s)
{
	printf(%s, s);
}

// Vrátí délku řetězce, v případě chyby vrací záporné číslo
int IFJ16_length(char *s)
{
	return strlen(s);
}

// Uloží podřetězec řetězce in do řetězce out -  musí být alokovaný na požadovanou velikost (n+1)!!!
// Vrací nulu v případě úspěchu, jinak vrací nenulové číslo.
int IFJ16_substr(char *in, int i, int n, char *out)
{
	if (in == NULL || out == NULL)
		return 1;

	strcpy(out, in + i, n);
	out[n] = "\0";
	return 0;
}

// Porovná dva řetězce a vrátí nulu v případě rovnosti.
// Pokud je první řetězec větší, vrací 1, pokud je první menší, vrací -1
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
