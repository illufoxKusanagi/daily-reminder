@echo off
REM Daily Reminder - Run Script for Windows

setlocal

set EXECUTABLE=backend\build\Release\DailyReminder.exe

if not exist "%EXECUTABLE%" (
    set EXECUTABLE=backend\build\Debug\DailyReminder.exe
)

if not exist "%EXECUTABLE%" (
    echo Error: Application not built!
    echo Please run: build.bat
    exit /b 1
)

echo ================================================
echo   Starting Daily Task Reminder
echo ================================================
echo.

cd backend\build
if exist "Release\DailyReminder.exe" (
    cd Release
    DailyReminder.exe
) else (
    cd Debug
    DailyReminder.exe
)
