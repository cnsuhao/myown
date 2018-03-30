call quit_platform.bat
for /L %%i in (1,1,180) do (
	tasklist | findstr "game_platform.exe"
	if errorlevel 1 goto OK
	if errorlevel 0 timeout /t 5 /nobreak
)
goto ERR

:OK
call close_room.bat
exit

:ERR
echo "game_platform procedures are not normally closed, the service failed!"
pause