@echo off
echo useage: clean user port [host:localhost] [port:3306] [district]

set user=%1
set pwd=%2
set host=%3
set port=%4
set district=%5

set db_district=nx_district
set db_base=nx_base
set db_log=nx_log

if /i "%host%" == "" (
	set host=localhost
)
if /i "%port%" == "" (
	set port=3306
)

if /i "%user%" == "" (
	echo warnning: user can't is emtpy, don't execute script
)

if /i "%pwd%" == "" (
	echo warnning: pwd can't is emtpy, don't execute script
)

if /i "%district%" == "" (
	echo not set district, use default db name.
) else (
	echo set district is '%district%', db name concat district
	set db_district=%db_district%_%district%
	set db_base=%db_base%_%district%
	set db_log=%db_log%_%district%
)

echo host is "%host%"
echo port is "%port%"
echo user is "%user%"
echo pwd is "%pwd%"
echo db district name is '%db_district%'
echo db base name is '%db_base%'
echo db log name is '%db_log%'

echo gen clean sql script begin.....

set temp=temp
if not exist %temp% ( mkdir %temp% )
set script=%temp%\mysql_clean.sql
if exist %script% ( del /Q /F %script% )

echo gen clean %db_district% script ....
echo # 清除账号								>> %script%
echo use %db_district%; 					>> %script%
echo delete from dc_accounts; 				>> %script%

echo gen clean %db_base% script ....
echo. >> %script%
echo # 清除游戏数据	 						>> %script%
echo use %db_base%;	 						>> %script%

echo. >> %script%
echo # mysql_database.sql 					>> %script%
echo delete from nx_chunks; 				>> %script%
echo delete from nx_letter; 				>> %script%
echo delete from nx_pubdata; 				>> %script%
echo delete from nx_roles; 					>> %script%
echo delete from nx_changename; 			>> %script%
echo delete from nx_roles_crossing; 		>> %script%

echo. >> %script%
echo # mysql_public.sql                     >> %script%
echo delete from nx_domains;                >> %script%
echo delete from nx_guilds;                 >> %script%

echo. >> ./%script%
echo # mysql_sns.sql                        >> ./%script%
echo delete from nx_sns;                    >> ./%script%
echo delete from nx_snslog;                 >> ./%script%

echo. >> %script%
echo # mysql_sns_relation.sql               >> %script%
echo delete from nx_sns_relation;           >> %script%
echo delete from nx_sns_feed;               >> %script%
                                            
echo. >> %script%
echo # mysql_trade.sql                      >> %script%
echo delete from nx_trade;                  >> %script%
                                            
echo. >> %script%  
echo # mysql_config.sql                     >> %script%
echo delete from nx_config;               	>> %script%

echo. >> %script%
echo # nx_role_dumpinfo.sql                 >> %script%
echo delete from nx_role_dumpinfo;          >> %script%

echo gen create %db_log% script ....
echo. >> %script%
echo # 清除日志(整个库删除重新创建)         >> %script%
echo drop database %db_log%;                >> %script%
echo create database %db_log%;				>> %script%

echo gen create table nx_res_dumpinfo script....
echo. >> %script%
echo use %db_log%;							>> %script%
echo DROP TABLE IF EXISTS `nx_res_dumpinfo`;													>> %script%
echo CREATE TABLE `nx_res_dumpinfo` (                                                           >> %script%
echo   `id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,                          >> %script%
echo   `name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,                    >> %script%
echo   `type` int(11) DEFAULT NULL,                                                             >> %script%
echo   `property` longtext CHARACTER SET utf8 COLLATE utf8_bin,                                 >> %script%
echo   `status` int(11) DEFAULT NULL,                                                           >> %script%
echo   `op_name` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,                 >> %script%
echo   `op_type` int(11) DEFAULT NULL,                                                          >> %script%
echo   `op_property` longtext CHARACTER SET utf8 COLLATE utf8_bin,                              >> %script%
echo   `op_status` int(11) DEFAULT NULL,                                                        >> %script%
echo   `op_version` int(11) DEFAULT NULL,                                                       >> %script%
echo   `query_id` varchar(128) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,                >> %script%
echo   `key_id` int(11) NOT NULL AUTO_INCREMENT,                                                >> %script%
echo   PRIMARY KEY (`key_id`),                                                                  >> %script%
echo   KEY `IX_nx_res_dumpinfo_4` (`query_id`)                                                  >> %script%
echo ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;                                   >> %script%

echo gen create table game_config script ....
echo. >> %script%
echo DROP TABLE IF EXISTS `game_config`;                                                        >> %script%
echo CREATE TABLE `game_config` (                                                               >> %script%
echo   `s_id` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,                         >> %script%
echo   `s_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,                   >> %script%
echo   `n_type` int(11) DEFAULT '0',                                                            >> %script%
echo   `s_item_type` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,              >> %script%
echo   `s_state` varchar(1) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '1',           >> %script%
echo   `s_comment` varchar(255) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL,               >> %script%
echo   KEY `idx_game_config_s_id` (`s_id`) USING BTREE,                                         >> %script%
echo   KEY `idx_game_config_n_type` (`n_type`) USING BTREE                                      >> %script%
echo ) ENGINE=MyISAM DEFAULT CHARSET=utf8;                                                      >> %script%

echo gen create table game_config_type script ....
echo. >> %script%
echo DROP TABLE IF EXISTS `game_config_type`;                                                   >> %script%
echo CREATE TABLE `game_config_type` (                                                          >> %script%
echo   `n_type` int(11) DEFAULT NULL,                                                           >> %script%
echo   `s_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin DEFAULT NULL                    >> %script%
echo ) ENGINE=MyISAM DEFAULT CHARSET=utf8;                                                      >> %script%

echo gen clean sql script end....

if /i "%user%" == "" (
	echo username is empty, don't execute script.....
	goto end
)

if /i "%pwd%" == "" (
	echo pwd is empty, don't execute script.....
	goto end
)

echo execute clean script....
mysql -h%host% -P%port% -u%user% -p%pwd% <%script%
if %errorlevel% == 0 (
	echo clean completed ....
	goto end
)
:err
echo clean failed...

:end
pause
@echo on