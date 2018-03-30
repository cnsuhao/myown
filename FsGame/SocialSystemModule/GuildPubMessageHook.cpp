//--------------------------------------------------------------------
// 文件名:		GuildPubMessageHook.cpp
// 内  容:		公会系统-处理公共服务器的消息
// 说  明:		
// 创建日期:	2014年06月10日
// 整理日期:	2014年06月10日
// 创建人:		  ( )
// 修改人:     
//    :	    
//--------------------------------------------------------------------

#include "GuildModule.h"
#include "../Define/PubDefine.h"
#include "../Define/GuildMsgDefine.h"
#include "../Define/CommandDefine.h"
#include "../Define/GameDefine.h"
#include "utils/util_func.h"
#include "utils/custom_func.h"
#include "utils/string_util.h"
#include "FsGame/CommonModule/LogModule.h"
#include "FsGame/SocialSystemModule/SystemMailModule.h"
#include "Define/ServerCustomDefine.h"
#include "ChatModule.h"
#include "SystemFunctionModule/RewardModule.h"
#include "SystemFunctionModule/CapitalModule.h"
#include "Define/SnsDefine.h"

// 处理公共数据服务器下传的消息
int GuildModule::OnPublicMessage(IKernel* pKernel, const PERSISTID& self,
                           const PERSISTID& sender, const IVarList& args)
{
    const char* spaceName = args.StringVal(0);
    const wchar_t* logicName = args.WideStrVal(1);
    // 不是公会的消息，不需要处理
    if (::strcmp(spaceName, PUBSPACE_GUILD) != 0
        || ::wcscmp(logicName, GUILD_LOGIC) != 0)
    {
        return 0;
    }

    int msgid = args.IntVal(2);
    switch (msgid)
    {
    case PS_GUILD_MSG_CREATE:// 创建公会结果回复
        {
            const wchar_t* guildName = args.WideStrVal(3);
            const wchar_t* playerName = args.WideStrVal(4);
            int result = args.IntVal(5);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_CREATE_RESULT << guildName << result);

			
        }
        break;
    case PS_GUILD_MSG_APPLY_JOIN:// 加入公会请求结果
        {
            const wchar_t* guildName = args.WideStrVal(3);
            const wchar_t* playerName = args.WideStrVal(4);
            int result = args.IntVal(5);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_APPLY_JOIN_RESULT << guildName << result);
        }
        break;
    case PS_GUILD_MSG_CANCEL_APPLY:// 取消申请加入公会
        {
            const wchar_t* guildName = args.WideStrVal(3);
            const wchar_t* playerName = args.WideStrVal(4);
            int result = args.IntVal(5);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_CANCEL_APPLY_RESULT << guildName << result);
        }
        break;
	case PS_GUILD_MSG_ACCEPT_JOIN:// 接受申请的结果
		{
			const wchar_t* guildName = args.WideStrVal(3);
			const wchar_t* playerName = args.WideStrVal(4);
			const wchar_t* applyName = args.WideStrVal(5);
			int onLine = args.IntVal(6);
			int result = args.IntVal(7);
            int position = args.IntVal(8);
            int sex = args.IntVal(9);
            int level = args.IntVal(10);
            int career = args.IntVal(11);
            int curContrib = args.IntVal(12);
            int maxContrib = args.IntVal(13);
            int fight = args.IntVal(14);
			int offlinehours = args.IntVal(15);
			int vip = args.IntVal(16);

            // 通知其申请加入公会的结果
            // 如果申请者不在线，则在其上线时再通知（记录保存在数据库中）
			if (onLine == ONLINE)
			{
                pKernel->CommandByName(applyName, CVarList() << COMMAND_GUILD_BY_NAME 
                    << SS_GUILD_ACCEPT_JOIN_RESULT << result << guildName);
			}
			if (result = GUILD_MSG_REQ_SUCC){

				const ConfigMailItem& mail = m_pMailModule->GetConfigMail(pKernel, "guild_join");

				// 邮件的内容
				std::wstring mail_content = SystemMailModule::ComposeMailContent(pKernel,
					mail.mail_content.c_str(), CVarList() << guildName);

				// 邮件通知，成功加入公会
				m_pMailModule->SendMailToPlayer(pKernel, mail.sender.c_str(), applyName,
					mail.mail_title.c_str(), mail_content.c_str(), mail.items);
			}



			// 通知审批者结果
			pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
				<< SS_GUILD_ACCEPT_RESULT << result << guildName << position 
                << applyName << sex << level << career << curContrib << maxContrib 
                << fight << onLine << offlinehours);

			// 输出加入提示
			if (result == GUILD_MSG_REQ_SUCC)
			{
				// 修改组织加入提示(lihl 2017-09-11)
				CVarList msg;
				msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_MEMBER_JOIN_BROADCAST << applyName << vip;
				GuildModule::BroadCastGuild(pKernel, guildName, msg);
			}
		}
		break;
    case PS_GUILD_MSG_REFUSE_JOIN:// 拒绝了加入公会的申请
        {
            // 通知审批者拒绝了加入公会的申请
            const wchar_t *playerName = args.WideStrVal(3);
            const wchar_t *applyName = args.WideStrVal(4);
            const wchar_t *guildName = args.WideStrVal(5);
            int result = args.IntVal(6);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_REFUSE_JOIN << result << applyName << guildName);
        }
        break;
    case PS_GUILD_MSG_CHANGE_NOTICE:// 修改公告结果
        {
            pKernel->CommandByName(args.WideStrVal(4), CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_NOTICE_RESULT << args.IntVal(5) << args.WideStrVal(6));
        }
        break;
	case PS_GUILD_CHANGE_DECLARATION:// 修改宣言结果
		{
			pKernel->CommandByName(args.WideStrVal(4), CVarList() << COMMAND_GUILD_BY_NAME 
				<< SS_GUILD_DECLARATION_RESULT << args.IntVal(5) << args.WideStrVal(6));
		}
		break;
    case PS_GUILD_MSG_QUIT:// 退出公会回复
        {
            const wchar_t *guildName = args.WideStrVal(3);
            const wchar_t *playerName = args.WideStrVal(4);
			const wchar_t *captainName =  args.WideStrVal(6);
            int result = args.IntVal(5);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_QUIT_RESULT << result << guildName);
			if(captainName !=playerName )
			{
				pKernel->CommandByName(captainName, CVarList() << COMMAND_GUILD_BY_NAME
					<< SS_GUILD_BE_PROMOTION << GUILD_POSITION_CAPTAIN );
			}
        }
        break;
    case PS_GUILD_MSG_FIRE:// 踢出公会回复
        {
            // 如果踢出成功，需要给被踢玩家发邮件
            // 如果被踢玩家在线，直接设置其退出公会
            // 如果被踢玩家不在线，则等其上线时设置其退出公会
            const wchar_t* guildName = args.WideStrVal(3);
            const wchar_t* playerName = args.WideStrVal(4);
            const wchar_t* memberName = args.WideStrVal(7);
            int result = args.IntVal(5); 

            if (result == GUILD_MSG_REQ_SUCC)
            {
                int onLine = args.IntVal(6);
                if (onLine == ONLINE)
                {
                    // 通知玩家退出了公会
                    pKernel->CommandByName(memberName, CVarList() << COMMAND_GUILD_BY_NAME 
                        << SS_GUILD_LEAVE<<guildName);
                }

                // 发邮件
				const ConfigMailItem& mail = SystemMailModule::GetConfigMail(pKernel, MAIL_GUILD_FIRE);
                std::wstring mailContent = SystemMailModule::ComposeMailContent(pKernel, mail.mail_content.c_str(), 
                    CVarList() << playerName << guildName);
                SystemMailModule::SendMailToPlayer(pKernel, mail.sender.c_str(), memberName, 
                    mail.mail_title.c_str(), mailContent.c_str(), "");
            }

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_FIRE_RESULT << result << memberName << guildName);
        }
        break;
    case PS_GUILD_MSG_DISMISS:// 解散公会回复
        {
            const wchar_t* guildName = args.WideStrVal(3);
			int nIMRoomId	= args.IntVal(4);
            
			// 记录公会日志add by   (2015-04-23)
			//注:系统自动解散公会
			//GuildLog log;
			//log.account            = "0";
			//log.playerName         = L"System";
			//log.operateType        = LOG_GUILD_DISMISS;
			//log.guildName          = guildName;
			//LogModule::m_pLogModule->SaveGuildLog(pKernel, PERSISTID(), log);
			::extend_warning(LOG_TRACE, "guild '%s' dismissed by system", StringUtil::WideStrAsString(guildName).c_str());
        }
        break;
    case PS_GUILD_MSG_DONATE:
        {
            const wchar_t *guildName = args.WideStrVal(3);
            const wchar_t *playerName = args.WideStrVal(4);
            int result = args.IntVal(5);
			CVarList msg;
			msg << COMMAND_GUILD_BY_NAME
				<< SS_GUILD_DONATE_RESULT << result << guildName << args.IntVal(6) << args.IntVal(7);
			

            pKernel->CommandByName(playerName, msg );
        }
        break;
    case PS_GUILD_MSG_PROMOTION:
        {
            const wchar_t *guildName = args.WideStrVal(3);
            const wchar_t *playerName = args.WideStrVal(4);
            int result = args.IntVal(5);
            const wchar_t *applyName = args.WideStrVal(6);
            int position = args.IntVal(7);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME
                << SS_GUILD_PROMOTION_RESULT << result << applyName << position << guildName);

			// 通知被晋升者
			if (result == GUILD_MSG_REQ_SUCC)
			{
				pKernel->CommandByName(applyName, CVarList() << COMMAND_GUILD_BY_NAME
					<< SS_GUILD_BE_PROMOTION << position);
			}
        }
        break;
    case PS_GUILD_MSG_DEMOTION:
        {
            const wchar_t *guildName = args.WideStrVal(3);
            const wchar_t *playerName = args.WideStrVal(4);
            int result = args.IntVal(5);
            const wchar_t *applyName = args.WideStrVal(6);
            int position = args.IntVal(7);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME
                << SS_GUILD_DEMOTION_RESULT << result << applyName << position << guildName);

			// 通知被降职者
			if (result == GUILD_MSG_REQ_SUCC)
			{
				pKernel->CommandByName(applyName, CVarList() << COMMAND_GUILD_BY_NAME
					<< SS_GUILD_BE_DEMOTION << position);
			}
        }
        break;
    case PS_GUILD_MSG_GUILD_GONE:// 通知成员公会解散
        {
            const wchar_t* guildName = args.WideStrVal(3);
            //const wchar_t* playerName = args.WideStrVal(4);
            int onLine = args.IntVal(5);
            const wchar_t* memberName = args.WideStrVal(6);
            if (onLine == ONLINE)
            {
                pKernel->CommandByName(memberName, CVarList() << COMMAND_GUILD_BY_NAME 
                    << SS_GUILD_LEAVE << guildName);
            }

            //// 发邮件通知玩家公会解散了
            //const ConfigMailItem& mail = SystemMailModule::GetConfigMail(pKernel, "guild_dismiss");
            //std::wstring mailContent = SystemMailModule::ComposeMailContent(pKernel, mail.mail_content.c_str(), 
            //    CVarList() << guildName << playerName);
            //SystemMailModule::SendMailToPlayer(pKernel, mail.sender.c_str(), memberName, 
            //    mail.mail_title.c_str(), mailContent.c_str(), "");
        }
        break;
    case PS_GUILD_MSG_MODIFY_DEVOTE:// 通知修改了个人剩余贡献值
        {
            pKernel->CommandByName(args.WideStrVal(4), CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_MODIFY_DEVOTE << args.IntVal(5));
        }
        break;
    case PS_GUILD_MSG_BUY_ITEM:// 通知购买商品扣除贡献值结果
        {
            int result = args.IntVal(3);
            const wchar_t *playerName = args.WideStrVal(4);
			int index = args.IntVal(5);
            const char *itemId = args.StringVal(6);
            int buyNum = args.IntVal(7);
            int consume = args.IntVal(8);
            int shopBuyCount = args.IntVal(9);
            int memberBuyRest = args.IntVal(10);

            // 记录日志
            //std::wstring strItemId = ::util_string_as_widestr(itemId);

            pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME 
                << SS_GUILD_BUY_ITEM_RESULT << result << index << itemId 
                << buyNum << consume << shopBuyCount << memberBuyRest);
        }
        break;
    case PS_GUILD_MSG_CAPTAIN_TRANSFER:
        {
            int result = args.IntVal(3);
            const wchar_t *oldCaptain = args.WideStrVal(4);
            const wchar_t *newCaptain = args.WideStrVal(5);
            int oldCaptainPosition = args.IntVal(6);
			const wchar_t* guildName = args.WideStrVal(7);

            pKernel->CommandByName(oldCaptain, CVarList() << COMMAND_GUILD_BY_NAME
                << SS_GUILD_CAPTAIN_TRANSFER << result << newCaptain << oldCaptainPosition << oldCaptain << guildName);

			if (result == GUILD_MSG_REQ_SUCC)
			{
				pKernel->CommandByName(newCaptain, CVarList() << COMMAND_GUILD_BY_NAME
					<< SS_GUILD_CAPTAIN_BE_TRANSFER);
			}
        }
        break;
	case PS_GUILD_CHANGE_GUILD_SHORT_NAME:
	{
		int ret = args.IntVal(3);
		if (ret == GUILD_MSG_REQ_SUCC)
		{
			const wchar_t *guildName = args.WideStrVal(4);
			CVarList var;
			var << COMMAND_GUILD_BY_NAME
				<< SS_GUILD_REFRESH_GUILD_SYSMBOL;
			m_pGuildModule->CommandToGuildMember(pKernel, guildName, var);
		}
		const wchar_t* playerName = args.WideStrVal(5);

		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SS_GUILD_CHANGE_GUILD_SYSMBOL << ret);
	
	}break;

