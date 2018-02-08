@echo off
title OCS Inventory NG Agent for Windows - Cleaning OneShot / Portable Agent 

echo.
echo * OCS Inventory NG agent for Windows	
echo * Cleaning portable agent 
echo.

Rem Don't touch this var unless you know what you are doing ?
set WORK_DIR=%~dp0

Rem Folders
rmdir /S /Q %WORK_DIR%download

Rem Files
del %WORK_DIR%history
del %WORK_DIR%ocsinventory.dat
del %WORK_DIR%ocsinventory.ini
del %WORK_DIR%last_state