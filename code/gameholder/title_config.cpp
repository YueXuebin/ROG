#include "gameholder_pch.h"
#include "title_config.h"
#include "player.h"
IMPLEMENT_SINGLETON(TitleConfig)
TitleConfig::TitleConfig(void)
{
    vec_title.clear();
}

TitleConfig::~TitleConfig(void)
{
}

bool TitleConfig::LoadTitleConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load title file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    bool b_find = false;

    for(TiXmlElement* titleEle = xmlRoot->FirstChildElement(); titleEle; titleEle = titleEle->NextSiblingElement())
    {
        LoaddTitle(titleEle);
        b_find = true;
    }

    if(b_find)
        return true;

    return false;
}
void TitleConfig::LoaddTitle(TiXmlElement* TitleEle)
{
    if(Crown::SDStrcmp(TitleEle->Value(), "tileget") == 0)
    {
        TitleStruct titleStruct;
        titleStruct.Clear();

        // 称号id
        int32 _id = 0;
        TitleEle->QueryIntAttribute("id", &_id);
        titleStruct.id = _id;

        // 称号小类
        int32 _type = 0;
        TitleEle->QueryIntAttribute("type", &_type);
        titleStruct.type = _type;

        // 称号的时间
        int32 _time = 0;
        TitleEle->QueryIntAttribute("time", &_time);
        titleStruct.time = _time;

        // 称号类型
        int32 _titletype = 0;
        TitleEle->QueryIntAttribute("titletype", &_titletype);
        titleStruct.titletype = _titletype;

        for(TiXmlElement* conditionEle = TitleEle->FirstChildElement(); conditionEle; conditionEle = conditionEle->NextSiblingElement())
        {
            // 条件
            if((Crown::SDStrcmp(conditionEle->Value(), "condition") == 0))
            {
                switch (_type)
                {
                case WORLD_BOSS_TITLE:
                case OFF_LINE_TITLE:
                case ANSWING_TITLE:
                    //暮光之战
                case GUID_BOSS_TITLE:
                    //
                case GUILD_RANK_TITLE:
                case BATTLE_RANK_TITLE:
                case HOROSCOPE_RANK_TITLE:
                case ASHURA_RANK_TITLE:
                case VIP_TITLE:
                case WING_RANK_TITLE:
                case PET_RANK_TITLE:
                case RIDE_RANK_TITLE:
                    {
                        LoadCoditionTemp(titleStruct,conditionEle);
                    }
                    break;
                case TASK_TITLE:
                case ASHURA_TITLE:
                case FOLLOW_TITLE:
                    //怪物讨伐令
                case TOWER_LIGHT_TITLE:
                case USEITEM_TITLE:
                    //
                case GET_VIP_REWARD_TITLE:
                case FB_LIKE_TITLE:
                    {
                        ConditionTemp conditTemp;
                        conditTemp.Clear();

                        int32 num =0;
                        conditionEle->QueryIntAttribute("num", &num);
                        conditTemp.num = num;

                        int32 reason =0;
                        conditionEle->QueryIntAttribute("reason", &reason);
                        conditTemp.reason = reason;
                        titleStruct.conditemp.push_back(conditTemp);
                    }
                    break;
                }

            }

            TiXmlElement * rewardsEles = conditionEle->FirstChildElement();
            // 奖励
            LoadReward(rewardsEles, titleStruct);
        }

        vec_title.push_back(titleStruct);
    }
    else if (Crown::SDStrcmp(TitleEle->Value(), "vips") == 0)
    {
        LoadTileNum(TitleEle);
    }

}

