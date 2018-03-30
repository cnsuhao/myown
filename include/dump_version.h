//-------------------------------------------------------------------
// 文 件 名：dump_version.h
// 内    容：
// 说    明：
// 创建日期：2014年2月24日
// 创 建 人：Liangxk
// 版权所有：苏州蜗牛电子有限公司
//-------------------------------------------------------------------


#if !defined __DUMP_VERSION_H__
#define __DUMP_VERSION_H__

/*
AAAA.BBBB
AA: 主版本号		转储管理器变动后变化
BB: 次要版本号	其他模块变化后变化
*/

//#define CURRENT_TOOLSET_VERSION "1000.1000"

/// 2014.4.16
/// 增加解析备份库->统计特定容器内物品数量的功能
/// dumptool_backuprole.cpp
/// 1000.1001

/// 2014.4.16
///	修正MemFile中如果写入大字符串(会直接写入文件, 而不是放入缓存), 则会导致memfile的内存部分内容和大字符串顺序颠倒的bug 
/// MemFile.cpp
/// bool MemFile::Write
/// 1001.1001

/// 2014.4.20
/// 修正构建的对象game_object根据子对象名称查找对象时逻辑错误(优先根据名字查找, 若没有名字则根据子对象的script查找)
/// game_object.cpp
/// IOBJECT* object_t::GetChildByName
/// 1002.1001


/// 2014.5.4
/// 修正解析二进制数据时, 如果没有名称的数据则可能导致异常的bug
/// ReadBin.cpp
/// void ReadBin::OnReadRole
/// 1003.1001


/// 2014.5.5
/// 修正二进制数据若损坏, 构建对象时可能失败, 析构时发生异常错误 导致宕机的bug
/// game_object.cpp 
/// game_object::DeleteObj

/// 2014.7.10
/// 增加64位版本，修改转储崩溃时dump文件的处理
/// 1005.1000

/// 2015.7.29
/// 增加DumpLoader启动时可以指定执行manage_command.ini中的命令
/// 1006.1000


#define CURRENT_TOOLSET_VERSION "1006.1001"




#endif // __DUMP_VERSION_H__
