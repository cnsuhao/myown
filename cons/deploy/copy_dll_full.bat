@echo off 
echo start copy dll begin...

if "%root%" == "" (
	set root=..
)

set pdb=%1

set outdir=bin
if not exist %outdir% (mkdir %outdir%)

echo copy sql...
set sql_root=%root%\..\sql
set sql_out=%outdir%\sql
if not exist %sql_out% ( mkdir %sql_out%)
if not exist %sql_out%\prepare (mkdir %sql_out%\prepare)

copy %sql_root%\* %sql_out%\ /Y
if %errorlevel% neq 0 ( 
	echo copy sql script failed...
	goto err
)
copy %sql_root%\prepare\* %sql_out%\prepare\ /Y
if %errorlevel% neq 0 ( 
	echo copy sql prepare failed...
	goto err
)

copy ..\PreCreatePlayerDataSQLFile\*.sql %sql_out%\prepare\ /Y
if %errorlevel% neq 0 ( 
	echo copy sql prepare create player failed...
	goto err
)

echo copy logagent...
xcopy %root%\logagent\* %outdir%\logagent\ /S /Y
if %errorlevel% neq 0 ( 
	echo copy logagent failed...
	goto err
)
del %outdir%\logagent\log\* /Q /F /S

echo copy logagent_xy...
echo %root%
xcopy %root%\logagent_xy\* %outdir%\logagent_xy\ /S /Y
if %errorlevel% neq 0 ( 
	echo copy logagent_xy failed...
	goto err
)
del %outdir%\logagent_xy\logs\* /Q /F /S

echo copy server controller script...
xcopy %root%\ServerControl\* %outdir%\ServerControl\ /S /Y
if %errorlevel% neq 0 ( 
	echo copy res failed...
	goto err
)
copy %root%\FsDebugger.exe .\%outdir%\FsDebugger.exe /Y
copy %root%\FsDebugger.ini .\%outdir%\FsDebugger.ini /Y
copy %root%\start64.bat .\%outdir%\start64.bat /Y
copy "%root%\单服维护（不关闭战场）.bat" ".\%outdir%\单服维护（不关闭战场）.bat" /Y
copy %root%\关闭平台服和房间服.bat .\%outdir%\关闭平台服和房间服.bat /Y
copy %root%\关闭语音.bat .\%outdir%\关闭语音.bat /Y
copy "%root%\批量维护（优先关战场）.bat" ".\%outdir%\批量维护（优先关战场）.bat" /Y
copy %root%\启动语音.bat .\%outdir%\启动语音.bat /Y
copy %root%\启动平台服和房间服.bat .\%outdir%\启动平台服和房间服.bat /Y
copy %root%\loggingserver64.bat .\%outdir%\loggingserver64.bat /Y
copy %root%\startall.bat .\%outdir%\startall.bat /Y
copy %root%\stopall.bat .\%outdir%\stopall.bat /Y
copy %root%\serverControl.exe .\%outdir%\serverControl.exe /Y

echo copy docs ...
copy %root%\绣春刀架服流程.docx .\%outdir%\绣春刀架服流程.docx /Y


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

copy .\depends\*.* %outdir%\LoggingServer\Release\x64\ /Y
if %errorlevel% neq 0 ( 
	echo copy depends dll failed...
	goto err
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
copy %dll_root%\*.dll %outdir%\		/Y
copy %dll_root%\*.exe %outdir%\		/Y
copy %dll_root%\*.ini %outdir%\		/Y

if "%pdb%" == "true" (
	echo copy dll pdb...
	copy %dll_root%\*.pdb %outdir%\		/Y
	copy %dll_root%\*.map %outdir%\		/Y
)

echo copy certs....
xcopy %dll_root%\certs %outdir%\certs\	/Y /S
if %errorlevel% neq 0 ( 
	echo copy certs failed...
	goto err
)

exit /b 0
echo copy dll completed...

:err
	echo error:copy dll failed....
	exit /b 1

@echo on