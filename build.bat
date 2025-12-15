@echo off
echo Compiling...

setlocal EnableDelayedExpansion

set SOURCES=

for %%f in (src\*.c) do (
    set SOURCES=!SOURCES! %%f
)

REM Compile the program
REM 
REM -fstack-protector-all
gcc -g -O0 !SOURCES! resources.o -o tetris.exe ^
    -ffunction-sections -fdata-sections -Wl,--subsystem,windows ^
    -I "C:\Users\LiamK\Documents\Uni\SDL\x86_64-w64-mingw32\include" ^
    -I "C:\Users\LiamK\Documents\Uni\SDL3_ttf\x86_64-w64-mingw32\include" ^
    -I "C:\Users\LiamK\Documents\Uni\SDL3_image\x86_64-w64-mingw32\include" ^
    -I "C:\Users\LiamK\Documents\Uni\tetris\src\include" ^
    -L "C:\Users\LiamK\Documents\Uni\SDL\x86_64-w64-mingw32\lib" ^
    -L "C:\Users\LiamK\Documents\Uni\SDL3_ttf\x86_64-w64-mingw32\lib" ^
    -L "C:\Users\LiamK\Documents\Uni\SDL3_image\x86_64-w64-mingw32\lib" ^
    -l SDL3 ^
    -l SDL3_ttf ^
    -l SDL3_image

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
tetris.exe  