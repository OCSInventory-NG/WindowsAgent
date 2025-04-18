@echo off
setlocal enableextensions enabledelayedexpansion

title OCS Inventory NG Agent for Windows - Building required libraries
echo.
echo *************************************************************************
echo *                 OCS Inventory NG Agent for Windows                   *
echo *                      Building required libraries                      *
echo *************************************************************************
echo.

rem ======= Configuration des chemins =======
set VC_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build
set PERL_PATH=C:\Strawberry\perl\bin
set OCSAGENT_PATH=C:\Users\devel\Documents\GitHub\WindowsAgent
set EXT_DEPS=%OCSAGENT_PATH%\External_Deps

set ZLIB_PATH=%EXT_DEPS%\zlib-1.3.1
set OPENSSL_PATH=%EXT_DEPS%\openssl-1.1.1w
set CURL_PATH=%EXT_DEPS%\curl-7.88.1
set TINYXML_PATH=%EXT_DEPS%\tinyxml
set ZIPARCHIVE_PATH=%EXT_DEPS%\ZipArchive

rem ======= Initialisation de l'environnement Visual Studio =======
call "%VC_PATH%\vcvarsall.bat" x64

rem ======= Ajout de Perl au PATH =======
set PATH=%PATH%;%PERL_PATH%

rem ======= Compilation de Zlib =======
title OCS Inventory NG Agent for Windows - Building Zlib DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building Zlib DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd /d "%ZLIB_PATH%"
nmake /f win32\Makefile.msc clean
nmake /f win32\Makefile.msc
if ERRORLEVEL 1 goto ERROR
copy zdll.lib %EXT_DEPS%
copy zlib1.dll %OCSAGENT_PATH%\Release

rem ======= Compilation d'OpenSSL =======
title OCS Inventory NG Agent for Windows - Building OpenSSL DLLs...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building OpenSSL DLLs...             *
echo *                                                                       *
echo *************************************************************************
echo.
cd /d "%OPENSSL_PATH%"
perl Configure VC-WIN64A no-asm --prefix=%OPENSSL_PATH%\out
nmake clean
nmake
nmake install
if ERRORLEVEL 1 goto ERROR
copy out\lib\libssl.lib %EXT_DEPS%
copy out\lib\libcrypto.lib %EXT_DEPS%
copy out\bin\libcrypto-1_1-x64.dll %OCSAGENT_PATH%\Release
copy out\bin\libssl-1_1-x64.dll %OCSAGENT_PATH%\Release

set INCLUDE=%OPENSSL_PATH%\out\include;%ZLIB_PATH%;%INCLUDE%
set LIB=%OPENSSL_PATH%\out\lib;%ZLIB_PATH%;%LIB%

rem ======= Compilation de cURL =======
title OCS Inventory NG Agent for Windows - Building cURL DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building cURL DLL...                 *
echo *                                                                       *
echo *************************************************************************
echo.
cd /d "%CURL_PATH%\winbuild"
nmake clean
nmake /f Makefile.vc WINBUILD_ACKNOWLEDGE_DEPRECATED=yes mode=dll VC=17 ENABLE_SSPI=no ENABLE_IDN=no ENBALE_IPV6=yes WITH_SSL=dll WITH_ZLIB=dll WITH_DEVEL=%OPENSSL_PATH%\out SSL_LIBS="libcrypto.lib libssl.lib"
if ERRORLEVEL 1 goto ERROR
copy ..\builds\libcurl-vc17-x64-release-dll-ssl-dll-zlib-dll-ipv6\lib\libcurl.lib %EXT_DEPS%
copy ..\builds\libcurl-vc17-x64-release-dll-ssl-dll-zlib-dll-ipv6\bin\libcurl.dll %OCSAGENT_PATH%\Release
if ERRORLEVEL 1 goto ERROR

rem ======= Compilation de ZipArchive =======
title OCS Inventory NG Agent for Windows - Building ZipArchive DLL...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Building ZipArchive DLL...           *
echo *                                                                       *
echo *************************************************************************
echo.
cd /d "%ZIPARCHIVE_PATH%"
Rem Fix ZipArchive DLL config for MS Visual C++ with lastest Service Pack
perl.exe -pi.bak -e "s#_CRT_RAND_S;%%\(PreprocessorDefinitions\)</PreprocessorDefinitions>#_CRT_RAND_S;_BIND_TO_CURRENT_VCLIBS_VERSION;%%\(PreprocessorDefinitions\)</PreprocessorDefinitions>#g" ZipArchive.vcxproj
devenv ZipArchive.sln /Build "Release Unicode STL MD DLL|x64"
if ERRORLEVEL 1 goto ERROR
cd "x64\Release Unicode STL MD DLL"
Rem copy libs to use them in OCS
copy ZipArchive.lib %EXT_DEPS%
copy ZipArchive.dll %OCSAGENT_PATH%\Release
if ERRORLEVEL 1 goto ERROR

cd %EXT_DEPS%

title OCS Inventory NG Agent for Windows - Building service message file...
echo.
echo *************************************************************************
echo *                                                                       *
echo * Preparing for OCS Inventory NG : Compiling service message file...    *
echo *                                                                       *
echo *************************************************************************
echo.
cd "%OCSAGENT_PATH%\Service"
mc.exe NTServiceMsg.mc
if ERRORLEVEL 1 goto ERROR

cd %EXT_DEPS%

rem ======= Fin du script =======
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
echo * - Have you reviewed paths at the beginning of this batch file ?       *
echo * - Have you updated Visual C++ version in cURL Makefile ?              *
echo *                                                                       *
echo *************************************************************************
cd %EXT_DEPS%
exit /b 1

:END
endlocal
