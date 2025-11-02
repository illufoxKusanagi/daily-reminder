@echo off
echo ======================================
echo Building Daily Task Reminder
echo ======================================

:: Build Frontend
echo.
echo 1. Building Frontend (Next.js)...
cd frontend
call npm install
call npm run build

if %ERRORLEVEL% NEQ 0 (
    echo X Frontend build failed!
    exit /b 1
)
echo √ Frontend build successful!

:: Build Backend
echo.
echo 2. Building Backend (Qt)...
cd ..\backend
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo X Backend build failed!
    exit /b 1
)
echo √ Backend build successful!

:: Copy frontend to backend
echo.
echo 3. Copying frontend to backend...
if not exist frontend mkdir frontend
xcopy /E /I /Y ..\..\frontend\out frontend

echo.
echo ======================================
echo √ Build Complete!
echo ======================================
echo Executable: backend\build\Release\DailyReminder.exe
echo.
echo To run: cd backend\build\Release ^&^& DailyReminder.exe
