#include "gameholder_pch.h"
#include "battle_offline_config.h"
#include "message_encoder.h"
#include "game_util.h"
IMPLEMENT_SINGLETON(BattleOfflineConfig)

// ==================================================
// 添加属性
// ==================================================
//void OfflinePlayerTemplate::AddProp(uint32 type, struct EquipPropSet* propSet)
//{
//    if(!propSet) return;
//
//    for(PropertyList::iterator propIt = propSet->props.begin(); propIt != propSet->props.end(); ++propIt)
//    {
//        switch(propIt->type)
//        {
//        case PROP_MAXHP:
//            m_max_hp.AddValue(type, propIt->value);
//            break;
//        case PROP_MAXMP:
//            m_max_mp.AddValue(type, propIt->value);
//            break;
//        case PROP_HP_REC_RATE:
//            m_res_hp.AddValue(type, propIt->value);
//            break;
//        case PROP_MP_REC_RATE:
//            m_res_mp.AddValue(type, propIt->value);
//            break;
//        case PROP_STRENGTH:
//            m_strength.AddValue(type, propIt->value);
//            break;
//        case PROP_AGILITY:
//            m_agility.AddValue(type, propIt->value);
//            break;
//        case PROP_SPIRIT:
//            m_spirit.AddValue(type, propIt->value);
//            break;
//        case PROP_CONSTITUTION:
//            m_constitution.AddValue(type, propIt->value);
//            break;
//        case PROP_ATTACK:
//            m_attack.AddValue(type, propIt->value);
//            break;
//        case PROP_DEFENCE:
//            m_defence.AddValue(type, propIt->value);
//            break;
//        case PROP_ATTACK_PER_SEC:
//            m_attack_speed.AddValue(type, propIt->value);
//            break;
//        case PROP_CRITICAL_ATTACK:
//            m_critical_attack.AddValue(type, propIt->value);
//            break;
//        case PROP_HIT_RATE:
//            m_hit_rate.AddValue(type, propIt->value);
//            break;
//        case PROP_DODGE:
//            m_dodge.AddValue(type, propIt->value);
//            break;
//        case PROP_MOVE_SPEED:
//            m_move_speed.AddValue(type, propIt->value);
//            break;
//        case PROP_CONSUME_MP:
//            m_consume_mp.AddValue(type, propIt->value);
//            break;
//        case PROP_DAMAGE_REDUCTION:
//            m_damage_reduction.AddValue(type, propIt->value);
//            break;
//        case PROP_FIRE_DEFENCE:
//            m_fire_defence.AddValue(type, propIt->value);
//            break;
//        case PROP_ICE_DEFENCE:
//            m_ice_defence.AddValue(type, propIt->value);
//            break;
//        case PROP_THUNDER_DEFENCE:
//            m_thunder_defence.AddValue(type, propIt->value);
//            break;
//        default:
//            break;
//        }
//    }
//}

