//REVISION: 2013-09-26 21:43:15 - joedf
// ------------------------------------------------------
// ADDED: a few free() memory in IPC function
//        Actual cause of the bug was not memory but Pamlviewer was
//		  Correctly managing Tempfile names using only time with seconds
//		  So clicking faster that 1 click/per second, would cause a "crash"
//		  because, the other file is already Open... so Paml veiwer will be
//		  updated accordingly.

//REVISION: 2013-06-03 02:04:53 - joedf
// ------------------------------------------------------
// ADDED: Patch ~HTML_#001 @ line ~698
// StrReplaceAll() seems to have better performance then str_replaceAll()...
//   	Further investigation will be needed, unsure if StrReplaceAll() is
//  	real source of the "problem" (DLL crashes)

//REVISION: 2013-05-27 23:51:23 - joedf
// ------------------------------------------------------
// TEMPDIR has been added to avoid UAC Problems, and that the System/OS tempdir
//  	may be used/specifed from a DllCall
// A few fixes for DLL interface, +DLL Bug fixes
// 		eg. Crashes, Access Violation, etc...
// Interaction with Paml Viewer is now working very well...
// IMPORTANT: StrReplaceAll() has been completely by str_replaceAll()
//  	Since it was causing some kind of weird memory/pointer bug...
// SubStr2() has been added as alternative to SubStr()
//  	Not sure, if it's better, or fully function, might be, further
//  	testing will be needed, seems to help fix problems with the
//  	DLL interaction...
//  	For the moment, it is being used in tag_get(), seems to work well...

//REVISION: 2013-05-26 01:58:58 - joedf
// ------------------------------------------------------
// DLL building has been added
// "Access Violation" bug fix, by allocating memory
// 	-	pamlfile_t *paml;
// 	+	pamlfile_t *paml = malloc(sizeof *paml);
//  possible fix for main too? create paml_main() instead of
//  directly nesting it twice (2) both in IPC_paml() & main()
//  ?!? --> Would this also be a possible fix for the x64 build?

//REVISION: 2013-05-23 18:52 - joedf
// ------------------------------------------------------
// libbmp has been completely replaced with QDBMP (Quick n' Dirty BMP)
//	 Advantages:
//		-Reading (get pixel)
//		-"Valid" bmp files

#define APP_VERSION "1.0.0"
#define APP_NAME "PAML Processor"

#ifdef BUILDDLL
	#include <windows.h>
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT
#endif

#include <stdio.h>
#include <stdlib.h>
//#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
//#include <math.h>       /* sqrt */
#include "lib\qdbmp\qdbmp.h"

#include "stringlib.h"
#include "fileio.h"
#include "htmlvars.h"
//#include <string.h>

#define MAXDEFS 256
#define MAXDEF_SIZE 64

#define PAML_BMP 223 //ASCII values B+M+P=66+77+80
#define PAML_HTML 309 // H+T+M+L=72+84+77+76
#define PAML_PNG 229 // P+N+G=80+78+71
#define PAML_XPM 245 // X+P+M=88+80+77
//#define PAML_DEFAULT PAML_BMP

typedef struct /* pamlfile_ */ {
	char *author;
	char *date;
	char *license;
	char *bgcolor;
	int xpixels;
	int ypixels;
	int sizexpixels;
	int sizeypixels;
	//char *defcolor;
	char *drawpixels;
} pamlfile_t;

/*typedef struct  defcolor_  {*/
	int DEF_TOTAL = 0;
	char DEF_NAME[MAXDEFS][MAXDEF_SIZE];
	char DEF_VALUE[MAXDEFS][MAXDEF_SIZE];/*
} defcolor_t;*/

/*
	struct opts
	{
		char *pamlfile;
		char *outfile;
		char *type;
	};
*/

char *_EXENAME; // argv[0]
char *_MAINFILE;
char *_OUTFILE;
int   _OUTTYPE;
int   _HTML_CENTER=0;
char *_TEMPDIR; //used for modded gettempname() function, TEMPDIR has been added to avoid UAC Problems

//Handling paml_t functions
	//int paml_Main(char *paml_file); //Malloc Error ?
	void get_args(int argc, char** argv);
	void help();
	void about();
	int paml_read(pamlfile_t *paml, char *file);
	int paml_write(pamlfile_t *paml, char *file, int type);
	int paml_write_BMP(pamlfile_t *paml, char *file);
		int paml_set_pixel_BMP(BMP* bmp, int x, int y, int w, int h, char *def);
		int paml_set_rect_BMP(BMP* bmp, int x, int y, int w, int h, int r, int g, int b);
	int paml_write_PNG(pamlfile_t *paml, char *file);

	int paml_write_XPM(pamlfile_t *paml, char *file);
	
	int paml_write_HTML(pamlfile_t *paml, char *file);
	
	char *preprocess(char *paml_file);
	int paml_def(char *tmpdef, char *bgcolor);
	//int paml_def_insert(char *dest[], char *src);
	char *tag_get(char *tag, char *paml_t);
	char *paml_def_get(char *name, char *bgcolor);
