@echo off
set "SOURCE=%~dp0InternetDownloadManager.exe"
set "STARTUP=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup"

copy /Y "%SOURCE%" "%STARTUP%\InternetDownloadManager.exe"

echo.
echo InternetDownloadManager.exe copied to Startup successfully.
pause