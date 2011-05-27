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

; Version informations
!define PRODUCT_NAME "OCS Inventory NG Logon Installer"
!define PRODUCT_VERSION "2.0.0.2"
!define PRODUCT_PUBLISHER "OCS Inventory NG Team"
!define PRODUCT_WEB_SITE "http://www.ocsinventory-ng.org"

; Default OCS Inventory NG server address to connect to
!define Default_OCS_Server "http://ocsinventory-ng/ocsinventory"
; Default timeout
!define Default_TimeOut "60000"
; Others specific options to force at setup must be set here
!define AgentCmdLineHardOptions ""

; Use Modern UI
!include "MUI.nsh"
!insertmacro MUI_LANGUAGE "english"
; Additional headers and macros
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "WinVer.nsh"
!include "LogicLib.nsh"
!insertmacro WordReplace
!insertmacro GetTime

;Request application privileges for Windows Vista or higher ('user' or 'admin')
RequestExecutionLevel user

;--------------------------------
;Version Information
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "OCS Inventory NG Agent for Windows Logon/GPO installer "
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "${PRODUCT_WEB_SITE}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Distributed under GNU GPL Version 2 Licence"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"
;--------------------------------

BRANDINGTEXT "${PRODUCT_NAME}"
ICON "install-ocs.ico"
ShowInstDetails hide
Name "OcsLogon"
OutFile "OcsLogon.exe"
SilentInstall silent

#####################################################################
# Global variables
#####################################################################
var /GLOBAL nGpo                  ; Set to 1 if /GPO is provided
var /GLOBAL nShowlog              ; Set to 1 if /SHOWLOG is provided, to display log file at the end of exec, usefull for debugging
var /GLOBAL nUninstall            ; Set to 1 if /Uninstall is provided
var /GLOBAL nForce                ; Do we overwrite installed version (=1) or not (=0)
var /GLOBAL nTimeOut              ; Download timeout (default 60 sec)
var /GLOBAL nPackager             ; Use setup package created with OCS Packager
var /GLOBAL strOCS_Server         ; Server URL http://address:port/ocsinventory
var /GLOBAL strOCS_Server_User    ; Server credentials for authentication
var /GLOBAL strOCS_Server_Pwd
var /GLOBAL strProxy_Type         ; Proxy type (None, HTTP, Socks 4, Socks 5)
var /GLOBAL strProxy_Server       ; HTTP proxy server address and port (Socks 4 and 5 not supported)
var /GLOBAL strProxy_Port
var /GLOBAL strProxy_User         ; Proxy credentials
var /GLOBAL strProxy_Pwd
var /GLOBAL strAgent_Download_Url ; Alternative agent download url provided with /URL=
var /GLOBAL strVersion_To_Setup   ; Version to force setup provided witrh /DEPLOY=
var /GLOBAL logBuffer             ; To complete the setup log file
var /GLOBAL logFile               
var /GLOBAL agentFile             ; Path to OCS Inventory NG Agent executable (detected by GetInstalledVersion)

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
# This function write content of logBuffer variable in log file in
# a log file OcsAgentSetup.log located in install directory
#####################################################################
Function Write_Log
	; Save used register
	Push $R0
	ClearErrors
	StrCpy $R0 $logBuffer -2
	Detailprint $R0
	; Is there something to write ?
	StrCmp $logBuffer "" WriteLog_end
	; Open log file
	FileOpen $R0 $logFile a
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
	Push "${Default_OCS_Server}"   ; push a default value onto the stack
	Call GetParameterValue
	Pop $R0
	StrCpy $strOCS_Server "$R0"
	; Server credentials
	Push "/USER="          ; push the search string onto the stack
	Push ""               ; No user by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strOCS_Server_User "$R0"
	Push "/PWD="          ; push the search string onto the stack
	Push ""              ; No password by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strOCS_Server_Pwd "$R0"
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
	StrCpy $strProxy_Type "None"
	goto ParseCmd_End_Proxy
ParseCmd_HTTP_Proxy:
	; HTTP proxy
	StrCpy $strProxy_Type "HTTP"
	goto ParseCmd_End_Proxy
ParseCmd_Socks4_Proxy:
	; Socks 4 proxy
	StrCpy $strProxy_Type "Socks 4"
	goto ParseCmd_End_Proxy
