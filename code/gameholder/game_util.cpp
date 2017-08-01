#include "gameholder_pch.h"
#include "game_util.h"
#include "sceneobject.h"
#include "regular_define.h"
#include "battle_unit.h"
#include "pet.h"
#include "scene.h"
#include "monster.h"
#include "boss.h"
#include "spawn.h"
#include "item_config_manager.h"
#include "formula_manager.h"
#include "skill_config.h"
#include "player.h"
#include "trigger.h"
#include "offline_player.h"
#include "config_reader.h"
#include "dungeon_data.h"

std::vector<uint32> g_BasePropertyIdList;
std::vector<uint32> g_AdvancePropertyIdList;

const std::vector<uint32>& GetBasePropertyIdList()
{
    if(g_BasePropertyIdList.size() == 0)
    {
        g_BasePropertyIdList.push_back(PROP_MAXHP);                     // 生命
        g_BasePropertyIdList.push_back(PROP_MAXMP);                     // 魔法
        g_BasePropertyIdList.push_back(PROP_HP_REC_RATE);               // 生命恢复
        g_BasePropertyIdList.push_back(PROP_MP_REC_RATE);               // 魔法恢复

        g_BasePropertyIdList.push_back(PROP_PHYSICS_DAMAGE);            // 基础伤害

        g_BasePropertyIdList.push_back(PROP_ARMOR);                     // 护甲
        g_BasePropertyIdList.push_back(PROP_DODGE);                     // 闪避
        g_BasePropertyIdList.push_back(PROP_MAX_ENERGY_SHIELD);         // 能量盾

        g_BasePropertyIdList.push_back(PROP_HIT_RATE);                  // 命中
        g_BasePropertyIdList.push_back(PROP_CRITICAL_ATTACK);           // 暴击
        g_BasePropertyIdList.push_back(PROP_CRITICAL_DAMAGE_ADDITION);  // 暴伤

        g_BasePropertyIdList.push_back(PROP_ATTACK_PER_SEC);            // 攻击速度
        g_BasePropertyIdList.push_back(PROP_CAST_PER_SEC);              // 施法速度
        g_BasePropertyIdList.push_back(PROP_MOVE_SPEED);                // 移动速度

        g_BasePropertyIdList.push_back(PROP_FIRE_DEFENCE);              // 火抗
        g_BasePropertyIdList.push_back(PROP_ICE_DEFENCE);               // 冰抗
        g_BasePropertyIdList.push_back(PROP_THUNDER_DEFENCE);           // 雷抗

        g_BasePropertyIdList.push_back(PROP_BLOCK);                     // 攻击格挡
        g_BasePropertyIdList.push_back(PROP_MAGIC_BLOCK);               // 法术格挡
    }
    return g_BasePropertyIdList;
}

const std::vector<uint32>& GetAdvancePropertyIdList()
{
    if(g_AdvancePropertyIdList.size() == 0)
    {
        g_AdvancePropertyIdList.push_back(PROP_MAX_FIRE_DEFENCE);           // 最大火焰抗性
        g_AdvancePropertyIdList.push_back(PROP_MAX_ICE_DEFENCE);            // 最大冰霜抗性
        g_AdvancePropertyIdList.push_back(PROP_MAX_LIGHTNING_DEFENCE);      // 最大闪电抗性
        g_AdvancePropertyIdList.push_back(PROP_MAX_ATTACK_BLOCK);           // 最大攻击格挡
        g_AdvancePropertyIdList.push_back(PROP_MAX_MAGIC_BLOCK);            // 最大法术格挡
    }
    return g_AdvancePropertyIdList;
}


// ==================================================
// 合并物品列表
// ==================================================
void MergeRewardTableItemList(REWARD_TABLE& outTable, const REWARD_ITEM& inItem)
{
    bool b_false = false;
    for (std::vector<REWARD_ITEM>::iterator  compareitem = outTable.items.begin(); compareitem != outTable.items.end();  compareitem++)
    {
        //if(compareitem->type == rewardItem.type && (compareitem->id == rewardItem.id))
        if(compareitem->id == inItem.id)
        {
            compareitem->num = compareitem->num + inItem.num;
            b_false = true;
            break;
        }
    }

    if(!b_false)
        outTable.items.push_back(inItem);
}

