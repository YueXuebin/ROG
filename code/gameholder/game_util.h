#ifndef game_util_h
#define game_util_h

#include "gameholder_pch.h"

const int32 NULL_GRID(-1);

class TiXmlElement;

const std::vector<uint32>& GetBasePropertyIdList();
const std::vector<uint32>& GetAdvancePropertyIdList();

void MergeRewardTable(REWARD_TABLE& outTable, const REWARD_TABLE& inTable);
void MergeRewardTableItemList(REWARD_TABLE& outTable, const REWARD_ITEM& inItem);

bool LoadRandRewardItem(TiXmlElement* itEle, RAND_REWARD_ITEM& rand_item);          // 加载随机奖励信息
bool LoadRewardItem(TiXmlElement* itEle, REWARD_ITEM& reward_item);                 // 加载奖励信息(TiXmlElement, 奖励项)

bool LoadRewardTable(TiXmlElement* itEle, REWARD_TABLE& rewardTable, int32 flag = REWARD_FLAG_NORMAL);  // 加载奖励信息(TiXmlElement, 奖励列表)
bool LoadRewardTable(const std::string& rewardString, REWARD_TABLE& rewardTable, int32 flag = REWARD_FLAG_NORMAL);   // 加载奖励信息(字符串, 奖励列表)



class SceneObject;
class BattleUnit;
class Scene;
class Player;

// 获得 Vector 中的 Bit 标记
uint32  GetVectorBitFlag(std::vector<uint32>* list, uint32 val, uint32 bitLen);

// 设置 Vector 中的 Bit 标记
void    SetVectorBitFlag(std::vector<uint32>* list, uint32 val, uint32 bitLen, uint32 bitFlag);

// 两单位间距离(减去两单位半径)
real    DistanceBySceneObject(SceneObject* pOne, SceneObject* pTwo);

// 默认的分配组
void    SetDefaultGroup( BattleUnit* creature );

// 场景中怪被杀完
bool    IsSceneEnemyClear( Scene* pScene );

// 是否是木桶
bool    IsBarrel(uint32 creatureId);

// 获得技能对应的符文ID
uint32 GetRuneTypeBySkillID( uint32 skillId );

// 技能是否的纯法系伤害
bool IsPureBySkillID( uint32 skillID );

// 获得TeamLeader的PlayerID
uint64  GetTeamLeaderID(TEAM_INFO* pTeamInfo);

// 字条串是否相同
bool    IsStrSame(const char* str1, const char* str2);
bool    IsStrSame(const std::string& str1, const std::string& str2);

// 合并相同的奖励
void    FuseSameReward(REWARD_TABLE& table);

// 万分比换算
real    ToPercent(int32 val);
int32   ToPercentInt(int32 val);

// 一级属性转二级属性
PropertyList CalcLinkageProp(int32 strength, int32 agility, int32 constitution, int32 spirit);

// 计算间隔天数
int32 GetDaysBetweenDates(time_t t1, time_t t2);

// PVP伤害减免(返回减伤率)
real  PVPDamageReduce(BattleUnit* pAttacker, BattleUnit* pDefender);

// 设置玩家位置到主城坐标
void SetPlayerToCityPosition(Player* player, Scene* scene);

// 是否是大地图(包含主城)
bool IsWorldMap(uint32 dungeonID);

// 网格位置转换
inline int32 RealPosToCellPos(real p, uint32 cellLength)
{
    return (p/cellLength)>0 ? int32((p/cellLength)) : int32(p/cellLength)-1;
}

inline real CellPosToRealPos(int32 p, uint32 cellLength)
{
    return real(p) * cellLength + real(cellLength)/2;
}


// ---------------------------------------------------------------
// 设置公告字符串类型
// ---------------------------------------------------------------
// 装备
std::string MarQueeEquip(int32 quality, uint32 apper_id);
// 物品
std::string MarQueeItem(uint32 type, uint32 num, uint32 itemID = 0);
// 骑宠
std::string MarQueeRideQuality(uint32 quality, uint32 rideId);
// 图鉴
std::string MarQueeRideCollect(uint32 atlasID);
// 跟宠
std::string MarQueeFollowPetQuality(uint32 quality, uint32 followPetID);
// 玩家
std::string MarQueePlayerName(uint32 viplevel, std::string name);
// 两派PK
std::string MarQueeFactionPk(uint32 groupType);
// 翅膀
std::string MarQueeWing(uint32 level);
// VIP
std::string MarQueeVIP(uint32 level);
// 虚空幻境频道开启类型
std::string MarQueeVoidFieldRegularType(std::string langID, uint32 channelNum);
// 虚空幻境SOLO频道开启类型
std::string MarQueeVoidFieldSoloRegularType(std::string langID, uint32 channelNum);
//化身
std::string MarFashions(uint32 id,uint32 quality);
// 品质
std::string MarQuality(uint32 quality);
// 玩家名称
std::string MarQueeLoginReceive(uint32 type, uint32 day);
// 砸蛋公告
std::string MarEgg(uint32 type, uint32 id, uint32 num);
#endif