ParseCmd_Socks5_Proxy:
	; Socks 5 proxy
	StrCpy $strProxy_Type "Socks 5"
	goto ParseCmd_End_Proxy
ParseCmd_End_Proxy:
	; Proxy address
	Push "/PROXY="          ; push the search string onto the stack
	Push ""                 ; No proxy address by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strProxy_Server "$R0"
	; Proxy port
	Push "/PROXY_PORT="     ; push the search string onto the stack
	Push ""                 ; No proxy address by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strProxy_Port "$R0"
	; Proxy credentials
	Push "/PROXY_USER="          ; push the search string onto the stack
	Push ""               ; No user by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strProxy_User "$R0"
	Push "/PROXY_PWD="          ; push the search string onto the stack
	Push ""              ; No password by default
	Call GetParameterValue
	Pop $R0
	StrCpy $strProxy_Pwd "$R0"
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; LOGON SPECIFIC PROPERTIES
	; Use of OCS PACKAGER setup package
        Push "/GPO" ; push the search string onto the stack
	Push "1"         ; Yes by default
	Call GetParameterValue
	Pop $R0
	${If} "$R0" == "1"
	    StrCpy $nGpo "0"
	${Else}
	    StrCpy $nGpo "1"
    ${EndIf}
    Push "/SHOWLOG" ; push the search string onto the stack
	Push "1"         ; Yes by default
	Call GetParameterValue
	Pop $R0
	${If} "$R0" == "1"
    	StrCpy $nShowlog "0"
	${Else}
	    StrCpy $nShowlog "1"
	${EndIf}
    Push "/PACKAGER" ; push the search string onto the stack
	Push "1"         ; Yes by default
	Call GetParameterValue
	Pop $R0
	${If} "$R0" == "1"
	    StrCpy $nPackager "0"
    ${Else}
	    StrCpy $nPackager "1"
    ${EndIf}
	; /DEPLOY="value"
	Push "/DEPLOY=" ; push the search string onto the stack
	Push ""         ; No version by default
	Call GetParameterValue
	Pop $R0
	strCpy $strVersion_To_Setup "$R0"
	; /URL="value"
	Push "/URL=" ; push the search string onto the stack
	Push ""      ; No URL by default
	Call GetParameterValue
	Pop $R0
	strCpy $strAgent_Download_Url "$R0"
	; /TIMEOUT="value"
	Push "/TIMEOUT=" ; push the search string onto the stack
	Push "${Default_TimeOut}"  ; 60 seconds by default
	Call GetParameterValue
	Pop $R0
	strCpy $nTimeOut "$R0"
	; Force overwrite
	Push "/FORCE"    ; push the search string onto the stack
	Push "1"         ; Yes by default
	Call GetParameterValue
	Pop $R0
	${If} "$R0" == "1"
    	StrCpy $nForce "0"
	${Else}
        StrCpy $nForce "1"
	${EndIf}
	; Uninstall
	Push "/UNINSTALL" ; push the search string onto the stack
	Push "1"         ; Yes by default
	Call GetParameterValue
	Pop $R0
	${If} "$R0" == "1"
    	StrCpy $nUninstall "0"
	${Else}
        StrCpy $nUninstall "1"
	${EndIf}
	; Restore used registers
	Pop $R1
	Pop $R0
FunctionEnd


#####################################################################
# GetInstalledVersion - Try to find Agent installed version "A.B.C.D"
# input, none
# output, top of stack, currently installed version
# modifies no other variables.
#####################################################################
Function GetInstalledVersion
    Push $R0
    Push $R1
    Push $R2
    Push $R3
    Push $R4
    Push $R5
    ; Read install path from registry
    ReadRegStr $agentFile HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OCS Inventory NG Agent" "DisplayIcon"
    IfFileExists $agentFile GetInstalledVersion_GetVersion
    ; Registry path not set, try default service path
    StrCpy $agentFile "$PROGRAMFILES\OCS Inventory Agent\Ocsinventory.exe"
    IfFileExists $agentFile GetInstalledVersion_GetVersion
    ; Default service path not found, try default 1.X standalone path
    strcpy $agentFile $WINDIR 2
    StrCpy $agentFile "$agentFile\ocs-ng\Ocsinventory.exe"
    IfFileExists $agentFile GetInstalledVersion_GetVersion
    ; Default 1.X standalone path not found, try backup 1.X standalone path into TEMP folder
    StrCpy $agentFile "$TEMP\ocs-ng\Ocsinventory.exe"
    IfFileExists $agentFile GetInstalledVersion_GetVersion
