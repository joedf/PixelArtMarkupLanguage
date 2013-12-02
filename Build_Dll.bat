@echo off
set build_x86=1
set build_x64=0
set build_res=1
set build_res_x64=0

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


set name=paml
set files=main.c lib\qdbmp\qdbmp.c stringlib.c htmlvars.c fileio.c %console% -DBUILDDLL


if %build_x86%_==0_ goto build_x86_end
	echo compiling %name%_x86.dll...
	gcc %files% %res% --shared -o bin\%name%_x86.dll
:build_x86_end

if %build_x64%_==0_ goto build_x64_end
	echo compiling %name%_x64.dll...
	gcc64 %files% %res_x64% --shared -o bin\%name%_x64.dll
:build_x64_end


:end
echo done.
pause>nul