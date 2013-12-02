#SingleInstance, force
#NoTrayIcon

if 0 != 0
	ExitApp

Show_Notif:
	NotifGUIx:=(A_ScreenWidth-350)/2		
	Gui,NotifGUI:-Border -Caption +AlwaysOnTop +ToolWindow +HwndNotifGUI +LabelNotifGUI
	Gui,NotifGUI:color, 0x1F1F1F
	Gui,NotifGUI:Font, s11, Tahoma
	gui,NotifGUI:Margin, 0, 0
	Gui,NotifGUI:Add, Text, x0 y0 w350 cWhite +Center, Press ESC, F11 or Alt+Enter to Exit FullScreen
	Gui,NotifGUI:Show, Hide NA x%NotifGUIx% y0 w350 h24, NotifGUI
	GroupAdd, Paml_viewer, ahk_id %hWindow%
	WinSet, Transparent, 1, ahk_id %NotifGUI%
	WinFadeIn("ahk_id " . NotifGUI,1000)
	Sleep 2000
	WinFadeClose("ahk_id " . NotifGUI,2000)
	ExitApp
return

NotifGUIescape:
gui, NotifGUI:destroy
ExitApp
return

NotifGUIclose:
gui, NotifGUI:destroy
ExitApp
return

#IfWinActive, ahk_group Paml_viewer
LAlt & Enter::
Esc::
F11::
WinActivate, Paml Viewer
Send {Esc}
ExitApp
return


WinFadeClose(WinTitle="A",Time=2000) {
	WinGet, WinID, ID, %WinTitle%
		WinActivate, Paml Viewer
	WinDelay := A_WinDelay
	SetWinDelay, 5
	WinGet, Trans, Transparent, ahk_id %WinID%
	Trans := Trans ? Trans : 255
	WinSet, ExStyle, +0x00000020, ahk_id %WinID%
	I := (J:=Trans)*10/(Time/=2)
	Loop, % Time/10 {
		Sleep, 10 ; a delay from 1 to 10 is generally equivalent to 10 or 15.6
		;WinActivate, Paml Viewer
		ifWinExist, ahk_id %WinID%
			WinSet, Transparent, % J-=I, ahk_id %WinID%
		else
			break
		;ToolTip % "OUT: " j
	}
	SetWinDelay, %WinDelay%
	WinClose, ahk_id %WinID%
	WinActivate, Paml Viewer
}

WinFadeIn(WinTitle="A",Time=2000,ActivateWinTitle="") {
	WinGet, WinID, ID, %WinTitle%
	WinActivate, Paml Viewer
	WinDelay := A_WinDelay
	SetWinDelay, 5
	WinGet, Trans, Transparent, ahk_id %WinID%
	;Trans := Trans ? Trans : 255
	WinSet, ExStyle, +0x00000020, ahk_id %WinID%
	I := (J:=Trans)*10/(Time/=2)
	Loop, % Time/10 {
		Sleep, 10 ; a delay from 1 to 10 is generally equivalent to 10 or 15.6
		;WinActivate, Paml Viewer
		ifWinExist, ahk_id %WinID%
			WinSet, Transparent, % J+=I*255, ahk_id %WinID%
		else
			break
		;ToolTip % "IN: " j
	}
	SetWinDelay, %WinDelay%
	WinActivate, Paml Viewer
}

/*
Show_Notif(Notif_label="",message="Hello World! :)",w=256,h=24) {
	global hDisplay
	x:=(A_ScreenWidth-w)/2		
	Gui,NotifGUI%Notif_label%:-Border -Caption +AlwaysOnTop +ToolWindow +HwndNotifGUI%Notif_label% +LabelNotifGUI
	Gui,NotifGUI%Notif_label%:color, 0x1F1F1F
	Gui,NotifGUI%Notif_label%:Font, s11, Tahoma
	gui,NotifGUI%Notif_label%:Margin, 0, 0
	Gui,NotifGUI%Notif_label%:Add, Text, x0 y0 w%w% cWhite +Center, %message%
	Gui,NotifGUI%Notif_label%:Show, Hide NA x%x% y0 w%w% h%h%, NotifGUI
	;guiAddBG(4,4,720,56,"2A2A2A","FS_Notif")
	idgui:=NotifGUI%Notif_label%
	WinSet, Transparent, 1, ahk_id %idgui%
	WinFadeIn("ahk_id " . idgui,1000,"ahk_id " . hDisplay)
	Sleep 2000
	WinFadeClose("ahk_id " . idgui,2000,"ahk_id " . hDisplay)
	return
	
	NotifGUIescape:	
	NotifGUIclose:
	gui, NotifGUI%Notif_label%:destroy
	return
}
*/

;eof