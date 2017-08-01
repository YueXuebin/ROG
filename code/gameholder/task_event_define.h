/*
 *	任务相关事件
 */
#ifndef task_event_define_h__
#define task_event_define_h__

enum
{
    //========================================================
    // 任务事件(任务事件id = 40000+任务目标)
    //========================================================
    TASK_EVENT_BEING                        = 40000,  // 任务事件开始
    TASK_EVENT_KILL_MONSTER                 = 40001,  // 杀怪
    TASK_EVENT_RADIS_KILL_MONSTER           = 40003,  // 扫荡杀怪
    TASK_EVENT_TALK_NPC                     = 40006,  // 跟NPC对话
    TASK_EVENT_PLAYER_DOOR_REWARD           = 40007,  // 领取勇者争霸奖励xx次数
    TASK_EVENT_ONLINE_GROW_REWARD           = 40008,  // 领取多少等级成长礼包
    TASK_EVENT_SYNTHETIZE_JEWEL_NUM_LEVEL   = 40009,  // 合成xx颗xx级宝石
    TASK_EVENT_PROTECT_SOUL_LEVEUP          = 40010,  // XX的护魂升级到xx级
    TASK_EVENT_GUID_POINT                   = 40011,  // 获得xx点公会贡献点
    TASK_EVENT_FASHION                      = 40012,  // 激活xx个xx化身
    TASK_EVENT_FASHION_BUY_BOX              = 40013,  // 化身系统购买xx宝箱xx次
    TASK_EVENT_ASHURA_KILL                  = 40014,  // 阿修罗杀人次数(废弃)
    TASK_EVENT_ZOMBIE_REWARD                = 40015,  // 领取多xx次僵尸生存礼包
    //TASK_EVENT_ACTIVE_TRIGGER               = 40016,  // 触发机关(废弃)
    TASK_EVENT_CHANGE_DUNGEON               = 40017,  // 跳转地图
    //TASK_EVENT_RADIS_DUNGEON                = 40018,  // 扫荡副本(废弃)
    TASK_EVENT_INLAID_EQUIP_JEWEL_LEVEL     = 40020,  // 镶嵌XX级宝石
    TASK_EVENT_UP_EQUIP_STAR                = 40023,  // 提升装备星
    TASK_EVENT_UP_EQUIP_LEVEL               = 40024,  // 提升装备等级
    TASK_EVENT_UP_EQUIP_QUALITY             = 40025,  // 提升装备品质
    TASK_EVENT_INLAID_EQUIP_JEWEL           = 40026,  // 镶嵌装备宝石
    TASK_EVENT_SYNTHETIZE_JEWEL             = 40027,  // 装备合成宝石
    TASK_EVENT_ACTIVE_PET                   = 40028,  // 宠物激发
    //TASK_EVENT_USE_SKILL                    = 40034,  // 使用技能(废弃)
    //TASK_EVENT_SET_FOLLOW_PET               = 40035,  // 设置跟宠(废弃)
    TASK_EVENT_SET_RIDE_PET                 = 40036,  // 设置骑宠
    //TASK_EVENT_TEAM_DUNGEON                 = 40037,  // 参与精英副本(废弃)
    TASK_EVENT_ADD_FRIEND                   = 40038,  // 添加好友
    TASK_EVENT_UP_HOROSCOPE                 = 40039,  // 开启星点
    //TASK_EVENT_USE_FASHION                  = 40040,  // 使用时装(废弃)
    TASK_EVENT_JOIN_ACTIVITY                = 40041,  // 参加活动
    TASK_EVENT_UP_WING                      = 40042,  // 翅膀升阶(弃用)
    TASK_EVENT_NPC_BUY                      = 40043,  // NPC购买物品
    TASK_EVENT_PLAYER_DOOR                  = 40045,  // 参加勇者争霸
    TASK_EVNET_CMPL_ONE_ROUND_DAILY         = 40046,  // 完成一轮日常任务
    TASK_EVENT_CRUSADE_TASK                 = 40047,  // 讨伐令
    TASK_EVENT_GOLD_TREE                    = 40049,  // 砸金像
    TASK_EVENT_TREASURY                     = 40050,  // 进入宝库
    //TASK_EVENT_WORLD_BOSS                   = 40051,  // 世界BOSS(废弃)
    TASK_EVENT_ONLINE_PK                    = 40052,  // 切磋 改为组队一次
    TASK_EVENT_GUARD                        = 40054,  // 护卫
    TASK_EVNET_SUMMON_RIDE                  = 40055,  // 召唤坐骑
    TASK_EVENT_EQUIP_LEVEL_STAT_NUM         = 40056,  // 装备升星次数
    TASK_EVENT_USE_HOROSCOPE                = 40057,  // 开启星点
    TASK_EVENT_USE_WING                     = 40058,  // 翅膀提升
    TASK_EVENT_UP_RUNE_LEVEL                = 40059,  // 技能符文升级
    //TASK_EVENT_DAILY_TASK                   = 40060,  // 日常任务(废弃)
    //TASK_EVENT_BIND_MONEY_BUY               = 40061,  // 绑定金购买(废弃)
    TASK_EVENT_JOIN_GUILD                   = 40062,  // 加入公会
    TASK_EVENT_DONATE_GUILD_POINT           = 40063,  // 捐赠公会点
    TASK_EVENT_GUILD_TASK                   = 40064,  // 完成多少次公会任务
    TASK_EVENT_PET_ONCE_ACTIVE              = 40065,  // 宠物一键激发
    TASK_EVENT_LEVE_UP_SWORD                = 40066,  // 圣剑升星多少次
    TASK_EVENT_LEVE_UP_EQUIP_TIMES          = 40069,  // 装备升级（次数）
    TASK_EVENT_LEVE_QUALITY_EQUIP_TIMES     = 40070,  // 装备升品（次数）
    TASK_EVENT_DAILY_CONSUME                = 40071,  // 每日消费
    TASK_EVENT_RECHARGE_TOTAL               = 40072,  // 累计充值
    TASK_EVENT_RECHARGE_SINGLE              = 40073,  // 每日单笔充值最大值
    TASK_EVENT_RECHARGE_DAILY               = 40075,  // 每日充值
    TASK_EVENT_UP_PLAYER_LEVEL              = 40076,  // 角色等级
    TASK_EVENT_UP_SKILL_RUNE                = 40077,  // 符文升级
    TASK_EVENT_RIDE_NUM_BY_QUALITY          = 40078,  // 获得坐骑品质数量
    TASK_EVENT_RECHARGE_OPENSVR             = 40079,  // 累计充值（开服）
    TASK_EVENT_CONSUME                      = 40080,  // 累计消费
    TASK_EVENT_INLAID_JEWEL_LEVEL           = 40081,  // 镶嵌装备宝石
    TASK_EVENT_RIDE_LEVEL                   = 40082,  // 提升坐骑指定等级
    TASK_EVENT_PASSIVE_SKILL_LEVEL          = 40083,  // 提升被动技能指定等级
    TASK_EVENT_ACTIVE_PROTECT_SOUL          = 40085,  // 宠物激活护魂
    TASK_EVENT_UP_PET_LEVEL                 = 40086,  // 提升宠物等级
    TASK_EVENT_EQUIP_JEWEL_NUM              = 40087,  // 镶嵌特定宝石数
    TASK_EVENT_MICRO_CLIENT                 = 40088,  // 微端登陆
    TASK_EVENT_LOGIN_DAY                    = 40095,  // 每日登陆
    TASK_EVENT_ONLIE_TIME                   = 40097,  // 累计在线时长