bool LoadRandRewardItem(TiXmlElement* itEle, RAND_REWARD_ITEM& rand_item)
{
    if(!itEle) return false;

    ConfigReader::Read(itEle, "rate", rand_item.rate);

    return LoadRewardItem(itEle, rand_item.item);
}

bool LoadRewardItem(TiXmlElement* itEle, REWARD_ITEM& reward_item)
{
    if(!itEle) return false;

    ConfigReader::Read(itEle, "type", reward_item.id);

    switch(reward_item.id)
    {
    case PROP_GAMEPOINT:
    case PROP_SOULPOINT:
    case PROP_EXP:
    case PROP_VIGOR:
    case PROP_MONEYPOINT:
    case PROP_GUILD_SCORE:
    case PROP_HONORPOINT:
    //case PROP_BATTLE_SOUL:
    case PROP_BIND_MONEYPOINT:
//    case PROP_UNLOCK_MONSTER_DOOR_BOSS:
    case PROP_TITLE:
    case PROP_EXPERIENCE_VIP:
        {
            ConfigReader::Read(itEle, "num", reward_item.num);
        }
        break;
    case PROP_ITEM:
        {
            ConfigReader::Read(itEle, "item_id", reward_item.id);
            ConfigReader::Read(itEle, "num", reward_item.num);

            //if(!ItemConfig::Instance()->GetItemTmpl(reward_item.id))
            //{
            //    CnWarn("Can't find item:%u\n", reward_item.id);
            //    return false;
            //}
        }
        break;
//     case PROP_CAREER_EQUIP:
//         {
//             ConfigReader::Read(itEle, "career", reward_item.id);
//             ConfigReader::Read(itEle, "equip_id", reward_item.num);
//         }
//         break;
    case PROP_RIDE_PET:
        {
            ConfigReader::Read(itEle, "ride_pet_id", reward_item.id);
        }
        break;
    case PROP_FOLLOW_PET:
        {
            ConfigReader::Read(itEle, "follow_pet_id", reward_item.id);
        }
        break; 
    default:
        break;
    }

    return true;
}




bool LoadRewardTable(TiXmlElement* itEle, REWARD_TABLE& rewardTable, int32 flag)
{
    REWARD_ITEM rewardItem;

    for(TiXmlElement* rewardIt = itEle->FirstChildElement(); rewardIt; rewardIt = rewardIt->NextSiblingElement())
    {
        rewardItem.Clear();
        rewardItem.flag = flag;
        if(!LoadRewardItem(rewardIt, rewardItem))
        {
            return false;
        }
        rewardTable.items.push_back(rewardItem);
    }

    return true;
}

bool LoadRewardTable(const std::string& rewardString, REWARD_TABLE& rewardTable, int32 flag)
{
    REWARD_ITEM rewardItem;

    std::vector<std::string> stringTable;
    stringTable.clear();
    stringTable = Crown::SDSplitStrings(rewardString, ' ');
    for(std::vector<std::string>::iterator stringItem = stringTable.begin(); stringItem != stringTable.end(); ++stringItem)
    {
        rewardItem.Clear();
        std::vector<std::string> stringNum = Crown::SDSplitStrings(*stringItem, '-');

        if(stringNum.size() <= 1 || stringNum.size() > 3)
        {
            CnError("reward size error!\n");
            return false;
        }

        rewardItem.id = SDAtoi(stringNum[0].c_str());
        rewardItem.flag = flag;

        switch(rewardItem.id)
        {
        case PROP_GAMEPOINT:
        case PROP_SOULPOINT:
        case PROP_EXP:
        case PROP_VIGOR:
        case PROP_MONEYPOINT:
        //case PROP_GUILDPOINT:
        case PROP_GUILD_SCORE:
        case PROP_HONORPOINT:
        //case PROP_BATTLE_SOUL:
        //case PROP_STAR_SOUL:
        case PROP_BIND_MONEYPOINT:
        //case PROP_UNLOCK_MONSTER_DOOR_BOSS:
            {
                rewardItem.num  = SDAtoi(stringNum[1].c_str());
            }
            break;
        case PROP_ITEM:
            {
                rewardItem.id   = Crown::SDAtou(stringNum[1].c_str());
                rewardItem.num  = SDAtoi(stringNum[2].c_str());

                //if(!ItemConfig::Instance()->GetItemTmpl(rewardItem.id))
                //{
                //    CnWarn("Can't find item:%u\n", rewardItem.id);
                //    continue;
                //}
            }
            break;
        default:
            {
                CnAssert(false);
                CnError("reward item id:%d is error!\n", rewardItem.id);
                return false;
            }
            break;
        }
        rewardTable.items.push_back(rewardItem);
    }

    return true;
}

