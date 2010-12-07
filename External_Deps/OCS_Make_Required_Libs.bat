@echo off

echo.
echo *************************************************************************
echo *                                                                       *
echo *                 OCS Inventory NG agent for Windows                    *
echo *                                                                       *
echo * Building of required libraries Zlib1, OpenSSL and cURL                *
echo *                                                                       *
echo *************************************************************************
echo.

REM ========= UPDATE CONSTANTS BELOW TO MEET YOUR CONFIGURATION NEED =========  

REM Set path to MS Visual C++
set VC_PATH=C:\Program Files\Microsoft Visual Studio 9.0\VC

REM Set path to MS Windows SDK, needed to build cURL
set WINDOWS_SDK_PATH="C:\Program Files\Microsoft SDKs\Windows\v6.0A"

REM Set path to Perl 5.6 or higher binary
set PERL_PATH=c:\xampp\perl\bin

REM Set path to Zlib sources
set ZLIB_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\zlib-1.2.5

REM Set path to OpenSSL sources
set OPENSSL_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\openssl-1.0.0c

REM Set path to cURL sources
set CURL_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\curl-7.21.2

REM Set path to tinyXML sources
SET XML_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\tinyxml

REM Set path to ZipArchive sources, for example
SET ZIP_PATH=D:\Developp\OCS Inventory NG\Bazaar\ocsinventory-windows-agent\External_Deps\ZipArchive

REM ========= DO NOT MODIFY BELOW, UNTIL YOU KNOW WHAT YOU ARE DOING =========

REM Ensure MS Visual C++ environnement is set
call "%VC_PATH%\VCVARSALL.BAT"
REM Add perl to PATH
set PATH=%PATH%;%PERL_PATH%

echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building Zlib DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%ZLIB_PATH%"
nmake -f Win32\Makefile.msc LOC="-DASMV -DASMINF" OBJA="inffas32.obj match686.obj"
if ERRORLEVEL 1 goto ERROR

Rem copy libs to use them in OCS
copy zdll.lib ..
mkdir ..\..\Release
copy zlib1.dll ..\..\Release
mkdir ..\..\Debug
copy zlib1.dll ..\..\Debug
if ERRORLEVEL 1 goto ERROR

cd ..
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building OpenSSL DLLs...             *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%OPENSSL_PATH%"
REM Configure OpenSSL for MS Visual C++
perl.exe configure VC-WIN32
if ERRORLEVEL 1 goto ERROR
REM Prepare OpenSSL build for MS Visual C++
call ms\do_ms.bat
if ERRORLEVEL 1 goto ERROR
REM Build OpenSSL
nmake -f ms\ntdll.mak
if ERRORLEVEL 1 goto ERROR
REM Test OpenSSL build
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
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building cURL DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%CURL_PATH%"
REM Disable LDAP support, not needed in OCS Inventory NG Agent
set WINDOWS_SSPI=0
REM Build cURL dll using OpenSSL Dlls and Zlib dll
nmake vc-dll-ssl-dll-zlib-dll
if ERRORLEVEL 1 goto ERROR

Rem copy libs to use them in OCS
copy "lib\release-dll-ssl-dll-zlib-dll\libcurl_imp.lib" ..
copy "lib\release-dll-ssl-dll-zlib-dll\libcurl.dll" ..\..\Release
copy "lib\release-dll-ssl-dll-zlib-dll\libcurl.dll" ..\..\Debug
if ERRORLEVEL 1 goto ERROR

cd ..
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
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : All done succesufully !              *
echo * You are now ready to build OCS Inventory NG Agent for Windows         *
echo *                                                                       *
echo * Enjoy OCS Inventory NG ;-)                                            *
echo *                                                                       *
echo *************************************************************************
goto END

:ERROR
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