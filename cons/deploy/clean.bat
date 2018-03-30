@echo off 
echo start clean old data begin...

set olddir=bin
if exist %olddir% (
	del %olddir%\*.* /S /Q
	rd %olddir% /S /Q
)

set oldoutdir=out
if exist %oldoutdir% (
	del %oldoutdir%\*.* /S /Q
	rd %oldoutdir% /S /Q
)
echo clean old data completed...
exit /b 0

:err
	echo error:clean old data failed....
	exit /b 1

@echo on