//---------------------------------------------------------
//文件名:       itemherlper.h
//内  容:       物品字符串辅助类
//说  明:       
//          
//创建日期:      2016年06月20日
//创建人:        李海罗
//修改人:
//   :         
//---------------------------------------------------------
#ifndef __ITEM_HERLPER__
#define __ITEM_HERLPER__

#include <string>
#include <vector>
#include "string_util.h"
#include <sstream>

struct ItemData
{
	std::string cfgid;
	int number;
};

struct ItemDataEx : ItemData
{
	int color;
};

class ItemHelper
{
public:
	static size_t SplitItem( std::vector<ItemData>& resultItems,  const char* pszString, char sp1 = ',', char sp2 = ':' )
	{
		resultItems.clear();
		if ( NULL == pszString || pszString[0] == 0 )
		{
			return 0;
		}

		resultItems.clear();
		CVarList arr;
		if ( StringUtil::ParseToVector(pszString, sp1, arr) > 0 )
		{
			for ( size_t i = 0 ; i < arr.GetCount(); ++i )
			{
				const char* pkv = arr.StringVal(i);
				if ( pkv[0] == '\0') continue;

				CVarList kvList;
				if ( StringUtil::ParseToVector(pkv, sp2, kvList) < 2 ) continue;

				try
				{
					const char* k = kvList.StringVal(0);
					if ( k[0] == 0 ) continue;
					int v = kvList.IntVal(1);
					ItemData item = {std::string(k), v};
					resultItems.push_back( item); 
				}
				catch (...)
				{
					
				}
			}
		}
		return resultItems.size();
	}

	static std::string ConcatItem( std::vector<ItemData>& items,  char sp1 = ',', char sp2 = ':' )
	{
		std::stringstream stream;
		if ( items.size() > 0 )
		{
			const ItemData& itd = items[0];
			stream << itd.cfgid << sp2 << itd.number;

			for( size_t i = 1; i < items.size(); ++i )
			{
				const ItemData& item = items[0];
				stream << sp1 << itd.cfgid << sp2 << itd.number;
			}
		}
		return stream.str();
	}
};

#endif // _CAU_H__
