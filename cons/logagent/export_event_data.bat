@echo off
echo ------------------import data dict begin -------------------
set projectName=%1
set dateString=%2
set outFileName=%3
set format=%4

if /i "%outFileName%" == "" (
	set outFileName=%dateString%.csv
)
if /i "%dateString%" == "" (
	echo please input query date
	goto ERR
)
if /i "%projectName%" == "" (
	set projectName=default
)
if /i "%format%" == "" (
	set format=csv
)

echo project: %projectName%
echo date: %dateString%
echo outFile:%outFileName%
echo format:%format%

set token=a0c46943512370988ee01a6f0585ae519850ad4a88ba7530ad58570467df52d2
set url=http://data.jyw.xy.com:8007/api/sql/query
set file=.\data\%fileName%.%format%
set request="%url%?project=%projectName%&token=%token%"

echo fullFile:%file%
echo request:%request%

@echo curl %request% -k -X POST -O %file% --data-urlencode "q=SELECT * FROM events where date='%dateString%'" --data-urlencode "format=%format%"
curl %request% -k -X POST -o %file% --data-urlencode "q=SELECT * FROM events where date='%dateString%'" --data-urlencode "format=%format%"

goto OK

:ERR
echo export data occur error!
echo simple: project date [outFile]
goto END

:OK
echo export data completed

:END
echo -------------------export data end -------------------------
@echo on