void TitleConfig::LoadReward(TiXmlElement * rewardsEles, TitleStruct& titleStruct)
{
    if(!rewardsEles)
        return;

    if ((Crown::SDStrcmp(rewardsEles->Value(), "rewards") == 0))
    {
        for(TiXmlElement* rewardEle= rewardsEles->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        {
            REWARD_ITEM rewarditem;
            rewarditem.Clear();

            int32 _num = 0;
            rewardEle->QueryIntAttribute("num", &_num);
            rewarditem.num = _num;

            int32 _type =0;
            rewardEle->QueryIntAttribute("type", &_type);
            //rewarditem.type = _type;
            rewarditem.id = 0;
            titleStruct.Rewarlist.push_back(rewarditem);
        }
    }
}

void TitleConfig::LoadCoditionTemp(TitleStruct&titleStruct, TiXmlElement* conditionEle)
{
    if(!conditionEle)
        return;

    ConditionTemp conditTemp;
    conditTemp.Clear();
    std::string places_value = "";
    places_value = conditionEle->Attribute("reason");

    if(places_value != "")
    {
        std::vector<std::string> values = Crown::SDSplitStrings(places_value, ' ');
        for(uint32 i=0; i<values.size(); ++i)
        {
            std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');

            if(key_value.size() == 2)
            {
                int32 minxrange = SDAtoi(key_value[0].c_str());
                int32 maxrange = SDAtoi(key_value[1].c_str());
                conditTemp.minRange = minxrange;
                conditTemp.maxRange = maxrange;
            }
            else if(key_value.size() == 1)
            {
                int32  minxrange = SDAtoi(key_value[0].c_str());
                conditTemp.minRange = minxrange;
            }
            titleStruct.conditemp.push_back(conditTemp);
        }
    }
}
void TitleConfig::LoadTileNum(TiXmlElement* TitleEle)
{
    for(TiXmlElement* vipEle= TitleEle->FirstChildElement(); vipEle; vipEle = vipEle->NextSiblingElement())
    {
        VipNum vipnum;
        vipnum.Clear();

        int32 num = 0;
        vipEle->QueryIntAttribute("num", &num);
        vipnum.num = num;  

        int32 level =0;
        vipEle->QueryIntAttribute("level", &level);
        vipnum.level = level;

        m_vec_vipnum.push_back(vipnum);
    }
}
int32 TitleConfig::GetVipNum(int32 viplevel)
{
    Vec_VipNum::iterator iter_vipNum;
    for (iter_vipNum = m_vec_vipnum.begin();iter_vipNum != m_vec_vipnum.end(); iter_vipNum++)
    {
        if (viplevel == iter_vipNum->level)
        {
            return iter_vipNum->num;
        }
    }

    return 0;
}


uint32 TitleConfig::GetTaskTitle(int32 type,uint32 taskid)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type == type)
        {
            Vec_ConditionTemp::iterator iter_conditon;
            for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
            {
                if (taskid == iter_conditon->reason)
                {
                    return  iterVec->id;
                }
            }
        }
    }
    return 0;
}
uint32 TitleConfig::GetGeneralTitle(int32 type,int32 reason,int32 num)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type != type) continue;
        Vec_ConditionTemp::iterator iter_conditon;
        for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
        {
            if(!reason) continue;
            if (reason == iter_conditon->reason)
                return  iterVec->id;
        }

    }
    return 0;
}
uint32 TitleConfig::DealAllPlayer(int32 type)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type != type) continue;
        Vec_ConditionTemp::iterator iter_conditon;
        for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
        {
            if((ALL_PLAYER_REASON == iter_conditon->reason))
            {
                return  iterVec->id;
            }
        }
    }

    return 0;
}
TitleVec* TitleConfig::GetAshuraTitle(int32 type,int32 reason,int32 num)
{
    m_titleVec.clear();

    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type != type) continue;
        Vec_ConditionTemp::iterator iter_conditon;
        for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
        {
            if ((reason == iter_conditon->reason)&& (num>=iter_conditon->num))
            {
                m_titleVec.push_back(iterVec->id);
            }
        }
    }

    return &m_titleVec;
}

int32 TitleConfig::GetGreaterCondition(int32 type,int32 level)
{
    //m_titleVec.clear();

    int32 maxviplevel = 0;
    int32 curtitleid = 0;
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type != type) continue;
        Vec_ConditionTemp::iterator iter_conditon;
        for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
        {
            if(iter_conditon->minRange == 0) continue;
            if(level >= iter_conditon->minRange)
            {
                if(iter_conditon->minRange >= maxviplevel)
                {
                    maxviplevel = iter_conditon->minRange;
                    curtitleid = iterVec->id;
                }
            }
                //m_titleVec.push_back(iterVec->id);
        }
    }
   return curtitleid;
}
TitleVec*  TitleConfig::GetRankTitle(int32 type,int32 rank)
{
    m_titleVec.clear();

    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(iterVec->type != type) continue;

        Vec_ConditionTemp::iterator iter_conditon;
        for (iter_conditon =  iterVec->conditemp.begin(); iter_conditon!=iterVec->conditemp.end(); iter_conditon++)
        {
            if(iter_conditon->maxRange != 0)
            {
                if(iter_conditon->minRange == 0) continue;

                if((rank >= iter_conditon->minRange) &&  (rank<=iter_conditon->maxRange) )
                    m_titleVec.push_back(iterVec->id);
            }
            else
            {
                if(iter_conditon->minRange == 0) continue;

                if((rank == iter_conditon->minRange))
                    m_titleVec.push_back(iterVec->id);
            }

        }
    }
    return &m_titleVec;
}

void TitleConfig::AddTitleReward(uint32 titleid, PropertySet& propSet)
{
    Vec_Title::iterator iterVec;
    for (iterVec = vec_title.begin(); iterVec != vec_title.end(); iterVec++)
    {
        if(iterVec->id != titleid) continue;
        RewardList::iterator iterReward;
        for (iterReward = iterVec->Rewarlist.begin(); iterReward != iterVec->Rewarlist.end(); iterReward++)
        {
            propSet.Add(iterReward->id, iterReward->num);
        }
    }
}

//得到称号模块类型
int32 TitleConfig::GetTitleModelType(uint32 titleid)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(titleid == iterVec->id)
        {
            return iterVec->type;
        }
    }
    return 0;
}

int32 TitleConfig::GetVipConditonReason(uint32 titleid)
{
    Vec_ConditionTemp::iterator vec_conditon;
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(titleid == iterVec->id)
        {
            //return iterVec->type;
            for(vec_conditon = iterVec->conditemp.begin(); vec_conditon!= iterVec->conditemp.end(); vec_conditon++)
            {
               return vec_conditon->minRange;
            }
        }
    }
    return 0;
}

int32 TitleConfig::GetTitleType(uint32 titleid)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(titleid == iterVec->id)
        {
            return iterVec->titletype;
        }
    }

    return 0;
}

uint32 TitleConfig::GetTitleTime(uint32 titleid)
{
    Vec_Title::iterator iterVec;
    for (iterVec =vec_title.begin(); iterVec!= vec_title.end();iterVec++)
    {
        if(titleid == iterVec->id)
        {
            return iterVec->time;
        }
    }
    return 0;
}
