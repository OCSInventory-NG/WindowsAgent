################################################################################
## Open Computer and Software Inventory Next Generation
## Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
## Web: http://www.ocsinventory-ng.org
##
## This code is open source and may be copied and modified as long as the source
## code is always made freely available.
## Please refer to the General Public Licence http://www.gnu.org/ or Licence.txt
################################################################################

setcompressor /SOLID lzma

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "OCS Inventory NG Agent"
!define PRODUCT_VERSION "2.0.0.14"
!define PRODUCT_PUBLISHER "OCS Inventory NG Team"
!define PRODUCT_WEB_SITE "http://www.ocsinventory-ng.org"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\OCSInventory.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_SERVICE_NAME "OCS Inventory Service"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "WinVer.nsh"
!insertmacro GetTime
!insertmacro WordReplace
;!insertmacro un.GetParent
;!insertmacro GetParent
; MUI 1.67 compatible ------
!include "MUI.nsh"
ICON "install-ocs.ico"
; MUI Settings
!define MUI_HEADERIMAGE
!define MUI_WELCOMEPAGE_TITLE_3LINES
;!define MUI_HEADERIMAGE_BITMAP "banner-ocs.bmp"
!define MUI_HEADERIMAGE_BITMAP "OCS-NG-48.bmp" ; optional
!define MUI_AbortWARNING
!define MUI_ICON "install-ocs.ico"
!define MUI_UNICON "uninstall-ocs.ico"
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\license.txt"
; Server, proxy and agent option pages
Page custom AskServerOptions ValidateServerOptions ""
Page custom AskProxyOptions ValidateProxyOptions ""
Page custom AskAgentOptions ValidateAgentOptions ""
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; View before start page
!insertmacro MUI_PAGE_INSTFILES
; Finshed page
!insertmacro MUI_PAGE_FINISH
; Confirl before uninstall
!insertmacro MUI_UNPAGE_INSTFILES
; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; Setup log file
!define SETUP_LOG_FILE "$exedir\OCS-NG-Windows-Agent-Setup.log"
!define Service_Time_Out "10"


