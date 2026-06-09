@echo off
setlocal
set "SCRIPT=%~dp0katlans.py"
if not exist "%SCRIPT%" (
    echo [Error] %SCRIPT% not found
    exit /b 1
)
python "%SCRIPT%" %*
exit /b %ERRORLEVEL%
