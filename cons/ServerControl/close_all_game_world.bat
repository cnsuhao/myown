@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  quit game_world.exe
start telnet.exe 127.0.0.1 15001 "quit"