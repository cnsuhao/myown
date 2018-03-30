@echo off

setlocal

set "LOG_AGENT_BAT_DIR=%~dp0"
set "LOG_AGENT_HOME=%LOG_AGENT_BAT_DIR%\..\"
set "LOG_AGENT_LIB=%LOG_AGENT_HOME%\lib\log_agent-1.0-SNAPSHOT.jar"
set "LOG_AGENT_LOG_DIR=%LOG_AGENT_HOME%\log"

if not exist "%LOG_AGENT_HOME%\bin\setenv.bat" goto checkSetenvHome
call "%LOG_AGENT_HOME%\bin\setenv.bat"
goto setenvDone
:checkSetenvHome
if exist "%LOG_AGENT_HOME%\bin\setenv.bat" call "%LOG_AGENT_HOME%\bin\setenv.bat"
:setenvDone

if not "%SA_LOG_AGENT_CONFIG_FILE%" == "" goto gotConfigFile
set "SA_LOG_AGENT_CONFIG_FILE=%LOG_AGENT_HOME%\logagent.conf"
:gotConfigFile

set "LOG_AGENT_MAIN_CLASS=com.sensorsdata.analytics.tools.logagent.LogAgentMain"
set CMD_LINE_ARGS=%*

set _EXECJAVA=java
%_EXECJAVA% -server -classpath "%LOG_AGENT_LIB%" -Dsa.root.logger=DEBUG,DRFA,console -Dsa.log.dir="%LOG_AGENT_LOG_DIR%" -Dsa.log.file=logagent.log -Dfile.encoding=UTF-8 -Xms256m -Xmx256m -XX:+UseParallelGC -XX:ParallelGCThreads=4 -XX:+UseParallelOldGC -XX:YoungGenerationSizeIncrement=20 -XX:TenuredGenerationSizeIncrement=20 %LOG_AGENT_MAIN_CLASS% %CMD_LINE_ARGS%
pause