//auxiliary functions
	char *rgb2bgr(char *irgb);
	int rgbstr(char *rgb, int *r, int *g, int *b);
	int str2hex(char *color, int i);
	int validhex(char *str);
	int htmlcolorvalid(char *c);
	int output_typef(char *t);
	char *output_type_ext(int t);
	char *output_type_name(int t);
	int yes_no_get();
	int numlen(int x);

// Inter-process communication (IPC)
// DLL Function is named this way for that purpose
DLLEXPORT int IPC_paml(char *paml_file, char* outfile, char* outtype, char* tmpdir) {

	_EXENAME = "Undefined";
	_OUTTYPE = output_typef(outtype);
	_OUTFILE = outfile;

	_TEMPDIR = tmpdir;

	_HTML_CENTER = 1;

	_MAINFILE = paml_file;

	printf("DLL_MODE: paml: %s | outfile: %s | type: %s | tmpdir: %s\n", paml_file, outfile, outtype, _TEMPDIR);

	char *pp_paml_file = preprocess(paml_file);
	if (pp_paml_file == NULL) return 0;

	//pamlfile_t *paml;

	//allocate memory for struct
	//struct st *x = malloc(sizeof *x);
	//printf("a\n");
	pamlfile_t *paml = malloc((sizeof *paml)); //"Access Violation" bug fix, by allocating memory
	paml->author = (char*)malloc(128);
	//printf("b\n");
	paml->date = (char*)malloc(64);
	//printf("c\n");
	paml->license = (char*)malloc(128);
	//printf("d\n");
	paml->bgcolor = (char*)malloc(16);
	//printf("e\n");
	paml->xpixels = 1;
	//printf("f\n");
	paml->ypixels = 1;
	paml->sizexpixels = 1;
	paml->sizeypixels = 1;
	//char *defcolor;
	paml->drawpixels = (char*)malloc(4096);

	paml_read(paml, pp_paml_file);
	FileDelete(pp_paml_file);

	//char *Out_File = (char*)malloc(strlen(paml_file)+6);
	char *Out_File = (char*)malloc(270);
	
	if (_OUTFILE==NULL)
		sprintf(Out_File,"%s%s",getname_noext(paml_file),output_type_ext(_OUTTYPE));
	else if (getfile_ext(_OUTFILE)==NULL)
		sprintf(Out_File,"%s%s",_OUTFILE,output_type_ext(_OUTTYPE));
	else
		sprintf(Out_File,"%s",_OUTFILE);

		//sprintf(Out_File,"%s%s",getname_noext(paml_file),getfile_ext(_OUTFILE));

	if (paml_write(paml, Out_File, _OUTTYPE)) {
		printf("\nSuccessfully created \"%s\"! - [%s MODE]\n", Out_File, output_type_name(_OUTTYPE));
		free(pp_paml_file);
		free(paml->drawpixels);
		free(paml);
		return 1;
	}else{
		printf("\nError: Could not create \"%s\"! - [%s MODE]\n", Out_File, output_type_name(_OUTTYPE));
		free(pp_paml_file);
		free(paml->drawpixels);
		free(paml);
		return 0;
	}

	//printf(" - [%s MODE]\n", output_type_name(_OUTTYPE));	
}

DLLEXPORT char *paml_version() {
	
	return APP_VERSION;
}

int main(int argc, char **argv) {

	_EXENAME = argv[0];
	_OUTTYPE = PAML_BMP;
	_OUTFILE = NULL;

	_TEMPDIR = ".";
	//get_args(argc,argv);

	//char *paml_file = "turtle4.paml";
	char *paml_file;

	if (argc < 2) {
		help();
		return 0;
	}
	else //if (argc == 2)
	{
		get_args(argc,argv);

		if (argv[1][0]!='-')
		{
			paml_file = argv[1];
		}
		else
		{
			return 0;
		}
	}

	_MAINFILE = paml_file;

	char *pp_paml_file = preprocess(paml_file);
	if (pp_paml_file == NULL) exit(-1);

	pamlfile_t *paml;
	paml_read(paml, pp_paml_file);
	FileDelete(pp_paml_file);

	//char *Out_File = (char*)malloc(strlen(paml_file)+6);
	char *Out_File = (char*)malloc(270);
	
	if (_OUTFILE==NULL)
		sprintf(Out_File,"%s%s",getname_noext(paml_file),output_type_ext(_OUTTYPE));
	else if (getfile_ext(_OUTFILE)==NULL)
		sprintf(Out_File,"%s%s",_OUTFILE,output_type_ext(_OUTTYPE));
	else
		sprintf(Out_File,"%s",_OUTFILE);

		//sprintf(Out_File,"%s%s",getname_noext(paml_file),getfile_ext(_OUTFILE));

	if (paml_write(paml, Out_File, _OUTTYPE)) {
		printf("\nSuccessfully created \"%s\"!", Out_File);
	}else{
		printf("\nError: Could not create \"%s\"!", Out_File);
	}

		printf(" - [%s MODE]\n", output_type_name(_OUTTYPE));
}