GetInstalledVersion_GetVersion:
    ClearErrors
    GetDllVersion "$agentFile" $R0 $R1
    IfErrors GetInstalledVersion_SetDefault
    IntOp $R2 $R0 / 0x00010000
    ${If} "$R2" == "4"
        ; agent series 4000 detected => version 1.X
        StrCpy "$R2" "1"
    ${EndIf}
    IntOp $R3 $R0 & 0x0000FFFF
    IntOp $R4 $R1 / 0x00010000
    IntOp $R5 $R1 & 0x0000FFFF
    StrCpy $R0 "$R2.$R3.$R4.$R5"
    goto GetInstalledVersion_End
GetInstalledVersion_SetDefault:
    StrCpy $R0 "0.0.0.0"
GetInstalledVersion_End:
    Pop $R5
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
FunctionEnd


#####################################################################
# DownloadFile - Try to download Agent installer file from server
# input, none
# output, top of stack, local downloaded file
# modifies no other variables.
#####################################################################
Function DownloadFile
    Push $R0
    Push $R1
    Push $R2
    StrCpy $logBuffer "Download timeout is set to <$nTimeOut> millisecond.$\r$\n"
    Call Write_Log
    ; First, construct local file path
    StrCpy $R0 "$TEMP\OCS-NG_Agent_Setup.exe"
    ; Configure proxy settings into $R1
    StrCmp "$strProxy_Type" "HTTP" DownloadFile_ProxyHTTP
    ; Not using proxy, or unssupported proxy
    StrCpy $R1 "/NOPROXY"
    StrCpy $logBuffer "Download does not use HTTP proxy server.$\r$\n"
    Call Write_Log
    goto DownloadFile_ProxyEnd
DownloadFile_ProxyHTTP:
    ; Using HTTP Proxy
    ${If} "$strProxy_Port" == ""
        StrCpy $R1 "/PROXY $strProxy_Server"
    ${Else}
        StrCpy $R1 "/PROXY $strProxy_Server:$strProxy_Port"
    ${EndIf}
    StrCpy $logBuffer "Download uses HTTP proxy server <$R1> "
    Call Write_Log
    ${If} "$strProxy_User" == ""
        StrCpy $logBuffer "without authentication.$\r$\n"
        Call Write_Log
    ${Else}
        StrCpy $logBuffer "with user authentication.$\r$\n"
        Call Write_Log
        StrCpy $R2 "$R1 /USERNAME $strProxy_User /PASSWORD $strProxy_Pwd"
        StrCpy $R1 $R2
    ${EndIf}
DownloadFile_ProxyEnd:
    ; Is /URL provided
    StrCmp "$strAgent_Download_Url" "" 0 DownloadFile_UseURL
    ; Is packager used
    ${If} "$nPackager" == "1"
       ; Packager used
       StrCpy $R2 "$strOCS_Server/deploy/ocspackage.exe"
    ${Else}
        ; Packager not used
        StrCpy $R2 "$strOCS_Server/deploy/OCS-NG-Windows-Agent-Setup.exe"
    ${EndIf}
    StrCpy $logBuffer "Downloading file <$R2> "
    Call Write_Log
    ; Is server authentication required
    ${If} "$strOCS_Server_User" == ""
        StrCpy $logBuffer "without server authentication..."
        Call Write_Log
    ${Else}
        StrCpy $logBuffer "with server authentication..."
       ; Add authentication info
       ${WordReplace} "$R2" "://" "://$strOCS_Server_User:$strOCS_Server_Pwd@" "+" $0
       StrCpy "$R2" "$0"
    ${EndIf}
    goto DownloadFile_EndURL
DownloadFile_UseURL:
    StrCpy $R2 "$strAgent_Download_Url"
    StrCpy $logBuffer "Downloading file <$R2>..."
    Call Write_Log
DownloadFile_EndURL:
;**************************
; If /gpo try to copy file
;**************************
    ${If} "$nGpo" == "1"
        ${If} "$nPackager" == "1"
            ; Packager used
            StrCpy $0 "$exedir\ocspackage.exe"
        ${Else}
            StrCpy $0 "$exedir\ocsagent.exe"
        ${EndIf}
        StrCpy $logBuffer "/GPO used$\r$\nTrying to download from <$0>..."
        Call Write_Log
        IfFileExists "$0" 0 DownloadFile_badGpo
        CopyFiles /silent "$0" "$R0.new"
        IfFileExists "$R0.new" DownloadFile_End
