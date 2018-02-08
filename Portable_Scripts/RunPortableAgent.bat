@echo off
title OCS Inventory NG Agent for Windows - Running OneShot / Portable Agent 

echo.
echo * OCS Inventory NG agent for Windows	
echo * Running portable agent 
echo.

Rem Set your server url hee, example : http://myserver/ocsinventory
set OCS_SERVER_URL=http://myserver/ocsinventory

Rem Set your client TAG
set OCS_CLIENT_TAG="MYTAG"

Rem Don't touch this var unless you know what you are doing ?
set WORK_DIR=%~dp0

set FINAL_COMMAND=%WORK_DIR%OCSInventory.exe /server=%OCS_SERVER_URL% /work_dir=%WORK_DIR% /tag=%OCS_CLIENT_TAG%

%FINAL_COMMAND%