; MUI end ------
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Setup ${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "${PRODUCT_WEB_SITE}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Distributed under GNU GPL Version 2 Licence"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"

BRANDINGTEXT "OCS Inventory NG"
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "OCS-NG-Windows-Agent-Setup.exe"
InstallDir "$PROGRAMFILES\OCS Inventory Agent"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowUnInstDetails show


#####################################################################
# Global variables
#####################################################################
var /GLOBAL logBuffer ; To complete the setup log file
var /GLOBAL OcsService ; To store if service was previously installed (TRUE) or not (FALSE)
var /GLOBAL OcsNoSplash ; To store if setup must display spash screen (FALSE) or not (TRUE)
var /GLOBAL OcsUpgrade ; To store if /UPGRADE option used (TRUE) or not (FALSE)
var /GLOBAL installSatus ; To store installation status
  
#####################################################################
# GetParameters
# input, none
# output, top of stack (replaces, with e.g. whatever)
# modifies no other variables.
#####################################################################
Function GetParameters
	Push $R0
	Push $R1
	Push $R2
	Push $R3
	StrCpy $R2 1
	StrLen $R3 $CMDLINE
	;Check for quote or space
	StrCpy $R0 $CMDLINE $R2
	StrCmp $R0 '"' 0 +3
	StrCpy $R1 '"'
	Goto loop
	StrCpy $R1 " "
loop:
	IntOp $R2 $R2 + 1
	StrCpy $R0 $CMDLINE 1 $R2
	StrCmp $R0 $R1 get
	StrCmp $R2 $R3 get
	Goto loop
get:
	IntOp $R2 $R2 + 1
	StrCpy $R0 $CMDLINE 1 $R2
	StrCmp $R0 " " get
	StrCpy $R0 $CMDLINE "" $R2
	Pop $R3
	Pop $R2
	Pop $R1
	Exch $R0
FunctionEnd

#####################################################################
# GetParameterValue
# Chris Morgan<cmorgan@alum.wpi.edu> 5/10/2004
# -Updated 4/7/2005 to add support for retrieving a command line switch
#  and additional documentation
#
# Searches the command line input, retrieved using GetParameters, for the
# value of an option given the option name.  If no option is found the
# default value is placed on the top of the stack upon function return.
#
# This function can also be used to detect the existence of just a
# command line switch like /OUTPUT  Pass the default and "/OUTPUT"
# on the stack like normal.  An empty return string "" will indicate
# that the switch was found, the default value indicates that
# neither a parameter or switch was found.
#
# Inputs - Top of stack is default if parameter isn't found,
#  second in stack is parameter to search for, ex. "/OUTPUT="
# Outputs - Top of the stack contains the value of this parameter
#  So if the command line contained /OUTPUT=somedirectory, "somedirectory"
#  will be on the top of the stack when this function returns
#
# USAGE:
#  Push "/OUTPUT="       ; push the search string onto the stack
#  Push "DefaultValue"   ; push a default value onto the stack
#  Call GetParameterValue
#  Pop $2
#  MessageBox MB_OK "Value of OUTPUT parameter is '$2'"
#####################################################################
Function GetParameterValue
	Exch $R0  ; get the top of the stack(default parameter) into R0
	Exch      ; exchange the top of the stack(default) with
			; the second in the stack(parameter to search for)
	Exch $R1  ; get the top of the stack(search parameter) into $R1

	;Preserve on the stack the registers used in this function
	Push $R2
	Push $R3
	Push $R4
	Push $R5

	Strlen $R2 $R1      ; store the length of the search string into R2

	Call GetParameters  ; get the command line parameters
	Pop $R3             ; store the command line string in R3

	# search for quoted search string
	StrCpy $R5 '"'      ; later on we want to search for a open quote
	Push $R3            ; push the 'search in' string onto the stack
	Push '"$R1'         ; push the 'search for'
	Call StrStr         ; search for the quoted parameter value
	Pop $R4
	StrCpy $R4 $R4 "" 1   ; skip over open quote character, "" means no maxlen
	StrCmp $R4 "" "" next ; if we didn't find an empty string go to next

	# search for non-quoted search string
	StrCpy $R5 ' '      ; later on we want to search for a space since we
					  ; didn't start with an open quote '"' we shouldn't
					  ; look for a close quote '"'
	Push $R3            ; push the command line back on the stack for searching
	Push '$R1'          ; search for the non-quoted search string
	Call StrStr
	Pop $R4

	; $R4 now contains the parameter string starting at the search string,
	; if it was found
next:
	StrCmp $R4 "" check_for_switch ; if we didn't find anything then look for
								 ; usage as a command line switch
	# copy the value after $R1 by using StrCpy with an offset of $R2,
	# the length of 'OUTPUT'
	StrCpy $R0 $R4 "" $R2  ; copy commandline text beyond parameter into $R0
	# search for the next parameter so we can trim this extra text off
	Push $R0
	Push $R5            ; search for either the first space ' ', or the first
					  ; quote '"'
					  ; if we found '"/output' then we want to find the
					  ; ending ", as in '"/output=somevalue"'
					  ; if we found '/output' then we want to find the first
					  ; space after '/output=somevalue'
	Call StrStr         ; search for the next parameter
	Pop $R4
	StrCmp $R4 "" done  ; if 'somevalue' is missing, we are done
	StrLen $R4 $R4      ; get the length of 'somevalue' so we can copy this
					  ; text into our output buffer
	StrCpy $R0 $R0 -$R4 ; using the length of the string beyond the value,
					  ; copy only the value into $R0
	goto done           ; if we are in the parameter retrieval path skip over
					  ; the check for a command line switch

	; See if the parameter was specified as a command line switch, like '/output'
check_for_switch:
	Push $R3            ; push the command line back on the stack for searching
	Push '$R1'         ; search for the non-quoted search string
	Call StrStr
	Pop $R4
	StrCmp $R4 "" done  ; if we didn't find anything then use the default
	StrCpy $R0 ""       ; otherwise copy in an empty string since we found the
					  ; parameter, just didn't find a value

done:
	Pop $R5
	Pop $R4
	Pop $R3
	Pop $R2
	Pop $R1
	Exch $R0 ; put the value in $R0 at the top of the stack
FunctionEnd

#####################################################################
# This function try to find a string in another one
# Case insensitive
#####################################################################
Function StrStr
	Exch $R1 ; st=haystack,old$R1, $R1=needle
	Exch    ; st=old$R1,haystack
	Exch $R2 ; st=old$R1,old$R2, $R2=haystack
	Push $R3
	Push $R4
	Push $R5
	StrLen $R3 $R1
	StrCpy $R4 0
	; $R1=needle
	; $R2=haystack
	; $R3=len(needle)
	; $R4=cnt
	; $R5=tmp
loop:
	StrCpy $R5 $R2 $R3 $R4
	StrCmp $R5 $R1 done
	StrCmp $R5 "" done
	IntOp $R4 $R4 + 1
	Goto loop
done:
	StrCpy $R1 $R2 "" $R4
	Pop $R5
	Pop $R4
	Pop $R3
	Pop $R2
	Exch $R1
FunctionEnd

#####################################################################
# This function try to find a string in another one, when uninstalling
# Case insensitive
#####################################################################
Function un.StrStr
	Exch $R1 ; st=haystack,old$R1, $R1=needle
	Exch    ; st=old$R1,haystack
	Exch $R2 ; st=old$R1,old$R2, $R2=haystack
	Push $R3
	Push $R4
	Push $R5
	StrLen $R3 $R1
	StrCpy $R4 0
	; $R1=needle
	; $R2=haystack
	; $R3=len(needle)
	; $R4=cnt
	; $R5=tmp
loop:
	StrCpy $R5 $R2 $R3 $R4
	StrCmp $R5 $R1 done
	StrCmp $R5 "" done
	IntOp $R4 $R4 + 1
	Goto loop
done:
	StrCpy $R1 $R2 "" $R4
	Pop $R5
	Pop $R4
	Pop $R3
	Pop $R2
	Exch $R1
FunctionEnd

#####################################################################
# This function write content of logBuffer variable in log file in
# a log file OcsAgentSetup.log located in install directory
#####################################################################
Function Write_Log
	; Save used register
	Push $R0
	ClearErrors
	strcpy $R0 $logBuffer -2
	Detailprint $R0
	; Is there something to write ?
	StrCmp $logBuffer "" WriteLog_end
	; Open log file
	FileOpen $R0 ${SETUP_LOG_FILE} a
	; Seek to end
	FileSeek $R0 END END
	IfErrors WriteLog_end
	; Write
	FileWrite $R0 "$logBuffer"
	StrCpy $logBuffer ""
	; Close file
	FileClose $R0
WriteLog_end:
	; Restore used register
	IfSilent 0 +2
	SLEEP 500
	Pop $R0
FunctionEnd

Function un.Write_Log
	; Save used register
	Push $R0
	ClearErrors
	; Is there something to write ?
	StrCmp $logBuffer "" WriteLog_end
	; Open log file
	FileOpen $R0 "$INSTDIR\uninst.log" a
	; Seek to end
	FileSeek $R0 END END
	IfErrors WriteLog_end
	; Write
	FileWrite $R0 "$logBuffer"
	StrCpy $logBuffer ""
	; Close file
	FileClose $R0
WriteLog_end:
	; Restore used register
	Pop $R0
FunctionEnd

#####################################################################
# This function parse command line arguments
#####################################################################
Function ParseCmd
	; Save used registers
	Push $R0
	Push $R1
	; Get command line paramaters
	Call GetParameters
	Pop $9

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; SERVER PROPERTIES
	; Server address
	Push "/SERVER="          ; push the search string onto the stack
	Push "http://ocsinventory-ng/ocsinventory"   ; push a default value onto the stack
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\server.ini" "Field 7" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/SERVER=$R0" "" "+" $R1
	StrCpy $9 $R1
	; Server credentials
	Push "/USER="          ; push the search string onto the stack
	Push ""               ; No user by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\server.ini" "Field 8" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/USER=$R0" "" "+" $R1
	StrCpy $9 $R1
	Push "/PWD="          ; push the search string onto the stack
	Push ""              ; No password by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\server.ini" "Field 9" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PWD=$R0" "" "+" $R1
	StrCpy $9 $R1
	; Certificate validation
	Push "/SSL="          ; push the search string onto the stack
	Push "1"             ; Certificate validation is required
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\server.ini" "Field 10" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/SSL=$R0" "" "+" $R1
	StrCpy $9 $R1
	Push "/CA="          ; push the search string onto the stack
	Push "cacert.pem"   ; Use CA Certificate vinto agent's folder by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\server.ini" "Field 11" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/CA=$R0" "" "+" $R1
	StrCpy $9 $R1
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; PROXY PROPERTIES
	; Proxy type
	Push "/PROXY_TYPE="      ; push the search string onto the stack
	Push "0"                 ; No proxy by default
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_HTTP_Proxy
	StrCmp "$R0" "2" ParseCmd_Socks4_Proxy
	StrCmp "$R0" "3" ParseCmd_Socks5_Proxy
	; No proxy
	StrCpy $0 "None"
	goto ParseCmd_End_Proxy
ParseCmd_HTTP_Proxy:
	; HTTP proxy
	StrCpy $0 "HTTP"
	goto ParseCmd_End_Proxy
ParseCmd_Socks4_Proxy:
	; Socks 4 proxy
	StrCpy $0 "Socks 4"
	goto ParseCmd_End_Proxy
ParseCmd_Socks5_Proxy:
	; Socks 5 proxy
	StrCpy $0 "Socks 5"
	goto ParseCmd_End_Proxy
ParseCmd_End_Proxy:
	WriteINIStr "$PLUGINSDIR\proxy.ini" "Field 7" "State" "$0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PROXY_TYPE=$R0" "" "+" $R1
	StrCpy $9 $R1
	; Proxy address
	Push "/PROXY="          ; push the search string onto the stack
	Push ""                 ; No proxy address by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\proxy.ini" "Field 8" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PROXY=$R0" "" "+" $R1
	StrCpy $9 $R1
	; Proxy port
	Push "/PROXY_PORT="     ; push the search string onto the stack
	Push ""                 ; No proxy address by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\proxy.ini" "Field 9" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PROXY_PORT=$R0" "" "+" $R1
	StrCpy $9 $R1
	; Proxy credentials
	Push "/PROXY_USER="          ; push the search string onto the stack
	Push ""               ; No user by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\proxy.ini" "Field 10" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PROXY_USER=$R0" "" "+" $R1
	StrCpy $9 $R1
	Push "/PROXY_PWD="          ; push the search string onto the stack
	Push ""              ; No password by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\proxy.ini" "Field 11" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/PROXY_PWD=$R0" "" "+" $R1
	StrCpy $9 $R1
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; AGENT AND SETUP PROPERTIES
	; Verbose log
	Push "/DEBUG="            ; push the search string onto the stack
	Push "0"                  ; No verbose by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 4" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/DEBUG=$R0" "" "+" $R1
	StrCpy $9 $R1
	; No TAG
	Push "/NOTAG" ; push the search string onto the stack
	Push "1"      ; No verbose by default
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_NO_NOTAG
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 5" "State" "1"
	goto ParseCmd_NOTAG_End
ParseCmd_NO_NOTAG:
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 5" "State" "0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/NOTAG" "" "+" $R1
	StrCpy $9 $R1
ParseCmd_NOTAG_End:
	; /TAG="value"
	Push "/TAG=" ; push the search string onto the stack
	Push ""      ; No verbose by default
	Call GetParameterValue
	Pop $R0
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 6" "State" "$R0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/TAG=$R0" "" "+" $R1
	StrCpy $9 $R1
	; No service
	Push "/NO_SERVICE" ; push the search string onto the stack
	Push "1"      ; No verbose by default
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_SERVICE
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 7" "State" "1"
	goto ParseCmd_SERVICE_End
ParseCmd_SERVICE:
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 7" "State" "0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/NO_SERVICE" "" "+" $R1
	StrCpy $9 $R1
ParseCmd_SERVICE_End:
	; No systray
	Push "/NO_SYSTRAY" ; push the search string onto the stack
	Push "1"      ; No verbose by default
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_SYSTRAY
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 8" "State" "1"
	goto ParseCmd_SYSTRAY_End
ParseCmd_SYSTRAY:
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 8" "State" "0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/NO_SYSTRAY" "" "+" $R1
	StrCpy $9 $R1
ParseCmd_SYSTRAY_End:
	; Launch immediate inventory
	Push "/NOW"             ; push the search string onto the stack
	Push "1"                ; Do not launch by default
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_NOW
	; Do not launch
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 9" "State" "1"
	Goto ParseCmd_NOW_end
ParseCmd_NOW:
	; Launch now
	WriteINIStr "$PLUGINSDIR\agent.ini" "Field 9" "State" "0"
	; Remove parsed arg from command line
	${WordReplace} "$9" "/NOW" "" "+" $R1
	StrCpy $9 $R1
ParseCmd_NOW_end:
	; Silent install option
	Push "$9"
	Push "/S"
	Call StrStr
	Pop $R0
	StrCpy $R1 $R0 3
	StrCmp "$R1" "/s" ParseCmd_Silent
	StrCmp "$R1" "/S " ParseCmd_Silent
	; Use normal interactive setup
	SetSilent normal
	Goto ParseCmd_Silent_end
ParseCmd_Silent:
	; Use silent setup
	SetSilent silent
	; Remove parsed arg from command line
	${WordReplace} "$9" "/S" "" "+" $R1
	StrCpy $9 $R1
ParseCmd_Silent_end:
	; No splash option
	Push "/NOSPLASH"        ; push the search string onto the stack
	Push "1"                ; push a default value onto the stack
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_Splash
	; Do not display spash screen
	StrCpy $OcsNoSplash "TRUE"
	Goto ParseCmd_Splash_end
ParseCmd_Splash:
	; Display spash screen
	StrCpy $OcsNoSplash "FALSE"
ParseCmd_Splash_end:
	; Remove parsed arg from command line
	${WordReplace} "$9" "/NOSPLASH" "" "+" $R1
	StrCpy $9 $R1
	; /UPGRADE switch to set deployment status
	Push "/UPGRADE"         ; push the search string onto the stack
	Push "1"                ; push a default value onto the stack
	Call GetParameterValue
	Pop $R0
	StrCmp "$R0" "1" ParseCmd_NoUpgrade
	; Write deployement status file
	StrCpy $OcsUpgrade "TRUE"
	Goto ParseCmd_Upgrade_end
ParseCmd_NoUpgrade:
	; Do not write deployement status file
	StrCpy $OcsUpgrade "FALSE"
ParseCmd_Upgrade_end:
	; Remove parsed arg from command line
	${WordReplace} "$9" "/UPGRADE" "" "+" $R1
	StrCpy $9 $R1
	; Restore used registers
	Pop $R1
	Pop $R0
FunctionEnd

#####################################################################
# This function try to stop service when installing/upgrading
# Service Key to stop is passed into top of stack
#####################################################################
Function StopService
	; Get service key
    Pop $9
	; Save used register
	Push $R0
	StrCmp $OcsService "TRUE" 0 stop_service_end_loop
	; Check service status
	StrCpy $logBuffer "Is Service <$9> running..."
	Call Write_Log
	services::IsServiceRunning "$9"
	pop $0
	StrCpy $logBuffer "$0$\r$\n"
	Call Write_Log
	; Stop service
	StrCpy $logBuffer "Trying to stop Service <$9>..."
	Call Write_Log
	services::SendServiceCommand "stop" "$9" ; This command dies silently on Win9x
	pop $0
	StrCpy $logBuffer "$0$\r$\n"
	Call Write_Log
	strcpy $1 0
stop_service_loop:
	intop $1 $1 + 1
	strcmp ${Service_Time_Out} $1 stop_service_time_out_reached
	sleep 950
	StrCpy $logBuffer "Is Service <$9> running..."
	Call Write_Log
	services::IsServiceRunning "$9"
	pop $0
	StrCpy $logBuffer "$0 - Waiting 1 second(s) for Service to stop...$\r$\n"
	Call Write_Log
	strcmp $0 "YES" stop_service_loop stop_service_end_loop
stop_service_time_out_reached:
	StrCpy $logBuffer "Error time out reached while waiting for service stop!$\r$\n"
	Call Write_Log
	StrCpy $logBuffer "Will try to kill processes..$\r$\n"
	Call Write_Log
stop_service_end_loop:
	; KillProcDLL ©2003 by DITMan, based upon the KILL_PROC_BY_NAME function programmed by Ravi, reach him at: http://www.physiology.wisc.edu/ravi/
	;* 0 = Process was successfully Trying to kill process d
	;* 603 = Process was not currently running
	;* 604 = No permission to Trying to kill process  process
	;* 605 = Unable to load PSAPI.DLL
	;* 602 = Unable to Trying to kill process  process for some other reason
	;* 606 = Unable to identify system type
	;* 607 = Unsupported OS
	;* 632 = Invalid process name
	;* 700 = Unable to get procedure address from PSAPI.DLL
	;* 701 = Unable to get process list, EnumProcesses failed
	;* 702 = Unable to load KERNEL32.DLL
	;* 703 = Unable to get procedure address from KERNEL32.DLL
	;* 704 = CreateToolhelp32Snapshot failed
	sleep 1000
	KillProcDLL::KillProc "OcsSystray.exe"
	StrCpy $logBuffer "Trying to kill process OcsSystray.exe...Result: $R0$\r$\n"
	Call Write_Log
	; If OcsSystray killed, perhaps there is another process running under another user session,
	; So try to kill OcsSystray.exe until there no process or error detected
	StrCmp "$R0" "0" stop_service_end_loop
	sleep 1000
	KillProcDLL::KillProc "OcsService.exe"
	StrCpy $logBuffer "Trying to kill process OcsService.exe...Result: $R0$\r$\n"
	Call Write_Log
	sleep 1000
	KillProcDLL::KillProc "OCSInventory.exe" ; $R0
	StrCpy $logBuffer "Trying to kill process OCSInventory.exe...Result: $R0$\r$\n"
	Call Write_Log
	sleep 1000
	KillProcDLL::KillProc "download.exe" ;$R0
	StrCpy $logBuffer "Trying to kill process download.exe...Result: $R0$\r$\n"
	Call Write_Log
	sleep 1000
	; For old agent using inst32.exe
	KillProcDLL::KillProc "inst32.exe"
	StrCpy $logBuffer "Trying to kill process inst32.exe...Result: $R0$\r$\n"
	Call Write_Log
	sleep 1000
	StrCpy $logBuffer "Waiting 10 seconds for processes to terminate...$\r$\n"
	Call Write_Log
	sleep 10000
	; Restore used register
	Pop $R0
FunctionEnd

#####################################################################
# This function try to stop service when uninstalling
#####################################################################
Function un.StopService
	; Save used register
	Push $R0
	; Check service status
	StrCpy $logBuffer "Is ${PRODUCT_SERVICE_NAME} running..."
	Call un.Write_Log
	services::IsServiceRunning "${PRODUCT_SERVICE_NAME}"
	pop $0
	StrCpy $logBuffer "$0$\r$\n"
	Call un.Write_Log
	; Stop service
	StrCpy $logBuffer "Trying to stop ${PRODUCT_SERVICE_NAME}..."
	Call un.Write_Log
	services::SendServiceCommand "stop" "${PRODUCT_SERVICE_NAME}" ; This command dies silently on Win9x
	pop $0
	StrCpy $logBuffer "$0$\r$\n"
	Call un.Write_Log
	strcpy $1 0
un.stop_service_loop:
	intop $1 $1 + 1
	strcmp ${Service_Time_Out} $1 un.stop_service_time_out_reached 0
	sleep 950
	StrCpy $logBuffer "Is ${PRODUCT_SERVICE_NAME} running..."
	Call un.Write_Log
	services::IsServiceRunning "${PRODUCT_SERVICE_NAME}"
	pop $0
	StrCpy $logBuffer "$0 - waiting 1 second(s) for service to stop...$\r$\n"
	Call un.Write_Log
	strcmp $0 "YES" un.stop_service_loop un.stop_service_end_loop
un.stop_service_time_out_reached:
	StrCpy $logBuffer "Error time out reached while waiting for service stop!$\r$\n"
	Call un.Write_Log
	StrCpy $logBuffer "Will try to kill processes..$\r$\n"
	Call un.Write_Log
un.stop_service_end_loop:
	; KillProcDLL ©2003 by DITMan, based upon the KILL_PROC_BY_NAME function programmed by Ravi, reach him at: http://www.physiology.wisc.edu/ravi/
	;* 0 = Process was successfully Trying to kill process d
	;* 603 = Process was not currently running
	;* 604 = No permission to Trying to kill process  process
	;* 605 = Unable to load PSAPI.DLL
	;* 602 = Unable to Trying to kill process  process for some other reason
	;* 606 = Unable to identify system type
	;* 607 = Unsupported OS
	;* 632 = Invalid process name
	;* 700 = Unable to get procedure address from PSAPI.DLL
	;* 701 = Unable to get process list, EnumProcesses failed
	;* 702 = Unable to load KERNEL32.DLL
	;* 703 = Unable to get procedure address from KERNEL32.DLL
	;* 704 = CreateToolhelp32Snapshot failed
	sleep 1000
	KillProcDLL::KillProc "OcsSystray.exe"
	StrCpy $logBuffer "Trying to kill process OcsSystray.exe...Result: $R0$\r$\n"
	Call un.Write_Log
	; If OcsSystray killed, perhaps there is another process running under another user session,
	; So try to kill OcsSystray.exe until there no process or error detected
	StrCmp "$R0" "0" un.stop_service_end_loop
	sleep 1000
	KillProcDLL::KillProc "OcsService.exe"
	StrCpy $logBuffer "Trying to kill process OcsService.exe...Result: $R0$\r$\n"
	Call un.Write_Log
	sleep 1000
	KillProcDLL::KillProc "OCSInventory.exe" ; $R0
	StrCpy $logBuffer "Trying to kill process OCSInventory.exe...Result: $R0$\r$\n"
	Call un.Write_Log
	sleep 1000
	KillProcDLL::KillProc "download.exe" ;$R0
	StrCpy $logBuffer "Trying to kill process download.exe...Result: $R0$\r$\n"
	Call un.Write_Log
	sleep 1000
	StrCpy $logBuffer "Waiting 10 seconds for processes to terminate...$\r$\n"
	Call un.Write_Log
	sleep 10000
	; Restore used register
	Pop $R0
FunctionEnd

#####################################################################
# This function install service if needed, and start it
# Uses OcsService variable initialized in TestInstall function
#####################################################################
Function StartService
	; Save used register
	Push $R0
	; Check if NT service is required
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 7" "State"
	StrCmp $R0 "1" StartService_Skip_Service
	; check if NT service was previously installed
	StrCmp "$OcsService" "TRUE" StartService_Skip_Install
	; NT service not installed, first install it
	StrCpy $logBuffer "Registering ${PRODUCT_SERVICE_NAME} into Windows Service Manager...$\r$\n"
	Call Write_Log
	nsExec::ExecToLog "$INSTDIR\OcsService.exe -install" $R0
StartService_Skip_Install:
	; Start NT service
	StrCpy $logBuffer "Starting ${PRODUCT_SERVICE_NAME}..."
	Call Write_Log
	services::SendServiceCommand "start" "${PRODUCT_SERVICE_NAME}" ; This command dies silently on Win9x
	Pop $R0
	StrCpy $logBuffer "$R0$\r$\n"
	Call Write_Log
	goto StartService_End
StartService_Skip_Service:
	StrCpy $logBuffer "Registering ${PRODUCT_SERVICE_NAME} into Windows Service Manager skipped...$\r$\n"
	Call Write_Log
StartService_End:
	Pop $R0
FunctionEnd

#####################################################################
# This function try to find if logged in user has admin rights
#####################################################################
Function IsUserAdmin
	Push $R0
	Push $R1
	Push $R2
	ClearErrors
	UserInfo::GetName
	IfErrors IsUserAdmin_Win9x
	; Assuming Windows NT
	Pop $R1
	UserInfo::GetAccountType
	Pop $R2
	StrCmp $R2 "Admin" 0 IsUserAdmin_Continue
	; Observation: I get here when running Win98SE. (Lilla)
	; The functions UserInfo.dll looks for are there on Win98 too,
	; but just don't work. So UserInfo.dll, knowing that admin isn't required
	; on Win98, returns admin anyway. (per kichik)
	; MessageBox MB_OK 'User "$R1" is in the Administrators group'
	StrCpy $R0 "true"
	Goto IsUserAdmin_end
IsUserAdmin_Continue:
	; You should still check for an empty string because the functions
	; UserInfo.dll looks for may not be present on Windows 95. (per kichik)
	StrCmp $R2 "" IsUserAdmin_Win9x
	StrCpy $R0 "false"
	;MessageBox MB_OK 'User "$R1" is in the "$R2" group'
	Goto IsUserAdmin_end
IsUserAdmin_Win9x:
	; comment/message below is by UserInfo.nsi author:
	; This one means you don't need to care about admin or
	; not admin because Windows 9x doesn't either
	;MessageBox MB_OK "Error! This DLL can't run under Windows 9x!"
	StrCpy $R0 "true"
IsUserAdmin_end:
	;MessageBox MB_OK 'User= "$R1"  AccountType= "$R2"  IsUserAdmin= "$R0"'
	Pop $R2
	Pop $R1
	Exch $R0
FunctionEnd

#####################################################################
# This function checks if logged in user has admin rights and if
# service was previously installed
#####################################################################
Function TestInstall
	; Save used register
	Push $R0
	; Does service exist ?
	StrCpy $logBuffer "Trying to determine if service was previously installed..."
	Call Write_Log
	ClearErrors
	Services::IsServiceInstalled "${PRODUCT_SERVICE_NAME}"
	Pop $R0
    StrCmp "$R0" "No" TestInstall_No_Service
    ; Services is installed
	StrCpy $OcsService "TRUE"
	StrCpy $logBuffer "Yes$\r$\n"
	Call Write_Log
	Goto TestInstall_Kill_Process
TestInstall_No_Service:
	StrCpy $OcsService "FALSE"
	StrCpy $logBuffer "No$\r$\n"
	Call Write_Log
TestInstall_Kill_Process:
	; If yes, stop it then kill processes
	StrCpy $logBuffer "Trying to stop service and kill processes...$\r$\n"
	Call Write_Log
	Push "${PRODUCT_SERVICE_NAME}"
	Call StopService
	; Restore used register
	Pop $R0
FunctionEnd

#####################################################################
# This function checks if we need to upgrade from agent 40000 series
# If yes, we need to move configuration files from $INSTDIR to
# $ALLUSERS APPDATA\OCS Inventory NG\Agent
#####################################################################
Function UpgradeFrom4000
	; Save used register
	Push $R0
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Check if it is an upgrade from old agent 4000 series
    ClearErrors
    FileOpen $R0 "$INSTDIR\OcsService.dll" r
    IfErrors TestInstall_End_Upgrade
    FileClose $R0
	StrCpy $logBuffer "Old agent 4000 series detected, running migration process...$\r$\n"
	Call Write_Log
	; First, stop service
	StrCpy $OcsService "TRUE"
	Push "OCS INVENTORY"
	Call StopService
    ; Copy ocsinventory.dat file to new folder, continue on error
    SetShellVarContext All
	StrCpy $logBuffer "Copying ocsinventory.dat file from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    CopyFiles /SILENT "$INSTDIR\ocsinventory.dat" "$APPDATA\OCS Inventory NG\Agent\ocsinventory.dat"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
    ; Copy admininfo.conf file to new folder, continue on error
	StrCpy $logBuffer "$\r$\nCopying admininfo.conf file from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    CopyFiles /SILENT "$INSTDIR\admininfo.conf" "$APPDATA\OCS Inventory NG\Agent\admininfo.conf"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
    ; Copy lasy_state file to new folder, continue on error
	StrCpy $logBuffer "$\r$\nCopying last_state file from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    CopyFiles /SILENT "$INSTDIR\last_state" "$APPDATA\OCS Inventory NG\Agent\last_state"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
    ; Copy cacert.pem file to new folder, continue on error
	StrCpy $logBuffer "$\r$\nCopying cacert.pem file from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    CopyFiles /SILENT "$INSTDIR\cacert.pem" "$APPDATA\OCS Inventory NG\Agent\cacert.pem"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
    ; Copy label file to new folder, continue on error
	StrCpy $logBuffer "$\r$\nCopying label file from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    CopyFiles /SILENT "$INSTDIR\label" "$APPDATA\OCS Inventory NG\Agent\label"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
    ; Transfer old service config to new file (TTO_WAIT, PROLOG_FREQ...)
	StrCpy $logBuffer "$\r$\nCopying service configuration from <$INSTDIR> to <$APPDATA\OCS Inventory NG\Agent>..."
	Call Write_Log
	ClearErrors
    ReadINIStr $R0 "$INSTDIR\service.ini" "OCS_SERVICE" "TTO_WAIT"
    WriteINIStr "$APPDATA\OCS Inventory NG\Agent\ocsinventory.ini" "${PRODUCT_SERVICE_NAME}" "TTO_WAIT" $R0
    ReadINIStr $R0 "$INSTDIR\service.ini" "OCS_SERVICE" "PROLOG_FREQ"
    WriteINIStr "$APPDATA\OCS Inventory NG\Agent\ocsinventory.ini" "${PRODUCT_SERVICE_NAME}" "PROLOG_FREQ" $R0
    ReadINIStr $R0 "$INSTDIR\service.ini" "OCS_SERVICE" "OLD_PROLOG_FREQ"
    WriteINIStr "$APPDATA\OCS Inventory NG\Agent\ocsinventory.ini" "${PRODUCT_SERVICE_NAME}" "OLD_PROLOG_FREQ" $R0
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
	; Transfer Downloads
	StrCpy $logBuffer "$\r$\nCopying current downloads from <$INSTDIR\download> to <$APPDATA\OCS Inventory NG\Agen\download>...$\r$\n"
	Call Write_Log
	ClearErrors
    CreateDirectory "$APPDATA\OCS Inventory NG\Agent\download"
    CopyFiles /SILENT "$INSTDIR\download\*.*" "$APPDATA\OCS Inventory NG\Agen\download"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed, but non blocking !"
	Call Write_Log
	; Uninstall old service
	StrCpy $logBuffer "$\r$\nUnregistering service from Windows Service Manager...$\r$\n"
	Call Write_Log
    nsExec::ExecToLog '"$INSTDIR\OcsService.exe" -uninstall'
    pop $0
    StrCpy $logBuffer "Service unregister ended with exit code $0$\r$\n"
    Call Write_Log
	Sleep 1000
    ; Remove old data files
	StrCpy $logBuffer "Removing old data files from <$INSTDIR>..."
	Call Write_Log
	ClearErrors
    Delete /REBOOTOK "$INSTDIR\admininfo.conf"
    Delete /REBOOTOK "$INSTDIR\cacert.pem"
    Delete /REBOOTOK "$INSTDIR\label"
    Delete /REBOOTOK "$INSTDIR\last_state"
    Delete /REBOOTOK "$INSTDIR\ocsinventory.dat"
    Delete /REBOOTOK "$INSTDIR\service.ini"
    Delete /REBOOTOK "$INSTDIR\*.log"
    rmdir /r /REBOOTOK "$INSTDIR\download"
    IfErrors 0 +3
	StrCpy $logBuffer "One or more file remove failed (perhaps missing file), but non blocking !"
	Call Write_Log
    ; Remove no more used files
	StrCpy $logBuffer "$\r$\nRemoving unused binary files from <$INSTDIR>..."
	Call Write_Log
	ClearErrors
    Delete /REBOOTOK "$INSTDIR\BiosInfo.exe"
    Delete /REBOOTOK "$INSTDIR\inst32.exe"
    Delete /REBOOTOK "$INSTDIR\Mfc42.dll"
    Delete /REBOOTOK "$INSTDIR\OcsService.dll"
    Delete /REBOOTOK "$INSTDIR\PsApi.dll"
    Delete /REBOOTOK "$INSTDIR\Zlib.dll"
	Delete /REBOOTOK "$STARTMENU\Ocs_Contact.lnk"
    IfErrors 0 +3
	StrCpy $logBuffer "One or more file remove failed (perhaps missing file), but non blocking !"
	Call Write_Log
	; Remove old uninstall registry key
	StrCpy $logBuffer "$\r$\nRemoving old uninstall key <HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OCS Inventory Agent>..."
	Call Write_Log
	ClearErrors
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OCS Inventory Agent"
    IfErrors 0 +3
	StrCpy $logBuffer "Failed to remove key, but non blocking !"
	Call Write_Log
	; Ensure service uninstall and migration process successfull
    IfFileExists "$INSTDIR\OcsService.dll" TestInstall_Upgrade_Error
	StrCpy $logBuffer "$\r$\nMigration process from old agent 4000 series succesfull, continuing setup...$\r$\n"
	Call Write_Log
    goto TestInstall_End_Upgrade
TestInstall_Upgrade_Error:
	StrCpy $logBuffer "$\r$\nMigration process from old agent 4000 series failed ! ABORTING$\r$\n"
	Call Write_Log
	Abort "Migration process from old agent 4000 series failed !"
TestInstall_End_Upgrade:
	; Restore used register
	Pop $R0
FunctionEnd

#####################################################################
# This function write service initialisation file
#####################################################################
Function WriteServiceIni
	; Save used registers
	Push $R0 ; to read param
	Push $R1
	Push $R2 ; Agent Command line
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; SERVER PROPERTIES
	; Server address
    ReadINIStr $R0 "$PLUGINSDIR\server.ini" "Field 7" "State"
    StrCpy $R2 "/SERVER=$R0"
	; Read server credentials
	ReadINIStr $R0 "$PLUGINSDIR\server.ini" "Field 8" "State"
	StrCpy $R1 "$R2 /USER=$R0"
	StrCpy $R2 $R1
	ReadINIStr $R0 "$PLUGINSDIR\server.ini" "Field 9" "State"
	StrCpy $R1 "$R2 /PWD=$R0"
	StrCpy $R2 $R1
    ; Read Server Security
	ReadINIStr $R0 "$PLUGINSDIR\server.ini" "Field 10" "State"
	StrCpy $R1 "$R2 /SSL=$R0"
	StrCpy $R2 $R1
	ReadINIStr $R0 "$PLUGINSDIR\server.ini" "Field 11" "State"
	StrCpy $R1 "$R2 /CA=$R0"
	StrCpy $R2 $R1
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; PROXY PROPERTIES
	; Proxy type
	ReadINIStr $R0 "$PLUGINSDIR\proxy.ini" "Field 7" "State"
	StrCmp "$R0" "HTTP" WriteServiceIni_HTTP_Proxy
	StrCmp "$R0" "Socks 4" WriteServiceIni_Socks4_Proxy
	StrCmp "$R0" "Socks 5" WriteServiceIni_Socks5_Proxy
	; No proxy by default
    StrCpy $R0 "0"
    goto WriteServiceIni_End_Proxy
WriteServiceIni_HTTP_Proxy:
	; HTTP Proxy
    StrCpy $R0 "1"
    goto WriteServiceIni_End_Proxy
WriteServiceIni_Socks4_Proxy:
	; Socks 4 proxy
    StrCpy $R0 "2"
    goto WriteServiceIni_End_Proxy
WriteServiceIni_Socks5_Proxy:
	; Socks 5 proxy
    StrCpy $R0 "3"
    goto WriteServiceIni_End_Proxy
WriteServiceIni_End_Proxy:
	StrCpy $R1 "$R2 /PROXY_TYPE=$R0"
	StrCpy $R2 $R1
	; Proxy address and port
	ReadINIStr $R0 "$PLUGINSDIR\proxy.ini" "Field 8" "State"
	StrCpy $R1 "$R2 /PROXY=$R0"
	StrCpy $R2 $R1
	ReadINIStr $R0 "$PLUGINSDIR\proxy.ini" "Field 9" "State"
	StrCpy $R1 "$R2 /PROXY_PORT=$R0"
	StrCpy $R2 $R1
	; Proxy credentials
	ReadINIStr $R0 "$PLUGINSDIR\proxy.ini" "Field 10" "State"
	StrCpy $R1 "$R2 /PROXY_USER=$R0"
	StrCpy $R2 $R1
	ReadINIStr $R0 "$PLUGINSDIR\proxy.ini" "Field 11" "State"
	StrCpy $R1 "$R2 /PROXY_PWD=$R0"
	StrCpy $R2 $R1
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; AGENT AND SETUP PROPERTIES
	; Verbose log
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 4" "State"
	StrCpy $R1 "$R2 /DEBUG=$R0"
	StrCpy $R2 $R1
	; No Tag
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 5" "State"
	StrCmp $R0 "1" 0 WriteServiceIni_Skip_NoTag
	StrCpy $R1 "$R2 /NOTAG"
	StrCpy $R2 $R1
WriteServiceIni_Skip_NoTag:
	; TAG
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 6" "State"
	StrCpy $R1 "$R2 /TAG=$R0"
	StrCpy $R2 $R1
	; No Service
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 7" "State"
	StrCmp $R0 "1" 0 WriteServiceIni_Skip_NoService
	StrCpy $R1 "$R2 /NO_SERVICE"
	StrCpy $R2 $R1
WriteServiceIni_Skip_NoService:
	; No Systray
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 8" "State"
	StrCmp $R0 "1" 0 WriteServiceIni_Skip_NoSystray
	StrCpy $R1 "$R2 /NO_SYSTRAY"
	StrCpy $R2 $R1
WriteServiceIni_Skip_NoSystray:
	Sleep 1000
	; Write TAG if provided
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 6" "State"
    StrCmp "$R0" "" WriteServiceIni_Skip_TAG
    StrCpy $logBuffer "Writing TAG <$R0> into <$APPDATA\OCS Inventory NG\Agent\admininfo.conf> file...$\r$\n"
    Call Write_Log
    WriteINIStr "$APPDATA\OCS Inventory NG\Agent\admininfo.conf" "OCS Inventory Agent" "TAG" "$R0"
WriteServiceIni_Skip_TAG:
    ; Ask agent to create config file
    StrCpy $logBuffer "Writing agent configuration file by launching ocsinventory.exe /SAVE_CONF..."
    Call Write_Log
    nsExec::ExecToLog '"$INSTDIR\ocsinventory.exe" /SAVE_CONF $R2'
    pop $0
    StrCpy $logBuffer "Result: $0$\r$\n"
    Call Write_Log
	Sleep 1000
	; Restore used register
	Pop $R2
	Pop $R1
	Pop $R0
FunctionEnd

#####################################################################
# This function checks if no multiple setup launched, if setup
# launched as silent and without spash screen
#####################################################################
Function .onInit
	; Init debug log
	strcpy $installSatus ";-)"
	Delete ${SETUP_LOG_FILE}
	StrCpy $logBuffer "********************************************************$\r$\n"
	Call Write_Log
	${GetTime} "" "L" $0 $1 $2 $3 $4 $5 $6
	StrCpy $logBuffer "Starting ${PRODUCT_NAME} ${PRODUCT_VERSION} setup on $0/$1/$2 at $4:$5:$6$\r$\n"
	Call Write_Log
	StrCpy $logBuffer "Checking if setup not already running..."
	Call Write_Log
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "OcsSetupNG") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 not_running
	StrCpy $logBuffer "Yes$\r$\nABORT: Setup already running !"
	Call Write_Log
	Abort