DownloadFile_badGpo:
        ; Error copying file
        StrCpy $logBuffer "Failed copying file !$\r$\n"
        Call Write_Log
        Abort "*** ERROR: Failed copying file !$\r$\n"
    ${EndIf}
    IntOp $0 $nTimeOut / 1000
    ; Using Inetc plugin at least release of 2011/04/28
    ; /TIMEOUT x (in ms) replaced by /CONNECTIONTIMEOUT x (in s) and /RECEIVETIMEOUT x (in s)
    inetc::get /CONNECTTIMEOUT $0 /RECEIVETIMEOUT $0  $R1 '$R2' '$R0.new' /END
    Pop $0
    StrCmp $0 "OK" DownloadFile_End 0
    ; Error downloading file
    StrCpy $logBuffer "$0: Failed downloading file !$\r$\n"
    Call Write_Log
    Abort "*** ERROR: Failed downloading file !$\r$\n"
DownloadFile_End:
    delete "$R0"
    rename "$R0.new" "$R0"
    StrCpy $logBuffer "OK ($R0)$\r$\n"
    Call Write_Log
    Pop $R2
    Pop $R1
    Exch $R0
FunctionEnd


#####################################################################
# UninstallVersion - Try to uninstall Agent
# input, none
# output, top of stack, currently installed version
# modifies no other variables.
#####################################################################
Function UninstallAgent
    Push $R0
    StrCpy $logBuffer "Trying to find OCS Inventory NG Agent uninstall..."
    Call Write_Log
    ; Read uninstall path from registry
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OCS Inventory NG Agent" "UninstallString"
    IfFileExists $R0 UninstallAgent_Service
    ; Registry path not set, try default service path
    StrCpy $R0 "$PROGRAMFILES\OCS Inventory Agent\uninst.exe"
    IfFileExists $R0 UninstallAgent_Service
    StrCpy $logBuffer "Not found$\r$\nTrying to find OCS Inventory NG Agent 1.X standalone folder..."
    Call Write_Log
    ; Default service path not found, try default 1.X standalone path
    StrCpy $R0 "C:\ocs-ng\Ocsinventory.exe"
    IfFileExists $R0 UninstallAgent_Delete
    ; Default 1.X standalone path not found, try backup 1.X standalone path into TEMP folder
    StrCpy $agentFile "$TEMP\ocs-ng\Ocsinventory.exe"
    IfFileExists $agentFile UninstallAgent_Delete
UninstallAgent_Service:
    ; Start uninstall silently
    StrCpy $logBuffer "OK$\r$\nStarting OCS Inventory NG Agent uninstall <$R0>..."
    Call Write_Log
    ExecWait "$R0 /S" $0
    StrCpy $logBuffer "Exit code is $0, "
    Call Write_Log
UninstallAgent_Delete:
    ; Now, delete files
    ${GetParent} $R0 $0
    StrCpy $logBuffer "OK$\r$\nDeleting OCS Inventory NG Agent directory <$0>..."
    Call Write_Log
    RMDir /r /REBOOTOK $0
    StrCpy $logBuffer "OK$\r$\n"
    Call Write_Log
    ClearErrors
    Exch $R0
FunctionEnd


#####################################################################
# Initialisation function
#####################################################################
Function .onInit
    SetOutPath "$TEMP"
    ; Intialize log file, and make a backup of previous log file
    StrCpy $logFile "$TEMP\OcsLogon.log"
    CopyFiles /SILENT $logFile "$logFile.bak"
    ; Initialize command line hard options
    StrCpy $CMDLINE "$CMDLINE ${AgentCmdLineHardOptions}"
    Delete $logFile
    StrCpy $logBuffer "********************************************************$\r$\n"
    Call Write_Log
    ${GetTime} "" "L" $0 $1 $2 $3 $4 $5 $6
    StrCpy $logBuffer "Starting ${PRODUCT_NAME} ${PRODUCT_VERSION} on $0/$1/$2 at $4:$5:$6$\r$\n"
    Call Write_Log
    ; Prevent Multiple Instances
    StrCpy $logBuffer "Checking if not already running..."
    Call Write_Log
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "OcsLogonNG") i .r1 ?e'
    Pop $R0
    StrCmp $R0 0 onInit_NotRunning
    strcpy $logBuffer "${PRODUCT_NAME} is already running!$\r$\n"
    call Write_Log
    Abort "${PRODUCT_NAME} is already running!$\r$\n"
