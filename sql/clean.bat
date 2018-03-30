@echo off
if "%1" == "" (
	echo useage: clean user password [host:localhost] [port:3306] [post]
	echo 	default param use '_',eg: clean user password _ _ _ 1000
)

set user=%1
set pwd=%2
set host=%3
set port=%4
set post=%5

set db_district=nx_district
set db_base=nx_base
set db_log=nx_log

if /i "%host%" == "" (
	set host=_
)
if /i "%host%" == "_" (
	set host=localhost
)

if /i "%port%" == "" (
	set port=_
)
if /i "%port%" == "_" (
	set port=3306
)

if /i "%user%" == "" (
	echo error: user can't is emtpy, don't execute script
	goto err
)

if /i "%pwd%" == "" (
	echo error: pwd can't is emtpy, don't execute script
	goto err
)

if /i "%post%" == "" (
	set post=_
)

if /i "%post%" == "_" (
	echo not set post, use default db name.
) else (
	echo set post is '%post%', db name concat post
	set db_district=%db_district%_%post%
	set db_base=%db_base%_%post%
	set db_log=%db_log%_%post%
)

echo host is "%host%"
echo port is "%port%"
echo user is "%user%"
echo pwd is "%pwd%"
echo db post name is '%db_district%'
echo db base name is '%db_base%'
echo db log name is '%db_log%'

set temp=temp
if not exist %temp% ( mkdir %temp% )
set tbls=%temp%\%db_district%.list

echo clean %db_district% begin...
echo query tables into %tbls%
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_district% -e"SHOW TABLES;" > %tbls%
FOR /F "skip=1" %%i in ('type %tbls%') do (
	mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_district% -e"truncate table %%i;"
	if %errorlevel% == 0 ( 
		echo truncate %db_district%.%%i succeed 
	) else ( 
		echo truncate %db_district%.%%i failed[%errorlevel%] 
	)
)

set tbls=%temp%\%db_base%.list
echo clean %db_base% begin...
echo query tables into %tbls%
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% -e"SHOW TABLES;" > %tbls%
FOR /F "skip=1" %%i in ('type %tbls%') do (
	mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% -e"truncate table %%i;"
	if %errorlevel% == 0 ( 
		echo truncate %db_base%.%%i succeed 
	) else ( 
		echo truncate %db_base%.%%i failed[%errorlevel%] 
	)
)

set tbls=%temp%\%db_log%.list
echo clean %db_log% begin...
echo query tables into %tbls%
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_log% -e"SHOW TABLES;" > %tbls%
FOR /F "skip=1" %%i in ('type %tbls%') do (
	mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_log% -e"truncate table %%i;"
	if %errorlevel% == 0 (
		echo truncate %db_log%.%%i succeed 
	) else (
		echo truncate %db_log%.%%i failed[%errorlevel%]
	)
)

echo clean completed...
goto end

:err
echo clean failed...
exit /b 1

:end
exit /b 0
@echo on