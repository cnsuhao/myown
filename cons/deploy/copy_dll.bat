@echo off 
echo start copy dll begin...

if "%root%" == "" (
	set root=..
)

set pdb=%1

set outdir=bin
if not exist %outdir% (mkdir %outdir%)

echo copy loggingserver64...
set logsrv_root=%root%\LoggingServer\Release\x64
xcopy %logsrv_root%\*.dll %outdir%\LoggingServer\Release\x64\ /Y /S
if %errorlevel% neq 0 ( 
	echo copy loggingserver64 dll failed...
	goto err
)
xcopy %logsrv_root%\*.exe %outdir%\LoggingServer\Release\x64\ /Y /S
if %errorlevel% neq 0 ( 
	echo copy loggingserver64 exe failed...
	goto err
)

copy .\depends\*.* %outdir%\LoggingServer\Release\x64\ /Y
if %errorlevel% neq 0 ( 
	echo copy depends dll failed...
	goto err
)

if "%pdb%" == "true" (
	echo copy loggingserver64 pdb...
	xcopy %logsrv_root%\*.pdb %outdir%\LoggingServer\Release\x64\ /Y /S
	if %errorlevel% neq 0 ( 
		echo copy loggingserver64 pdb failed...
		goto err
	)
) else (
	echo don't copy loggingserver64 pdb...
)

echo copy dll...
set outdir=%outdir%\Release64
if not exist %outdir% (mkdir %outdir%)
copy .\depends\*.* %outdir%\ /Y
if %errorlevel% neq 0 ( 
	echo copy depends to dll release64 failed...
	goto err
)

set dll_root=%root%\Release64
copy %dll_root%\FsGmcc.dll %outdir%\		/Y
copy %dll_root%\FsCharge.dll %outdir%\		/Y
copy %dll_root%\FsGame.dll %outdir%\		/Y
copy %dll_root%\FsPubLogic.dll %outdir%\	/Y
copy %dll_root%\FsSnsLogic.dll %outdir%\	/Y

if "%pdb%" == "true" (
	echo copy dll pdb...
	copy %dll_root%\FsGmcc.pdb %outdir%\	/Y
	copy %dll_root%\FsCharge.pdb %outdir%\	/Y
	copy %dll_root%\FsGame.pdb %outdir%\	/Y
	copy %dll_root%\FsPubLogic.pdb %outdir%\	/Y
	copy %dll_root%\FsSnsLogic.pdb %outdir%\	/Y
	
	copy %dll_root%\FsGmcc.map %outdir%\		/Y
	copy %dll_root%\FsCharge.map %outdir%\		/Y
	copy %dll_root%\FsGame.map %outdir%\		/Y
	copy %dll_root%\FsPubLogic.map %outdir%\	/Y
	copy %dll_root%\FsSnsLogic.map %outdir%\	/Y	
)
exit /b 0
echo copy dll completed...

:err
	echo error:copy dll failed....
	exit /b 1

@echo on