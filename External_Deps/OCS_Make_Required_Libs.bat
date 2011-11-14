@echo off
title OCS Inventory NG Agent for Windows - Building required libraries
echo.
echo *************************************************************************
echo *                                                                       *
echo *                 OCS Inventory NG agent for Windows                    *
echo *                                                                       *
echo *                      Building required libraries                      *
echo *                                                                       *
echo *************************************************************************
echo.

Rem ========= UPDATE CONSTANTS BELOW TO MEET YOUR CONFIGURATION NEED =========  

Rem Set path to MS Visual C++
set VC_PATH=C:\Program Files\Microsoft Visual Studio 9.0\VC

Rem Set path to MS Windows SDK, needed to build cURL
set WINDOWS_SDK_PATH="C:\Program Files\Microsoft SDKs\Windows\v6.0A"

Rem Set path to Perl 5.6 or higher binary
set PERL_PATH=c:\xampp\perl\bin

Rem Set path to Zlib sources
set ZLIB_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\zlib-1.2.5

Rem Set path to OpenSSL sources
set OPENSSL_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\openssl-1.0.0c

Rem Set path to cURL sources
set CURL_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\curl-7.21.3

Rem Set path to tinyXML sources
SET XML_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\tinyxml

Rem Set path to ZipArchive sources, for example
SET ZIP_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\ZipArchive

Rem Set path to Net-SNMP sources, for example
SET SNMP_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\net-snmp-5.7

Rem ========= DO NOT MODIFY BELOW, UNTIL YOU KNOW WHAT YOU ARE DOING =========

Rem Ensure MS Visual C++ environnement is set
call "%VC_PATH%\VCVARSALL.BAT"
Rem Add perl to PATH
set PATH=%PATH%;%PERL_PATH%

title OCS Inventory NG Agent for Windows - Building Zlib DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building Zlib DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%ZLIB_PATH%"
Rem Build Zlib using precompiled asm code for MS Visual C++ with lastest Service Pack ( -D_BIND_TO_CURRENT_VCLIBS_VERSION)
nmake /NOLOGO -f Win32\Makefile.msc LOC="-DASMV -DASMINF -D_BIND_TO_CURRENT_VCLIBS_VERSION" OBJA="inffas32.obj match686.obj"
if ERRORLEVEL 1 goto ERROR

Rem copy libs to use them in OCS
copy zdll.lib ..
mkdir ..\..\Release
copy zlib1.dll ..\..\Release
mkdir ..\..\Debug
copy zlib1.dll ..\..\Debug
if ERRORLEVEL 1 goto ERROR

cd ..
title OCS Inventory NG Agent for Windows - Building OpenSSL DLLs...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building OpenSSL DLLs...             *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%OPENSSL_PATH%"
Rem Configure OpenSSL for MS Visual C++ with lastest Service Pack ( -D_BIND_TO_CURRENT_VCLIBS_VERSION)
perl.exe configure VC-WIN32 -D_BIND_TO_CURRENT_VCLIBS_VERSION
if ERRORLEVEL 1 goto ERROR
Rem Prepare OpenSSL build for MS Visual C++
call ms\do_ms.bat
if ERRORLEVEL 1 goto ERROR
Rem Build OpenSSL
nmake /NOLOGO -f ms\ntdll.mak
if ERRORLEVEL 1 goto ERROR
Rem Test OpenSSL build
cd out32dll
call "..\ms\test.bat"
if ERRORLEVEL 1 goto ERROR

Rem copy libs to use them in OCS
copy ssleay32.lib ..\..
copy libeay32.lib ..\..
copy ssleay32.dll ..\..\..\Release
copy libeay32.dll ..\..\..\Release
copy ssleay32.dll ..\..\..\Debug
copy libeay32.dll ..\..\..\Debug
if ERRORLEVEL 1 goto ERROR


