# IFJ_Team-054_a-3-II
Zadání:
https://wis.fit.vutbr.cz/FIT/st/course-files-st.php/course/IFJ-IT/projects/ifj2016.pdf

Podpůrné materiály:
http://www.fit.vutbr.cz/study/courses/IFJ/public/project/

Tasks:

 Knuth-Morris-Pratt algorithm: https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
  - used to find substring in string in function ifj16.find
  
Shell sort: https://en.wikipedia.org/wiki/Shellsort
  - used for sorting (duh) in function ifj16.sort
  
Hashing table: https://en.wikipedia.org/wiki/Hash_table
  - used as "symbol table"
  

Error codes:
 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
 
 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
 
 3 - sémantická chyba v programu – nedefinovaná trída/funkce/promenná, pokus o redefinici trídy/funkce/promenné, atd.
 
 4 - sémantická chyba typové kompatibility v aritmetických, retezcových a relacních výrazech, príp. špatný pocet ci typ parametru u volání funkce.
 
 6 - ostatní sémantické chyby.
 
 7 - behová chyba pri nacítání císelné hodnoty ze vstupu.
 
 8 - behová chyba pri práci s neinicializovanou promennou.
 
 9 - behová chyba delení nulou.
 
 10 - ostatní behové chyby.
 
 99 - interní chyba interpretu tj. neovlivnená vstupním programem (napr. chyba alokace pameti, chyba pri otvírání souboru s rídicím programem, špatné parametry príkazové rádky atd.)
 
 
Properties of programming language IFJ16:
 - case sensitive
 - entry point of every program is compulsory class Main and it's function run
 - reserved keywords: boolean, break, class, continue, do, double, else, false, for, if, int, return, String, static, true, void, while
 
Operators priority table:

  Priority   Operator

3         * /

4         + -

6         < > <= >=

7         == !=


Built-in functions: 
 - int ReadInt();
 - double ReadDouble();
 - String ReadString();
 - void print(text);
 - int length(String s);
 - String substr(String s, int i, int n);
 - int compare(String s1, String s2);
 - int find(String s, String search);
 - String sort(String s);