not_running:
    ; Check if Windows 2000 or higher
	StrCpy $logBuffer "OK.$\r$\nChecking Operating System..."
	Call Write_Log
    ${If} ${AtLeastWin2000}
   	    StrCpy $logBuffer "OK, Windows 2000 or higher.$\r$\n"
	    Call Write_Log
    ${Else}
   	    StrCpy $logBuffer "ABORT: Setup running on unsupported Windows 9X or NT4 !$\r$\n"
	    Call Write_Log
	    Abort
    ${EndIf}
    ; Initializing plugins dir and extracting custom option pages
	InitPluginsDir
	File /oname=$PLUGINSDIR\server.ini "server.ini"
	File /oname=$PLUGINSDIR\proxy.ini "proxy.ini"
	File /oname=$PLUGINSDIR\agent.ini "agent.ini"
	File /oname=$PLUGINSDIR\splash.bmp "banner-ocs.bmp"
	File /oname=$PLUGINSDIR\SetACL.exe "SetACL.exe"
	; Don't know
	FileOpen $9 "ocsdat" r
	FileRead $9 "$2"
	FileClose $9
	StrCmp $9 "" +2 0
	StrCpy $CMDLINE '"$PLUGINSDIR\" $2'
	StrCpy $logBuffer "Command line is: $CMDLINE$\r$\n"
	Call Write_Log
	StrCpy $logBuffer "Parsing command line arguments..."
	Call Write_Log
	Call ParseCmd
	StrCpy $logBuffer "OK$\r$\n"
	Call Write_Log
	; Checking if Silent mode enabled
	StrCpy $logBuffer "Checking for silent mode..."
	Call Write_Log
	IfSilent Enable_silent
	; Disable_silent mode
	StrCpy $logBuffer "Disabled.$\r$\n"
	Call Write_Log
	SetSilent normal
	Goto Check_no_splash
