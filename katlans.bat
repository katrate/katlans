@echo off
setlocal
set "ROOT=%~dp0"
set "SCRIPT=%ROOT%files\katlans.py"

if exist "%SCRIPT%" (
    python "%SCRIPT%" %*
    exit /b %ERRORLEVEL%
)

echo [Error] katlans.py not found at %SCRIPT%
exit /b 1
