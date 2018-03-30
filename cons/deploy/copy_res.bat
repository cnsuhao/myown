@echo off 
echo start copy resource begin...

set all_res=%1

if "%root%" == "" (
	set root=..
)

set outdir=bin
if not exist %outdir% (mkdir %outdir%)

echo copy game server res...
xcopy %root%\res\* %outdir%\res\ /S /Y
if %errorlevel% neq 0 ( 
	echo copy res failed...
	goto err
)

echo copy lua res
xcopy %root%\Lua %outdir%\Lua\ /S /Y
if %errorlevel% neq 0 ( 
	echo copy lua res failed...
	goto err
)

if "%all_res%" neq "true" (
	del %outdir%\res\game.ini /Q /F
	del %outdir%\res\game_charge.properties /Q /F
	del %outdir%\res\game_gmcc.properties /Q /F
	del %outdir%\res\log4plush.properties /Q /F
	del %outdir%\res\loggingserver.properties /Q /F
)

echo copy resource completed...
exit /b 0

:err
	echo error:copy res failed....
	exit /b 1

@echo on