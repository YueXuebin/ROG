/*
 *	事件相关定义
 */
#ifndef event_define_h__
#define event_define_h__

#include "task_event_define.h"

enum
{
    ///////////////////////////////////////////////////////////////////////////////////
    //玩家死亡
    PLAYER_EVENT_DEAD  = 100,
    // 战斗事件
    BATTLE_EVENT_ONHIT,                         // 被攻击事件(arg1=攻击者id)
    BATTLE_EVENT_ATTACK,                        // 攻击事件
    BATTLE_EVENT_MONSTER_DEAD,                  // 怪物死亡(arg1=objectId, arg2=杀手objectId)
    BATTLE_EVENT_BOSS_DEAD,                     // boss死亡(arg1=生物id)
    BATTLE_EVENT_PLAYER_DEAD,                   // 玩家死亡(arg1=生物唯一id)
    BATTLE_EVENT_OFFLINE_PLAYER_DEAD,           // 离线玩家死亡
    BATTLE_EVENT_WORLD_BOSS_DEAD,               // 世界Boss死亡(arg1=生物id, arg2=凶手id)
    BATTLE_EVENT_GUILD_BOSS_DEAD,               // 公会Boss死亡(arg1=凶手id)
    BATTLE_EVENT_DAMAGE,                        // 造成伤害(arg1=攻击者ID, arg2=受到伤害量, arg3=被攻击者ID)
    BATTLE_EVENT_SELF_HURT,                     // 受到伤害(只发给自己,arg1=攻击者ID, arg2=受到伤害量)
    BATTLE_EVENT_SELF_SKILL_CRITICAL,           // 技能暴击(只发给自己, arg1=被攻击者ID)
    BATTLE_EVENT_SELF_SKILL_MELEE,              // 技能近战击中(只发给自己, arg1=被攻击者ID)
    BATTLE_EVENT_SELF_SKILL_PROJECTILE,         // 技能投射命中(只发给自己, arg1=被攻击者ID)
    BATTLE_EVENT_RESURGENCE,                    // 复活(arg1=triggerID)
    BATTLE_EVENT_SCENE_ATTACK,                  // 场景广播攻击

    ///////////////////////////////////////////////////////////////////////////////////
    // 场景事件
    SCENE_EVENT_DEAD = 201,                 // 有战斗对象死亡(arg1=死者id, arg2=凶手id)
    SCENE_EVENT_CLEAR,                      // 场景怪清场
    SCENE_EVENT_TRIGGER_PLAYER_ENTER,       // 玩家进入Trigger(arg1=triggerID, arg2=进入的玩家id)
    SCENE_EVENT_TRIGGER_PLAYER_LEAVE,       // 玩家离开Trigger(arg1=triggerID, arg2=离开的玩家id)
    SCENE_EVENT_SPAWN_WAVE_START,           // 每波数开始刷怪通知    
    SCENE_EVENT_SPAWN_WAVE_ALL_DEAD,        // 该波怪死光
    SCENE_EVENT_SPAWN_ALL_DEAD,             // 刷怪点刷出来的怪死光(arg1=刷怪点ID, 0)
    SCENE_EVENT_BOSS_DEAD_DROP_ITEM,        // BOSS死亡掉落物品
    SCENE_EVENT_SCOURGE_CORPSE_DROP_ITEM,   // 鞭尸掉落物品

    ///////////////////////////////////////////////////////////////////////////////////
    // 系统事件
    SYSTEM_EVENT_LOAD_OFFLINE_PLAYER    = 501,        // 加载离线玩家数据成功(arg1=playerID_h, arg2=playerID_l)
    //SYSTEM_EVENT_WORLD_BOSS_START       = 502,      // 开启世界BOSS(arg1=dungeonID)
    //SYSTEM_EVENT_WORLD_BOSS_END         = 503,      // 结束世界BOSS(arg1=dungeonID, arg2=WorldBossID)

    ///////////////////////////////////////////////////////////////////////////////////
    // 称号事件
    EVENT_WORLD_BOSS_DAMAGE_RANK        = 601,  //杀死世界boss排名
    EVENT_ANSWER_RANK                   = 602,  //答题排名
    EVENT_OFFLINE_RANK                  = 603,  //离线竞技
    EVENT_HOSERUNING_RANK               = 604,  //宠物竞速
    EVENT_TASK_TITLE                    = 605,  //任务称号
    EVENT_ASHUAR_TITLE                  = 606,  //修罗场排名
    EVENT_FOLLOW_TITLE                  = 607,  //宠物称号
    EVENT_GUIDBOSS_RANK                 = 608,  //魔物讨伐令
    EVENT_TWIGHT_WAR                    = 609,  //暮光之战排名
    EVENT_USE_ITEM                      = 610,  //使用物品得到称号
    EVENT_GUILD_RANK_TITLE              = 611,  //公会排行称号
    EVENT_BATTLE_RANK_TITLE             = 612,  //战斗力排行
    EVENT_HOROSCOPE_RANK_TITLE          = 613,  //星空图排行称号
    EVENT_ASHURA_RANK_TITLE             = 614,  //修罗场称号
    EVENT_GET_VIP_REWARD_TITLE          = 615,  //得到vip4奖励
    EVENT_WING_RANK_TITLE               = 616,  //翅膀排行
    EVENT_PET_RANK_TITLE                = 617,  //宠物排行
    EVENT_RIDE_RANK_TITLE               = 618,  //坐骑排行
    EVENT_FB_LIKE_TITLE                 = 619,  //FB点赞

