@echo off 
echo start deploy game begin...

set root=%1
set version=%2
if "%root%" == "" (
 set root=..
)

call clean.bat
call svnupdate.bat

echo check 7z.exe setup
call which.bat 7z.exe
if %errorlevel% == 0 ( 
	echo 7z.exe in path
) else (
	echo err:7z.exe not in path
	goto err
)

call copy_res.bat true
if %errorlevel% neq 0 ( 
	goto err
)

call copy_dll_full.bat %debug%
if %errorlevel% neq 0 ( 
	goto err
)

call pack.bat %version%
if %errorlevel% neq 0 ( 
	echo makecab data failed...
	goto err
)

echo deploy game completed...
exit /b 0

:err
	echo error:deploy game failed....
	exit /b 1

@echo on