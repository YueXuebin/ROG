#include "gameholder_pch.h"
#include "buff_manager.h"
#include "game_define.h"
#include "buff_data.h"
#include "buff.h"
#include "aura.h"
#include "buff_define.h"
#include "battle_define.h"
#include "skill_config_manager.h"

IMPLEMENT_SINGLETON(BuffManager)

BuffManager::BuffManager()
{

}

BuffManager::~BuffManager()
{
    for(BuffXMLMap::iterator iter = m_buffXmlMap.begin(); iter != m_buffXmlMap.end(); iter++)
    {
        CnDelete iter->second;
    }

    for(BuffDataMap::iterator iter = m_buffDataMap.begin(); iter != m_buffDataMap.end(); iter++)
    {
        CnDelete iter->second;
    }
}

bool BuffManager::Init()
{
    if(!ParseBuffList())
        return false;

    return true;
}

const BuffData* BuffManager::GetBuffData(uint32 buffId)
{
    BuffDataMap::iterator buffIt = m_buffDataMap.find(buffId);

    if(buffIt == m_buffDataMap.end())
    {
        return NULL;
    }

    return buffIt->second;
}

Buff*  BuffManager::CreateBuff(const BuffData* pBuffData, int32 buff_val1, int32 buff_val2, uint32 caster_id)
{
    Buff* pBuff = NULL;
    if(pBuffData->m_BuffInfo.is_aura)
    {
        pBuff = CnNew Aura(pBuffData, caster_id, buff_val1, buff_val2);
    }
    else
    {
        pBuff = CnNew Buff(pBuffData, caster_id, buff_val1, buff_val2);
    }

    return pBuff;
}

