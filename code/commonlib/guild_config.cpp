#include "commonlib_pch.h"
#include "guild_config.h"
#include "config_reader.h"

IMPLEMENT_SINGLETON(GuildConfig)

GuildConfig::GuildConfig()
{
    Clear();
}

GuildConfig::~GuildConfig()
{

}

bool GuildConfig::LoadConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load guild file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* xmlEle = xmlRoot->FirstChildElement(); xmlEle; xmlEle = xmlEle->NextSiblingElement())
    {
        if(strcmp(xmlEle->Value(), "create_guild_money_point") == 0)
        {
            m_create_guild_money_point = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "rand_turntable_cost") == 0)
        {
            m_rand_turntable_cost = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "member_turntable_times_cost") == 0)
        {
            m_member_turntable_times_cost = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "crusade_boss_max_player_num") == 0)
        {
            m_crusade_boss_max_player_num = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "twilight_war_max_guild_num") == 0)
        {
            m_twilight_war_max_guild_num = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "twilight_war_max_player_num") == 0)
        {
            m_twilight_war_max_player_num = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "buy_guild_point") == 0)
        {
            m_buy_guildpoint_cost = SDAtoi(xmlEle->Attribute("money_point"));
            m_buy_guildpoint_num = SDAtoi(xmlEle->Attribute("guild_point"));
            m_buy_guildpoint_return_rate = SDAtoi(xmlEle->Attribute("return_rate"));
        }
        else if(strcmp(xmlEle->Value(), "guild_boss_levelup_rebate_rate") == 0)
        {
            m_guild_boss_levelup_rebate_rate = SDAtoi(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "join_guild_interval") == 0)
        {
            m_join_guild_interval = Crown::SDAtou(xmlEle->Attribute("value"));
        }
        else if(strcmp(xmlEle->Value(), "member_turntable_cost") == 0)
        {
            LoadMemberTurntableCost(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "levelup") == 0)
        {
            LoadLevelUpConfig(xmlEle);
        }
        //else if(strcmp(xmlEle->Value(), "promote") == 0)
        //{
        //    LoadPromoteConfig(xmlEle);
        //}
        //else if(strcmp(xmlEle->Value(), "donate_game_point") == 0)
        //{
        //    LoadPromoteDonateGamePointConfig(xmlEle);
        //}
        //else if(strcmp(xmlEle->Value(), "donate_money_point") == 0)
        //{
        //    LoadPromoteDonateMoneyPointConfig(xmlEle);
        //}
        //else if(strcmp(xmlEle->Value(), "addition_promote_game_point_time") == 0)
        //{
        //    m_addition_promote_game_point_time = SDAtoi(xmlEle->Attribute("value"));
        //}
        //else if(strcmp(xmlEle->Value(), "addition_promote_money_point_time") == 0)
        //{
        //    m_addition_promote_money_point_time = SDAtoi(xmlEle->Attribute("value"));
        //}
        else if(strcmp(xmlEle->Value(), "rand_turntable") == 0)
        {
            LoadRandTurntable(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "member_turntable") == 0)
        {
            LoadMemberTurntable(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "crusade_boss") == 0)
        {
            LoadCrusadeBoss(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "twilight_war") == 0)
        {
            LoadTwilightWar(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "activity_list") == 0)
        {
            LoadActivityList(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "city_fight_war") == 0)
        {
            LoadCityFightWar(xmlEle);
        }
        else if(strcmp(xmlEle->Value(), "donate") == 0)
        {
            LoadDonate(xmlEle);
        }
        else
        {
            //CnAssert(false);
        }
    }

    return true;
}

int32 GuildConfig::GetMaxLevel()
{
    return (m_levelup.size()-1);    // 等级表从1开始
}

int32 GuildConfig::GetMaxExp(int32 level)
{
    if(level < 1) return 0;

    // 如果没查到匹配等级，则设置int32的最大值
    if((uint32)level >= m_levelup.size()) return SDU_INT32_MAX;

    return m_levelup[level].max_exp;
}

