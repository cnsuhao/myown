@echo off
echo ================gen class define=================
call script\gen_class.bat

echo ================gen fields define=================
call script\gen_fields.bat

echo ================copy code to project=================
copy out\*.* ..\FsGame\Define

echo execute ok
@echo on
pause