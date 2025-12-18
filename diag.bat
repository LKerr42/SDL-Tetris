@echo off
echo Diagnosing...

REM idk
drmemory ^
  -ignore_kernel ^
  -no_check_leaks ^
  -logdir "C:\Users\LiamK\Documents\Uni\tetris\logs" ^
  -suppress C:\Users\LiamK\Documents\Uni\tetris\sdl.supp ^
  -- C:\Users\LiamK\Documents\Uni\tetris\tetris.exe
  
pause