// 获得 Vector 中的 Bit 标记
uint32 GetVectorBitFlag(std::vector<uint32>* list, uint32 val, uint32 bitLen)
{
    uint32 index = 0;
    uint32 pos = 0;
    uint32 bit = 0;
    uint32 mark = 0;

    if(!list || !bitLen)
    {
        CnAssert(false);
        return 0;
    }

    for(uint32 i=0; i<bitLen; ++i)
    {
        mark |= 1 << i;
    }

    index = val / (32 / bitLen);
    pos = val % (32 / bitLen);

    if(index >= list->size())
    {
        return 0;
    }

    bit = (*list)[index] & (mark << (pos*bitLen));

    return bit >> (pos*bitLen);
}

// 设置 Vector 中的 Bit 标记
void SetVectorBitFlag(std::vector<uint32>* list, uint32 val, uint32 bitLen, uint32 bitFlag)
{
    uint32 index = 0;
    uint32 pos = 0;
    uint32 bit = 0;
    uint32 mark = 0;

    if(!list || !bitLen)
    {
        CnAssert(false);
        return;
    }

    for(uint32 i=0; i<bitLen; ++i)
    {
        mark |= 1 << i;
    }

    index = val / (32 / bitLen);
    pos = val % (32 / bitLen);

    if(index >= list->size())
    {
        return;
    }

    bit = (*list)[index] & ~(mark << (pos*bitLen));
    (*list)[index] = bit | (bitFlag << (pos*bitLen));
}

real DistanceBySceneObject( SceneObject* pOne, SceneObject* pTwo )
{
    CnAssert(pOne);
    CnAssert(pTwo);

    CoVec2 p;
    p.x = real(pOne->GetX() - pTwo->GetX());
    p.y = real(pOne->GetY() - pTwo->GetY());

    real dis = p.Length() - pOne->GetRadius() - pTwo->GetRadius();
    return (dis < 0 ? 0 : dis);
}

bool IsBarrel(uint32 creatureId)
{
    if(creatureId == 3999 || creatureId == 3998)
        return true;
    return false;
}

void SetDefaultGroup( BattleUnit* creature )
{
    // 设置清场规则，默认组
    uint32 CreatureType = creature->GetObjectType();
    switch(CreatureType)
    {
    case OBJ_PLAYER:
        creature->SetGroup(FRIEND_GROUP);
        break;
    case OBJ_OFFLINE_PLAYER:
        creature->SetGroup(FRIEND_GROUP);
        break;
    case OBJ_PET:
        if(((Pet*)creature)->GetMaster())
            creature->SetGroup( ((Pet*)creature)->GetMaster()->GetGroup() );
        break;
    case OBJ_MONSTER:
        creature->SetGroup(ENEMY_GROUP);
        break;
    case OBJ_BOSS:
        creature->SetGroup(ENEMY_GROUP);
        break;
    default:
        CnAssert(false);
        break;
    }
}

bool IsSceneEnemyClear( Scene* pScene )
{
    // 所有敌对怪物都死亡
    bool MonsterClear = true;
    Scene::MonsterMap& Monsters = pScene->GetMonsters();
    for(Scene::MonsterMap::iterator msIter = Monsters.begin(); msIter != Monsters.end(); msIter++)
    {
        Monster* pMonster = msIter->second;
        if(IsBarrel(pMonster->GetCreatureId()))
            continue;

        if(pMonster->GetGroup() == ENEMY_GROUP)
        {
            MonsterClear = false;
            break;
        }
    }

    // 所有敌对Boss都死亡
    bool BossClear = true;
    Scene::BossMap& Bosses = pScene->GetBosses();
    for(Scene::BossMap::iterator bsIter = Bosses.begin(); bsIter != Bosses.end(); bsIter++)
    {
        Boss* pBoss = bsIter->second;
        if(pBoss->GetGroup() == ENEMY_GROUP)
        {
            BossClear = false;
            break;
        }
    }

    // 所有出怪点，出完
    bool SpawnClear = true;
    Scene::SpawnMap& Spawns = pScene->GetSpawn();
    for(Scene::SpawnMap::iterator spIter = Spawns.begin(); spIter != Spawns.end(); spIter++)
    {
        Spawn* pSpawn = spIter->second;
        if(!pSpawn->IsSpawnEnd())
        {
            SpawnClear = false;
            break;
        }
    }

    return (MonsterClear && SpawnClear && BossClear);
}

