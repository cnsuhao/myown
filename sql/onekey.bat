@echo off
if "%1" == "" (
	echo useage: onekey user password [host:localhost] [port:3306] [post] [districtid]
	echo 	default param use '_',eg: onekey user password _ _ _ 1000
)

call install.bat %1 %2 %3 %4 %5
if %errorlevel% neq 0 (
	echo install failed...
	goto err
)

call prepare.bat %1 %2 %3 %4 %5 %6
if %errorlevel% neq 0 (
	echo prepare data failed...
	goto err
)

goto end

:err
echo onekey install failed...
exit /b 1

:end
echo onekey install completed...
exit /b 0
@echo on