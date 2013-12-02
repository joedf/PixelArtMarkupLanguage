;
; AutoHotkey Version: 1.1.10.01 - Tested On 1.1.13.00
; Language:       English
; Platform:       Win9x/NT
; Author:         Joe DF  |  http://joedf.co.nr  |  joedf@live.ca
; Date:           May 27th, 2013
; "mysterous" bugged fixed: 21:50 2013-09-26
;
; Script description:
; 	Paml Viewer - .paml file viewer
; 	Written by Joe DF - Interface for PAML (Pixel Art Markup Language)
;

;[INITIALIZATION] ;{
;-----------------------------------------------------
	#SingleInstance, off
	#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
	#Warn  ; Recommended for catching common errors.
	SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
	SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
	
	viewerversion=1.1
	viewerversionstage=Beta
	
	CurrentDir:=A_ScriptDir
	
	HTML_Editor=Notepad.exe
	
	OnExit, AppExit
	f_run:=1
	tried:=0
	ZoomLevel:=0
	filecount:=0 ;Prevent the "mysterous" bug... :P

	;-------- [DLL get]-------- ;{
		if (!A_IsCompiled)  ;C:\Users\Joachim\Documents\Multimedia\MyPrograms\AutoHotkey\My scripts\PixelArt-lang\Official\C-Version\bin\
			dll_dir=%A_ScriptDir%\..\bin\
		else
			dll_dir=%A_ScriptDir%\
		if (A_PtrSize = 8)
			paml_dll := dll_dir "paml_x64.dll"
		else ; if (A_PtrSize = 4)
			paml_dll := dll_dir "paml_x86.dll"
		
		CheckDLL(paml_dll)
		dllpamlversion:=DllVersion() ;}
	
	;-------- [Argc & Argv get] -------- ;{
		argc=%0%
		argv:=Object() ;initialize the array
		argv[0]:=A_ScriptName
		
		Loop, %0%  ; For each parameter:
			argv[A_Index] := %A_Index%  ; Fetch the contents of the variable whose name is contained in A_Index.
	;-------- [Argc & Argv get] -------- ;}
	
	DefaultHTML =
	( Ltrim Join
		<!DOCTYPE html>
		<html>
		<head>
			<meta name="viewport" content="width=device-width, user-scalable=no" />
			<style>
				*{text-shadow:0px 1px 0px #FFFFFF;font-family:tahoma;}
				div {color: #888888;font-size:24px}
				#title{position:absolute;left:50`%;top:50`%;margin-left:-92px;margin-top:-15px;}
				#note{position:absolute;left:50`%;top:50`%;margin-left:-156px;margin-top:+20px;}
			</style>
		</head>
		<body style="background-color: #dde4ec;">
		<div id="title">Paml Viewer v%viewerversion%</div>
		<div style="position:absolute;left:8px;top:8px;">%viewerversionstage%</div>

		<p id="note">
			<table border="0" cellpadding="0" cellspacing="0">
			<tr>
				<td width="50" valign="top"><b>Note: </b></td>
				<td valign="top">No file was specified to be displayed.</td>
			</tr>
			</table>
		</p>
		</body>
		</html>
	)
	
	SetTimer,ReduceMem,10000
;}-----------------------------------------------------

;[MainChunk] ;{
;-------------------------------------------------------
	out_file=
	paml_file:=argv[1]
	Last_paml_file:=paml_file
	;short_pname:=shorten_path(paml_file)
	;out_file=%A_Temp%\pamlv_temp[%A_now%].html

	if (argc > 0)
	{
		gosub, MainOpen
	}
	else
	{
		short_pname=No file...
		ActiveX_control=Shell.Explorer
		gosub, MakeGUI
		DefaultHTML_path=%A_Temp%\pamlv_temp[%A_now%]_%filecount%.html
		FileAppend,%DefaultHTML%,%DefaultHTML_path%
		Display(DefaultHTML_path)
		;Display.write:=(DefaultHTML)
		
		;Display.body.style.overflow := "hidden" ;toggles Show scrollbars when zooming in...
	}
	return
;}-------------------------------------------------------

;[LABELS] ;{
;-------------------------------------------------------
	MainOpen: ;{
		if paml_file =
			return
		
		SplitPath,paml_file,tmpverifname
		if tmpverifname =
			return
		
		filecount+=1 ;Prevent the "mysterous" bug... :P
		
		short_pname:=shorten_path(paml_file)
		out_file=%A_Temp%\pamlv_temp[%A_now%]_%filecount%.html
		
		SplitPath,paml_file,,CurrentDir
		
		errorcode:=paml_process(paml_file,out_file,"HTML")
		if (!errorcode) ;it was successful...
		{
			;continue...
			ToolTip
			if (f_run)
			{
				ActiveX_control=Shell.Explorer
				gosub, MakeGUI
			}
			Display(out_file)
			PamlMetaData:=getPAML_Meta(out_file)
			SB_SetText("Image Size: " getPAML_imgSize(PamlMetaData),1)
			SB_SetText("Author: " getPAML_Author(PamlMetaData),2)
			WinSetTitle, ahk_id %hWindow%, , Paml Viewer - %short_pname%
			Last_paml_file:=paml_file
		}
		else ;it failed
		{
			if (tried < 3)
			{
				tried+=1
				ToolTip, Loading...
				Sleep, 1000
				goto MainOpen
			}
			MsgBox, 16, Paml Viewer - Error, An error occured...`nThis means "%short_pname%" could not be opened...`n`nError code: %errorcode%
			;MsgBox % paml_file
			tried:=0
			if (f_run)
				ExitApp
		}
	return ;}

	AppExit: ;{
		gosub, cleanUp
		ExitApp
	return ;}

	cleanUp: ;{
	FileDelete,%A_Temp%\pamlv_temp[*
	return ;}

	ReduceMem: ;{
	ReduceMem()
	return ;}
;-------------------------------------------------------
;}

;[HOTKEYS] ;{
;-------------------------------------------------------
	#IfWinActive ahk_group Paml_viewer
	
	WheelUp::
		if short_pname = No file...
			return
		Send {Ctrl Down}{WheelUp}{Ctrl Up}
		;ControlSend,Internet Explorer_Server1,{Ctrl Down}{WheelUp}{Ctrl Up}, ahk_id %hDisplay%
		ZoomLevel-=1
	return

	;#If MouseIsOver("ahk_id " . hDisplay)
	WheelDown::
		if short_pname = No file...
			return
		Send {CtrlDown}{WheelDown}{Ctrl Up}
		;ControlSend,Internet Explorer_Server1,{Ctrl Down}{WheelDown}{Ctrl Up}, ahk_id %hDisplay%
		ZoomLevel+=1
	return

	Down::
	Left::
		gosub, D ;Left Arrow
	return

	Up::
	Right::
		gosub, E ;Right Arrow
	return
	
	Esc::
	if (gui_isNotFS) ;IF FULLSCREEN False
		return
	F11::
	LAlt & Enter::
		goto, C ;Set FullScreen
	return
	
	#IfWinActive
;-------------------------------------------------------
;}

;[FUNCTIONS] ;{
;-------------------------------------------------------
	enableGuiDrag(GuiLabel=1,idv="GuiDrag") {
		global
		local A_w
		local A_h
		WinGetPos,,,A_w,A_h,A
		Gui, %GuiLabel%:Add, Text, x0 y0 w%A_w% h%A_h% v%idv% +BackgroundTrans gGUI_Drag
		return
		
		GUI_Drag:
		PostMessage 0xA1,2  ;-- Goyyah/SKAN trick
		;http://www.autohotkey.com/board/topic/80594-how-to-enable-drag-for-a-gui-without-a-titlebar/#entry60075
		return
	}

	shorten_path(path, length=50) {
		VarSetCapacity(sPath, length * (A_IsUnicode ? 2 : 1) + 1)
		DllCall("shlwapi\PathCompactPathEx", "str", sPath, "str", path, "uint", length, "uint", 0)
		return sPath
	}

	Paml_process(file, ofile, otype) {
		
		global paml_dll
		hModule := DllCall("LoadLibrary", "str", paml_dll)
		tpdir=%A_Temp%
		VarSetCapacity(pamlf, StrLen(file) * (A_IsUnicode ? 2 : 1) + 1)
		VarSetCapacity(outf, StrLen(ofile) * (A_IsUnicode ? 2 : 1) + 1)
		VarSetCapacity(outt, StrLen(otype) * (A_IsUnicode ? 2 : 1) + 1)
		VarSetCapacity(tdir, StrLen(tpdir) * (A_IsUnicode ? 2 : 1) + 1)
		pamlf:=file
		outf:=ofile
		outt:=otype
		;TEMPDIR has been added to avoid UAC Problems
		tdir:=tpdir
		x:=DllCall(paml_dll . "\IPC_paml", "AStr", pamlf, "AStr", outf, "AStr", outt, "AStr", tdir, "cdecl Int")
		DllCall("FreeLibrary","UInt",hModule)
		;if errorlevel
			;MsgBox, 16, Dllcall error, error code: %errorlevel%
		;else
			;MsgBox, 64, Dllcall Success, (IPC_paml) DLL Function call was successful!
		if x=0
			return 0x0000FF
		else
			return errorlevel
	}
	
	DllVersion() {
		global paml_dll
		return DllCall(paml_dll . "\paml_version", "AStr")
	}

	Display(html) {
		
		global Display
		global ActiveX_control
		if ActiveX_control = HTMLFILE
		{
			FileRead,dispTMP,%html%
			Display.Write(dispTMP)
		}
		else ;if ActiveX_control = Shell.Explorer
		{
			Display.Silent := True
			Display.Navigate("file://" . html)
		}
		ReduceMem()
	}
	
	ReduceMem() {
		pid := DllCall("GetCurrentProcessId")
		h := DllCall("OpenProcess", "UInt", 0x001F0FFF, "Int", 0, "Int", pid)
		DllCall("SetProcessWorkingSetSize", "UInt", h, "Int", -1, "Int", -1)
		DllCall("CloseHandle", "Int", h)
	}
	
	CheckDLL(dll) {
		if (!FileExist(dll))
		{
			SplitPath,dll,z
			MsgBox, 16, Paml Viewer - Error, Error missing "%z%" !`nThe application will now exit.
			ExitApp
		}	
	}
	
	FileList(dir) {
		fList =
		Loop %dir%
		   fList = %fList%%A_LoopFileName%`n
		return fList
	}
	
	strGetLine(string, line) {
		
		x:=InStr(string,"`n",false,1,line-1)
		y:=InStr(string,"`n",false,1,line)
		if (x=0 and y=0)
		{
			StringGetPos,a,string,`n,R,1
			return SubStr(string,a+2,(StrLen(string)-a)-2)
		}
		else if (x = 0)
		{
			StringGetPos,a,string,`n
			return SubStr(string,1,a)
		}
		else if (y = 0)
			return SubStr(string,1,InStr(string,"`n",false,1,1)-1)
		else
			return SubStr(string,x+1,(y-x)-1)
	}
	
	FilePrev(file, dir="") {
		if dir =
			dir:=A_WorkingDir
		SplitPath,file,file,,fExt
		dlist:=FileList(dir . "\*." . fExt)
		;MsgBox % dlist
		loop, parse, dlist, `n
		{
			;MsgBox % `"%A_LoopField%`" " = " file
			if (A_LoopField = file)
			{
				return strGetLine(dlist,(A_Index-1))
			}
		}
		return file
	}
	
	FileNext(file, dir="") {
		if dir =
			dir:=A_WorkingDir
		SplitPath,file,file,,fExt
		dlist:=FileList(dir . "\*." . fExt)
		loop, parse, dlist, `n
		{
			if (A_LoopField = file)
			{
				return strGetLine(dlist,(A_Index+1))
			}
		}
		return file
	}
	
	FileFirst(fExt, dir="") {
		if dir =
			dir=%A_WorkingDir%
		dir=%dir%\*.%fExt%
		loop, %dir%
		{
			return A_LoopFileName
		}
	}
	
	getPAML_Meta(html) {
		FileRead,z,%html%
		StringGetPos,a,z,<!--PAML_METADATA
		StringGetPos,b,z,PAML_METADATA-->
		a:=a+18
		b:=b-1
		return SubStr(z,a,(b-a))
	}
	
	getPAML_imgSize(pamlmeta) {
		a:=getPAML_element(pamlmeta, "xpixels")
		x:=getPAML_element(pamlmeta, "sizexpixels")
		b:=getPAML_element(pamlmeta, "ypixels")
		y:=getPAML_element(pamlmeta, "sizeypixels")
		w:=a*x
		h:=b*y
		size=%w%x%h%
		return size
	}
	
	getPAML_Author(pamlmeta) {
		return getPAML_element(pamlmeta, "Author")
	}
	
	getPAML_element(pamlmeta, element) {
		RegExMatch(pamlmeta, element . ": .+",r)
		a:=StrLen(element)+2
		StringTrimLeft,z,r,a
		return z
	}
	
	Display_ResetZoom() {
		
		;controlsend, Internet Explorer_Server1, ^{end}, ahk_id %hWindow%
		#IfWinActive ahk_group Paml_viewer
		global ZoomLevel
		global hWindow
		global hDisplay
		if (ZoomLevel < 0) ;negative [zoomed out]
		{
			i:=abs(ZoomLevel)
			loop, %i%
			{
				Send {Ctrl Down}{WheelDown}{Ctrl Up}
				;ControlSend,Internet Explorer_Server1,{Ctrl Down}{WheelUp}{Ctrl Up}, ahk_id %hWindow%
				ZoomLevel:=ZoomLevel + 1
			}
		}
		else ;positive [zoomed in]
		{
			i:=abs(ZoomLevel)
			loop, %i%
			{
				Send {Ctrl Down}{WheelUp}{Ctrl Up}
				;ControlSend,Internet Explorer_Server1,{Ctrl Down}{WheelUp}{Ctrl Up}, ahk_id %hWindow%
				ZoomLevel:=ZoomLevel - 1
			}
		}
		#IfWinActive
	}
	
	MouseIsOver(WinTitle) {
		MouseGetPos,,, Win
		return WinExist(WinTitle . " ahk_id " . Win)
	}
	
	SoundClick() {
		sound:="C:\Windows\Media\Windows Navigation Start.wav"
		SoundPlay, %sound%
	}
	
	keySleep(time=2000) {
		Loop % time/10
		{
			Sleep 10
			if A_TimeIdle < 10
				break
		}
	}
;-------------------------------------------------------
;}

;[GUI] ;{
;---------------------------------------------------------
	MakeGUI: ;{
	gui_h:=556
	gui_w:=656
	ico_h:=gui_h-60
	
	Gui_ClientOffsetX:=7  ;U   ;7 R
	Gui_ClientOffsetY:=29 ;L   ;8 D
	Gui_ClientOffsetZ:=8       ;= here
	Gui_OffsetX:=8
	Gui_OffsetY:=8
	Gui_OffsetZ:=76
	
	gui_MaxSizeExtra:=100
	gui_MaxSizeW:=A_ScreenWidth+(Gui_OffsetX*2)+gui_MaxSizeExtra
	gui_MaxSizeH:=A_ScreenHeight+Gui_OffsetZ+gui_MaxSizeExtra
	
	gui_isNotFS:=1
	
	Gui +HwndhWindow
	GroupAdd, Paml_viewer, ahk_id %hWindow%
	GUI_Handle=ahk_id %hWindow%
	Gui, +Resize +MinSize372x336 +MaxSize%gui_MaxSizeW%x%gui_MaxSizeH%
	Gui, Color, 0x888888, 0x0
	Gui, Add, ActiveX, % "x8 y8 w" (gui_w-16) " h" (gui_h-76) " vDisplay HwndhDisplay", %ActiveX_control%
	Gui, Add, Picture, % "x8 y" ico_h " w32 h32 Icon7 vA gA", %paml_dll%
	Gui, Add, Picture, % "xp+38 y" ico_h " w32 h32 Icon9 vB gB", %paml_dll%
	Gui, Add, Picture, % "xp+38 y" ico_h " w32 h32 Icon10 vB2 gB2", %paml_dll%
	Gui, Add, Picture, % "x" ((gui_w-32)/2) " y" ico_h "w32 h32 Icon5 vC gC", %paml_dll%
	Gui, Add, Picture, % "xp-38 y" ico_h " w32 h32 Icon3 vD gD", %paml_dll%
	Gui, Add, Picture, % "xp+76 y" ico_h " w32 h32 Icon4 vE gE", %paml_dll%
	Gui, Add, Picture, % "x" (gui_w-40) " y" ico_h " w32 h32 Icon1 vF gF", %paml_dll%

	Gui, Add, StatusBar,, Loading...
	SB_SetParts(128)
	SB_SetText("Image Size: Unknown",1)
	SB_SetText("Author: Unknown",2)
	
	;Gui, Add, Draggerpic
	
	Gui, Show, Center w%gui_w% h%gui_h%, Paml Viewer - %short_pname%
	enableGuiDrag()
	f_run:=0
	WinGetPos,nFS_x,nFS_y,nFS_w,nFS_h,%GUI_HANDLE%
	
	;Display.document.body.style.overflow := "scroll"
	Return
	
	GuiClose: 
	ExitApp	;}

	GuiSize: ;{
	gui_h:=A_GuiHeight
	gui_w:=A_GuiWidth
	ico_h:=gui_h-60
	GuiControl, move, Display, % "x8 y8 w" (gui_w-16) " h" (gui_h-76)
	GuiControl, move, A, % "x8 y" ico_h
	GuiControl, move, B, % "x46 y" ico_h
	GuiControl, move, B2, % "x84 y" ico_h
	GuiControl, move, C, % "x" ((gui_w-32)/2) " y" ico_h
	GuiControl, move, D, % "x" (((gui_w-32)/2)-38) " y" ico_h
	GuiControl, move, E, % "x" (((gui_w-32)/2)+38) " y" ico_h
	GuiControl, move, F, % "x" (gui_w-40) " y" ico_h
	GuiControl, move, GuiDrag, % "w" gui_w " h" gui_h
	return ;}

	GuiDropFiles: ;{
	Loop, parse, A_GuiEvent, `n
	{
		GUIDRAG_FirstFile = %A_LoopField%
		Break
	}
	paml_file:=GUIDRAG_FirstFile
	gosub, MainOpen
	;MsgBox % CurrentDir "`n" FilePrev(paml_file,CurrentDir) "`n" FileNext(paml_file,CurrentDir)
	return ;}
	
	A: ;+/- symbol Real/Fit size Zoom ;{
	;#h::
	;KeyWait, MButton, U
	;Display_ResetZoom() ;Not working very well :P
	MsgBox, 64, Paml Viewer - Real/Fit size Zoom, This is where the "Real/Fit size Zoom" option will be accessed...
	return ;}
	
	B: ;options - wrench icon ;{
	MsgBox, 64, Paml Viewer - Options, This is where the "options GUI" will be accessed...
	return ;}
	
	B2: ;view source icon ;{
	;MsgBox
	if out_file !=
		run, %HTML_Editor% %out_file%
	return ;}
	
	C: ;fullscreen symbol ;{
	if (gui_isNotFS)
	{
		;Save old GUI values
		WinGetPos,nFS_x,nFS_y,nFS_w,nFS_h,%GUI_HANDLE%
		
		;Set New GUI Values
		FS_x:=(0-Gui_OffsetX)-Gui_ClientOffsetX
		FS_y:=(0-Gui_OffsetY)-Gui_ClientOffsetY
		FS_w:=A_ScreenWidth+(Gui_OffsetX*2)+(Gui_ClientOffsetX*2)
		FS_h:=A_ScreenHeight+Gui_ClientOffsetY+Gui_ClientOffsetZ+Gui_OffsetZ
		WinSet,AlwaysOnTop,On,%GUI_Handle%
		WinMove,%GUI_Handle%,,%FS_x%,%FS_y%,%FS_w%,%FS_h%
		gui_isNotFS:=0 ;FULLSCREEN = TRUE
		
		;Show FullScreen Notif
		;Show_Notif("FS_NOTIF","Press ""ESC, F11 or Alt+Enter"" to Exit FullScreen",350)
		if (!A_isCompiled)
			run, %A_scriptdir%\bin\FS_Notify.exe
		else
			run, %A_scriptdir%\FS_Notify.exe
	}
	else
	{
		WinSet,AlwaysOnTop,Off,%GUI_Handle%
		WinMove,%GUI_Handle%,,%nFS_x%,%nFS_y%,%nFS_w%,%nFS_h%
		gui_isNotFS:=1 ;FULLSCREEN = FALSE
		
		if (!A_isCompiled)
			run, %A_scriptdir%\bin\FS_Notify.exe /Off
		else
			run, %A_scriptdir%\FS_Notify.exe /Off
	}
	return ;}
	
	D: ;left arrow ;{
	if paml_file =
		paml_file:=FileFirst("paml",CurrentDir)
	paml_file:=CurrentDir "\" FilePrev(paml_file,CurrentDir)
	if (Last_paml_file = paml_file)
	{
		soundClick()
		return
	}
	;MsgBox % paml_file
	gosub, MainOpen
	return ;}

	E: ;right arrow ;{
	if paml_file =
		paml_file:=FileFirst("paml",CurrentDir)
	paml_file:=CurrentDir "\" FileNext(paml_file,CurrentDir)
	if (Last_paml_file = paml_file)
	{
		soundClick()
		return
	}
	;MsgBox % paml_file
	gosub, MainOpen
	return ;}
	
	F: ;about ;{
	MsgBox, 64, Paml Viewer - About, Paml Viewer v%viewerversion% %viewerversionstage%`nPaml Processor v%dllpamlversion%`n`nThis is FREE software. Selling is allowed but immoral.`nThis software is under the MIT License.`nPaml (Pixel Art Markup Language)`n(c) 2013 Joe DF (joedf@users.sourceforge.net)
	return ;}
;}-------------------------------------------------------------

