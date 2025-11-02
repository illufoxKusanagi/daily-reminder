@echo off
REM Build Windows executable for Daily Reminder

echo Building Daily Reminder for Windows...

REM 1. Build frontend
echo.
echo [1/4] Building frontend...
cd frontend
call npm install
call npm run build
cd ..

REM 2. Build backend with CMake
echo.
echo [2/4] Building backend...
cd backend
mkdir build 2>nul
cd build

REM Configure with CMake (assuming Qt is in PATH)
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ..

REM Build
cmake --build . --config Release

cd ..\..

REM 3. Deploy Qt dependencies
echo.
echo [3/4] Deploying Qt dependencies...
cd backend\build\Release

REM Run windeployqt to copy all Qt DLLs
windeployqt.exe backend.exe --release --no-translations

cd ..\..\..

REM 4. Create distribution folder
echo.
echo [4/4] Creating distribution package...
mkdir dist 2>nul
xcopy /E /I /Y backend\build\Release dist\DailyReminder
xcopy /E /I /Y frontend\out dist\DailyReminder\frontend

REM Create run script
(
echo @echo off
echo start "" "%%~dp0backend.exe"
) > dist\DailyReminder\DailyReminder.bat

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: dist\DailyReminder\backend.exe
echo.
echo To run: double-click dist\DailyReminder\DailyReminder.bat
echo.
echo Optional: Use Inno Setup to create installer:
echo   1. Download Inno Setup from https://jrsoftware.org/isdl.php
echo   2. Create installer script pointing to dist\DailyReminder
echo   3. Compile to create setup.exe
echo.
pause
