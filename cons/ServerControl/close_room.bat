@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  quit game_room.exe
start telnet.exe 127.0.0.1 15600-15632 "quit"

rem  quit game_log.exe
start telnet.exe 127.0.0.1 15011 "quit"