Enable_silent:
	StrCpy $logBuffer "Enabled.$\r$\n"
	Call Write_Log
	SetSilent silent
	Goto Check_no_splash
Check_no_splash:
	; Checking if /nosplash option
	StrCpy $logBuffer "Checking for splash screen..."
	Call Write_Log
	StrCmp "$OcsNoSplash" "TRUE" Disable_splash
	; Enable splash screen
	StrCpy $logBuffer "Enabled.$\r$\n"
	Call Write_Log
	advsplash::show 1000 500 1000 -1 $PLUGINSDIR\splash
	Goto Check_User
Disable_splash:
	; Splash disabled
	StrCpy $logBuffer "Disabled.$\r$\n"
	Call Write_Log
Check_User:
	; Detect is user has admin right
	StrCpy $logBuffer "Checking if logged in user has Administrator privileges..."
	Call Write_Log
	Call IsUserAdmin
	Pop "$R0"
	StrCmp $R0 "true" Okadmin 0
	IfSilent 0 +2
	messagebox MB_ICONSTOP "Your are not logged on with Administrator privileges.$\r$\nYou cannot setup ${PRODUCT_NAME} as a Windows Service!"
	StrCpy $logBuffer "NO$\r$\nABORT: unable to install Agent as a service without Administrator privileges !$\r$\n"
	Call Write_Log
	Abort