cd ..\..
title OCS Inventory NG Agent for Windows - Building cURL DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building cURL DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%CURL_PATH%"
Rem Disable LDAP support, not needed in OCS Inventory NG Agent
set WINDOWS_SSPI=0
cd lib
Rem Fix cURL DLL config for MS Visual C++ with lastest Service Pack ( -D_BIND_TO_CURRENT_VCLIBS_VERSION)
perl.exe -pi.bak -e "s# /DBUILDING_LIBCURL# /DBUILDING_LIBCURL /D_BIND_TO_CURRENT_VCLIBS_VERSION#g" Makefile.vc9
Rem Build cURL dll using OpenSSL Dlls and Zlib dll
nmake /NOLOGO /f Makefile.vc9 cfg=release-dll-ssl-dll-zlib-dll
if ERRORLEVEL 1 goto ERROR
Rem Insert manifest into DLL
cd release-dll-ssl-dll-zlib-dll
mt -manifest libcurl.dll.manifest -outputresource:libcurl.dll;2
if ERRORLEVEL 1 goto ERROR

Rem copy libs to use them in OCS
copy "libcurl_imp.lib" ..\..\..
copy "libcurl.dll" ..\..\..\..\Release
copy "libcurl.dll" ..\..\..\..\Debug
if ERRORLEVEL 1 goto ERROR

cd ..\..\..
title OCS Inventory NG Agent for Windows - Building Net-SNMP DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Configuring Net-SNMP DLL...          *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%SNMP_PATH%\win32"
Rem Configure Net-SNMP
perl.exe Configure --linktype=dynamic
if ERRORLEVEL 1 goto ERROR
cd libsnmp_dll
Rem Fix Net-SNMP DLL config for MS Visual C++ with lastest Service Pack ( -D_BIND_TO_CURRENT_VCLIBS_VERSION)
perl.exe -pi.bak -e "s# /D \"WIN32\"# /D \"WIN32\" /D_BIND_TO_CURRENT_VCLIBS_VERSION#g" Makefile
Rem Build Net-SNMP dll
nmake /NOLOGO
if ERRORLEVEL 1 goto ERROR
cd ..
copy "bin\release\netsnmp.dll" ..\..\..\Release
copy "bin\release\netsnmp.dll" ..\..\..\Debug
if ERRORLEVEL 1 goto ERROR
copy "lib\release\netsnmp.lib" ..\..
if ERRORLEVEL 1 goto ERROR

cd ..\..
title OCS Inventory NG Agent for Windows - Building ZipArchive DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Configuring ZipArchive DLL...        *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%ZIP_PATH%\Release Unicode STL MD DLL"
Rem copy libs to use them in OCS
copy "ZipArchive.lib" ..\..
copy "ZipArchive.dll" ..\..\..\Release
copy "ZipArchive.dll" ..\..\..\Debug
if ERRORLEVEL 1 goto ERROR

cd ..\..
title OCS Inventory NG Agent for Windows - Building service message file...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Compiling service message file...    *
echo *                                                                       *
echo *************************************************************************
echo.
cd "..\Service"
mc.exe NTServiceMsg.mc
if ERRORLEVEL 1 goto ERROR

cd ..\External_Deps
title OCS Inventory NG Agent for Windows - SUCCESSFUL build of required libraries 
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : All done succesufully !              *
echo *                                                                       *
echo * Enjoy OCS Inventory NG ;-)                                            *
echo *                                                                       *
echo *************************************************************************
goto END

:ERROR
title OCS Inventory NG Agent for Windows - ERROR building required libraries  !!!!
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Error while buiding required library *
echo *                                                                       *
echo * Please, fix problem before trying to build OCS Inventory NG !         *
echo *                                                                       *
echo * Here is some common errors:                                           *
echo * - Have you reviewed paths at the begining of this batch file ?        *
echo * - Have you updated Visual C++ version in cURL Makefile ?              *
echo * - Have you build ZipArchive "Release Unicode STL MD DLL" ?            *
echo *                                                                       *
echo *************************************************************************

:END