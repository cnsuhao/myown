@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  quit game_worldtoroom
start telnet.exe 127.0.0.1 15001 "setp SoloWorld UseRoomService false"

timeout /t 5 /nobreak

rem  quit game_world.exe
start telnet.exe 127.0.0.1 15001 "quit"