// ==================================================
// 计算属性
// ==================================================
void OfflinePlayerTemplate::CalProp()
{
    //LevelUp* levelUp = PropertyConfig::Instance()->GetLevelUP(m_career, m_level);
    //if(!levelUp) return;

    //// 根据角色等级添加基本属性
    //// --------------------------------------------------
    //m_strength.SetBaseValue(levelUp->strength);
    //m_agility.SetBaseValue(levelUp->agility);
    //m_spirit.SetBaseValue(levelUp->spirit);
    //m_constitution.SetBaseValue(levelUp->constitution);
    //m_attack.SetBaseValue(levelUp->attack);
    //m_defence.SetBaseValue(levelUp->defence);
    //m_fire_defence.SetBaseValue(levelUp->fire_defence);
    //m_ice_defence.SetBaseValue(levelUp->ice_defence);
    //m_thunder_defence.SetBaseValue(levelUp->thunder_defence);
    //m_res_hp.SetBaseValue(levelUp->hp_rate);
    //m_res_mp.SetBaseValue(levelUp->mp_rate);
    //m_max_hp.SetBaseValue(levelUp->max_hp);
    //m_max_mp.SetBaseValue(levelUp->max_mp);
    //// 根据装备添加属性
    //// --------------------------------------------------
    //EquipPropSet* equipProp = 0;
    //for(std::vector<uint32>::iterator equipIt = m_equip_list.begin(); equipIt != m_equip_list.end(); ++equipIt)
    //{
    //    equipProp = PropertyConfig::Instance()->GetEquipProp(*equipIt);
    //    if(!equipProp) continue;

    //    AddProp(PROP_UNIT_EQUIP, equipProp);
    //}

    //// 根据职业计算出二级属性
    //// --------------------------------------------------
    //int32 strength = m_strength.GetValueByType(PROP_UINT_ALL);
    //int32 agility = m_agility.GetValueByType(PROP_UINT_ALL);
    //int32 constitution = m_constitution.GetValueByType(PROP_UINT_ALL);
    //int32 spirit = m_spirit.GetValueByType(PROP_UINT_ALL);

    //switch(m_career)
    //{
    //case CAREER_WARRIOR:
    //    {
    //        m_attack.AddValue(PROP_UNIT_EQUIP, int32(strength * ToPercent(16500)));
    //        m_defence.AddValue(PROP_UNIT_EQUIP, int32(agility * ToPercent(16000)));
    //        m_max_hp.AddValue(PROP_UNIT_EQUIP, int32((strength + constitution) * ToPercent(31000)));
    //        m_max_mp.AddValue(PROP_UNIT_EQUIP, int32(spirit *ToPercent(31000)));
    //    }
    //    break;
    //case CAREER_MAGE:
    //    {
    //        m_attack.AddValue(PROP_UNIT_EQUIP, int32(spirit * ToPercent(16000)));
    //        m_defence.AddValue(PROP_UNIT_EQUIP, int32(agility * ToPercent(14000)));
    //        m_max_hp.AddValue(PROP_UNIT_EQUIP, int32((strength + constitution)* ToPercent(27000)));
    //        m_max_mp.AddValue(PROP_UNIT_EQUIP, int32((spirit * ToPercent(68000))));
    //    }
    //    break;
    //case CAREER_PREIST:
    //    {
    //        m_attack.AddValue(PROP_UNIT_EQUIP,int32(agility* ToPercent(14000)));
    //        m_defence.AddValue(PROP_UNIT_EQUIP, int32(agility * ToPercent(15000)));
    //        m_max_hp.AddValue(PROP_UNIT_EQUIP, int32((strength + constitution)* ToPercent(20000)));
    //        m_max_mp.AddValue(PROP_UNIT_EQUIP, int32(spirit * ToPercent(65000)));
    //    }
    //    break;
    //default:
    //    break;
    //}
}

BattleOfflineConfig::BattleOfflineConfig()
{
    
}

BattleOfflineConfig::~BattleOfflineConfig()
{
    
}

