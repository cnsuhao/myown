@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  connect game_worldtoroom
start telnet.exe 127.0.0.1 15001 "setp SoloWorld UseRoomService true"