#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//returns negtive on error
int strpos(char *needle, char *haystack);

//returns negtive on error //strpos from Right to Left
int strRpos(char *needle, char *haystack);

//returns negtive on error, returns pos of needle, request occurence
int strchposo(char c, char *haystack, int occurence);

//returns number of occurences of char in a string
int strchro(char *str, char c);

//returns number of occurences of string in a string
int strstro(char *str, char *needle);

//END position NOT length!!!
char *SubStr(const char *string, int startpos, int endpos);

//END position NOT length!!!
char *SubStr2(const char *string, int startpos, int endpos);

//replace first occurence
char *StrReplace(char *haystack, char *needle, char *replacetext);

//Function adapted from: http://stackoverflow.com/a/780024/883015
char *StrReplaceAll(char const * const original, char const * const pattern, char const * const replacement);

//Function by "chantra": http://coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring
char *str_replaceAll(const char *string, const char *substr, const char *replacement);

//returns trimmed string from the right
char *strRtrim(const char *string, int trim);

//returns trimmed string from the left
char *strLtrim(const char *string, int trim);

//convert string to lowercase
char *strlower(char *string);

//convert string to uppercase
char *strupper(char *string);

//Strips space(char)
char *stripspace(char *tempf);

//Strips CRLF from char*
char *stripCRLF(char *tempf);

//Strips CRLF from char[]
int stripCRLFarr(char tempf[]);