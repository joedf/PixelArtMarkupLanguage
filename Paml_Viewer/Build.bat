@echo off
rem 	Build.bat for Paml_Viewer.ahk
rem 	
rem 	Compiler's available options
rem 	Ahk2exe.exe /in MyScript.ahk [/out MyScript.exe][/icon MyIcon.ico][/pass password][/bin AHK_BINFILES][/mpress]
rem 	
rem 	AHK_BINFILES: "AutoHotkeySC.bin", "ANSI 32-bit.bin", "Unicode 32-bit.bin", "Unicode 64-bit.bin"
rem 	Fullpath: 	"C:\Program Files\AutoHotkey\Compiler\Ahk2exe.exe"

rem Set Definitions
	set ResH_Dir=C:\Program Files (x86)\Resource Hacker
	set ResH_Exe=ResHacker.exe
	set ResH="%ResH_Dir%\%ResH_Exe%"
	set goRC="C:\tools\goRC.exe"
	set mpress_exe="C:\tools\mpress.exe"
	set upx_exe="C:\tools\UPX\upx.exe"
	set compilerExe=Ahk2exe.exe
	set compilerDir=C:\Program Files\AutoHotkey\Compiler
	set compiler="%compilerDir%\%compilerExe%"
	set bin_Default="%compilerDir%\AutoHotkeySC.bin"
	set bin_ANSI="%compilerDir%\ANSI 32-bit.bin"
	set bin_U32="%compilerDir%\Unicode 32-bit.bin"
	set bin_U64="%compilerDir%\Unicode 64-bit.bin"
	
rem Set Files and Options
	set script=pamlv_source.ahk
	set exe=bin\PAML_Viewer.exe
	set icon=res\turtle-logo.ico
	set vinfo=res\v_info.rc
	set vinfo_RES=res\v_info.RES

rem Code [START]
echo MAKE SURE! mpress is set to off in AHK2Exe
echo -----------------------------------------------
echo.
echo Compiling "%script%" to "%exe%" ...
echo Using %bin_U32%
%compiler% /in "%script%" /out "%exe%" /icon "%icon%" /bin %bin_U32%
%compiler% /in "FS_Notify.ahk" /out "bin\FS_Notify.exe" /icon "res\FS_Notify.ico" /bin %bin_ANSI%

echo Adding Version Information...
%goRC% /r %vinfo% >NUL
rem Write ResHacker Script, to add Version info
set ResH_script="%cd%\ChangeVersionInfo.script"
	>>%ResH_script% echo [FILENAMES]
	>>%ResH_script% echo Exe=^"%cd%\%exe%^"
	>>%ResH_script% echo SaveAs=^"%cd%\%exe%^"
	>>%ResH_script% echo Log=^"%cd%\ResH.log^"
	>>%ResH_script% echo [COMMANDS]
	>>%ResH_script% echo -delete Versioninfo , 1 , 1033
	>>%ResH_script% echo -addoverwrite ^"%cd%\%vinfo_RES%^" , Versioninfo , 1 , 
%ResH% -script %ResH_script% >NUL
del "%cd%\ResH.log"
del %ResH_script%
del "%cd%\%vinfo_RES%"
echo Compressing Exe with Mpress...
%mpress_exe% "%cd%\%exe%" >NUL
%mpress_exe% "bin\FS_Notify.exe" >NUL
rem echo Compressing Exe with UPX...
rem %upx_exe% "%cd%\%exe%" >NUL

echo done. &pause>NUL
