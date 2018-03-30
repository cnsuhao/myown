@echo off
if "%1" == "" (
	echo useage: install user password [host:localhost] [port:3306] [post]
	echo 	default param use '_',eg: onekey user password _ _ lihl
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

echo gen create db script begin.....
set temp=temp
if not exist %temp% ( mkdir %temp% )
set script=%temp%\mysql_create.sql
if exist %script% ( del /Q /F %script% )
echo DROP DATABASE IF EXISTS `%db_district%`; 	>> %script%
echo DROP DATABASE IF EXISTS `%db_base%`;		>> %script%
echo DROP DATABASE IF EXISTS `%db_log%`;		>> %script%
echo CREATE DATABASE `%db_district%`;			>> %script%
echo CREATE DATABASE `%db_base%`;				>> %script%
echo CREATE DATABASE `%db_log%`;				>> %script%
echo gen create db script end.....

echo execute create db script.....
mysql -h%host% -P%port% -u%user% -p%pwd% <%script%
if %errorlevel% neq 0 ( 
	echo execute create script failed....
	goto err
)
echo execute create db script succeed.....

echo execute mysql_district.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_district% <mysql_district.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_district script failed....
	goto err
)
echo execute mysql_district.sql succeed.....

echo execute mysql_database.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_database.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_database script failed....
	goto err
)
echo execute mysql_database.sql succeed.....

echo execute mysql_public.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_public.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_public script failed....
	goto err
)
echo execute mysql_public.sql succeed.....

echo execute mysql_sns.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_sns.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_sns script failed....
	goto err
)
echo execute mysql_sns.sql succeed.....

echo execute mysql_sns_relation.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_sns_relation.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_sns_relation script failed....
	goto err
)
echo execute mysql_sns_relation.sql succeed.....


echo execute mysql_trade.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_trade.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_trade script failed....
	goto err
)
echo execute mysql_trade.sql succeed.....


echo execute mysql_config.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <mysql_config.sql
if %errorlevel% neq 0 ( 
	echo execute mysql_config script failed....
	goto err
)
echo execute mysql_config.sql succeed.....


echo execute nx_role_dumpinfo.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% <nx_role_dumpinfo.sql
if %errorlevel% neq 0 ( 
	echo execute nx_role_dumpinfo script failed....
	goto err
)
echo execute nx_role_dumpinfo.sql succeed.....


echo execute nx_res_dumpinfo.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_log% <nx_res_dumpinfo.sql
if %errorlevel% neq 0 ( 
	echo execute nx_res_dumpinfo script failed....
	goto err
)
echo execute nx_res_dumpinfo.sql succeed.....


echo execute game_config.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_log% <game_config.sql
if %errorlevel% neq 0 ( 
	echo execute game_config script failed....
	goto err
)
echo execute game_config.sql succeed.....


echo execute game_config_type.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_log% <game_config_type.sql
if %errorlevel% neq 0 ( 
	echo execute game_config_type script failed....
	goto err
)
echo execute game_config_type.sql succeed.....

echo install succeed
goto end

:err
echo install failed...
exit /b 1

:end
exit /b 0

rem @echo on