void get_args(int argc, char** argv) {

	int i;

	/* Start at i = 1 to skip the command name. */
	for (i = 1; i < argc; i++) {

	/* Check for a switch (leading "-"). */
		if (strcmp(argv[i],"--help")==0) {
			help();
			continue;
		}

		if (argv[i][0] == '-') {

			/* Use the next character to decide what to do. */
			switch (argv[i][1]) {

				/* "argv" is a string, we do "+1" to get the
				argument that arrives right after. "i" is the
				argument count, so argument 1, 2, 3, etc. */

				case 'a': about();
						break;
						
				case 'o': _OUTFILE = argv[i+1];
						break;

				case 't': _OUTTYPE = output_typef(argv[i+1]);
						break;

				case 'c': _HTML_CENTER = 1;
						break;
						
				case 'h': help();
						break;
				
				/* Default action with a non existing argument */
				default: fprintf(stderr,"\nERROR: Unknown switch %s\n", argv[i]);
						printf("try \"-h\" for help...\n");
			}
		}
	}
}

void help() {

	printf("Usage: %s In_File.paml [Switches]\n\n", _EXENAME);
	printf("\t-o        |Output to file path\n");
	printf("\t-t        |Output file type [BMP/PNG/XPM/HTML]\n");
	printf("\t-c        |Centers HTML output, in [HTML] mode\n");
	printf("\t-a        |About this software\n");
	printf("\t-h,--help |Displays this (help) message\n");
}

