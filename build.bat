@echo off
echo Compiling...

REM Compile the program
gcc -g source.c -o source.exe -I "C:\Users\LiamK\Documents\Uni\SDL\x86_64-w64-mingw32\include" -L "C:\Users\LiamK\Documents\Uni\SDL\x86_64-w64-mingw32\lib" -l SDL3

REM Check if compilation failed
if errorlevel 1 (
    echo.
    echo failed
    pause
    exit /b
)

echo Compiled
echo Running program...
echo.

REM Run the program
source.exe