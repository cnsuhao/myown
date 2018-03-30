//--------------------------------------------------------------------
// 文件名:		GMModule.cpp
// 内  容:		GM命令
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "GMModule.h"
#include "FsGame/Define/ClientCustomDefine.h"
#include "FsGame/Define/ServerCustomDefine.h"
#include "public/VarList.h"
#include "utils/custom_func.h"
#include "utils/util_func.h"
#include "utils/extend_func.h"
#include "utils/util_ini.h"
#include "FsGame/CommonModule/LuaScriptModule.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/CommonModule/LuaExtModule.h"
#include "utils/string_util.h"
#include "ContainerModule.h"
#ifndef FSROOMLOGIC_EXPORTS
#include "ReLoadConfigModule.h"
#endif

GMModule* GMModule::m_pGMModule = NULL;

int nx_reload_gm_config(void* state)
{
	// 获得核心指针
	IKernel* pKernel = LuaExtModule::GetKernel(state);
	// 如果指针存在，表明该模块已经创建
	if(NULL != GMModule::m_pGMModule)
	{
		GMModule::m_pGMModule->LoadGMFile(pKernel);
	}
	return 0;
}

int nx_fill_player_full_data(void* state)
{
	IKernel *pKernel = LuaExtModule::GetKernel(state);
	// 检查参数数量
	CHECK_ARG_NUM(state, nx_fill_player_full_data, 2);
	// 检查每个参数类型
	CHECK_ARG_OBJECT(state, nx_fill_player_full_data, 1);
	CHECK_ARG_STRING(state, nx_fill_player_full_data, 2);

	PERSISTID player = pKernel->LuaToObject(state, 1);
	IGameObj* pObj = pKernel->GetGameObj(player);
	// 填充背包的物品
	const char* pszItem = pKernel->LuaToString(state, 2);
	if (NULL == pObj)
	{
		pKernel->LuaPushBool(state, false);
	}
	else
	{
		// 填充表
		int nRecordCount = pObj->GetRecordCount();
		for (int i = 0; i < nRecordCount; ++i)
		{
			IRecord* pRecord = pObj->GetRecordByIndex(i);
			if (NULL != pRecord)
			{
				int nRows = pRecord->GetRows();
				int nMaxRows = pRecord->GetRowMax();
				for (int i = nRows; i < nMaxRows; ++i)
				{
					pRecord->AddRow(-1);
				}
			}
		}

		// 填充背包
		PERSISTID box = ContainerModule::m_pContainerModule->GetBoxContainer(pKernel, player, pszItem);
		if (!pKernel->Exists(box))
		{
			pKernel->LuaPushBool(state, false);
		}
		else
		{
			int free_pos_count = ContainerModule::m_pContainerModule->GetFreePosCount( pKernel, box);
			int max_amount = atoi(pKernel->GetConfigProperty(pszItem, TEXT_MAX_AMOUNT));
			// 循环保护
			LoopBeginCheck(h);
			for (int i = 0; i <= free_pos_count; i++)
			{
				LoopDoCheck(h);
				ContainerModule::m_pContainerModule->PlaceItem(pKernel, box, pszItem, max_amount, FUNCTION_EVENT_ID_GM_COMMAND);
			}

			int max_usable_pos = pKernel->GetCapacity(box);
			// 循环保护
			LoopBeginCheck(i);
			for (int i = CONTAINER_START_POS; i <= max_usable_pos; i++)
			{
				LoopDoCheck(i);
				PERSISTID item = pKernel->GetItem(box, i);
				if (!item.IsNull())
				{
					IGameObj* pItem = pKernel->GetGameObj(item);
					if (NULL == pItem)
					{
						continue;
					}

					// 填充表
					int nRecordCount = pItem->GetRecordCount();
					for (int i = 0; i < nRecordCount; ++i)
					{
						IRecord* pRecord = pItem->GetRecordByIndex(i);
						if (NULL != pRecord)
						{
							int nRows = pRecord->GetRows();
							int nMaxRows = pRecord->GetRowMax();
							for (int i = nRows; i < nMaxRows; ++i)
							{
								pRecord->AddRow(-1);
							}
						}
					}
				}
			}

			pKernel->LuaPushBool(state, true);
		}
	}

	return 1;
}

// 初始化
bool GMModule::Init( IKernel* pKernel )
{
	if (NULL == pKernel)
	{
		Assert(false);

		return false;
	}

	m_pGMModule = this;

	pKernel->AddIntCustomHook( "player", CLIENT_CUSTOMMSG_GM, GMModule::OnCustomGM );
	pKernel->AddIntCustomHook( "player", CLIENT_CUSTOMMSG_GM_MULTIBYTE, GMModule::OnCustomGM );

	// 读取GM命令级别限制的文件gm.ini
	if (!LoadGMFile( pKernel ))
	{
		Assert(false);

		return false;
	}

	DECL_LUA_EXT(nx_reload_gm_config);
	DECL_LUA_EXT(nx_fill_player_full_data);
#ifndef FSROOMLOGIC_EXPORTS
	RELOAD_CONFIG_REG("GMModule", GMModule::ReloadConfig);
#endif
	return true;
}

// 释放
bool GMModule::Shut( IKernel* pKernel )
{
	return true;
}

