@echo off
rem curl "https://whalesgame.cloud.sensorsdata.cn/api/property/dict/upload?propertyName=coin_id&type=property&isIncrement=false&token=b13b3f9b0ed58438402d0485e40e5d23db9c65ea672f19478330be034b937b35" -k -X POST -F "file=@E:\work\XCD\trunk\server\all_src\cons\logagent\dict\capitals.txt"
rem curl "https://whalesgame.cloud.sensorsdata.cn/api/property/dict/upload?propertyName=job&type=property&isIncrement=false&token=b13b3f9b0ed58438402d0485e40e5d23db9c65ea672f19478330be034b937b35" -k -X POST -F "file=@E:\work\XCD\trunk\server\all_src\cons\logagent\dict\jobs.txt"
rem curl "https://whalesgame.cloud.sensorsdata.cn/api/property/dict/upload?propertyName=flag&type=property&isIncrement=false&token=b13b3f9b0ed58438402d0485e40e5d23db9c65ea672f19478330be034b937b35" -k -X POST -F "file=@E:\work\XCD\trunk\server\all_src\cons\logagent\dict\camp.txt"
rem curl "https://whalesgame.cloud.sensorsdata.cn/api/property/dict/upload?propertyName=task_log_type&type=property&isIncrement=false&token=b13b3f9b0ed58438402d0485e40e5d23db9c65ea672f19478330be034b937b35" -k -X POST -F "file=@E:\work\XCD\trunk\server\all_src\cons\logagent\dict\camp.txt"
set project=%1

if /i "%project%" == "" (
	set project=default
)
set dict_path=%~dp0
set dict_path=%dict_path%dict
echo dict path is %dict_path%

echo 导入货币字典
call import.bat %project% capitals.txt coin_id >> import.log

echo 导入职业字典
call import.bat %project% jobs.txt job >> import.log

echo 导入阵营字典
call import.bat %project% camp.txt flag >> import.log

echo 导入任务日志字典 
call import.bat %project% task_log_type.txt task_log_type >> import.log

echo 导入功能来源日志字典 
call import.bat %project% function_id.txt function_id >> import.log

echo 导入游戏功能日志字典 
call import.bat %project% game_func_log_type.txt game_func_log_type >> import.log

echo 导入升级日志字典 
call import.bat %project% role_upgrade_type.txt role_upgrade_type >> import.log

echo 导入惩罚日志字典 
call import.bat %project% punish_log_type.txt punish_log_type >> import.log

echo 导入玩家日志字典 
call import.bat %project% player_log_type.txt player_log_type >> import.log

echo 导入好友日志字典 
call import.bat %project% friend_log_type.txt friend_log_type >> import.log

echo 导入装备记录日志字典 
call import.bat %project% equip_log_type.txt equip_log_type >> import.log

echo 导入公会日志字典 
call import.bat %project% guild_log_type.txt guild_log_type >> import.log

echo 导入游戏玩法日志字典 
call import.bat %project% game_log_state.txt game_log_state >> import.log

echo 导入宠物坐骑日志字典 
call import.bat %project% pet_ride_log_type.txt pet_ride_log_type >> import.log

echo 导入国家系统日志字典 
call import.bat %project% nation_log_type.txt nation_log_type >> import.log

echo 导入称号日志字典 
call import.bat %project% title_log_type.txt title_log_type >> import.log

echo 导入经脉系统字典 
call import.bat %project% meridian_log_type.txt meridian_log_type >> import.log

echo 导入物品品质
call import.bat %project% color.txt item_color >> import.log
@echo on