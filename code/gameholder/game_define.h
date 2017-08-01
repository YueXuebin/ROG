/*
 *	游戏相关的一些参数定义
 */
#ifndef game_define_h__
#define game_define_h__

const uint32 g_DungeonEndTime = 10000;          // 副本无人，自动关闭时间

const uint32 g_FactionPK_WAIT_NUM = 25;         // 圣剑争锋等待大厅人数

const uint32 g_MoveDelta = 500;                 // 移动误差

const uint32 g_MaxBuyNum = 10000;               // 购买的最大数量

const uint32 g_BattleStateTime = 5000;          // 战斗状态退出时间

const uint32 g_TeamFlyNearDistance = 200;       // 飞到队友身边，离队友的距离

const int32  g_ResurgenceHp = 10000;            // 复活后回血量

const real  g_TeamMateRewardDisCount = 0.2f;     // 队友的副本奖励折扣(至尊魔宫)

const uint32 g_TeamPostionSyncTime = 5000;      // n秒一次队伍位置同步


/////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------
//  固定道具ID
//----------------------------------------------------
const uint32 ITEM_ID_BEGIN = 10000;             // 物品类ID起始值

const uint32 MATERIAL_ID_EQUIP_CHANGE_COLOR = 14101;    // 幻色石
const uint32 MATERIAL_ID_EQUIP_ADD_HOLE     = 14102;    // 打孔石
const uint32 MATERIAL_ID_EQUIP_REFINEMENT   = 14115;    // 洗练石
const uint32 MATERIAL_ID_EQUIP_STAR_SUCCESS = 14119;    // 升星辅石


//----------------------------------------------------
//  商城服务
//----------------------------------------------------

const uint32 MALL_SERVICE_START_ID = 25000;	// 开始ID
const uint32 MALL_SERVICE_END_ID = 29999;	// 结束ID

// 装备洗练加锁
const uint32 MALL_SERVICE_EQUIP_WASH_UP_LOCK_1	= 25000; 	// 装备洗练加1个锁
const uint32 MALL_SERVICE_EQUIP_WASH_UP_LOCK_2	= 25001; 	// 装备洗练加2个锁
const uint32 MALL_SERVICE_EQUIP_WASH_UP_LOCK_3	= 25002; 	// 装备洗练加3个锁

// 幻色加锁
const uint32 MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_1	= 25010;	// 装备宝石孔幻色加1个锁
const uint32 MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_2	= 25011;	// 装备宝石孔幻色加2个锁
const uint32 MALL_SERVICE_EQUIP_GEM_HOLE_CHNAGE_COLOR_LOCK_3	= 25012;	// 装备宝石孔幻色加3个锁

// 技能石融合自动补全
const uint32 MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_1	= 25020;	// 技能石融合补全1个白色品质
const uint32 MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_2	= 25021;	// 技能石融合补全1个绿色品质
const uint32 MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_3	= 25022;	// 技能石融合补全1个蓝色品质
const uint32 MALL_SERVICE_FUSION_AUTO_FILL_SKILL_STONE_4	= 25023;	// 技能石融合补全1个紫色品质

//----------------------- END ------------------------


#endif // game_define_h__
