@echo off
REM Daily Reminder - Package Script for Windows
REM Creates distributable packages
REM Usage: package.bat

setlocal

echo ================================================
echo   Daily Task Reminder - Package Script
echo   Platform: Windows
echo ================================================

REM Check if build exists
if not exist "backend\build\Release\DailyReminder.exe" (
    if not exist "backend\build\Debug\DailyReminder.exe" (
        echo Error: Application not built!
        echo Please run: build.bat release
        exit /b 1
    )
)

cd backend\build

echo.
echo [Windows Packaging]

REM Check for cpack
where cpack >nul 2>&1
if errorlevel 1 (
    echo Error: cpack not found. Install CMake.
    exit /b 1
)

REM Create NSIS installer
echo Creating Windows installer...
cpack -G NSIS
if errorlevel 0 echo Installer created

REM Create portable ZIP
echo Creating portable ZIP...
cpack -G ZIP
if errorlevel 0 echo Portable ZIP created

REM Run windeployqt if available
where windeployqt >nul 2>&1
if not errorlevel 1 (
    echo.
    echo Running windeployqt to bundle Qt libraries...
    if exist "Release\DailyReminder.exe" (
        windeployqt Release\DailyReminder.exe
    ) else (
        windeployqt Debug\DailyReminder.exe
    )
    echo Qt libraries bundled
)

echo.
echo ================================================
echo Packaging Complete!
echo ================================================
echo Packages created in: backend\build\
dir *.exe *.zip 2>nul
echo ================================================

cd ..\..
