@echo off
set build_x86=1
set build_x64=0
set build_dll=0
set build_res=1
set build_res_x64=0
set show_console=1

if %build_dll%_==0_ goto build_dll_end

	set name=strlib
	set files=stringlib.c

	echo compiling %name%.dll...
	gcc %files% -DBUILD_DLL -shared -o bin\%name%.dll
	echo compiling %name%_x64.dll...
	gcc64 %files% -DBUILD_DLL -shared -o bin\%name%_x64.dll

	echo compiling strlib_tester.exe...
	gcc strlib_tester.c bin\%name%.dll -o bin\strlib_tester.exe
	echo compiling strlib_tester_x64.exe...
	gcc64 strlib_tester.c bin\%name%_x64.dll -o bin\strlib_tester_x64.exe

:build_dll_end

if %build_res%_==0_ goto build_res_end
	echo compiling x86 resources...
	set res=res\res.o
	windres res\resources.rc -o %res%
:build_res_end

if %build_res_x64%_==0_ goto build_res_x64_end
	echo compiling x64 resources...
	set res_x64=res\res_x64.o
	windres64 res\resources.rc -o %res_x64%
:build_res_x64_end

if %show_console%_==0_ set console=-mwindows

set name=main
set files=main.c lib\qdbmp\qdbmp.c stringlib.c htmlvars.c fileio.c %console%

if %build_x86%_==0_ goto build_x86_end
	echo compiling %name%.exe...
	gcc %files% %res% -o bin\%name%.exe
:build_x86_end


if %build_x64%_==0_ goto build_x64_end
	echo compiling %name%_x64.exe...
	gcc64 %files% %res_x64% -o bin\%name%_x64.exe
:build_x64_end

:end
echo done.
pause>nul