Okadmin:
	StrCpy $logBuffer "OK$\r$\n"
	Call Write_Log
FunctionEnd

#####################################################################
# This function ask user for agent and server options
#####################################################################
Function AskServerOptions
	!insertmacro MUI_HEADER_TEXT "OCS Inventory NG Server properties" "Fill in OCS Inventory NG Server address and options..."
	InstallOptions::dialog "$PLUGINSDIR\server.ini"
FunctionEnd

Function ValidateServerOptions
FunctionEnd

Function AskProxyOptions
	!insertmacro MUI_HEADER_TEXT "Proxy Server properties" "If needed, specify proxy server to use..."
	InstallOptions::dialog "$PLUGINSDIR\proxy.ini"
FunctionEnd

Function ValidateProxyOptions
FunctionEnd

Function AskAgentOptions
	!insertmacro MUI_HEADER_TEXT "OCS Inventory NG Agent for Windows properties" "If needed, specify OCS Inventory NG Agent options..."
	InstallOptions::dialog "$PLUGINSDIR\agent.ini"
FunctionEnd

Function ValidateAgentOptions
FunctionEnd


#####################################################################
# This section copy files service, install and start service
#####################################################################
Section "!OCS Inventory Agent" SEC01
    ; Create data directory for agent = working directory
    SetShellVarContext All
 	StrCpy $logBuffer "Creating directory <$APPDATA\OCS Inventory NG\Agent>...$\r$\n"
	Call Write_Log
    CreateDirectory "$APPDATA\OCS Inventory NG\Agent"
    ; Set authenticated users right to modify data
    StrCpy $logBuffer "SetACL on <$APPDATA\OCS Inventory NG\Agent>..."
    Call Write_Log
    nsExec::ExecToLog 'SetACL -on "$APPDATA\OCS Inventory NG\Agent" -ot file -rec cont_obj -actn ace -ace "n:S-1-5-32-545;p:read_ex,change;s:y"'
    pop $0
    StrCpy $logBuffer "Result: $0$\r$\n"
    Call Write_Log
    ; Test old release install and remove it with saving conf
    Call UpgradeFrom4000
	; Test previous for install and stop service if needed
	Call TestInstall
	; Copy files
	StrCpy $logBuffer "Copying new files to directory <$INSTDIR>...$\r$\n"
	Call Write_Log
	SetOutPath "$INSTDIR"
	SetOverwrite on
	StrCpy $logBuffer ""
	clearerrors
    ; Communication provider
    WriteINIStr "$APPDATA\OCS Inventory NG\Agent\ocsinventory.ini" "OCS Inventory Agent" "ComProvider" "ComHTTP.dll"
    ; Openssl and libcurl needs special version for Windows 2000
    ${If} ${IsWin2000}
        File "curl-7.21.3-ssl-sspi-zlib-static-bin-w32\libcurl.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libcurl.dll $\r$\n"
        clearerrors
        File "curl-7.21.3-ssl-sspi-zlib-static-bin-w32\libeay32.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libeay32.dll $\r$\n"
        clearerrors
        File "curl-7.21.3-ssl-sspi-zlib-static-bin-w32\libssl32.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libssl32.dll $\r$\n"
        clearerrors
    ${Else}
        File "..\Release\libcurl.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libcurl.dll $\r$\n"
        clearerrors
        File "..\Release\libcurl.dll.manifest"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libcurl.dll.manifest $\r$\n"
        clearerrors
        File "..\Release\libeay32.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying libeay32.dll $\r$\n"
        clearerrors
        File "..\Release\ssleay32.dll"
        Iferrors 0 +3
        StrCpy $logBuffer "$logBuffer ERROR copying ssleay32.dll $\r$\n"
        clearerrors
    ${EndIf}
    ; Zlib and ZipArchive library
	File "..\Release\ZipArchive.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying ZipArchive.dll $\r$\n"
	clearerrors
	File "..\Release\zlib1.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying zlib1.dll $\r$\n"
	clearerrors
	; Commuication provider
	File "..\Release\ComHTTP.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying ComHTTP.dll $\r$\n"
	clearerrors
	; System informations
	File "..\Release\OcsWmi.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying OcsWmi.dll $\r$\n"
	clearerrors
	File "..\Release\SysInfo.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying SysInfo.dll $\r$\n"
	clearerrors
	; Common framework
	File "..\Release\OCSInventory Front.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying OCSInventory Front.dll $\r$\n"
	clearerrors
	; Agent
	File "..\Release\OCSInventory.exe"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying OCSInventory.exe $\r$\n"
	clearerrors
	; Downloader and Installer
	File "..\Release\download.exe"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying download.exe $\r$\n"
	clearerrors
	; Windows service
	File "..\Release\OcsService.exe"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying OcsService.exe $\r$\n"
	clearerrors
	; Windows systray applet
	File "..\Release\OcsSystray.exe"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying OcsSystray.exe $\r$\n"
	clearerrors
	File "Ocs-Transform.xsl"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying Ocs-Transform.xsl $\r$\n"
	clearerrors