uint64 GetTeamLeaderID( TEAM_INFO* pTeamInfo )
{
    uint64 leaderID = 0;
    for(std::vector<TEAM_MEMBER>::iterator iter = pTeamInfo->member_list.begin(); iter != pTeamInfo->member_list.end(); iter++)
    {
        if(iter->is_offline)
            continue;

        leaderID = U64ID(iter->player_id_h, iter->player_id_l);
        break;
    }
    return leaderID;
}

bool IsStrSame(const char* str1, const char* str2)
{
    return Crown::SDStrcmp(str1, str2) == 0;
}

bool IsStrSame(const std::string& str1, const std::string& str2)
{
    return Crown::SDStrcmp(str1.c_str(), str2.c_str()) == 0;
}

void FuseSameReward(REWARD_TABLE& table)
{
    std::vector<REWARD_ITEM> newRewards;
    newRewards.clear();

    for(std::vector<REWARD_ITEM>::iterator rewardIt = table.items.begin(); rewardIt != table.items.end(); rewardIt++)
    {
        std::vector<REWARD_ITEM>::iterator newRewardIt;

        // 去除0的数
        if(rewardIt->num == 0)
        {
            continue;
        }

        // 新奖励表中有相同物品
        for(newRewardIt = newRewards.begin(); newRewardIt != newRewards.end(); newRewardIt++)
        {
            //if(rewardIt->type == newRewardIt->type && rewardIt->flag == newRewardIt->flag && rewardIt->id == newRewardIt->id)
            if(rewardIt->flag == newRewardIt->flag && rewardIt->id == newRewardIt->id)
            {
                break;
            }
        }

        // 新奖励表中没有
        if(newRewardIt == newRewards.end())
        {
            newRewards.push_back(*rewardIt);
        }
        else
        {
            newRewardIt->num += rewardIt->num;
        }
    }
    table.items = newRewards;
}

real ToPercent(int32 val)
{
    return (real)val/BASE_RATE_NUM;
}

int32 ToPercentInt(int32 val)
{
    return (int32)(val/BASE_RATE_NUM);
}

PropertyList CalcLinkageProp(int32 strength, int32 agility, int32 constitution, int32 spirit)
{
    PropertyList propList;
    Property prop;

    // 攻击力
    prop.type = PROP_ATTACK;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_ATTACK, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // 防御力
    prop.Clear();
    prop.type = PROP_ARMOR;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_DEFENCE, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // 暴击率
    prop.Clear();
    prop.type = PROP_CRITICAL_ATTACK;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_CRITICAL_ATTACK, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // 暴击加深
    prop.Clear();
    prop.type = PROP_CRITICAL_DAMAGE_ADDITION;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_CRITICAL_DAMAGE_ADDITION, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // 命中率
    prop.Clear();
    prop.type = PROP_HIT_RATE;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_HIT_RATE, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // 闪避
    prop.Clear();
    prop.type = PROP_DODGE;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_DODGE, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // MaxMP
    prop.Clear();
    prop.type = PROP_MAXMP;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_MAXMP, strength, agility, constitution, spirit);
    propList.push_back(prop);

    // MaxHP
    prop.Clear();
    prop.type = PROP_MAXHP;
    prop.value = FormulaManager::Instance()->CalcPlayerSecondProp(FORMULA_ID_CALC_PLAYER_MAXHP, strength, agility, constitution, spirit);
    propList.push_back(prop);

    return propList;
}

