/*
Projekt IFJ16
xverny00 - Jan Verny
xvalac08 - Dominik Valachovic 
xosker02 - Jan Oskera
xvlach16 - Antonin Vlach
xzikmu08 - David Zikmund
*/

// ifj16.h
// 23. 11. 2016
// (c) Antonín Vlach
// V rámci projektu do předmětů IFJ a IAL



#ifndef IFJ16_
#define IFJ16_

// Read integer from standard input
// iptr - output parameter, input value will be given here
// returns 0 in case of success
//         7 in case of incorrect input
int IFJ16_readInt(int *);

// Read real number from standard input
// dptr - output parameter, input value will be given here
// returns 0 in case of success
//         7 in case of incorrect input
int IFJ16_readDouble(double *);

// Read string ended by EOF or \n from standard input
// sptr - pointer to string to be read 
// ATTENTION!!! this function use malloc for *sptr don't forget to free the memory !!!
// returns 0 in case of success
//        99 in case of malloc error
int IFJ16_readString(char **);

// Print integer to standard output
// i - integer to be printed
void IFJ16_printInt(int);

// Print real number to standard output
// d - real number to be prented
void IFJ16_printDouble(double);

// Print string to standard output
// s - string to be printed
void IFJ16_printString(char *);

// Returns length of string or negative number in case of error
int IFJ16_length(char *);

// Copy substring of in to out
// in - input string
// i - start index of subrstring
// n - nuber of chars (length of substring)
// out - output string - must be allocated at least for n+1 (ending \0 char)!!!
// Returns 0 in case of succes
//         1 in case of NULL params
//         10 if substring is out of inputs range
int IFJ16_substr(char *in, int i, int n, char *out);

// Compare tvo strings (s1, s2)
// Returns 0 if strings are equal
//         1 if s1 > s2
//        -1 if s1 < s2
int IFJ16_compare(char *, char *);

#endif//IFJ16_
