@ECHO OFF
ECHO. 
SetLocal EnableDelayedExpansion

rem  quit game_entry.exe
start telnet.exe 127.0.0.1 15200-15208 "quit"

rem  quit game_member.exe
start telnet.exe 127.0.0.1 15300-15323 "quit"

rem  quit game_log.exe
start telnet.exe 127.0.0.1 15011 "quit"

rem  quit game_public.exe
start telnet.exe 127.0.0.1 15003 "quit"

rem  quit game_store.exe
start telnet.exe 127.0.0.1 15100-15103 "quit"

rem  quit game_lister.exe
start telnet.exe 127.0.0.1 15400-15403 "quit"

rem  quit game_snsstore.exe
start telnet.exe 127.0.0.1 15500-15503 "quit"

rem  quit game_snsmanager.exe
start telnet.exe 127.0.0.1 15023 "quit"

rem  quit game_sns.exe
start telnet.exe 127.0.0.1 15025 "quit"

rem  quit game_dumper.exe
taskkill /f /im game_dumper.exe &

rem  quit FsSoloHttp.exe
taskkill /f /im FsSoloHttp.exe &

rem  quit game_logincache.exe
taskkill /f /im game_logincache.exe &
