@echo off
SetLocal EnableDelayedExpansion
if "%1" == "" (
	echo useage: prepare user password [host:localhost] [port:3306] [post] [district]
	echo 		default param use '_',eg: prepare user password _ _ lihl 
)

set user=%1
set pwd=%2
set host=%3
set port=%4
set post=%5
set district=%6

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
	echo warnning: not set post, use default database
) else (
	echo set post is '%post%', db name concat post
	set db_district=%db_district%_%post%
	set db_base=%db_base%_%post%
	set db_log=%db_log%_%post%
)

echo import nx_domains.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% < prepare\nx_domains.sql
if %errorlevel% == 0 ( 
	echo import nx_domains.sql succeed 
) else ( 
	echo import nx_domains.sql failed[%errorlevel%] 
	goto err
)

echo import nx_roles.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% < prepare\nx_roles.sql
if %errorlevel% == 0 ( 
	echo import nx_roles.sql succeed 
) else ( 
	echo import nx_roles.sql failed[%errorlevel%] 
	goto err
)

echo import nx_sns.sql...
mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% < prepare\nx_sns.sql
if %errorlevel% == 0 ( 
	echo import nx_sns.sql succeed 
) else ( 
	echo import nx_sns.sql failed[%errorlevel%] 
	goto err
)

if /i "%district%" neq "" (
	echo modify table nx_domains data...
	mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% -e"UPDATE nx_domains SET name=concat(left(name, length(name)-7),%district%);"
	if %errorlevel% == 0 ( 
		echo modify table nx_domains succeed 
	) else ( 
		echo modify table nx_domains failed[%errorlevel%] 
		goto err
	)

	echo modify table nx_roles data...
	mysql -h%host% -P%port% -u%user% -p%pwd% -D%db_base% -e"UPDATE nx_roles SET server_id=%district%;"
	if %errorlevel% == 0 ( 
		echo modify table nx_roles succeed 
	) else ( 
		echo modify table nx_roles failed[%errorlevel%] 
		goto err
	)
)

echo import prepare completed...
goto end

:err
echo import prepare failed...
exit /b 1

:end
exit /b 0
@echo on