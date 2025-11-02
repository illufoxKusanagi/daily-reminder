@echo off
REM Daily Reminder - Build Script for Windows
REM Usage: build.bat [debug|release]

setlocal EnableDelayedExpansion

REM Default build type
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=release

REM Validate build type
if /I not "%BUILD_TYPE%"=="debug" if /I not "%BUILD_TYPE%"=="release" (
    echo Error: Build type must be 'debug' or 'release'
    echo Usage: build.bat [debug^|release]
    exit /b 1
)

echo ================================================
echo   Daily Task Reminder - Build Script
echo   Build Type: %BUILD_TYPE%
echo ================================================

REM Step 1: Install dependencies
echo.
echo [1/5] Installing dependencies...
if not exist "node_modules" (
    call npm install
) else (
    echo Dependencies already installed
)

REM Step 2: Build Frontend
echo.
echo [2/5] Building Next.js frontend...
cd frontend
call npm run build
cd ..
echo Frontend build complete

REM Step 3: Build Qt Backend
echo.
echo [3/5] Building Qt backend (%BUILD_TYPE%)...
cd backend
if not exist "build" mkdir build
cd build

if /I "%BUILD_TYPE%"=="debug" (
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build . --config Debug
) else (
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --config Release
)

cd ..\..
echo Backend build complete

REM Step 4: Copy frontend to backend
echo.
echo [4/5] Copying frontend to backend...
if not exist "backend\build\frontend" mkdir backend\build\frontend
xcopy /E /I /Y frontend\out backend\build\frontend
echo Frontend copied

REM Step 5: Summary
echo.
echo ================================================
echo Build Complete!
echo ================================================
echo Build type: %BUILD_TYPE%
echo Executable: backend\build\%BUILD_TYPE%\DailyReminder.exe
echo Data will be stored in: %%LOCALAPPDATA%%\DailyReminder\
echo.
echo To run the application:
echo   run.bat
echo.
echo To package for distribution:
echo   package.bat windows
echo ================================================