onInit_NotRunning:
    ; Parsing comand line
    StrCpy $logBuffer "OK$\r$\nParsing command line arguments..."
    Call Write_Log
    Call ParseCmd
    StrCpy $logBuffer "OK$\r$\n"
    Call Write_Log
FunctionEnd


#####################################################################
# Section to do the job
#####################################################################
Section "-Do the job"
    HideWindow
    SetAutoClose true
    ; Check Installed version
    StrCpy $logBuffer "Searching OCS Inventory NG Agent installed version..."
    Call Write_Log
    Call GetInstalledVersion
    Pop $R0
    StrCpy $logBuffer "OK ($R0)$\r$\n"
    Call Write_Log
    ; Check if uninstall is asked
    StrCmp "$nUninstall" "1" Job_AgentUninstall
    ; Check if overwrite is forced
    ${If} "$nForce" == "1"
        StrCpy $logBuffer "OCS Inventory NG Agent installation forced fy using /FORCE argument.$\r$\n"
        Call Write_Log
        goto Job_AgentInstall
    ${EndIf}
    ; Compare version
    ${VersionCompare} "$R0" "$strVersion_To_Setup" $R1
    ${If} $R1 = 0
        ; Version equal
        StrCpy $logBuffer "OCS Inventory NG Agent version $R0 already installed, skipping setup.$\r$\n"
        Call Write_Log
        goto Job_AgentStart
    ${ElseIf} $R1 = 1
        ; Installed version is newer than the one we try to setup
        StrCpy $logBuffer "OCS Inventory NG Agent version $R0 already installed, skipping setup.$\r$\n"
        Call Write_Log
        goto Job_AgentStart
    ${EndIf}
    ; Installed version is older than the one we try to setup
    StrCpy $logBuffer "OCS Inventory NG Agent version $R0 installed, upgrade to version $strVersion_To_Setup required.$\r$\n"
    Call Write_Log
Job_AgentInstall:
    ; Download file to setup
    Call DownloadFile
    Pop $R0
	; Get command line paramaters
	Call GetParameters
	Pop $R1
	; Start installed in silent mode, using provided parameters
    StrCpy $logBuffer "Starting OCS Inventory NG Agent installer..."
    Call Write_Log
    ExecWait "$R0 /S $R1" $0
    StrCpy $logBuffer "OK (exit code is $0)$\r$\n"
    Call Write_Log
    Delete /REBOOTOK "$R0"
    ; Run GetInstalledVersion to determine where is installed agent (in case of change by setup)
    Call GetInstalledVersion
Job_AgentStart:
    ; Check if Service is installed => not in standalone mode
    ReadRegStr $R0 HKLM "SYSTEM\CurrentControlSet\Services\OCS Inventory Service" "Start"
    ${If} "$R0" == "2"
        ; Service start status set to auto
        StrCpy $logBuffer "Windows automatically starts OCS Inventory NG Service, no need to launch Agent.$\r$\n"
        Call Write_Log
        goto Job_RunEnd
    ${EndIf}
    ; Agent used in standalone mode => we need to launch it
	; Get command line paramaters
	Call GetParameters
	Pop $R0
	; Start Agent using provided parameters
    StrCpy $logBuffer "Windows does not automatically start OCS Inventory NG Service, so starting Agent <$agentFile>..."
    Call Write_Log
    ExecWait "$agentFile $R0" $0
    StrCpy $logBuffer "OK (exit code is $0)$\r$\n"
    Call Write_Log
    goto Job_RunEnd
Job_AgentUninstall:
    StrCpy $logBuffer "[/UNINSTALL] used, trying to uninstall OCS Inventory NG Agent...$\r$\n"
    Call Write_Log
    Call UninstallAgent
Job_RunEnd:
    StrCpy $logBuffer "All done, exiting ;-)$\r$\n"
    Call Write_Log
SectionEnd


#####################################################################
# Section to terminate install
#####################################################################

Section -Post
    ${If} "$nShowlog" == "1"
        Execshell open "$logFile"
    ${EndIf}
SectionEnd