bool BuffManager::ParseBuffList()
{
    // 加载buff配置文件
    Crown::CSDFile buffFile;
    if(!buffFile.Open(RES_BUFF_PATH""BUFF_LIST_FILE, "rt"))
    {
        CnDbg("load buff file failed\n");
        return false;
    }

    buffFile.SeekToEnd();
    int32 fileLen = buffFile.GetPosition();
    buffFile.SeekToBegin();

    char* fileString = (char*)CnMalloc(sizeof(char)*(fileLen+1));

    uint32 readlen = buffFile.Read(fileString, fileLen);
    fileString[readlen] = '\0';

    buffFile.Close();

    // 正则表达式解析
    const char* pattern = "^.*buf";
    pcre *re;
    const char *error;
    int32 erroffset;
    re = pcre_compile(pattern, PCRE_MULTILINE, &error, &erroffset, NULL);
    CnAssert(re);

    int32 options = PCRE_NOTEMPTY_ATSTART;
    int32 start_offset = 0;

    const int32 overCount = 30;
    int32 ovector[overCount];

    char buffFileName[SD_MAX_PATH];
    std::vector<std::string>    m_buffFileNames;

    int rc;
    while (start_offset < fileLen && (rc = pcre_exec(re, 0, fileString, fileLen, start_offset, 0, ovector, sizeof(ovector))) >= 0)
    {
        for(int i = 0; i < rc; ++i)
        {
            int32 fileNameLen = ovector[2*i+1] - ovector[2*i];
            memcpy(buffFileName, (char*)fileString+ovector[2*i], fileNameLen);
            buffFileName[fileNameLen] = '\0';
            m_buffFileNames.push_back(buffFileName);
            //printf("%2d: %.*s\n", i, fileNameLen, fileString + ovector[2*i]);
        }
        start_offset = ovector[1];
    }

    pcre_free(re);
    CnFree(fileString);

    // 加载各Buff文件
    for(std::vector<std::string>::iterator iter = m_buffFileNames.begin(); iter != m_buffFileNames.end(); iter++)
    {
        //CnDbg((*iter).c_str());
        int32 tmpInt = 0;
        // 加载文件
        std::string fileName = RES_BUFF_PATH;
        fileName += (*iter).c_str();
        TiXmlDocument* pXMLDoc = CnNew TiXmlDocument(fileName.c_str());
        if(!pXMLDoc->LoadFile())
        {
            CRITICAL("buff %s load failed", fileName.c_str());
            CnDelete pXMLDoc;
            continue;
        }

        // buff数据
        BuffData* pBuffData = CnNew BuffData;

        // 查找id
        TiXmlElement* pRoot = pXMLDoc->RootElement();
        int32 buffID = 0; 
        pRoot->Attribute("id", &buffID);
        if(buffID<=0)
        {
            CnError("buff %s no id", fileName.c_str());
            CnDelete pXMLDoc;
            CnDelete pBuffData;
            continue;
        }
        pBuffData->m_BuffInfo.buff_id = buffID;

        // 读取buff相关配置
        const BUFF_ROOT_BUFF_STRUCT* buffConfig = SkillConfigManager::Instance()->GetBuffConfigData(buffID);
        if(!buffConfig)
        {
            CnError("buff %s id=%d not config\n", fileName.c_str(), buffID);
            CnDelete pXMLDoc;
            CnDelete pBuffData;
            continue;
        }

        // 效果类别
        if(buffConfig->effect_type == 1)
        {   // 增益
            pBuffData->m_BuffInfo.buff_type = BUFF_TYPE_GOOD;
        }
        else if(buffConfig->effect_type == 2)
        {   // 减益
            pBuffData->m_BuffInfo.buff_type = BUFF_TYPE_BAD;
        }
        else if(buffConfig->effect_type == 3)
        {   // 光环
            pBuffData->m_BuffInfo.buff_type = BUFF_TYPE_AURA;
        }
        else
        {   // 通用
            pBuffData->m_BuffInfo.buff_type = BUFF_TYPE_NORMAL;
        }

        // 是否为光环
        pBuffData->m_BuffInfo.is_aura = buffConfig->is_aure;

        if(pBuffData->m_BuffInfo.is_aura)
        {   // 光环
            pBuffData->m_BuffInfo.buff_overlap = BUFF_CAN_UPDATE;                   // 光环必为可更新
            pBuffData->m_BuffInfo.aura_radius = buffConfig->aura_radius;            // 光环半径
            pBuffData->m_BuffInfo.aura_buff_id = buffConfig->func_id;               // 光环生成的buffid

            if(pBuffData->m_BuffInfo.buff_type == BUFF_TYPE_GOOD)
            {   // 增益型光环(对自己\队友)
                pBuffData->m_BuffInfo.aura_self = true;
                pBuffData->m_BuffInfo.aura_friend = true;
                pBuffData->m_BuffInfo.aura_enemy = false;
            }
            else if(pBuffData->m_BuffInfo.buff_type == BUFF_TYPE_BAD)
            {   // 减益型光环(对敌人)
                pBuffData->m_BuffInfo.aura_self = false;
                pBuffData->m_BuffInfo.aura_friend = false;
                pBuffData->m_BuffInfo.aura_enemy = true;
            }
            else
            {
                CnAssert(false);            // 不应该存在此类光环
                pBuffData->m_BuffInfo.aura_self = true;
                pBuffData->m_BuffInfo.aura_friend = false;
                pBuffData->m_BuffInfo.aura_enemy = false;
            }

            // aura功能
            if(buffConfig->func_id == 2004)
            {   // 迅捷光环
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;        // 魔法流失
                buffFunc.arg1 = PROP_MP_LOSS_RATE;
                buffFunc.buff_val_index = 1;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 2005)
            {   // 活力光环
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;        // 魔法流失
                buffFunc.arg1 = PROP_MP_LOSS_RATE;
                buffFunc.buff_val_index = 1;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
        }
        else
        {   // buff
            pBuffData->m_BuffInfo.buff_overlap = buffConfig->overlap;

            // buff功能
            if(buffConfig->func_id == 1)
            {   // 击晕
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_STUN;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 2)
            {   // 不朽战吼
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_IMMORTAL;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 3)
            {   // 暴风盾
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_BLOCK;                             // 增加攻击格挡
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_REFLECT_DAMAGE_MUL;                // 反射伤害
                buffFunc.buff_val_index = 1;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 4)
            {   // 迅捷光环buff(提升攻击速度,施法速度,移动速度)
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_MOVE_SPEED;                        // 移动速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_ATTACK_PER_SEC;                    // 攻击速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_CAST_PER_SEC;                      // 施法速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 5)
            {   // 活力光环buff(每秒回复生命)
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_HEAL;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 6)
            {   // 脆弱诅咒(减少护甲和闪避)
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_ARMOR;                             // 护甲倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_DODGE;                             // 闪避倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 7)
            {   // 点燃
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_DAMAGE;
                buffFunc.arg1 = DAMAGE_CLASS_FIRE;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 8)
            {   // 冰缓
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_MOVE_SPEED;                        // 移动速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_ATTACK_PER_SEC;                    // 攻击速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_CAST_PER_SEC;                      // 施法速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 9)
            {   // 冰冻
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_STUN;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 10)
            {   // 感电
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_SHOCK;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 11)
            {   // 流血
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_BLEED;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 12)
            {   // 耐力球
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_SUB_PHYSICS_HURT;                  // 物理伤害免疫
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_SUB_MAGIC_HURT;                    // 元素伤害免疫
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 13)
            {   // 狂怒球
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_MOVE_SPEED;                        // 增加移动速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_ATTACK_PER_SEC;                    // 增加攻击速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_SCALE;
                buffFunc.arg1 = PROP_CAST_PER_SEC;                      // 增加施法速度
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 14)
            {   // 暴击球
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_CRITICAL_ATTACK;                        // 增加暴击率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 101)
            {   // 获得经验(倍率)
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_EXP_MUL;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 102)
            {   // 提升物理伤害和元素伤害（值）
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_PHYSICS_DAMAGE_MUL;                        // 物理伤害倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_MAGIC_DAMAGE_MUL;                          // 元素伤害倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 103)
            {   // 增加物理免伤和元素免伤（值）
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_SUB_PHYSICS_HURT;                       // 物理免伤倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
                buffFunc.buff_func_type = BUFF_FUNC_PROP_CHANGE;
                buffFunc.arg1 = PROP_SUB_MAGIC_HURT;                         // 元素免伤倍率
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else if(buffConfig->func_id == 104)
            {   // 提高怪物掉落品质（值）
                BUFF_FUNC buffFunc;
                buffFunc.buff_func_type = BUFF_FUNC_DROP_QUALITY;
                buffFunc.buff_val_index = 0;
                pBuffData->m_BuffInfo.buff_func.push_back(buffFunc);
            }
            else
            {
                CnAssert(false);            // 没有定义buff的功能
                CnWarn("buff %d has not func\n", buffID);
            }
        }
    
        CnAssert(m_buffXmlMap.find(buffID) == m_buffXmlMap.end());
        m_buffXmlMap[buffID] = pXMLDoc;
        m_buffDataMap[buffID] = pBuffData;
    }

    return true;
}