;	File "..\Release\uac.manifest"
;	Iferrors 0 +3
;	StrCpy $logBuffer "$logBuffer ERROR copying uac.manifest $\r$\n"
;	clearerrors
	SetOutPath "$INSTDIR\Plugins"
	File "..\Release\Plugins\DO_NOT_REMOVE.txt"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying Plugins\DO_NOT_REMOVE.txt $\r$\n"
	clearerrors
	; MSVC 9 CRT redist
	SetOutPath "$INSTDIR"
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying Microsoft.VC90.CRT.manifest $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcm90.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying msvcm90.dll $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcp90.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying msvcp90.dll $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying msvcr90.dll $\r$\n"
	clearerrors
	; MSVC 9 MFC redist
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\\Microsoft.VC90.MFC\Microsoft.VC90.MFC.manifest"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying \Microsoft.VC90.MFC.manifest $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\\Microsoft.VC90.MFC\mfc90.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying mfc90.dll $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\\Microsoft.VC90.MFC\mfc90u.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying mfc90u.dll $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\\Microsoft.VC90.MFC\mfcm90.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying mfcm90.dll $\r$\n"
	clearerrors
	File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\\Microsoft.VC90.MFC\mfcm90u.dll"
	Iferrors 0 +3
	StrCpy $logBuffer "$logBuffer ERROR copying mfcm90u.dll $\r$\n"
	clearerrors
	; Create service configuration file
	strcmp $logBuffer "" +2
	strcpy $installSatus ":("
	Call Write_Log
	strcpy $logBuffer "Copying new files status is $installSatus$\r$\n"
	Call Write_Log
	; Write configuration file
	Call WriteServiceIni
	; Launch inventory now
	ReadINIStr $R0 "$PLUGINSDIR\agent.ini" "Field 9" "State"
	StrCmp $R0 "1" 0 WriteServiceIni_Skip_Now
	StrCpy $logBuffer '[/NOW] used, so launching "$INSTDIR\ocsinventory.exe"...'
	Call Write_Log
	nsExec::ExecToLog "$INSTDIR\ocsinventory.exe" $R0
	StrCpy $logBuffer "Result: $R0$\r$\n"
	Call Write_Log