bool BattleOfflineConfig::LoadConfig(const char* path)
{
    OfflinePlayerTemplate tmpl;
    OFFLINE_PLAYER_INFO player_record;
    TiXmlDocument xmlDoc;

    uint32 equip_id = 0;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load item file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) return false;

    //服务器配置信息
    for(TiXmlElement* itemEle = xmlRoot->FirstChildElement(); itemEle; itemEle = itemEle->NextSiblingElement())
    {
        tmpl.Clear();

        tmpl.m_playerID = Crown::SDAtou(itemEle->Attribute("id"));
        tmpl.m_name = itemEle->Attribute("name");

        tmpl.m_sex = Crown::SDAtou(itemEle->Attribute("sex"));
        tmpl.m_career = Crown::SDAtou(itemEle->Attribute("career"));
        tmpl.m_level = SDAtoi(itemEle->Attribute("level"));
        //tmpl.m_move_speed.SetValue(PROP_UNIT_BASE, SDAtoi(itemEle->Attribute("speed")));

        equip_id = Crown::SDAtou(itemEle->Attribute("weapon"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("forehead"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("shoulder"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("clothes"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("gloves"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("shoes"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("necklace"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("amulet"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("leftring"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        equip_id = Crown::SDAtou(itemEle->Attribute("rightring"));
        if(equip_id) tmpl.m_equip_list.push_back(equip_id);

        tmpl.CalProp();

        player_record.Clear();
        player_record.player_id_l = tmpl.m_playerID;
        player_record.player_id_h = 0;
        player_record.name = tmpl.m_name;
        player_record.sex = tmpl.m_sex;
        player_record.career = tmpl.m_career;
        player_record.level = tmpl.m_level;
        player_record.battle_power = tmpl.m_battle_power;
        //player_record.attack = tmpl.m_attack.GetValue();
        //player_record.defence = tmpl.m_defence.GetValue();
        //player_record.attack_speed = tmpl.m_attack_speed.GetValue();
        //player_record.critical_attack = tmpl.m_critical_attack.GetValue();
        //player_record.hit_rate = tmpl.m_hit_rate.GetValue();
        //player_record.dodge = tmpl.m_dodge.GetValue();
        //player_record.res_hp = tmpl.m_res_hp.GetValue();
        //player_record.res_mp = tmpl.m_res_mp.GetValue();
        //player_record.max_hp = tmpl.m_max_hp.GetValue();
        //player_record.max_mp = tmpl.m_max_mp.GetValue();
        //player_record.fire_defence = tmpl.m_fire_defence.GetValue();
        //player_record.ice_defence = tmpl.m_ice_defence.GetValue();
        //player_record.thunder_defence = tmpl.m_thunder_defence.GetValue();
        //player_record.move_speed = tmpl.m_move_speed.GetValue();
        if(tmpl.m_equip_list.size() == 0)
        {
            CnFatal("offline player %s no equip\n", tmpl.m_name.c_str());
        }
        for(std::vector<uint32>::iterator equipIt = tmpl.m_equip_list.begin(); 
            equipIt != tmpl.m_equip_list.end(); ++equipIt)
        {
            OFFLINE_EQUIP_INFO equip_info;
            equip_info.Clear();
            equip_info.appear_id = *equipIt;
            equip_info.star = tmpl.m_level;
            player_record.equip_list.push_back(equip_info);
        }

        if(m_players.find(tmpl.m_playerID) == m_players.end())
        {
            m_players[tmpl.m_playerID] = player_record;

            int32 power_segment = player_record.battle_power;
            if(power_segment < 3000) { power_segment = 0; }
            else if((3000<=power_segment) && (power_segment < 5000)) { power_segment = 1; }
            else if((5000<=power_segment) && (power_segment < 7000)) { power_segment = 2; }
            else { power_segment = 3; }

            if(m_player_segment.find(power_segment) == m_player_segment.end())
            {
                BofPlayerList bof_player_list;
                bof_player_list.push_back(&m_players[tmpl.m_playerID]);
                m_player_segment[power_segment] = bof_player_list;
            }
            else
            {
                m_player_segment[power_segment].push_back(&m_players[tmpl.m_playerID]);
            }
        }
        else
        {
            CnWarn("Duplicate playerid: %u\n", tmpl.m_playerID);
        }
    }

    return true;
}

OFFLINE_PLAYER_INFO* BattleOfflineConfig::GetPlayerTemplate(uint32 playerID)
{
    BofPlayerMap::iterator playerIt = m_players.find(playerID);
    if(playerIt != m_players.end())
    {
        return &playerIt->second;
    }
    return 0;
}

void BattleOfflineConfig::GetPlayerTmplList(uint32 battle_power, std::vector<OFFLINE_PLAYER_INFO*>& list)
{
    BofPlayerList rand_list;
    BofPowerSegment::iterator powerSegIt;

    // 查找是否存在此分数段内的玩家模板列表
    if(battle_power < 3000) { battle_power = 0; }
    else if((3000<=battle_power) && (battle_power < 5000)) { battle_power = 1; }
    else if((5000<=battle_power) && (battle_power < 7000)) { battle_power = 2; }
    else { battle_power = 3; }

    powerSegIt = m_player_segment.find(battle_power);
    if(powerSegIt == m_player_segment.end()) return;

    // 如果少于 MAX_BOF_PLAYER_NUM 个则全部放入列表中
    if(powerSegIt->second.size() <= MAX_BOF_PLAYER_NUM)
    {
        for(BofPlayerList::iterator playerIt = powerSegIt->second.begin(); 
                playerIt != powerSegIt->second.end(); ++playerIt)
        {
            list.push_back(*playerIt);
        }
        return;
    }

    rand_list = powerSegIt->second; // 复制一遍列表
    for(uint32 i=0; (i<MAX_BOF_PLAYER_NUM) && rand_list.size(); ++i)
    {
        // 计算随机位置，并将此模板压入队列中
        uint32 rand_num = rand_num = Crown::SDRandom(rand_list.size());
        list.push_back(rand_list[rand_num]);
        // 删除已压入队列的模板
        rand_list.erase(rand_list.begin() + rand_num);
    }
}