// 客户端请求使用GM命令
int GMModule::OnCustomGM( IKernel* pKernel, const PERSISTID& self,
						 const PERSISTID& sender, const IVarList& args )
{
	int msgid = args.IntVal(0);

	std::wstring wstrContent = L"";

	switch (msgid)
	{
	case CLIENT_CUSTOMMSG_GM: // 宽串 参数
		wstrContent = args.WideStrVal( 1 );
		break;
	case CLIENT_CUSTOMMSG_GM_MULTIBYTE: // 窄串 参数
		wstrContent = StringUtil::StringAsWideStr(args.StringVal( 1 )).c_str();
		break;
	default:	// 其他消息不处理
		return 0;
	}

	m_pGMModule->DoGMCommand( pKernel, self, wstrContent.c_str() );

	return 1;
}

// 执行GM命令
bool GMModule::DoGMCommand( IKernel* pKernel, const PERSISTID& self, const wchar_t* content )
{
	CVarList res;
	util_split_wstring( res, content, L" " );

	if ( res.GetCount() <= 0 )
	{
		return false;
	}

	fast_string cmd = util_widestr_as_string( res.WideStrVal( 0 ) ).c_str();
	if ( !CanExecute( pKernel, self, cmd.c_str() ) )
	{
		return false;
	}

	//// 分解参数
	//CVarList luaargs;
	//luaargs << self;
	//luaargs.Append( res, 1, res.GetCount() - 1 );

	//// 执行脚本
	//if ( !RunLua( pKernel, "gm.lua", cmd.c_str(), luaargs ) )
	std::string strMsg_;
	if (!RunGmCommand(pKernel, self, cmd.c_str(), res, 1, (int)res.GetCount() - 1, strMsg_))
	{
		// 执行GM命令失败!
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_3201, CVarList());

		return 0;
	}

#ifndef FSROOMLOGIC_EXPORTS
	//保存到LOG记录
	 LogModule::m_pLogModule->SaveGMLog(pKernel, self, content);
#endif // FSROOMLOGIC_EXPORTS

	return true;
}


bool GMModule::RunGmCommand(IKernel* pKernel, const PERSISTID& self, const char* cmd, const IVarList& args, int nOffset, int nCount, std::string& outMsg)
{
	Assert(NULL != cmd);

	// 分解参数
	CVarList luaargs;
	luaargs << self;
	luaargs.Append(args, nOffset, nCount);

	if ( strncmp(cmd, "x_", 2) == 0)
	{
		CVarList outargs;
		// gm_x.lua函数必须返回一个json字符串, 格式:{"content":"xxxx"}/{"content":{xxx}}/{"content":[xxxx]}
		bool bOK = RunLua(pKernel, "gm_x.lua", cmd, luaargs, 1, &outargs);
		if (bOK)
		{
			if (outargs.GetCount() == 0 || outargs.GetType(0) != VTYPE_STRING)
			{
				outMsg = "{\"error\":\"command not return value\"}";
			}
			else
			{
				outMsg = outargs.StringVal(0);
			}
		}
		else
		{
			outMsg = "{\"error\":\"command not exists or exception\"}";
		}
		return bOK;
	}

	return RunLua(pKernel, "gm.lua", cmd, luaargs);
}

// gm配置重读
void GMModule::ReloadConfig(IKernel *pKernel)
{
	m_pGMModule->LoadGMFile(pKernel);
}

// 能否执行GM命令
bool GMModule::CanExecute( IKernel* pKernel, const PERSISTID& self, const char* command )
{
//#ifndef _DEBUG
	int gmlimitlevel = m_nGMDefaultRight;

	// 查找此命令所需的限制级别, 此命令没有在GM.ini文件中填写级别限制，则需要最高级别的GM才能执行
	std::unordered_map<std::string, int>::iterator it = m_mapGMLimit.find( command );

	if ( it != m_mapGMLimit.end() )
	{
		gmlimitlevel = (*it).second;
	}

	// 当前GM的级别
	IGameObj* pSelf = pKernel->GetGameObj(self);
	if (NULL == pSelf)
	{
		return false;
	}
	int gmlevel = pSelf->QueryInt( "GameMaster" );

	// 限制级别,当前级别大于等于限制级别,可以执行此命令
	if ( gmlevel < gmlimitlevel )
	{
		// 当前级别小于限制级别，不可以执行此命令
		CVarList msg;
		::CustomSysInfo(pKernel, self, SYSTEM_INFO_ID_3202, CVarList());

		return false;
	}
//#endif
	return true;
}

// 载入GM命令配置文件
bool GMModule::LoadGMFile(IKernel* pKernel)
{
	m_mapGMLimit.clear();

	fast_string path = pKernel->GetResourcePath();

	path += "ini\\gm.ini";

	CUtilIni ini(path.c_str());

	if (!ini.LoadFromFile())
	{
		::extend_warning(pKernel, "[Error]GMModule::LoadGMFile: No found file:");
		::extend_warning(pKernel, path.c_str());

		return false;
	}

	size_t command_size = ini.GetSectionItemCount(0);

	LoopBeginCheck(a);
	for (size_t i = 0; i < command_size; ++i)
	{
		LoopDoCheck(a);
		const char* command = ini.GetSectionItemKey(0, i);

		int level = ::atoi(ini.GetSectionItemValue(0, i));

		std::unordered_map<std::string, int>::iterator it = m_mapGMLimit.find(command);

		if (it != m_mapGMLimit.end())
		{
			pKernel->Trace("GM Level Limit ERROR:Repeated!");
			pKernel->Trace(command);

			continue;
		}

		m_mapGMLimit.insert(std::make_pair(command, level));
	}

	m_nGMDefaultRight = ::atoi(ini.GetSectionItemValue(1, 0));
	return true;
}
