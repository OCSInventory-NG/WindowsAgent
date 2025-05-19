@echo off
setlocal
signtool sign /fd SHA256 /n "FactorFX" /t http://timestamp.digicert.com Release\*.exe
signtool sign /fd SHA256 /n "FactorFX" /t http://timestamp.digicert.com Release\*.dll
endlocal