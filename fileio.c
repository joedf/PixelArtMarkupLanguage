#include "fileio.h"

char *getname_noext(char *input) {

	int pos = strpos(".",input);
	return strRtrim(input, (strlen(input)-pos+1));
}

char *getname_nopath(char *input) {


	
}

char *getfile_ext(char *input) {

	int pos = strpos(".",input);
	if (pos < 0)
		return NULL;
	else
		return strLtrim(input, pos+1);
}

char *FileRead(char *file) {

	FILE *pFile;
	long lSize;
	char *buffer;
	size_t result;


	pFile = fopen (file , "rb");
	if (pFile==NULL) {fputs("FileRead error",stderr); return NULL;} //exit(1);

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = (char*) malloc(sizeof(char)*lSize);
	if (buffer == NULL) {fputs("Memory error",stderr); exit(2);}

	// copy the file into the buffer:
	result = fread(buffer,1,lSize,pFile);
	if (result != lSize) {fputs("Reading error",stderr); exit(3);}

	/* the whole file is now loaded in the memory buffer. */

	// terminate
	fclose(pFile);
	//free(buffer);
	return buffer;
}

int FileExist(const char * filename) {
	
	FILE *file = fopen(filename, "r");
	if (file)
	{
		fclose(file);
		return 1;
	}
	return 0;
}

//modded version, for vs UAC in programfiles
//TEMPDIR has been added to avoid UAC Problems
extern char *_TEMPDIR;
char *gettempname() {

	//use _TEMPDIR
	int dlen = strlen(_TEMPDIR);
	char tempname[16+dlen];
	char *tempnam = (char*)malloc(18+dlen);
	int i = 0;
	do {
		sprintf(tempname,"%s\\%d.paml_temp",_TEMPDIR,i);
		i++;
	} while(FileExist(tempname) && (i < 999));
	
	strcpy(tempnam,tempname);

	if (!FileExist(tempname))
		return tempnam;
	else
		return NULL;
}

/* original working version
char *gettempname(char *dir) {

	char tempname[16];
	char *tempnam = (char*)malloc(18);
	int i = 0;
	do {
		sprintf(tempname,"%d.paml_temp",i);
		i++;
	} while(FileExist(tempname) && (i < 999));
	
	strcpy(tempnam,tempname);

	if (!FileExist(tempname))
		return tempnam;
	else
		return NULL;
}
*/

int FileDelete(char *file) {

	if (!FileExist(file)) {return 1;}

	if (remove(file) != 0 && FileExist(file))
	{
		//perror("Error deleting file");
		//fprintf(stderr, "FileDelete error\n");
		return 0;
	}
	else
	{
		//puts("File successfully deleted");
		return 1;
	}
}
