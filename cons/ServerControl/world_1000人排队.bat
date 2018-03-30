@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem   felix server engine command
rem   author: liugang
rem   Date : 2015-08-04 13:16:00

telnet 127.0.0.1 15001 "setp SoloWorld MaxPlayers 1000"

pause