// 	case PS_GUILD_DEFEND_IS_APPLY:
// 		{
// 			const wchar_t* playName = args.WideStrVal(3);
// 			int result = args.IntVal(4);
// 			int gmType = args.IntVal(5);
// 			pKernel->CommandByName(playName, CVarList() << COMMAND_GUILD_DEFEND_ACTIVITY << SS_GUILD_DEFEND_IS_APPLY << result << gmType);
// 		}
// 		break;
// 	case PS_GUILD_DEFEND_APPLY:
// 		{
// 			const wchar_t* playName = args.WideStrVal(3);
// 			int result = args.IntVal(4);
// 			pKernel->CommandByName(playName, CVarList() << COMMAND_GUILD_DEFEND_ACTIVITY << SS_GUILD_DEFEND_APPLY << result);
// 		}
// 		break;
//         //弹劾完成结果
//     case PS_GUILD_IMPEACH_RESULT:
//         {
//             GuildImpeach::InformMemberResult(pKernel, args.WideStrVal(3), args.WideStrVal(4), GUILD_IMPEACH_RESULT(args.IntVal(5)));
//         }
//         break;
	case PS_GUILD_MSG_NEW_APPLY:
	{
		const wchar_t* playName = args.WideStrVal(3);
		
		pKernel->CommandByName(playName, CVarList() << COMMAND_GUILD_BY_NAME << SS_GUILD_NEW_APPLAY);

	}break;
	case PS_GUILD_NUM_CHANGE:				   // 帮会资金变动
	{
		CVarList msg;
		msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_NUM_VALUE<<args.IntVal(4)<<args.IntVal(5);
		pKernel->CustomByName(args.WideStrVal(3), msg);

	}break;
