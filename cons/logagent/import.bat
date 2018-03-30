@echo off
echo ------------------import data dict begin -------------------
set project=%1
set fileName=%2
set propName=%3
set propType=%4
set propInc=%5

if /i "%dict_path%" == "" (
	echo please set dict path
	goto ERR
)

if /i "%fileName%" == "" (
	echo please input fileName
	goto ERR
)
if /i "%propName%" == "" (
	echo please input propName
	goto ERR
)
if /i "%propType%" == "" (
	set propType=property
)
if /i "%propInc%" == "" (
	set propInc=false
)

echo project: %project%
echo fileName: %fileName%
echo propertyName: %propName%
echo dictType:%propType%
echo increment:%propInc%

set token=60baee4376f1c67de36ff72848f4f985c8b424165ccdd9d5f6468436b305b4f0
set url=http://data.jyw.xy.com:8007/api/property/dict/upload
set file=%dict_path%\%fileName%
set request="%url%?project=%project%&propertyName=%propName%&type=%propType%&isIncrement=%propInc%&token=%token%"

echo fullFile:%file%
echo request:%request%

@echo curl %request% -X POST -F "file=@%file%"
call curl %request% --X POST -F "file=@%file%"
goto OK

:ERR
echo import occur error!
echo simple: import fileName propName [propType propInc]
goto END

:OK
echo import completed

:END
echo -------------------import data dict end -------------------------
@echo on