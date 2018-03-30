@echo off 
echo start pack data begin...

set bindir=bin
set outdir=out
set version=%1
if "%version%" == "" (
	set version=1.0.0
)

if not exist %bindir% ( 
	echo err:directory '%bindir%' not found
	goto err
)
if not exist %outdir% (mkdir %outdir%)

call which.bat 7z.exe
if %errorlevel% neq 0 ( 
	echo err:not found 7z.exe, please add 7z.exe dir to path
	goto err
)

echo version:v%version% > %bindir%\version.txt
echo pack game data...
set zipname=%date:~,4%%date:~5,2%%date:~8,2%_v%version%.zip
7z.exe a %outdir%\%zipname% %bindir%

if %errorlevel% neq 0 ( 
	echo makecab data failed...
	goto err
)

echo copy pack data completed...
exit /b 0

:err
	echo error:pack data failed....
	exit /b 1

@echo on