uint32 GuildConfig::GetCrusadeStartTime(uint32 id)
{
    for(GuildActivityStartTimeVert::iterator it = m_crusade_start_time.begin();
        it != m_crusade_start_time.end(); ++it)
    {
        if(it->id == id)
        {
            return it->start_time;
        }
    }
    return 0;
}

const GuildLevelTmpl* GuildConfig::GetGuildLeveTmpl(int32 level)
{
    if(level < 1) return 0;

    // 如果没查到匹配等级，则设置int32的最大值
    if((uint32)level >= m_levelup.size()) return 0;

    return &m_levelup[level];
}

const GuildMemberTurntableCost* GuildConfig::LoadMemberTurntableCost(int32 times)
{
    for(GuildMemberTurntableCostVert::iterator costIt = m_member_turntable_cost.begin(); 
            costIt != m_member_turntable_cost.end(); ++costIt)
    {
        if(costIt->m_times == times)
        {
            return &(*costIt);
        }
    }
    return 0;
}

const GuildTurntableTmpl* GuildConfig::GetRandTurnTable(int32 level)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;

    guildLevelTmpl = GetGuildLeveTmpl(level);
    if(!guildLevelTmpl)
    {
        return 0;
    }

    for(TurntableVert::iterator it = m_rand_turntable.begin(); it != m_rand_turntable.end(); ++it)
    {
        if(it->turntable_id == guildLevelTmpl->rand_turntable_id)
        {
            return &(*it);
        }
    }

    return 0;
}

const GuildTurntableTmpl* GuildConfig::GetMemberTurnTable(int32 level)
{
    const GuildLevelTmpl* guildLevelTmpl = 0;

    guildLevelTmpl = GetGuildLeveTmpl(level);
    if(!guildLevelTmpl)
    {
        return 0;
    }

    for(TurntableVert::iterator it = m_member_turntable.begin(); it != m_member_turntable.end(); ++it)
    {
        if(it->turntable_id == guildLevelTmpl->member_turntable_id)
        {
            return &(*it);
        }
    }

    return 0;
}

const GuildBossTmpl* GuildConfig::GetGuildBossTmpl(int32 level)
{
    GuildBossTmplMap::iterator it = m_crusade_boss.find(level);
    if(it != m_crusade_boss.end())
    {
        return &(it->second);
    }
    return 0;
}

const GuildBossTmpl* GuildConfig::GetTwilightWarTmplByLevel(int32 level)
{
    GuildBossTmplMap::iterator it = m_twilight_boss.find(level);
    if(it != m_twilight_boss.end())
    {
        return &(it->second);
    }
    return 0;
}

const GuildBossTmpl* GuildConfig::GetTwilightWarTmplByID(int32 boss_id)
{
    for(GuildBossTmplMap::iterator it = m_twilight_boss.begin(); it != m_twilight_boss.end(); ++it)
    {
        if(it->second.boss_id == boss_id)
        {
            return &(it->second);
        }
    }
    return 0;
}

const TwilightWarRankReward* GuildConfig::GetTwilightWarRankReward(int32 rank)
{
    if (rank > 4)   // 第四名以后奖励都同第四名 By ZCD
    {
        rank = 4;
    }

    TWarRankRewardMap::iterator rewardIt;

    rewardIt = m_twarRankReward.find(rank);
    if(rewardIt != m_twarRankReward.end())
    {
        return &(rewardIt->second);
    }

    return 0;
}

uint32 GuildConfig::GetTwilightWarRanRewardNum()
{
    return m_twarRankReward.size();
}