    //成就称号
    EVENT_ACHIVE_TITLE                  = 697,//至成就称号
    // 成就称号
    EVENT_TITLE_TREASURY                = 699,

    ///////////////////////////////////////////////////////////////////////////////////
    // 成就事件
    EVENT_ACHIVE_BATTLE                 = 701,      //至尊魔攻事件
    EVENT_ACHIVE_MONEY_TREEE            = 702,      //摇钱树
    EVENT_ACHIVE_ACHIVE_TREASURY        = 703,      //宝库副本
    EVENT_ACHIVE_DAILY_DUNGE            = 704,      //日常副本
    EVENT_ACHIVE_TEAM_DUNGE_HARD        = 705,      //多人副本困难
    EVENT_ACHIVE_TEAM_DUNGE_HELL        = 706,      //多人副本地狱
    EVENT_ACHIVE_CRUSADE_ORDER_LOW      = 707,      //讨伐令
    EVENT_ACHIVE_WORLD_BOSS             = 708,      //世界boss
    EVENT_ACHIVE_OFF_LINE_PK            = 709,      //离线竞技
    EVENT_AHIVE_ON_LINE_PK              = 710,      //在线竞技
    EVENT_AHIVE_LANDER                  = 711,      //天梯赛
    EVENT_ACHIVE_ASHURA                 = 712,      //修罗场任务
    EVENT_ACHIVE_ANSWERING              = 713,      //答题
    EVENT_ACHIVE_HOSERUING              = 714,      //宠物竞速
    EVENT_ACHIVE_FARM_WORK              = 715,      //种地，没有实现
    EVENT_ACHIVE_GUARD                  = 716,      //护卫任务
    EVENT_ACHIVE_TOWER                  = 717,      //塔防
    EVENT_ACHIVE_BOSS_CHALLENGE         = 718,      //boss挑战
    EVENT_ACHIVE_ADVENTUER              = 719,      //冒险
    EVENT_ACHIVE_MINIG                  = 720,      //采矿

    ///////////////////////////////////////////////////////////////////////////////////
    // 开旗事件
    EVENT_TOUCH_ACTIVATE                = 800,      // 采集点激活

    ///////////////////////////////////////////////////////////////////////////////////
    // 宝库事件
    EVENT_TREASURY_KILL_MONSTER         = 900,      // 宝库杀怪
    EVENT_TREASURY_GAMESTART            = 901,      // 宝库开始
    EVENT_TREASURY_GAMEEND              = 902,      // 宝库结束

    // 金像事件
    EVENT_GOLDTREE_DAMAGE               = 910,      // 金像伤害
    EVENT_GOLDTREE_GAMESTART            = 911,      // 金像开始
    EVENT_GOLDTREE_GAMEEND              = 912,      // 金像结束

    ///////////////////////////////////////////////////////////////////////////////////
    // 玩家复活事件
    EVENT_PLAYER_RESURGENCE             = 1000,     // 玩家复活事件
    EVENT_PLAYER_RESURGENCE_REQ         = 1001,     // 玩家请求复活事件
    EVENT_DUNGEON_ADD_LIFE_COUNT_REQ    = 1002,     // 玩家请求增加副本复活机会事件

    EVENT_ACIIVATION_PET                        = 1010,     //发送激活宠物事件
    EVENT_RIDE_MARK_PET                         = 1011,     //发送骑宠刻印槽的标记
    EVNET_ADD_PlAYER_EXP                        = 1012,     //添加玩家经验
    EVENT_CHANGE_VIP                            = 1013,     //vip改变
    EVENT_NOACIIVATION_PET                      = 1014,     //如果没有解锁，那么设置宠物状态
    EVENT_VIP_OUT_OF_DATE                       = 1015,     //vip过期

    EVENT_PETFORMATION_STATE                    = 1016,     //设置宠物上阵 状态

    // 副本事件
    DUNGEON_FINISH                              = 1100,     //副本结算完成

    // 世界尽头事件
    EVENT_JUMPING_OFF_BUILD                     = 1200,     // 建塔
    EVENT_JUMPING_OFF_SKILL                     = 1201,     // 使用全屏技能
    EVENT_JUMPING_OFF_SKILL_MONEYPOINT          = 1202,     // 使用全屏技能花魔石的回调

    ///////////////////////////////////////////////////////////////////////////////////
    EVENT_ACTIVE_PASSIVE_SKILL                  = 2010,     // 激活被动技能

