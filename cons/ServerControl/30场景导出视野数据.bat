@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem   felix server engine command
rem   author: liugang
rem   Date : 2015-08-04 13:16:00

telnet 127.0.0.1 15301 "method SoloMember DumpScene 30 vl"

pause