real PVPDamageReduce(BattleUnit* pAttacker, BattleUnit* pDefender)
{
    if(!pDefender)
    {
        CnAssert(false);
        return 1.0f;
    }
    // PVP伤害削减,可能的情况,玩家、离线玩家、战宠(主人是玩家)
    if(pAttacker)
    {
        int32 attacker_battlepower = 1;
        int32 defender_battlepower = 1;

        // 攻击者
        bool IsAttackPVP = false;
        BattleUnit* pvpAttacker = pAttacker;
        if(pvpAttacker->GetObjectType() == OBJ_PET)
        {   // 战宠
            BattleUnit* master = ((Pet*)pvpAttacker)->GetMaster();
            if(master)
            {
                if(master->GetObjectType() == OBJ_PLAYER || master->GetObjectType() == OBJ_OFFLINE_PLAYER)  // 战宠的主人是 玩家 离线玩家
                    pvpAttacker = master;
            }
        }

        if(pvpAttacker->GetObjectType() == OBJ_PLAYER)  // 玩家
        {
            IsAttackPVP = true;
            attacker_battlepower = ((Player*)pvpAttacker)->GetBattlePower();
        }
        else if(pvpAttacker->GetObjectType() == OBJ_OFFLINE_PLAYER)   // 离线玩家
        {
            IsAttackPVP = true;
            attacker_battlepower = ((OfflinePlayer*)pvpAttacker)->GetPlayerInfo().battle_power;
        }
        // 防御者
        bool IsDefenderPVP = false;
        BattleUnit* pvpDefender = pDefender;
        if(pvpDefender->GetObjectType() == OBJ_PET)
        {   // 战宠
            BattleUnit* master = ((Pet*)pvpDefender)->GetMaster();
            if(master)
            {
                if(master->GetObjectType() == OBJ_PLAYER || master->GetObjectType() == OBJ_OFFLINE_PLAYER)  // 战宠的主人是 玩家 离线玩家
                    pvpDefender = master;
            }
        }

        if(pvpDefender->GetObjectType() == OBJ_PLAYER)  // 玩家
        {
            IsDefenderPVP = true;
            defender_battlepower = ((Player*)pvpDefender)->GetBattlePower();
        }
        else if(pvpDefender->GetObjectType() == OBJ_OFFLINE_PLAYER)   // 离线玩家
        {
            IsDefenderPVP = true;
            defender_battlepower = ((OfflinePlayer*)pvpDefender)->GetPlayerInfo().battle_power;
        }

        // 两者都为玩家，则进行pvp伤害减免
        bool IsPVP = IsAttackPVP && IsDefenderPVP;

        if(IsPVP)
        {
            //计算免伤
            int32 aver_battle_power = (attacker_battlepower + defender_battlepower)/2;
            // 战斗力大于40万的不做减伤处理
            if(aver_battle_power > 400000)
                return 1.0f;

            // 差值计算
            int32 difference_power = (attacker_battlepower - defender_battlepower);
            if(difference_power < 0)
                difference_power = 0;
            if(difference_power >BASE_RATE_NUM)
                difference_power = BASE_RATE_NUM;

            real pvpdamage_per = FormulaManager::Instance()->CalPvPDamagePercentage(aver_battle_power, difference_power);
            if(pvpdamage_per > BASE_RATE_NUM)
                pvpdamage_per = BASE_RATE_NUM;

            real percent = ToPercent((int32)pvpdamage_per);
            return percent;
        }
    }
    
    return 1.0f;
}

int32 GetDaysBetweenDates(time_t t1, time_t t2)
{
    Crown::CSDDateTime localDay1 = Crown::CSDDateTime(t1);
    Crown::CSDDateTime localDay2 = Crown::CSDDateTime(t2);

    if (t1 == 0)
        localDay1.SetDate(1970, 1, 2);

    if (t2 == 0)
        localDay2.SetDate(1970, 1, 2);

    localDay1.SetTime(0, 0, 0);
    localDay2.SetTime(0, 0, 0);

    int64 diffDay = localDay1.DiffDayNoDst(localDay2);
    return (int32)diffDay;
}

uint32 GetRuneTypeBySkillID( uint32 skillId )
{
    //const SkillTemplate* skillTmp = SkillConfig::Instance()->GetSkillTemplate(skillId);
    //uint32 skillRuneType = SKILL_RUNE_NONE;
    //if(skillTmp)
    //    skillRuneType = skillTmp->rune_type;
    //else
    //    CnError("no skill %d data\n", skillId);
    //return skillRuneType;
    return 1;
}

bool IsPureBySkillID( uint32 skillID )
{
    return true;
}

