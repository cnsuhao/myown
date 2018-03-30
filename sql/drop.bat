@echo off
if "%1" == "" (
	echo useage: drop user password [host:localhost] [port:3306] [post]
	echo 		default param use '_',eg: drop user password _ _ lihl 
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

echo clean %db_district% begin...
echo query tables into %tbls%
mysql -h%host% -P%port% -u%user% -p%pwd% -e"DROP DATABASE %db_district%;"
if %errorlevel% == 0 ( 
	echo drop %db_district% succeed 
) else ( 
	echo drop %db_district% failed[%errorlevel%] 
)

mysql -h%host% -P%port% -u%user% -p%pwd% -e"DROP DATABASE %db_base%;"
if %errorlevel% == 0 ( 
	echo drop %db_base% succeed 
) else ( 
	echo drop %db_base% failed[%errorlevel%] 
)

mysql -h%host% -P%port% -u%user% -p%pwd% -e"DROP DATABASE %db_log%;"
if %errorlevel% == 0 ( 
	echo drop %db_log% succeed 
) else ( 
	echo drop %db_log% failed[%errorlevel%] 
)

echo drop completed...
goto end

:err
echo clean failed...
exit /b 1

:end
exit /b 0

@echo on