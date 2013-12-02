#include <windows.h>
#include <stdio.h>
#include "stringlib.h"

int main() {

	LoadLibrary("strlib.dll");

	//const char *String = "This is a test.";

	printf("1:\"%s\"\n",      strLtrim("This is a test.", 5));        //Returns "is a test."       -- PASS
	printf("2:\"%s\"\n",      strRtrim("This is a test.", 6));        //Returns "This is a"        -- PASS
	printf("3:\"%s\"\n",      strlower("This is a test."));           //Returns "this is a test."  -- PASS
	printf("4:\"%s\"\n",      strupper("This is a test."));           //Returns "THIS IS A TEST."  -- PASS
	printf("5:\"%s\"\n",    StrReplace("This is a test.", "i", "z")); //Returns "Thzs is a test."  -- PASS
	printf("6:\"%s\"\n", StrReplaceAll("This is a test.", "i", "z")); //Returns "Thzs zs a test."  -- PASS
	printf("7:\"%s\"\n",        SubStr("This is a test.", 6, 6+4));   //Returns "is a"             -- PASS
	printf("8:\"%i\"\n",        strpos("test", "This is a test."));   //Returns "11"               -- PASS

}

