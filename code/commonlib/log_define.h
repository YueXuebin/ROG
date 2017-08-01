/*
 *	日志相关定义
 */
#ifndef log_define_h__
#define log_define_h__


// 玩法系统ID定义
enum 
{
    // 技能
    PLAYSYSTEM_SKILL_UP = 101,                      // 技能升级
    PLAYSYSTEM_PASSIVE_SKILL_UP = 102,              // 被动技能升级
    // 装备
    PLAYSYSTEM_EQUIP_LEVELUP = 201,                 // 升级
    PLAYSYSTEM_EQUIP_STARUP = 202,                  // 升星
    PLAYSYSTEM_EQUIP_QULITYUP = 203,                // 升品
    PLAYSYSTEM_EQUIP_STONE = 204,                   // 嵌宝石
    // 战宠
    PLAYSYSTEM_FOLLOWPET_OPEN = 301,                // 激活
    PLAYSYSTEM_FOLLOWPET_SOUL_OPEN = 302,           // 护魂激活
    PLAYSYSTEM_FOLLOWPET_SOUL_UP = 303,             // 护魂升级
    // 星图
    PLAYSYSTEM_HOROSCOPE_UP = 401,                  // 星空图升级
    // 翅膀
    PLAYSYSTEM_WING_UNLOCK  = 501,                  // 翅膀属性解锁
    PLAYSYSTEM_WING_UP = 502,                       // 翅膀升阶
    PLAYSYSTEM_WING_OPEN = 503,                     // 解锁幻化
    // 坐骑
    PLAYSYSTEM_RIDE_OPEN = 601,                     // 获得坐骑
    PLAYSYSTEM_RIDE_MARK = 602,                     // 坐骑刻印
    // 化身
    PLAYSYSTEM_FASHION_OPEN = 701,                  // 获得化身
    // 圣剑(未完成)

    // 女神赐福
    PLAYSYSTEM_ONLINE_REWARD = 901,                 // 在线奖励
    PLAYSYSTEM_ONLINE_REWARD_BESTOW = 902,          // 福利奖励
    PLAYSYSTEM_ONLINE_REWARD_LEVEL = 903,           // 等级奖励
    // 活跃度
    PLAYSYSTEM_LIVENESS = 1001,                      // 活跃度奖励
    // 七日目标(未完成)
    
    // 签到
    PLAYSYSTEM_SIGN_SIGN = 1101,                    // 签到
    PLAYSYSTEM_SIGN_REWARD = 1102,                  // 签到奖励
    PLAYSYSTEM_SIGN_SEVEN_DAY = 1103,               // 七日登陆奖励

    // 勇者争霸
    PLAYSYSTEM_PLAYER_DOOR_REWARD = 1201,           // 勇者争霸奖励

    // VIP
	PLAYSYSTEM_VIP_GIFT_REWARD = 1301,              // 玩家领取vip礼包

    // 微端
    PLAYSYSTEM_WD_REWARD = 1401,                    // 领取微端奖励

    // 称号
    PLAYSYSTEM_TITLE_ADD = 1501,                    // 获得称号
    PLAYSYSTEM_TITLE_DEL = 1502,                    // 失去称号

};


// 财产ID定义
enum
{
    WEALTH_VIGOR        = 101,                      // 体力
    WEALTH_GUILDPOINT   = 102,                      // 公会贡献点
    WEALTH_INCARNATION  = 103,                      // 化身点
    WEALTH_HONORPOINT   = 104,                      // 荣誉点

};


#endif // log_define_h__