void GuildConfig::Clear()
{
    m_create_guild_money_point          = 0;
    m_crusade_boss_dungeon_id           = 0;
    m_twilight_war_dungeon_id           = 0;

    m_rand_turntable_cost               = 0;
    m_member_turntable_times_cost       = 0;

    m_buy_guildpoint_cost               = 0;
    m_buy_guildpoint_num                = 0;
    m_buy_guildpoint_return_rate        = 0;

    m_guild_boss_levelup_rebate_rate    = 0;

    m_crusade_boss_max_player_num       = 0;
    m_twilight_war_max_guild_num        = 0;
    m_twilight_war_max_player_num       = 0;

    m_join_guild_interval               = 0;
    m_levelup.clear();
    //m_additions.clear();
    //m_promoteGamePoints.clear();
    //m_promoteMoneyPoints.clear();
    //m_addition_promote_game_point_time  = 0;
    //m_addition_promote_money_point_time = 0;
    m_rand_turntable.clear();
    m_member_turntable.clear();
    m_crusade_start_time.clear();
    m_crusade_end_time          = 0;
    //m_max_crusade_boss_level    = 0;
    m_crusade_boss.clear();

    m_gamepointDonate.Clear();
    m_moneypointDonate.Clear();
}

bool GuildConfig::LoadLevelUpConfig(TiXmlElement* xmlEle)
{
    GuildLevelTmpl      guildLevelUp;
    REWARD_ITEM         rewardItem;
    std::set<uint32>    activity;
    uint32              activityID = 0;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    guildLevelUp.Clear();
    m_levelup.push_back(guildLevelUp);

    int32 i = 1;
    for(TiXmlElement* levelEle = xmlEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
    {
        guildLevelUp.Clear();

        guildLevelUp.level = SDAtoi(levelEle->Attribute("level"));

        activityID = Crown::SDAtou(levelEle->Attribute("activity"));
        if(activityID)
            activity.insert(activityID);

        guildLevelUp.activity = activity;

        guildLevelUp.max_exp = SDAtoi(levelEle->Attribute("max_exp"));
        guildLevelUp.max_member = SDAtoi(levelEle->Attribute("max_member"));
        guildLevelUp.sale_id = Crown::SDAtou(levelEle->Attribute("sale_id"));
        guildLevelUp.donate_game_point = SDAtoi(levelEle->Attribute("donate_game_point"));
        guildLevelUp.donate_back_guild_point = SDAtoi(levelEle->Attribute("donate_back_guild_point"));
        guildLevelUp.donate_exp = SDAtoi(levelEle->Attribute("donate_exp"));
        guildLevelUp.rand_turntable_id = SDAtoi(levelEle->Attribute("rand_turntable_id"));
        guildLevelUp.member_turntable_id = SDAtoi(levelEle->Attribute("member_turntable_id"));

        if(i != guildLevelUp.level)
        {
            CnError("Guild levelup data i:%d, level:%d\n", i, guildLevelUp.level);
        }

        guildLevelUp.reward_welfare.reason = REWARD_REASON_GUILD_REWRAD_PERSONAL_WELFARE;
        for(TiXmlElement* rewardEle = levelEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        {
            for(TiXmlElement* itemEle = rewardEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
            {
                rewardItem.Clear();
                // 加载奖励项目
                if(!ConfigReader::LoadRewardItem(itemEle, rewardItem))
                {
                    //CnWarn("Error guild level:%d, reward type:%d\n", guildLevelUp.level, rewardItem.type);
                }
                guildLevelUp.reward_welfare.items.push_back(rewardItem);
            }
        }

        m_levelup.push_back(guildLevelUp);
        ++i;
    }

    return true;
}
//
//bool GuildConfig::LoadPromoteConfig(TiXmlElement* xmlEle)
//{
//    GuildAdditionTmpl       guildAddition;
//    GuildPromoteLevelTmpl   guildPromoteLevel;
//
//    for(TiXmlElement* additionEle = xmlEle->FirstChildElement(); additionEle; additionEle = additionEle->NextSiblingElement())
//    {
//        guildAddition.Clear();
//
//        guildAddition.id = SDAtoi(additionEle->Attribute("id"));
//        guildAddition.maxlevel = SDAtoi(additionEle->Attribute("maxlevel"));
//
//        for(TiXmlElement* levelEle = additionEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
//        {
//            guildPromoteLevel.Clear();
//
//            guildPromoteLevel.level = SDAtoi(levelEle->Attribute("level"));
//            guildPromoteLevel.addvalue = SDAtoi(levelEle->Attribute("addvalue"));
//            guildPromoteLevel.exp = SDAtoi(levelEle->Attribute("exp"));
//
//            guildAddition.levels.push_back(guildPromoteLevel);
//        }
//
//        m_additions.push_back(guildAddition);
//    }
//
//    return true;
//}
//
//bool GuildConfig::LoadPromoteDonateGamePointConfig(TiXmlElement* xmlEle)
//{
//    GuildPromoteDonateGamePointTmpl guildPromoteTmpl;
//
//    for(TiXmlElement* promoteEle = xmlEle->FirstChildElement(); promoteEle; promoteEle = promoteEle->NextSiblingElement())
//    {
//        guildPromoteTmpl.Clear();
//
//        guildPromoteTmpl.times = SDAtoi(promoteEle->Attribute("times"));
//        guildPromoteTmpl.guild_point = SDAtoi(promoteEle->Attribute("guild_point"));
//        guildPromoteTmpl.exp = SDAtoi(promoteEle->Attribute("exp"));
//        guildPromoteTmpl.donate_game_point = SDAtoi(promoteEle->Attribute("donate_game_point"));
//
//        m_promoteGamePoints.push_back(guildPromoteTmpl);
//    }
//
//    return true;
//}
//
//bool GuildConfig::LoadPromoteDonateMoneyPointConfig(TiXmlElement* xmlEle)
//{
//    GuildPromoteDonateMoneyPointTmpl guildPromoteTmpl;
//
//    for(TiXmlElement* promoteEle = xmlEle->FirstChildElement(); promoteEle; promoteEle = promoteEle->NextSiblingElement())
//    {
//        guildPromoteTmpl.Clear();
//
//        guildPromoteTmpl.times = SDAtoi(promoteEle->Attribute("times"));
//        guildPromoteTmpl.guild_point = SDAtoi(promoteEle->Attribute("guild_point"));
//        guildPromoteTmpl.exp = SDAtoi(promoteEle->Attribute("exp"));
//        guildPromoteTmpl.donate_money_point = SDAtoi(promoteEle->Attribute("donate_money_point"));
//
//        m_promoteMoneyPoints.push_back(guildPromoteTmpl);
//    }
//
//    return true;
//}

bool GuildConfig::LoadMemberTurntableCost(TiXmlElement* xmlEle)
{
    GuildMemberTurntableCost memberTurntableCost;

    memberTurntableCost.Clear();

    for(TiXmlElement* costEle = xmlEle->FirstChildElement(); costEle; costEle = costEle->NextSiblingElement())
    {
        memberTurntableCost.m_times = SDAtoi(costEle->Attribute("times"));
        memberTurntableCost.m_money_point = SDAtoi(costEle->Attribute("money_point"));
        m_member_turntable_cost.push_back(memberTurntableCost);
    }

    return true;
}

bool GuildConfig::LoadRandTurntable(TiXmlElement* xmlEle)
{
    GuildTurntableTmpl turntableTmpl;
    GuildTurntableRound turntableRound;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    turntableTmpl.Clear();
    for(TiXmlElement* turntableEle = xmlEle->FirstChildElement(); turntableEle; turntableEle = turntableEle->NextSiblingElement())
    {
        turntableTmpl.Clear();
        turntableTmpl.turntable_id = Crown::SDAtou(turntableEle->Attribute("id"));

        for(TiXmlElement* roundEle = turntableEle->FirstChildElement(); roundEle; roundEle = roundEle->NextSiblingElement())
        {
            turntableRound.Clear();
            turntableRound.id = Crown::SDAtou(roundEle->Attribute("id"));
            turntableRound.item_id = Crown::SDAtou(roundEle->Attribute("item_id"));
            turntableRound.num = SDAtoi(roundEle->Attribute("num"));
            turntableRound.probability = SDAtoi(roundEle->Attribute("probability"));
            //if(!ItemConfig::Instance()->GetItemTmpl(turntableRound.item_id))
            //{
            //    CnError("Can't find item:%u in guild.xml\n", turntableRound.item_id);
            //    continue;
            //}
            turntableTmpl.list.push_back(turntableRound);
        }

        m_rand_turntable.push_back(turntableTmpl);
    }

    return true;
}

bool GuildConfig::LoadMemberTurntable(TiXmlElement* xmlEle)
{
    GuildTurntableTmpl turntableTmpl;
    GuildTurntableRound turntableRound;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    turntableTmpl.Clear();
    for(TiXmlElement* turntableEle = xmlEle->FirstChildElement(); turntableEle; turntableEle = turntableEle->NextSiblingElement())
    {
        turntableTmpl.Clear();
        turntableTmpl.turntable_id = Crown::SDAtou(turntableEle->Attribute("id"));

        for(TiXmlElement* roundEle = turntableEle->FirstChildElement(); roundEle; roundEle = roundEle->NextSiblingElement())
        {
            turntableRound.Clear();
            turntableRound.id = Crown::SDAtou(roundEle->Attribute("id"));
            turntableRound.item_id = Crown::SDAtou(roundEle->Attribute("item_id"));
            turntableRound.num = SDAtoi(roundEle->Attribute("num"));
            turntableRound.probability = SDAtoi(roundEle->Attribute("probability"));
            turntableTmpl.list.push_back(turntableRound);
        }

        m_member_turntable.push_back(turntableTmpl);
    }

    return true;
}

bool GuildConfig::LoadCrusadeBoss(TiXmlElement* xmlEle)
{
    GuildBossTmpl           guildBossTmpl;
    REWARD_ITEM             rewardItem;
    GuildActivityStartTime  crusadeStartTime;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    for(TiXmlElement* childEle = xmlEle->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if(strcmp(childEle->Value(), "crusade_start_time") == 0)
        {
            crusadeStartTime.Clear();
            for(TiXmlElement* timeEle = childEle->FirstChildElement(); timeEle; timeEle = timeEle->NextSiblingElement())
            {
                crusadeStartTime.id = Crown::SDAtou(timeEle->Attribute("id"));
                crusadeStartTime.start_time = Crown::SDAtou(timeEle->Attribute("time"));
                m_crusade_start_time.push_back(crusadeStartTime);
            }
        }
        else if(strcmp(childEle->Value(), "crusade_end_time") == 0)
        {
            m_crusade_end_time = Crown::SDAtou(childEle->Attribute("time"));
        }
        else if(strcmp(childEle->Value(), "boss") == 0)
        {
            for(TiXmlElement* bossEle = childEle->FirstChildElement(); bossEle; bossEle = bossEle->NextSiblingElement())
            {
                guildBossTmpl.Clear();
                guildBossTmpl.level = SDAtoi(bossEle->Attribute("level"));
                guildBossTmpl.boss_id = Crown::SDAtou(bossEle->Attribute("boss_id"));
                guildBossTmpl.max_exp = SDAtoi(bossEle->Attribute("max_exp"));
                guildBossTmpl.cost_money_point = SDAtoi(bossEle->Attribute("cost_money_point"));
                guildBossTmpl.add_exp = SDAtoi(bossEle->Attribute("add_exp"));

                // 公会 BOSS 的最大等级
                if(m_max_crusade_boss_level < guildBossTmpl.level)
                {
                    m_max_crusade_boss_level = guildBossTmpl.level;
                }

                for(TiXmlElement* rewardEle = bossEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                {
                    if(strcmp(rewardEle->Value(), "rewards") == 0)
                    {
                        for(TiXmlElement* itemEle = rewardEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
                        {
                            rewardItem.Clear();
                            // 加载奖励项目
                            if(!ConfigReader::LoadRewardItem(itemEle, rewardItem))
                            {
                                //CnWarn("Error crusade_boss boss:%u, reward type:%d\n", guildBossTmpl.boss_id, rewardItem.type);
                            }
                            guildBossTmpl.rewardTable.items.push_back(rewardItem);
                        }
                    }
                }

                if(m_crusade_boss.find(guildBossTmpl.level) == m_crusade_boss.end())
                {
                    m_crusade_boss[guildBossTmpl.level] = guildBossTmpl;
                }
                else
                {
                    CnWarn("Duplicate crusade boss level:%d\n", guildBossTmpl.level);
                }
            }
        }
    }

    return true;
}

bool GuildConfig::LoadTwilightWar(TiXmlElement* xmlEle)
{
    if(!xmlEle) return false;



    // 多存了等级为 0 的数据
    for(TiXmlElement* childEle = xmlEle->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if(strcmp(childEle->Value(), "boss") == 0)
        {
            LoadTwilightWarBossTmpl(childEle);
        }
        else if(strcmp(childEle->Value(), "rank_reward") == 0)
        {
            LoadTwilightWarReward(childEle);
        }
        //else if(strcmp(childEle->Value(), "twilight_war_start_time") == 0)
        //{
        //    LoadTwilightWarStartTime(childEle);
        //}
        //else if(strcmp(childEle->Value(), "twilight_war_end_time") == 0)
        //{
        //    m_twilight_war_end_time = Crown::SDAtou(childEle->Attribute("time"));
        //}
    }

    return true;
}

bool GuildConfig::LoadTwilightWarBossTmpl(TiXmlElement* xmlEle)
{
    GuildBossTmpl           guildBossTmpl;
    REWARD_ITEM             rewardItem;

    if(!xmlEle) return false;

    for(TiXmlElement* bossEle = xmlEle->FirstChildElement(); bossEle; bossEle = bossEle->NextSiblingElement())
    {
        guildBossTmpl.Clear();
        guildBossTmpl.level = SDAtoi(bossEle->Attribute("level"));
        guildBossTmpl.boss_id = Crown::SDAtou(bossEle->Attribute("boss_id"));

        for(TiXmlElement* rewardEle = bossEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
        {
            if(strcmp(rewardEle->Value(), "rewards") == 0)
            {
                for(TiXmlElement* itemEle = rewardEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
                {
                    rewardItem.Clear();
                    // 加载奖励项目
                    if(!ConfigReader::LoadRewardItem(itemEle, rewardItem))
                    {
                        //CnWarn("Error crusade_boss boss:%u, reward type:%d\n", guildBossTmpl.boss_id, rewardItem.type);
                    }
                    guildBossTmpl.rewardTable.items.push_back(rewardItem);
                }
            }
        }

        if(m_twilight_boss.find(guildBossTmpl.level) == m_twilight_boss.end())
        {
            m_twilight_boss[guildBossTmpl.level] = guildBossTmpl;
        }
        else
        {
            CnWarn("Duplicate twilight war boss level:%d\n", guildBossTmpl.level);
        }
    }

    return true;
}

bool GuildConfig::LoadTwilightWarReward(TiXmlElement* xmlEle)
{
    TwilightWarRankReward   twilightWarRankReward;
    REWARD_ITEM             rewardItem;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    for(TiXmlElement* rankEle = xmlEle->FirstChildElement(); rankEle; rankEle = rankEle->NextSiblingElement())
    {
        if(strcmp(rankEle->Value(), "rank") == 0)
        {
            twilightWarRankReward.Clear();
            twilightWarRankReward.rank = Crown::SDAtou(rankEle->Attribute("id"));
            for(TiXmlElement* rewardEle = rankEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
            {
                for(TiXmlElement* itemEle = rewardEle->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
                {
                    rewardItem.Clear();
                    // 加载奖励项目
                    if(!ConfigReader::LoadRewardItem(itemEle, rewardItem))
                    {
                        CnWarn("Error twilight war rank:%u reward\n", twilightWarRankReward.rank);
                    }
                    twilightWarRankReward.reward_table.items.push_back(rewardItem);
                }

                if(m_twarRankReward.find(twilightWarRankReward.rank) == m_twarRankReward.end())
                {
                    m_twarRankReward[twilightWarRankReward.rank] = twilightWarRankReward;
                }
                else
                {
                    CnWarn("Duplicate twilight war rank:%u reward\n", twilightWarRankReward.rank);
                }
            }
        }
        else if(strcmp(rankEle->Value(), "rank_reward") == 0)
        {
            LoadTwilightWarReward(rankEle);
        }
    }

    return true;
}

//bool GuildConfig::LoadTwilightWarStartTime(TiXmlElement* xmlEle)
//{
//    GuildActivityStartTime guildActivityStartTime;
//
//    if(!xmlEle) return false;
//
//    for(TiXmlElement* activityEle = xmlEle->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
//    {
//        guildActivityStartTime.Clear();
//
//        guildActivityStartTime.id = Crown::SDAtou(activityEle->Attribute("id"));
//        guildActivityStartTime.start_time = Crown::SDAtou(activityEle->Attribute("time"));
//
//        m_twilight_war_start_time.push_back(guildActivityStartTime);
//    }
//
//    return true;
//}

bool GuildConfig::LoadActivityList(TiXmlElement* xmlEle)
{
    uint32 id = 0;
    uint32 dungeonID = 0;

    if(!xmlEle) return false;

    // 多存了等级为 0 的数据
    for(TiXmlElement* activityEle = xmlEle->FirstChildElement(); activityEle; activityEle = activityEle->NextSiblingElement())
    {
        id = Crown::SDAtou(activityEle->Attribute("id"));
        dungeonID = Crown::SDAtou(activityEle->Attribute("dungeon_id"));
        if(id == GUILD_ACTIVITY_CRUSADE_BOSS)
        {
            m_crusade_boss_dungeon_id = dungeonID;
        }
        else if(id == GUILD_ACTIVITY_TWILIGHT_WAR)
        {
            m_twilight_war_dungeon_id = dungeonID;
        }
    }

    return true;
}

//uint32 GuildConfig::GetTwilightWarStartTime(uint32 id)
//{
//    for(GuildActivityStartTimeVert::iterator it = m_twilight_war_start_time.begin();
//            it != m_twilight_war_start_time.end(); ++it)
//    {
//        if(it->id == id)
//        {
//            return it->start_time;
//        }
//    }
//    return 0;
//}

bool GuildConfig::LoadCityFightWar( TiXmlElement* xmlEle )
{
    if (!xmlEle) 
        return false;

    m_city_fight_guild_num_per_room = Crown::SDAtou(xmlEle->Attribute("guild_num_per_room"));
    m_city_fight_player_num_per_guild = Crown::SDAtou(xmlEle->Attribute("player_num_per_guild"));
    m_city_fight_win_score = Crown::SDAtou(xmlEle->Attribute("win_score"));

    REWARD_ITEM rewardItem;
    CityFightWarCityConfig cityConfig;
    for (TiXmlElement* childEle = xmlEle->FirstChildElement(); childEle; childEle = childEle->NextSiblingElement())
    {
        if(strcmp(childEle->Value(), "city_list") == 0)
        {
            for (TiXmlElement* cityEle = childEle->FirstChildElement(); cityEle; cityEle = cityEle->NextSiblingElement())
            {
                if(strcmp(cityEle->Value(), "city") == 0)
                {
                    cityConfig.Clear();
                    cityConfig.city_id = Crown::SDAtou(cityEle->Attribute("id"));
                    cityConfig.dungeon_id = Crown::SDAtou(cityEle->Attribute("dungeon_id"));

                    for (TiXmlElement* rewardEle = cityEle->FirstChildElement()->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                    {
                        rewardItem.Clear();
                        // 加载奖励项目
                        if(!ConfigReader::LoadRewardItem(rewardEle, rewardItem))
                        {
                            CnWarn("Error city fight war city: %d reward\n", cityConfig.city_id);
                        }
                        cityConfig.reward_table.items.push_back(rewardItem);
                    }

                    m_city_fight_war_city_map.insert(std::make_pair(cityConfig.city_id, cityConfig));
                }
            }
        }
        else if (strcmp(childEle->Value(), "score_reward") == 0)
        {
            CityFightScoreReward scoreReward;
            REWARD_ITEM rewardItem;
            for (TiXmlElement* rewardsEle = childEle->FirstChildElement(); rewardsEle; rewardsEle = rewardsEle->NextSiblingElement())
            {
                if(strcmp(rewardsEle->Value(), "rewards") == 0)
                {
                    scoreReward.Clear();
                    scoreReward.socre = Crown::SDAtou(rewardsEle->Attribute("score"));
                    for (TiXmlElement* rewardEle = rewardsEle->FirstChildElement(); rewardEle; rewardEle = rewardEle->NextSiblingElement())
                    {
                        // 加载奖励项目
                        rewardItem.Clear();
                        if(!ConfigReader::LoadRewardItem(rewardEle, rewardItem))
                        {
                            CnWarn("Error city fight score reward: %d\n", scoreReward.socre);
                        }

                        scoreReward.reward_table.items.push_back(rewardItem);
                    }
                }

                m_city_fight_score_reward_list.push_back(scoreReward);
            }
        }
    }

    return true;
}

bool GuildConfig::LoadDonate(TiXmlElement* xmlEle)
{
    for(TiXmlElement* donateEle = xmlEle->FirstChildElement(); donateEle; donateEle = donateEle->NextSiblingElement())
    {
        if(strcmp(donateEle->Value(), "gamepoint") == 0)
        {
            donateEle->QueryIntAttribute("use_gamepoint", &m_gamepointDonate.num);
            donateEle->QueryIntAttribute("guildpoint", &m_gamepointDonate.guildpoint);
            donateEle->QueryIntAttribute("exp", &m_gamepointDonate.exp);
        }
        else if(strcmp(donateEle->Value(), "moneypoint") == 0)
        {
            donateEle->QueryIntAttribute("use_moneypoint", &m_moneypointDonate.num);
            donateEle->QueryIntAttribute("guildpoint", &m_moneypointDonate.guildpoint);
            donateEle->QueryIntAttribute("exp", &m_moneypointDonate.exp);
        }
        else
            CnAssert(false);
    }

    return true;
}

const CityFightWarCityConfig* GuildConfig::GetCityFightWarCity( uint32 cityId )
{
    CityFightWarCityMap::const_iterator iter = m_city_fight_war_city_map.find(cityId);
    if (iter != m_city_fight_war_city_map.end())
        return &(iter->second);

    return NULL;
}

uint32 GuildConfig::GetCityFightWarCityID(uint32 dungeonID)
{
    for(CityFightWarCityMap::const_iterator iter = m_city_fight_war_city_map.begin();
            iter != m_city_fight_war_city_map.end(); ++iter)
    {
        if(iter->second.dungeon_id == dungeonID)
        {
            return iter->second.city_id;
        }
    }
    return 0;
}

void GuildConfig::GetCityFightScoreReward( uint32 socre, REWARD_TABLE& reward_table )
{
    for(CityFightScoreRewardList::reverse_iterator iter = m_city_fight_score_reward_list.rbegin();
        iter != m_city_fight_score_reward_list.rend(); ++iter)
    {
        if(socre >= iter->socre)
        {
            reward_table = iter->reward_table;
            return;
        }
    }

    return;
}