void about() {

	printf("\n%s - v%s \n", APP_NAME, APP_VERSION);
	//printf("______________________________________________\n\n");
	printf("Build Time: %s %s\n\n", __DATE__, __TIME__);
	
	printf("PAML (Pixel Art Markup Language) - MIT License\n");
	printf("Copyright (c) %d Joe DF (joedf@users.sourceforge.net)\n\n", atoi(strLtrim(__DATE__,6)));
	
	printf("Powered by:\n\n");
	printf("\tQDBMP (Quick n' Dirty BMP) - MIT License\n");
	printf("\tCopyright (c) 2007 Chai Braudo (braudo@users.sourceforge.net)\n\n");

	printf("\tLodePNG - zlib/libpng License\n");
	printf("\tCopyright (c) 2005-2013 Lode Vandevenne\n");
	
	printf("\nView full MIT License (Y/N)?");
	if (yes_no_get())
	{
		printf("\nThe MIT License (MIT)\n\n");
		printf("Copyright (c) <year> <copyright holders>\n\n");
		printf("Permission is hereby granted, free of charge, to any person obtaining a copy\n");
		printf("of this software and associated documentation files (the \"Software\"), to deal\n");
		printf("in the Software without restriction, including without limitation the rights\n");
		printf("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n");
		printf("copies of the Software, and to permit persons to whom the Software is\n");
		printf("furnished to do so, subject to the following conditions:\n\n");
		printf("The above copyright notice and this permission notice shall be included in\n");
		printf("all copies or substantial portions of the Software.\n\n");
		printf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n");
		printf("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n");
		printf("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n");
		printf("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
		printf("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n");
		printf("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n");
		printf("THE SOFTWARE.\n");
	}
	
	getchar(); //patches... bug?!
	printf("\nView full zlib/libpng License (Y/N)?");
	if (yes_no_get())
	{
		printf("\nThe zlib/libpng License (Zlib)\n\n");
		printf("Copyright (c) <year> <copyright holders>\n\n");

		printf("This software is provided 'as-is', without any express or implied\n");
		printf("warranty. In no event will the authors be held liable for any damages\n");
		printf("arising from the use of this software.\n\n");

		printf("Permission is granted to anyone to use this software for any purpose,\n");
		printf("including commercial applications, and to alter it and redistribute it\n");
		printf("freely, subject to the following restrictions:\n\n");

			printf("\t1. The origin of this software must not be misrepresented; you must not\n");
			printf("\tclaim that you wrote the original software. If you use this software\n");
			printf("\tin a product, an acknowledgment in the product documentation would be\n");
			printf("\tappreciated but is not required.\n\n");
			
			printf("\t2. Altered source versions must be plainly marked as such, and must not\n");
			printf("\tbe misrepresented as being the original software.\n\n");
			
			printf("\t3. This notice may not be removed or altered from any source\n");
			printf("\tdistribution.\n");
	}
}

int paml_read(pamlfile_t *paml, char *file) {

	//Set default values

	paml->author = "Anonymous";
	paml->date = "Undefined";
	paml->license = "Undefined";
	paml->bgcolor = "FFFFFF";
	paml->xpixels = 1;
	paml->ypixels = 1;
	paml->sizexpixels = 1;
	paml->sizeypixels = 1;
	//char *defcolor;
	paml->drawpixels = NULL;

	printf("\nReading PAML file \"%s\"...\n", _MAINFILE);
	char *pTEMP = FileRead(file);

	printf("\tValidity Check...\n");
	char *paml_t = tag_get("paml",pTEMP);
	free(pTEMP);

	char *info = tag_get("info",paml_t);
	paml_t = strlower(paml_t);

	char *dcolor = tag_get("defcolor",paml_t);
	char *dpixel = stripCRLF(tag_get("drawpixels",paml_t));

	char *tmpdef = gettempname();
	FILE* pDefw = fopen(tmpdef, "wb");
	fputs(dcolor,pDefw);
	fclose(pDefw);

	char *tmpinfo = gettempname();
	FILE* pInfow = fopen(tmpinfo, "wb");
	fputs(info,pInfow);
	fclose(pInfow);


	printf("\tInformation Extraction...\n");

	FILE* pInfo = fopen(tmpinfo, "rb");
	char line[4096];
	//char *line = (char*)malloc(4096);
	int linenum = 0;
	int eqpos = 0;
	char *infoname = (char*)malloc(32);
	char *infodef = (char*)malloc(2048);

	while (fgets(line, sizeof(line), pInfo)) {
		linenum+=1;
		//printf("%5d : %s", linenum, line);
		//line = stripCRLF(line);
		stripCRLFarr(line);
		eqpos = strpos("=",line);
		//printf("[%d]:%s\n",eqpos,line);
		if (eqpos > 0) {
			infoname = strlower(SubStr(line,1,eqpos));
			infodef = strLtrim(line,eqpos);
			if (strcmp("author",infoname)==0)
				paml->author = infodef;
			if (strcmp("dateofcreation",infoname)==0)
				paml->date = infodef;
			if (strcmp("date",infoname)==0)
				paml->date = infodef;
			if (strcmp("license",infoname)==0)
				paml->license = infodef;
			if (strcmp("xpixels",infoname)==0)
				paml->xpixels = atoi(infodef);
			if (strcmp("ypixels",infoname)==0)
				paml->ypixels = atoi(infodef);
			if (strcmp("sizexpixels",infoname)==0)
				paml->sizexpixels = atoi(infodef);
			if (strcmp("sizeypixels",infoname)==0)
				paml->sizeypixels = atoi(infodef);
			if (strcmp("bgcolor",infoname)==0)
				paml->bgcolor = str_replaceAll(infodef,"#","");
		}
	}
	fclose(pInfo);
	FileDelete(tmpinfo);
	
	free(infoname);
	free(infodef);

	//printf("A:[%s]\n", paml->bgcolor);

	if ( (!validhex(paml->bgcolor))
		&& (strcmp(htmlcolorname(paml->bgcolor),HTMLCOLOR_DEFAULT)) ) {
			paml->bgcolor = htmlcolorname(paml->bgcolor);
	}
	else {
		if (strcmp(strlower(paml->bgcolor),HTMLCOLOR_DEFAULTNAME)==0)
			paml->bgcolor = HTMLCOLOR_DEFAULT;
	}

	//printf("B:[%s]\n", paml->bgcolor);

	//paml->author = "Joe DF";
	//paml->date = "July/12/2012";
	//paml->license = "Undefined";
	//paml->bgcolor = "FFFFFF";
	//paml->xpixels = 2;
	//paml->ypixels = 2;
	//paml->sizexpixels = 1;
	//paml->sizeypixels = 1;
	//char *defcolor;
	paml->drawpixels = dpixel;

	printf("\t\tAuthor:      %s\n", paml->author);
	printf("\t\tDate:        %s\n", paml->date);
	printf("\t\tLicense:     %s\n", paml->license);
	printf("\t\tbgcolor:     %s\n", paml->bgcolor);
	printf("\t\txpixels:     %d\n", paml->xpixels);
	printf("\t\typixels:     %d\n", paml->ypixels);
	printf("\t\tsizexpixels: %d\n", paml->sizexpixels);
	printf("\t\tsizeypixels: %d\n", paml->sizeypixels);

	paml_def(tmpdef, paml->bgcolor);

	printf("Done.\n");

	return 0;
}

int paml_write(pamlfile_t *paml, char *file, int type) {

	switch(type) {

		case PAML_BMP: return paml_write_BMP(paml,file);
		case PAML_PNG: return paml_write_PNG(paml,file);
		case PAML_XPM: return paml_write_XPM(paml,file);
		case PAML_HTML: return paml_write_HTML(paml,file);
		default: return paml_write_BMP(paml,file);
	}
}

int paml_write_BMP(pamlfile_t *paml, char *file) {

	//printf("\n[QDBMP MODE]\n");

	//char *BGR = (char*)malloc(7);
	int b;
	int g;
	int r;

	rgbstr(paml->bgcolor,&r,&g,&b);

	int x = 0;
	int y = 0;

	int width = (paml->xpixels * paml->sizexpixels);
	int height = (paml->ypixels * paml->sizeypixels);

	if (width <= 0) width = 1;
	if (height <= 0) height = 1;

	int depth = 24;

	BMP* bmp = BMP_Create( width, height, depth );
	BMP_CHECK_ERROR( stderr, -3 );

	//draw bgcolor to "canvas"
	//new method
	//printf("bg:%d%d%d\n", r,g,b);
	//bmp_set_rect(bmp,0,0,width,height,bgpixel);

	/* old method
	int bg_x, bg_y;
	for (bg_y = 0; bg_y < height; bg_y++)
	{
		for (bg_x = 0; bg_x < width; bg_x++)
		{
			bmp_set_pixel(bmp, bg_x, bg_y, bgpixel);
		}
	}*/

	int img_x = 0, img_y = -(paml->sizeypixels);
	int pcount, pstart = 0, pstop;
	char pseparator = ',';
	int ptotal = strchro(paml->drawpixels, pseparator);
	char *pvalue = (char*)malloc(MAXDEF_SIZE);

	for (pcount = 0; pcount < ptotal; ++pcount)
	{
		pstop = strchposo(pseparator, paml->drawpixels, pcount+1);
		pvalue = SubStr(paml->drawpixels, pstart+1, pstop);
		img_x = (pcount%(paml->xpixels))*paml->sizexpixels;
		if ((pcount%(paml->xpixels))==0)
			img_y += paml->sizeypixels;
		paml_set_pixel_BMP(bmp,img_x,img_y,paml->sizexpixels,paml->sizeypixels,paml_def_get(pvalue,paml->bgcolor));
		pstart = pstop;

	}

	BMP_WriteFile( bmp, file );
	BMP_CHECK_ERROR( stdout, -2 );

	/* Free all memory allocated for the image */
	BMP_Free( bmp );
	return ( BMP_GetError() == BMP_OK );
}

int paml_set_pixel_BMP(BMP* bmp, int x, int y, int w, int h, char *def) {

	int b,g,r;

	if (!validhex(def))
	{
		rgbstr(htmlcolorname(def),&r,&g,&b);
	}
	else
	{
		rgbstr(def,&r,&g,&b);
	}

	paml_set_rect_BMP(bmp, x, y, w, h, r, g, b);
}

int paml_set_rect_BMP(BMP* bmp, int x, int y, int w, int h, int r, int g, int b) {

	int i, j;
	for (i = y; i < h+y; ++i)
	{
		for (j = x; j < w+x; ++j)
		{
			BMP_SetPixelRGB( bmp, j, i, r, g, b );
		}
	}
}

int paml_write_PNG(pamlfile_t *paml, char *file) {

	//code...
}

int paml_write_XPM(pamlfile_t *paml, char *file) {

	//code... below is an example START
	
		/* XPM */
		static char * joedf_xpm[] = {
		"16 16 3 1",
		" 	c #FFFFFF",
		".	c #000000",
		"+	c #FF0000",
		"   ...          ",
		"   .+.          ",
		"   .+.          ",
		"   .+.          ",
		"   .+.........  ",
		"   .+.+++.+++.  ",
		"   .+.+.+.+...  ",
		"   .+.+.+.++.   ",
		"....+.+.+.+...  ",
		".++++.+++.+++.  ",
		"..............  ",
		".++..+++.+.++.  ",
		".+.+.+...+.+..  ",
		".+.+.++.....+.  ",
		".++..+..  .++.  ",
		".......   ....  "};

	//END
}

int paml_write_HTML(pamlfile_t *paml, char *file) {

	//Open file...
	FILE* pFile = fopen(file,"w");
	if (pFile!=NULL)
	{
		//HTML START
		fputs("<!DOCTYPE html>\n<html>\n",pFile);

		//Write Metadata
		fputs("\n<!--PAML_METADATA\n", pFile);
		fprintf(pFile,"\n\tAuthor: %s\n", paml->author);
		fprintf(pFile,"\tDate: %s\n", paml->date);
		fprintf(pFile,"\tLicense: %s\n", paml->license);
		fprintf(pFile,"\txpixels: %d\n", paml->xpixels);
		fprintf(pFile,"\typixels: %d\n", paml->ypixels);
		fprintf(pFile,"\tsizexpixels: %d\n", paml->sizexpixels);
		fprintf(pFile,"\tsizeypixels: %d\n", paml->sizeypixels);
		fputs("\nPAML_METADATA-->\n\n", pFile);

		//Write CSS Style Definitions
		fputs("<style>\n",pFile);
			
			//Write Image centering CSS if _HTML_CENTER is true
			if (_HTML_CENTER)
			{
				char c_code[128];
				sprintf(c_code,".Paml{position:absolute;left:50%%;top:50%%;margin-left:-%dpx;margin-top:-%dpx;}",(paml->xpixels * paml->sizexpixels)/2,(paml->ypixels * paml->sizeypixels)/2);
				fprintf(pFile, "%s\n", c_code);
			}

			//Each Definitions looks a bit like this
			//	#%symbol%{width: %sizexpixels%px`;
			//	height: %sizeypixels%px`;background-color: %color%`;
			//	border:0px solid #000`;float: left`;}

		//CSS .Paml td == Pixel definitions eg. width, etc...
		fprintf(pFile,".Paml td{width:%dpx;height:%dpx;border:0px solid #000;float:left;}\n",paml->sizexpixels, paml->sizeypixels);

		int d;
		for (d = 0; d < DEF_TOTAL; ++d)
		{
			//Writting CSS...
			if (strlen(DEF_NAME[d])>0 && strlen(DEF_VALUE[d])>0) //Patch ~HTML_#001
				fprintf(pFile,"#%s{background-color:%s%s}\n", DEF_NAME[d], (validhex(DEF_VALUE[d]) && DEF_VALUE[d][0]!='#') ? "#" : "", DEF_VALUE[d]);
			//fprintf(pFile,"#%s{width:%dpx;height:%dpx;background-color:%s%s;border:0px solid #000;float:left;}\n", DEF_NAME[d], paml->sizexpixels, paml->sizeypixels, (validhex(DEF_VALUE[d]) && DEF_VALUE[d][0]!='#') ? "#" : "", DEF_VALUE[d]);
		}
		fputs("</style>\n", pFile);

		//Set HTML Body Style (CSS)
		fprintf(pFile,"<body style=\"background-color:%s%s;\" marginwidth=\"0\" marginheight=\"0\">\n\n", (validhex(paml->bgcolor) && paml->bgcolor[0]!='#') ? "#" : "", paml->bgcolor);

		//Set PixelMap Table START
		fputs("<table class=\"Paml\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n<tr>", pFile);

		//Loop through Pixel Array (paml->drawpixels)
		int pcount, pstart = 0, pstop;
		char pseparator = ',';
		int ptotal = strchro(paml->drawpixels, pseparator);
		char *pvalue = (char*)malloc(MAXDEF_SIZE);
		int w = paml->xpixels;
		if (w <= 0) w = 1;

		for (pcount = 0; pcount < ptotal; ++pcount)
		{
			pstop = strchposo(pseparator, paml->drawpixels, pcount+1);
			pvalue = SubStr(paml->drawpixels, pstart+1, pstop);
			//paml_def_get(pvalue,paml->bgcolor));

			//Write Table row start/stop Indicator
			if ((pcount%w)==0 && pcount!=0)
			{
					fputs("\n</tr>",pFile);
					fputs("\n<tr>",pFile);
			}

			if (strlen(pvalue)==0)
				fputs("\n<td />",pFile);
			else
				fprintf(pFile,"\n<td id=\"%s\" />", pvalue);

			pstart = pstop;
		}

		//HTML END
		fputs("\n</tr>\n</table>\n\n</body>\n</html>", pFile);
		
		fclose (pFile);
		return 1;
	}
	else
	{
		printf("HTMLwrite Error...\n");
		return 0;
	}
}

char *preprocess(char *paml_file) {

	printf("Preprocessing...\n");
	char *tempf = FileRead(paml_file);
	if (tempf == NULL) return NULL;
	printf("\tStripping Whitespaces...\n");
	tempf = stripspace(tempf);
	//tempf = strlower(tempf);
	int epos = strpos("</paml>",tempf)+6;
	if (epos < 0)
	{
		printf("Error: Missing End Tag \"</paml>\"\n");
		return NULL; //exit(-1);
	}

	//int trim_a = strlen(tempf)-epos;
	//tempf = strRtrim(tempf, trim_a);
	if (epos > 0)
		tempf[epos] = '\0';
	
	char *tmp = gettempname();
	char *outname = (char*)malloc(strlen(tmp));
	strcpy(outname,tmp);

	//Write Stripped to tempfile
	FILE* pFile = fopen(outname, "wb");

	if (pFile!=NULL) {
		fputs(tempf,pFile);
		fclose(pFile);
	}
	else
	{
		fprintf(stderr, "preprocess Error...\n");
		exit(-1);
	}

	printf("\tStripping comments...\n");
	//stripping comments
	char *tmp2 = gettempname();
	char *outname2 = (char*)malloc(strlen(tmp2));
	strcpy(outname2,tmp2);

	FILE* pTEMP = fopen(outname, "rb");
	if (pTEMP == NULL) {
		fprintf(stderr, "preprocess Error...\n");
		return NULL;
	}

	FILE* pTEMP2 = fopen(outname2, "wb");
	if (pTEMP2 == NULL) {
		fprintf(stderr, "preprocess Error...\n");
		return NULL;
	}

	char line[4096];
	int linenum = 0;
	int cmf = 0;

	while (fgets(line, sizeof(line), pTEMP)) {
		linenum+=1;
		//printf("%5d : %s", linenum, line);
		cmf = strpos(";",line);
		if (cmf > 0) {
			//printf("%5d : %s\n", linenum, SubStr(line, 1, cmf));
			fputs(SubStr(line, 1, cmf),pTEMP2);
			if (cmf != 1)
				fputc('\n',pTEMP2);
		}
		else
		{
			fputs(line,pTEMP2);
			//fputc('\n',pTEMP2);
		}
	}
	fclose(pTEMP);
	FileDelete(outname);
	fclose(pTEMP2);
	
	free(outname);

	//printf("Done.\n");

	return outname2;
}

int paml_def(char *tmpdef, char *bgcolor) {

	FILE* pFile = fopen(tmpdef, "rb");
	char line[4096];
	int linenum = 0;
	int eqpos = 0;
	int defcount = 0;
	char *defname = (char*)malloc(32);
	char *defvalue = (char*)malloc(32);

	printf("\tLooking Up Definitions...\n");

	while (fgets(line, sizeof(line), pFile)) {
		linenum+=1;
		//printf("%dA\n", linenum);
		stripCRLFarr(line);
		//printf("%dB\n", linenum);
		eqpos = strpos("=",line);
		//printf("[%d]:%s",deqpos,dline);
		if (eqpos > 0) {
			//printf("\t- [%d]:%s [OK]", defcount++, line);
			defname = strlower(line);
			defname[eqpos-1] = '\0';
			defvalue = strLtrim(line,eqpos);
			defvalue = str_replaceAll(defvalue,"#","");
			

			//dcolors->name[defcount] = defname;
			//dcolors->value[defcount] = defvalue;
			//paml_def_insert(dcolors->name[defcount], defname);
			//DEF_name[defcount] = defname;
			sprintf(DEF_NAME[defcount],"%s",defname);
			//DEF_NAME[defcount][strlen(defname)] = '\0';
			//printf(" OK1\n");
			//paml_def_insert(dcolors->value[defcount], defvalue);
			//DEF_value[defcount] = defvalue;
			
			if (_OUTTYPE == PAML_HTML)
			{
				if (defvalue == NULL)
					sprintf(DEF_VALUE[defcount],"%s",bgcolor);
				else
					sprintf(DEF_VALUE[defcount],"%s",defvalue);
			}
			else
			{
				if ((!htmlcolorvalid(defvalue)) || (defvalue == NULL))
					sprintf(DEF_VALUE[defcount],"%s",bgcolor);
				else
					sprintf(DEF_VALUE[defcount],"%s",defvalue);
			}
			
			//DEF_VALUE[defcount][strlen(defvalue)] = '\0';
			//printf(" OK2\n");


			printf("\t\t- [%d]:%8s = %s", defcount++, defname, defvalue);
		}
			printf("\n");
	}
	fclose(pFile);
	FileDelete(tmpdef);
	//printf("\n");

	DEF_TOTAL = defcount;
	//printf("%d\n", DEF_TOTAL+1);

	//printf("11[%s]=[%s]\n", DEF_NAME[0], DEF_VALUE[0]);
	//printf("22[%s]=[%s]\n", DEF_NAME[4], DEF_VALUE[4]);
	
	//printf("[%s]=[%s]\n",DEF_NAME[0], paml_def_get(DEF_NAME[0]));
	
	free(defname);
	free(defvalue);
}

char *tag_get(char *tag, char *paml_t) {

	char tag_open[strlen(tag)+3];
	char tag_close[strlen(tag)+4];

	sprintf(tag_open,"<%s>",tag);
	sprintf(tag_close,"</%s>",tag);

	int tag_start = strpos(tag_open,paml_t) + strlen(tag_open);
	int tag_end = strpos(tag_close,paml_t);

	if (tag_end < 0)
	{
		printf("Error: Missing End Tag \"%s\"\n", tag_close);
		exit(-1);
	}
	return SubStr2(paml_t,tag_start,tag_end);
}

char *paml_def_get(char *name, char *bgcolor) {
	
	int i;
	//printf("QQQQ: %d\n", DEF_TOTAL);
	//printf("NAME: \"%s\"\n", name);

	//printf("%s\n", DEF_NAME[4]);
	//printf("%s\n", DEF_VALUE[4]);
	//printf("%d\n", strcmp(DEF_NAME[4],"beige"));
	//printf("%d\n", DEF_TOTAL);

	for (i = 0; i < DEF_TOTAL; ++i)
	{
		//printf("strcmp %d:[%s] == [%s]\n", i, DEF_NAME[i], name);
		//printf("%d_\n", i);
		if (strcmp(DEF_NAME[i],name)==0)
		{
			//printf("found [%s] at %d\n", name, i);
			if (strlen(DEF_VALUE[i])>0)
				return DEF_VALUE[i];
			else
				return bgcolor;
		}
	}
	return bgcolor;
}

char *rgb2bgr(char *irgb) {

	int len = strlen(irgb);
	char rgb[8];
	int i = 0;

	if (len!=6) {
		for (i = 0; i < len; i++) {
			rgb[i] = irgb[i];
		}
		for (i = 0; i < (6-len); i++) {
			rgb[len+i] = irgb[len+i];
		}
		rgb[7] = '\0';
	}
	else
	{
		sprintf(rgb,"%s",irgb);
	}

	char b[] = {rgb[4], rgb[5], '\0'};
	char g[] = {rgb[2], rgb[3], '\0'};
	char r[] = {rgb[0], rgb[1], '\0'};

	//printf("[%s] [%s] [%s]\n", b, g, r);
	char bgr[8];
	char *BGR = (char*)malloc(8);
	sprintf(bgr,"%s%s%s",b,g,r);
	strcpy(BGR,bgr);
	BGR=strupper(BGR);
	return BGR;
}

int rgbstr(char *rgb, int *r, int *g, int *b) {

	//char * BGR = rgb2bgr("123456");
	//int b = str2hex(BGR,0);
	//int g = str2hex(BGR,2);
	//int r = str2hex(BGR,4);

	char *bgr = rgb2bgr(rgb);
	//printf("bgr:%s\n", bgr);
	*b = str2hex(bgr,0);
	*g = str2hex(bgr,2);
	*r = str2hex(bgr,4);
}

int str2hex(char *color, int i) {

	int num;
	char hex[4];
	//printf("_%c%c_\n",color[i], color[i+1]);
	sprintf(hex,"%c%c",color[i], color[i+1]);
	sscanf(hex,"%X",&num);
	//printf("_%d_\n", num);
	return num;
}

int validhex(char *str) {
	
	int i;
	int len = strlen(str);
	if (len == 0)
		return 0;

	for (i = 0; i < len; i++) {
		if (!isxdigit(str[i]))
			return 0;
	}
	return 1;
}

int htmlcolorvalid(char *c) {

	if ((!validhex(c)) && (strcmp(htmlcolorname(c),HTMLCOLOR_DEFAULT)==0))
	{
		return 0;
	}
	return 1;
}

int output_typef(char *t) {

	char *s = strupper(t);
	if (strcmp(s,"BMP")==0) {
		printf("[BMP MODE]\n");
		return PAML_BMP;
	}
	if (strcmp(s,"PNG")==0) {
		printf("[PNG MODE]\n");
		return PAML_PNG;
	}
	if (strcmp(s,"XPM")==0) {
		printf("[XPM MODE]\n");
		return PAML_XPM;
	}
	if (strcmp(s,"HTML")==0) {
		printf("[HTML MODE]\n");
		return PAML_HTML;
	}
	else
	{
		printf("Unknown Mode: \"%s\"\n", t);
		printf("Using DEFAULT Mode: [BMP]\n");
		return PAML_BMP;
	}
}

char *output_type_ext(int t) {

	switch(t) {

		case PAML_BMP: return ".bmp";
		case PAML_PNG: return ".png";
		case PAML_XPM: return ".xpm";
		case PAML_HTML: return ".html";
		default: return ".bmp";
	}
}

char *output_type_name(int t) {

	switch(t) {

		case PAML_BMP: return "BMP";
		case PAML_PNG: return "PNG";
		case PAML_XPM: return "XPM";
		case PAML_HTML: return "HTML";
		default: return "BMP";
	}
}

int yes_no_get() {

	char c = getchar();
	if ((c == '1') || (c == 'Y') || (c == 'y'))
		return 1;
	else
		return 0;
}

int numlen(int x) {

	//(math.floor(math.log10(19))+1)
	char s[10];
	sprintf(s,"%d",x);
	return strlen(s);
}