WriteServiceIni_Skip_Now:
	; Install service if needed
	Call StartService
	; Create startup menu item for systray applet
	Call CreateMenu
SectionEnd


#####################################################################
# This function start menu items
#####################################################################
Function CreateMenu
	; Read /NO_SYSTRAY
	ReadINIStr $R0 "$PLUGINSDIR\Agent.ini" "Field 8" "State"
	StrCmp "$R0" "1" CreateMenu_NO_SYSTRAY
	; Create startup menu item to launch systray applet
	setshellvarcontext all
	StrCpy $logBuffer 'Creating Menu ShortCut "$SMSTARTUP\OCS Inventory NG Systray.lnk" to start Systray applet...$\r$\n'
	Call Write_Log
	CreateShortCut "$SMSTARTUP\OCS Inventory NG Systray.lnk" "$INSTDIR\OcsSystray.exe"
	Exec "$INSTDIR\OcsSystray.exe"
	goto CreateMenu_End
CreateMenu_NO_SYSTRAY:
	StrCpy $logBuffer '[/NO_SYSTRAY] used, so disabling Systray applet shortcut in All Users Startup menu...$\r$\n'
	Call Write_Log
	Delete /REBOOTOK "$SMSTARTUP\OCS Inventory NG Systray.lnk"
CreateMenu_End:
FunctionEnd


