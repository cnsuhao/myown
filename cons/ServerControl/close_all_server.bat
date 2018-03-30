call close_all_game_world.bat
for /L %%i in (1,1,180) do (
	tasklist | findstr "game_world.exe"
	if errorlevel 1 goto OK
	if errorlevel 0 timeout /t 5 /nobreak
)
goto ERR

:OK
call close_other.bat
exit

:ERR
echo "Game_word procedures are not normally closed, the service failed!"
pause