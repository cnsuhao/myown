@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  quit game_platform.exe
start telnet.exe 127.0.0.1 15007 "quit"