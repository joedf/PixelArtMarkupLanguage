#include "stringlib.h"

//returns the filename without its extension
char *getname_noext(char *input);

//returns the filename without its full path
char *getname_nopath(char *input);

//returns the filename's extension
char *getfile_ext(char *input);

//returns a pointer to the buffer containing the whole file
char *FileRead(char *file);

//returns a non-zero value if the file exists
int FileExist(const char * filename);

//returns a pointer of a tmpfile name that does not exist
// with ".paml_temp" extension
char *gettempname();

//returns a non-zero value if the file deleted successfully
int FileDelete(char *file);