// 	case PS_GUILD_DONATE_UNLOCK_SKILL:			//解锁技能结果
// 	{
// 		const wchar_t*guildName = args.WideStrVal(3);
// 		
// 		// 检查公会的申请列表是否已满
// 		IPubData* pGuildData = m_pGuildModule->GetPubData(pKernel, guildName);
// 		if (pGuildData == NULL)
// 		{
// 			return 0;
// 		}
// 		// 成员表格
// 		IRecord* pRecord = pGuildData->GetRecord(GUILD_MEMBER_REC);
// 		if (pRecord == NULL)
// 		{
// 			return 0;
// 		}
// 		// 成员数量
// 		int memberCount = pRecord->GetRows();
// 		// 消息
// 		CVarList msg;
// 		 msg << COMMAND_GUILD_BY_NAME << SS_GUILD_SKILL_UNLOCK;
// 		// 通知成员帮会等级改变
// 		LoopBeginCheck(a);
// 		for (int i = 0; i < memberCount; ++i)
// 		{
// 			LoopDoCheck(a);
// 
// 			const wchar_t* playerName = pRecord->QueryWideStr(i, GUILD_MEMBER_REC_COL_NAME);
// 			if (StringUtil::CharIsNull(playerName))
// 			{
// 				continue;
// 			}
// 			if (pKernel->GetPlayerScene(playerName) > 0)
// 			{
// 					// 通知玩家
// 					pKernel->CommandByName(playerName, msg);
// 			}
// 		}
// 
// 	}break;
// 	case PS_GUILD_SKILL_OPERATE_RET:
// 	{
// 		const wchar_t* 	playerName = args.WideStrVal(3);
// 		int nResult_ = args.IntVal(4);
// 		int nSkillId_ = args.IntVal(5);
// 		int nLevel_ = args.IntVal(6);
// 		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_SKILL_OPERATE_RET 
// 			<< nResult_ << nSkillId_ << nLevel_);
// 
// 	}break;
// 	
// 	
// 	case PS_GUILD_BUILDING_LEVEL_UP:
// 	{
// 		const wchar_t* playerName = args.WideStrVal(3);
// 		int ret = args.IntVal(4);
// 			
// 		CVarList msg;
// 		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SS_GUILD_BUILDING_LEVEL_UP<<ret);
// 
// 	}break;
// 	case PS_GUILD_BUILDING_LEVEL_UP_BROADCAST:
// 	{
// 		const wchar_t* guildName = args.WideStrVal(3);
// 		int building_ = args.IntVal(4);
// 		int level_ = args.IntVal(5);
// 
// 		CVarList msg;
// 		msg << SERVER_CUSTOMMSG_GUILD << SC_GUILD_BUILDING_LVUP_BROADCAST << building_ << level_;
// 		GuildModule::BroadCastGuild(pKernel, guildName, msg);
// 	}
// 	break;
// 	case PS_GUILD_NUM_BUY:
// 	{
// 		const wchar_t*playerName = args.WideStrVal(3);
// 		int guildNumType = args.IntVal(4);
// 		int ret = args.IntVal(5);
// 		int value = args.IntVal(6);
// 		CVarList msg;
// 		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SS_GUILD_NUM_BUY << guildNumType << ret<<value);
// 	}break;
// 	case PS_GUILD_JIAN_KANG_DU_GIF:
// 	{
// 		const wchar_t * playerName = args.WideStrVal(3);
// 		const char *award = args.StringVal(4);
// 		
// 		pKernel->CommandByName(playerName, CVarList() << COMMAND_GUILD_BY_NAME << SS_GUILD_GET_JIAN_KANG_DU_GIF << award );
// 	}break;
// 	case PS_GUILD_ADD_PREPARE_GUILD:
// 	{
// 		const wchar_t*guildName = args.WideStrVal(3);
// 		const wchar_t*applyName = args.WideStrVal(4);
// 
// 		pKernel->CommandByName(applyName, CVarList() << COMMAND_GUILD_BY_NAME
// 			<< SS_GUILD_ACCEPT_JOIN_RESULT << GUILD_MSG_REQ_SUCC << guildName);
// 
// 
// 		const ConfigMailItem& mail = m_pMailModule->GetConfigMail(pKernel, "guild_join");
// 
// 		// 邮件的内容
// 		std::wstring mail_content = SystemMailModule::ComposeMailContent(pKernel,
// 			mail.mail_content.c_str(), CVarList() << guildName);
// 
// 		// 邮件通知，成功加入公会
// 		m_pMailModule->SendMailToPlayer(pKernel, mail.sender.c_str(), applyName,
// 			mail.mail_title.c_str(), mail_content.c_str(), mail.items);
// 
// 
// 	
// 	}break;
// 	case PS_GUILD_USE_GUILD_RARE_TREASURE:
// 	{
// 		const wchar_t* guildName = args.WideStrVal(3);
// 		const wchar_t * playerName = args.WideStrVal(4);
// 		int ret = args.IntVal(5);
// 		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_USE_GUILD_RARE_TREASURE << ret);
// 	
// 	};
// 	case PS_GUILD_GIVE_OUT_BONUS_RESUTL:
// 	{
// 		const wchar_t * playerName = args.WideStrVal(3);
// 		const wchar_t *reason = args.WideStrVal(4);
// 		int giveValue = args.IntVal(5); //给予每个玩家多少
// 		
// 		RewardModule::AwardEx award;
// 		award.srcFunctionId = FUNCTION_EVENT_ID_GUILD_BONUS;
// 		std::string capName = CapitalModule::m_pCapitalModule->GetCapitalName(CAPITAL_SILVER);
// 		award.capitals[capName.c_str()] = giveValue;
// 		award.name = GUILD_BONUS_MAIL;
// 
// 
// 		for (size_t i = 6; i < args.GetCount(); i++)
// 		{
// 			const wchar_t*name = args.WideStrVal(i);
// 			CVarList mail_param;
// 			mail_param<< giveValue;
// 			RewardModule::m_pRewardInstance->RewardByRoleName(pKernel, name, &award, mail_param);
// 		}
// 
// 		pKernel->CustomByName(playerName, CVarList() << SERVER_CUSTOMMSG_GUILD << SC_GUILD_GIVE_OUT_BONUS);
// 	
// 	}break;
    default:
        break;
    }

    return 0;
}