    // 玩家升级事件
    EVENT_PLAYER_LEVEL_UP                       = 2050,     // 玩家升级事件

    ///////////////////////////////////////////////////////////////////////////////////
    // 活跃度事件
    LIVENESS_EVENT_BEGIN                        = 3000,     // 活跃度事件起始
    LIVENESS_EVENT_TARGET_SIGN                  = 3001,     // 签到
    LIVENESS_EVENT_TARGET_GOLD_TREE             = 3002,     // 探索魔王金像
    LIVENESS_EVENT_TARGET_TREASURY_DUNGEON      = 3003,     // 探索藏宝库
    LIVENESS_EVENT_TARGET_UPGRADE_PET           = 3004,     // 宠物激发
    LIVENESS_EVENT_TARGET_RIDE_TRAIN            = 3005,     // 坐骑训练
    LIVENESS_EVENT_TARGET_PLAYER_DOOR           = 3006,     // 勇者争霸挑战
    LIVENESS_EVENT_TARGET_FINISH_PLOT_DUNGEON   = 3007,     // 挑战剧情副本
    LIVENESS_EVENT_TARGET_DUNGEON_TEAM          = 3008,     // 挑战精英副本
    LIVENESS_EVENT_TARGET_PROTECT_SOUL          = 3009,     // 挑战护魂迷宫
    LIVENESS_EVENT_TARGET_MONSTER_DOOR          = 3010,     // 怪物之门挑战
    LIVENESS_EVENT_TARGET_BABEL                 = 3011,     // 挑战至尊魔宫
    LIVENESS_EVENT_TARGET_ACCEPT_CRUSADE        = 3012,     // 领取讨伐令委托
    LIVENESS_EVENT_TARGET_FINISH_ALL_DAILY_TASK = 3013,     // 进行1轮日常任务
    LIVENESS_EVENT_TARGET_GUARD                 = 3014,     // 护卫
    LIVENESS_EVENT_TARGET_ANSWER                = 3015,     // 参与勇者问答
    LIVENESS_EVENT_TARGET_ASHURA                = 3016,     // 修罗场挂机
    LIVENESS_EVENT_TARGET_WORLD_BOSS            = 3017,     // 参加世界boss
    LIVENESS_EVENT_TARGET_BUY_FROM_MALL         = 3018,     // 商城购买
    LIVENESS_EVENT_TARGET_ALCHEMY               = 3019,     // 炼金
    LIVENESS_EVENT_TARGET_SEND_BINGMONEY        = 3020,     // 花费礼金
    LIVENESS_EVENT_TARGET_SWORD_UPGRADE_STAR    = 3021,     // 圣剑升星
    LIVENESS_EVENT_TARGET_DEFEND_CARROT         = 3022,     // 保卫萝卜
    LIVENESS_EVENT_TARGET_ZOMBIE_LIVE           = 3023,     // 僵尸生存
    LIVENESS_EVENT_TARGET_BRAVE_HEART           = 3024,     // 勇敢的心
    LIVENESS_EVENT_TARGET_FASHION               = 3025,     // 抽取化身
    LIVENESS_EVENT_TARGET_JOIN_VOID_FIELD       = 3026,     // 参与虚空幻境
    LIVENESS_EVENT_TARGET_PLAYER_PK             = 3027,     // (与玩家切磋三次)改为组队一次
    LIVENESS_EVENT_TARGET_MICRO_CLIENT          = 3028,     // 微端登录
    LIVENESS_EVENT_END                          = 3100,     // 活跃度事件结束

    // 资源挽回事件
    GBACK_ACTIVITY_ENTER                        = 3300,     // 资源挽回进入活动事件 (活动类型)
    GBACK_SEND_INFO                             = 3301,     // 下个UPDATE刷新资源挽回信息

    // 罪恶值相关事件
    EVENT_EVIL_VALUE_CHANGED                    = 3401,     // 罪恶值改变
    EVENT_EVIL_CHANGE_TO_RED                    = 3402,     // 罪恶状态改变（切换到红名arg1=玩家id）
    EVENT_EVIL_CHANGE_TO_WHITE                  = 3403,     // 罪恶状态改变（切换到白名arg1=玩家id）

    EVENT_VOID_FIELD_CLEAR_INTOTIME             = 3404,      //虚空幻境的累计时间清0

    //野外boss事件
    WORD_MAP_PLYAER_OFFLINE                     = 3501,     //玩家线下线

    //VIP独享副本
    EVENT_VIP_ONLY_DUNGEON_END                  = 3601,     //VIP独享副本结束

    //任务计数事件
    EVENT_TASK_NUM_DAILY_LOGIN                  = 3701,     //每日登陆
    EVENT_TASK_NUM_TOTAL_CONSUMPTION            = 3702,     //累计消费(循环)
    EVENT_TASK_NUM_CUMULATIVE_RECHARGE          = 3703,     //累计充值(循环)
};

#endif // event_define_h__