    TASK_EVENT_FASHION_BUY_TOTAL_NUM        = 40100,  // 所有使用宝藏卡和魔石的抽取行为抽XX次
    TASK_EVENT_FASHION_BUY_PLATINUM_NUM     = 40101,  // 铂金抽取XX次
    TASK_EVENT_FASHION_BUY_EXTREME_NUM      = 40102,  // 至尊抽取XX次

    TASK_EVENT_VIP_STATISTICS_REFRESH       = 40104,  // vip玩家数量变更
    TASK_EVENT_VIP_CHANGE                   = 40105,  // vip等级变更

    TASK_EVENT_VOID_FIELD_BOSS_PAR          = 40106,  // 虚空珍兽参与
    TASK_EVENT_VOID_FIELD_MOONLESS_PAR      = 40107,  // 虚空月黑参与
    TASK_EVENT_VOID_FIELD_BOSS_KILL         = 40108,  // 虚空珍兽击杀
    TASK_EVENT_TOP_UP_RANK_TOPN             = 40109,  // 充值排行奖励区间
    TASK_EVENT_TOP_UP_RANK_ENTER            = 40110,  // 充值排行上榜

    TASK_EVENT_TOP_UP_RANK_WING             = 40112,  // 翅膀排行奖励区间
    TASK_EVENT_TOP_UP_RANK_PET              = 40113,  // 宠物排行奖励区间
    TASK_EVENT_TOP_UP_RANK_RIDE             = 40114,  // 坐骑排行奖励区间
    TASK_EVENT_TOP_UP_RANK_BATTLE           = 40115,  // 战力排行奖励区间
    TASK_EVENT_TOP_UP_RANK_LEVEL            = 40116,  // 等级排行奖励区间
    TASK_EVENT_LOGIN_DAY_RESET              = 40117,  // 每日登陆(重置)
    TASK_EVENT_EGG_SMASH_NUM                = 40118,  // 砸金蛋次数
    TASK_EVENT_TOTAL_CONSUMPTION            = 40119,  // 累计消费(循环)
    TASK_EVENT_CUMULATIVE_RECHARGE          = 40120,  // 累计充值(循环)
    TASK_EVENT_NEW_UP_WING                  = 40121,  // 翅膀升阶
    TASK_EVENT_NEW_LEVE_UP_SWORD            = 40122,  // 圣剑升星多少次
    TASK_EVENT_TOP_UP_RANK_FASHION          = 40123,  // 化身战力排行活动
    TASK_EVENT_TOP_UP_RANK_OFFLINE_PLAYER   = 40124,  // 勇者争霸排行活动

    TASK_EVENT_BATTLE_POWER_FASHION         = 40125,  // 化身战力
    TASK_EVENT_BATTLE_POWER_PET             = 40126,  // 宠物战力
    TASK_EVENT_BATTLE_POWER_WING            = 40127,  // 翅膀战力
    TASK_EVENT_BATTLE_POWER_MOUNT           = 40128,  // 坐骑战力
    TASK_EVENT_BATTLE_POWER_TOTAL           = 40129,  // 总战力
    TASK_EVENT_FASHION_LEVEL                = 40131,  // 升级任意化身，多少级
    TASK_EVENT_FASHION_NUM_STAR_LEVEL       = 40132,  // 任意指定XX数量的XX星级以上的化身，升级到XX等级

    // vip领域
    TASK_EVENT_END                          = 49999,  // 任务事件结束
    //========================================================



};

#endif // task_event_define_h__
