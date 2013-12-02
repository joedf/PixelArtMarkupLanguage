//REVISION: 2013-04-29 20:38:10 - Joe DF
//Compile as strlib.dll

#include "stringlib.h"

#ifdef BUILD_DLL
	#include <windows.h>
	#define DLLEXPORT __declspec(dllexport)
	#define DLLIMPORT __declspec(dllimport)
#endif

#ifndef BUILD_DLL
	#define DLLEXPORT
	#define DLLIMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

	DLLEXPORT int strpos(char *needle, char *haystack) {

		char *dest = strstr(haystack, needle);
		return (dest - haystack) + 1;
	}

	DLLEXPORT int strRpos(char *needle, char *haystack) {

		
	}

	DLLEXPORT int strchposo(char c, char *haystack, int occurence) {

		int i;
		int len = strlen(haystack);
		//printf("len %d - ", len);
		int found = 0;
		for (i=0;((i<len) && (found<occurence));i++) {
			//printf("comparing %c to %c\n", haystack[i], c);
			if (haystack[i] == c) {
				found++;
			}
		}
		if (found == 0)
			return -1;
		else
			return i;
	}

	DLLEXPORT int strchro(char *str, char c) {

		char *pch;
		int found = 0;
		pch=strchr(str,c);
		while (pch!=NULL)
		{
			//printf("found at %d\n",pch-str+1);
			found++;
			pch=strchr(pch+1,c);
		}
		return found;
	}

	DLLEXPORT int strstro(char *str, char *needle) {

		char *pch;
		int found = 0;
		char *str2 = (char*)malloc(strlen(str)+1);
		strcpy(str2,str);
		pch = strstr(str2,"simple");
		strncpy(pch,"",1);
		while (pch!=NULL)
		{
			found++;
			pch = strstr(str2,"simple");
			strncpy(pch,"",1);
		}
		free(str2);
		return found;
	}

	DLLEXPORT char *SubStr(const char *string, int startpos, int endpos) { //END position NOT length!!!

		int len = strlen(string);
		if ((len >= endpos) && (endpos >= startpos))
		{
			int p = endpos - startpos; //+1;
			//printf("%d\n", p);
			char *to = (char*) malloc(p+2);
			char *str = strncpy(to, string+startpos-1, p+1);
			str[p] = '\0';
			//sprintf(to,"%s",str);
			//to[p+1] = '\0';
			//printf("%d\n", strlen(str));
			//free(to);
			return str;
		}
		else
		{
			return NULL;
		}
	}

	DLLEXPORT char *SubStr2(const char *string, int startpos, int endpos) { //END position NOT length!!!

		int len = strlen(string);
		if ((len >= endpos) && (endpos >= startpos))
		{
			int i;
			int p = endpos - startpos;
			char *str = (char*)malloc(p+1);
			for (i = startpos; i < endpos; ++i)
			{
				str[i-startpos] = string[i];
			}
			str[p] = '\0';
			return str;
		}
		else
		{
			return NULL;
		}
	}

	DLLEXPORT char *StrReplace(char *haystack, char *needle, char *replacetext) {
		
		static char buffer[4096];
		char *p;

		if(!(p = strstr(haystack, needle)))  // Is 'needle' even in 'haystack'?
			return haystack;

		strncpy(buffer, haystack, p-haystack); // Copy characters from 'haystack' start to 'needle' st$
		buffer[p-haystack] = '\0';

		sprintf(buffer+(p-haystack), "%s%s", replacetext, p+strlen(needle));

		return buffer;
	}

	// Replaced by str_replaceAll(), since this one is buggy
	//Function adatpted from: http://stackoverflow.com/a/780024/883015
	DLLEXPORT char *StrReplaceAll(char const * const original, char const * const pattern, char const * const replacement) {
		
		size_t const replen = strlen(replacement);
		size_t const patlen = strlen(pattern);
		size_t const orilen = strlen(original);

		size_t patcnt = 0;
		const char * oriptr;
		const char * patloc;

		// find how many times the pattern occurs in the original string
		for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
		{
			patcnt++;
		}

		{
			// allocate memory for the new string
			size_t const retlen = orilen + patcnt * (replen - patlen);
			char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

			if (returned != NULL)
			{
				// copy the original string, 
				// replacing all the instances of the pattern
				char * retptr = returned;
				for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
				{
					size_t const skplen = patloc - oriptr;
					// copy the section until the occurence of the pattern
					strncpy(retptr, oriptr, skplen);
					retptr += skplen;
					// copy the replacement 
					strncpy(retptr, replacement, replen);
					retptr += replen;
				}
				// copy the rest of the string.
				strcpy(retptr, oriptr);
			}
				return returned;
		}
	}
	//

	//http://coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring
	DLLEXPORT char *str_replaceAll(const char *string, const char *substr, const char *replacement) {
		char *tok = NULL;
		char *newstr = NULL;
		char *oldstr = NULL;
		char *head = NULL;

		/* if either substr or replacement is NULL, duplicate string a let caller handle it */
		if ( substr == NULL || replacement == NULL ) return strdup (string);
		newstr = strdup (string);
		head = newstr;
		while ( (tok = strstr ( head, substr ))){
			oldstr = newstr;
			newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
			/*failed to alloc mem, free old string and return NULL */
			if ( newstr == NULL ){
				free (oldstr);
				return NULL;
			}
			memcpy ( newstr, oldstr, tok - oldstr );
			memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
			memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
			memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
			/* move back head right after the last replacement */
			head = newstr + (tok - oldstr) + strlen( replacement );
			free (oldstr);
		}
		return newstr;
	}

	DLLEXPORT char *strRtrim(const char *string, int trim) { //returns trimmed string from the right
		
		int len = (strlen(string)-trim);
		char *str = (char*)malloc(len);
		strncpy(str, string, len);
		str[len] = '\0';   /* null character manually added */

		return str;
	}

	DLLEXPORT char *strLtrim(const char *string, int trim) { //returns trimmed string from the left
		
		//char *str = SubStr(string, trim, strlen(string));
		int len = strlen(string);
		char *to = (char*) malloc(len);
		char *str = strncpy(to, string+trim, len-1);
		str[len-1] = '\0';
		//free(to);
		return str;
	}

	DLLEXPORT char *strlower(char *string) {
		
		int x = strlen(string);
		int i;
		char *lower = (char*)malloc(x+1);
		for (i = 0; i <= x; i++)
		{
			lower[i] = tolower(string[i]);
		}
		return lower;
	}

	DLLEXPORT char *strupper(char *string) {
		
		int x = strlen(string);
		int i;
		char *upper = (char*)malloc(x+1);
		for (i = 0; i <= x; i++)
		{
			upper[i] = toupper(string[i]);
		}
		return upper;
	}

	DLLEXPORT char *stripspace(char *tempf) {
		tempf = str_replaceAll(tempf," ","");
		tempf = str_replaceAll(tempf,"\t","");
		tempf = str_replaceAll(tempf,"\r\n\r\n","\r\n");
		tempf = str_replaceAll(tempf,"\r\r","\r");
		tempf = str_replaceAll(tempf,"\n\n","\n");
		return tempf;
	}

	DLLEXPORT char *stripCRLF(char *tempf) {
		/* old Method */
		tempf = str_replaceAll(tempf,"\r","");
		tempf = str_replaceAll(tempf,"\n","");
		return tempf;
		

		/*new method
		int n = strpos("\n", tempf);
		int r = strpos("\r", tempf);

		printf("[n:%d/r:%d]\n", n, r);

		if ((n > 0) && (r > 0)) {
			if (n < r) {
				//strncpy(tempf,tempf,n-1);
				return strLtrim(tempf,strlen(tempf)-n+1);
			}
			else if (r < n) {
				//strncpy(tempf,tempf,r-1);
				return strLtrim(tempf,strlen(tempf)-r+1);
			}
		}
		else
		{
			return tempf;
		}
		*/
	}

	DLLEXPORT int stripCRLFarr(char tempf[]) {
		/* old method
		char *tmp = (char*)malloc(strlen(tempf+1));
		sprintf(tmp,"%s",tempf);
		tmp = stripCRLF(tmp);
		sprintf(tempf,"%s",tmp);
		//return tempf;
		*/

		/*new method*/
		int n = strpos("\n", tempf);
		int r = strpos("\r", tempf);

		//printf("[n:%d/r:%d]\n", n, r);

		if ((n > 0) && (r > 0))
		{
			if (n < r) {
				//strncpy(tempf,tempf,n-1);
				tempf[n-1] = '\0';
				return 1;
			}
			else if (r < n) {
				//strncpy(tempf,tempf,r-1);
				tempf[r-1] = '\0';
				return 1;
			}
		}
		else if ((n > 0) && (r < 0))
		{
			tempf[n-1] = '\0';
			return 1;
		}
		else if ((r > 0) && (n < 0))
		{
			tempf[r-1] = '\0';
			return 1;
		}
		else
		{
			return 0;
		}
		
	}

#ifdef __cplusplus
}
#endif