void SetPlayerToCityPosition(Player* player, Scene* scene)
{
    // 角色回城到出生点
    Trigger* FirstTrigger = scene->GetFirstTrigger();
    if(!FirstTrigger)
    {
        CnWarn("first trigger is not exist! scene id:%d!", scene->GetSceneID());
    }
    else
    {
        player->SetX(FirstTrigger->GetX());
        player->SetY(FirstTrigger->GetY());
        player->SetAngle(FirstTrigger->GetAngle());
    }
}

// 是否是大地图(包含主城)
bool IsWorldMap(uint32 dungeonID)
{
    return GetDungeonClass(dungeonID) == DUNGEON_CLASS_WORLD_ID || dungeonID == CITY_MAIN_CITY_MAP;
}

std::string MarQueeEquip(int32 quality, uint32 apper_id)
{
    return Crown::ToString(MARQUEE_EQUIP_TYPE) + "|" + Crown::ToString(quality) + "|" + Crown::ToString(apper_id);
}

std::string MarQueeItem(uint32 type, uint32 num, uint32 itemID)
{
    return Crown::ToString(MARQUEE_ITEM_TYPE) + "|" + Crown::ToString(type)  + "|" + Crown::ToString(itemID) + "|" + Crown::ToString(num);
}

std::string MarQueeRideQuality(uint32 quality, uint32 rideId)
{
    return Crown::ToString(MARQUEE_RIDE_TYPE) + "|" + Crown::ToString(quality) + "|" + Crown::ToString(rideId);
}

std::string MarQueeRideCollect(uint32 atlasID)
{
    return Crown::ToString(MARQUEE_RIDE_COLLECT_TYPE) + "|" + Crown::ToString(atlasID);
}

std::string MarQueeFollowPetQuality(uint32 quality, uint32 followPetID)
{
    return Crown::ToString(MARQUEE_FOLLOW_PET_TYPE) + "|" + Crown::ToString(quality) + "|" + Crown::ToString(followPetID);
}

std::string MarQueePlayerName(uint32 viplevel, std::string name)
{
    return Crown::ToString(MARQUEE_PLAYERNAME_TYPE) + "|" + Crown::ToString(viplevel) + "|" + name;
}

std::string MarQueeFactionPk(uint32 groupType)
{
    return Crown::ToString(MARQUEE_FACTION_TYPE) + "|" + Crown::ToString(groupType);
}

std::string MarQueeWing(uint32 level)
{
    return Crown::ToString(MARQUEE_WING_TYPE) + "|" + Crown::ToString(level);
}

std::string MarQueeVIP(uint32 level)
{
    return Crown::ToString(MARQUEE_VIP) + "|" + Crown::ToString(level);
}

std::string MarQueeVoidFieldRegularType(std::string langID, uint32 channelNum)
{
    return Crown::ToString(MARQUEE_VOID_FIELD_TYPE) + "|" + langID + "|" + Crown::ToString(channelNum);
}

std::string MarQueeVoidFieldSoloRegularType(std::string langID, uint32 channelNum)
{
    return Crown::ToString(MARQUEE_VOID_FIELD_SOLO_TYPE) + "|" + langID + "|" + Crown::ToString(channelNum);
}

std::string MarFashions(uint32 id,uint32 quality)
{
    return Crown::ToString(MARQUEE_FASHION_TYPE) + "|" + Crown::ToString(id) + "|" + Crown::ToString(quality);
}

std::string MarQuality(uint32 quality)
{
    return Crown::ToString(MARQUEE_QUALITY_TYPE) + "|" + Crown::ToString(quality);
}

std::string MarQueeLoginReceive(uint32 type, uint32 day)
{
    return Crown::ToString(type) + "|" + Crown::ToString(day);
}

std::string MarEgg( uint32 type, uint32 id, uint32 num )
{
    return Crown::ToString(MARQUEE_EGG) + "|" + Crown::ToString(num) + "|" + Crown::ToString(type) + "|" + Crown::ToString(id);
}

void MergeRewardTable(REWARD_TABLE& outTable, const REWARD_TABLE& inTable)
{
    for (std::vector<REWARD_ITEM>::const_iterator cIter = inTable.items.begin(); cIter != inTable.items.end(); ++cIter)
    {
        MergeRewardTableItemList(outTable, *cIter);
    }
}
