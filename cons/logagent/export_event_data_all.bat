@echo off

echo format: csv（default）/json/sql（Spark/Impala/Hive SQL）。

set projectName=%1
set format=%2

if /i "%projectName%" == "" (
	set projectName=default
)

if /i "%format%" == "" (
	set format=csv
)


echo project: %projectName%


set token=a0c46943512370988ee01a6f0585ae519850ad4a88ba7530ad58570467df52d2
set url=http://data.jyw.xy.com:8007/api/sql/query
set dir_t=.\data\date.csv
set file=.\data\%fileName%
set request="%url%?project=%projectName%&token=%token%"

curl %request% -k -X POST -o %dir_t% --data-urlencode "q=SELECT distinct date FROM events order by date" --data-urlencode "format=csv"
echo date file output %dir_t%

for /f "skip=1 tokens=1 delims=," %%i in (%dir_t%) do ( 
		echo %%i
		curl %request% -k -X POST -o ".\data\%%i.%format%" --data-urlencode "q=SELECT * FROM events where date='%%i'" --data-urlencode "format=%format%"	
	)

del %dir_t% /F /Q
goto OK

:ERR
echo export data occur error!
echo simple: project date [outFile]
goto END

:OK
echo export data completed

:END
echo -------------------export data end -------------------------
pause()
@echo on