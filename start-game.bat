@echo off
@REM set PATH=G:\nodejs;%PATH%

echo ========================================
echo   Pokemon MUD - Starting...
echo ========================================
echo.

echo [1/3] Compiling backend...
cd /d "%~dp0server"
node node_modules\typescript\bin\tsc 2>nul
if %errorlevel% neq 0 (
    echo Compile failed! Check server dir.
    pause
    exit /b 1
)
echo        Done.

echo [2/3] Starting backend on port 4010...
start "PokemonMUD-Backend" cmd /k "cd /d %~dp0server && node dist\index.js"
timeout /t 2 /nobreak >nul

echo [3/3] Starting frontend on port 5173...
start "PokemonMUD-Frontend" cmd /k "cd /d %~dp0client && node node_modules\vite\bin\vite.js --host"
timeout /t 3 /nobreak >nul

start http://localhost:5173

echo.
echo ========================================
echo   Game started! Open http://localhost:5173
echo   Close backend/frontend windows to stop.
echo ========================================
echo.
pause