#####################################################################
# This section writes uninstall into Windows
#####################################################################
Section -Post
	WriteUninstaller "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\OCSInventory.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\OCSInventory.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
	; Write deployement status file if required
	StrCmp "$OcsUpgrade" "TRUE" 0 Post_end
	; WRITE ../done
	SetOutPath "$exedir"
	FileOpen $1 "..\done" w
	FileWrite $1 "SUCCESS"
	FileClose $1
	; WRITE done into Download directory, because in 2.0 or higher, package is unzipped into system TEMP directory
	; and no more under "$APPDATA\OCS Inventory NG\Agent\Download\PackID\Temp"
	FileOpen $1 "$APPDATA\OCS Inventory NG\Agent\Download\done" w
	FileWrite $1 "SUCCESS"
	FileClose $1
Post_end:
SectionEnd


#####################################################################
# This function writes install status into log file when sucessfull install
#####################################################################
Function .onInstSuccess
	${GetTime} "" "L" $0 $1 $2 $3 $4 $5 $6
	strcmp "$installSatus" ";-)" 0 onInstSuccess_Error
	StrCpy $logBuffer "SUCESS: ${PRODUCT_NAME} ${PRODUCT_VERSION} successfuly installed on $0/$1/$2 at $4:$5:$6$\r$\n$installSatus$\r$\n "
	goto onInstSuccess_end
onInstSuccess_Error:
	StrCpy $logBuffer "ERROR: ${PRODUCT_NAME} ${PRODUCT_VERSION} may not work correctly since $0/$1/$2 at $4:$5:$6$\r$\n$installSatus$\r$\n "
onInstSuccess_end:
	Call Write_Log
FunctionEnd


#####################################################################
# This function writes install status into log file when install failed
#####################################################################
Function .onInstFailed
	StrCpy $logBuffer "ABORT: installation of ${PRODUCT_NAME} ${PRODUCT_VERSION} failed :-($\r$\n"
	Call Write_Log
FunctionEnd


#####################################################################
# This function ask uninstall confirmation, if not launched with
# silent argument /S
#####################################################################
Function un.onInit
	Push "$CMDLINE"
	Push " /S"
	Call un.StrStr
	Pop $R9
	StrLen $0 $R9
	IntCmp $0 2 unOnInit_silent 0 unOnInit_silent
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure to unistall $(^Name)?" IDYES +2
	Abort
unOnInit_silent:
FunctionEnd


#####################################################################
# This section stop service, uninstall service and remove files
#####################################################################
Section Uninstall
	SetShellVarContext  all
	StrCpy $logBuffer "Sarting ${PRODUCT_NAME} ${PRODUCT_VERSION} UNISTALL...$\r$\n"
	Call un.Write_Log
	Call un.StopService
	; Uninstall NT service
	nsExec::ExecToLog "$INSTDIR\ocsservice.exe -uninstall" $R0
	; Remove startup links
	Delete /REBOOTOK "$SMSTARTUP\OCS Inventory NG Systray.lnk"
	; Remove files
	Delete /REBOOTOK "$INSTDIR\uninst.exe"
	Delete /REBOOTOK "$INSTDIR\zlib1.dll"
	Delete /REBOOTOK "$INSTDIR\AipArchive.dll"
	Delete /REBOOTOK "$INSTDIR\uac.manifest"
	Delete /REBOOTOK "$INSTDIR\SysInfo.dll"
	Delete /REBOOTOK "$INSTDIR\ssleay32.dll"
	Delete /REBOOTOK "$INSTDIR\OcsWmi.dll"
	Delete /REBOOTOK "$INSTDIR\OcsSystray.exe"
	Delete /REBOOTOK "$INSTDIR\OcsService.exe"
	Delete /REBOOTOK "$INSTDIR\OCSInventory.exe"
	Delete /REBOOTOK "$INSTDIR\libeay32.dll"
	Delete /REBOOTOK "$INSTDIR\libcurl.dll"
	Delete /REBOOTOK "$INSTDIR\download.exe"
	RMDir /r /REBOOTOK "$INSTDIR"
	; Remove registry key
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
	DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
	SetAutoClose true
SectionEnd


#####################################################################
# This function ask to restart computer when uninstalling, if not
# launched with silent argument /S
#####################################################################
Function un.onUninstSuccess
	HideWindow
	Push "$CMDLINE"
	Push " /S"
	Call un.StrStr
	Pop $R9
	StrLen $0 $R9
	IntCmp $0 2 unOnUninstSuccess_silent 0 unOnUninstSuccess_silent
	MessageBox MB_OK|MB_ICONEXCLAMATION "${PRODUCT_NAME} ${PRODUCT_VERSION} was successfully uninstalled."
unOnUninstSuccess_silent:
	StrCpy $logBuffer "${PRODUCT_NAME} ${PRODUCT_VERSION} was successfully uninstalled."
	Call un.Write_Log
FunctionEnd
