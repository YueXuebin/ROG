/*
 *	副本相关的定义
 */
#ifndef dungeon_type_h__
#define dungeon_type_h__

#include "message_encoder.h"

// 场景id分段(0保留)
// |--------频道id 11bit ---------|------- 副本id 13bit -----|------- 场景id 8bit -------|
#define CHANNEL_ID_MAX 0x7ff        // 最大频道id 2047
#define DUNGEON_ID_MAX 0x1fff       // 最大副本id 8191
#define SCENE_ID_MAX 0xff           // 最大场景id 255

#define UNION_ID(channel_id, dungeon_id, scene_id) ( ((channel_id & CHANNEL_ID_MAX)<<21) | ((dungeon_id & DUNGEON_ID_MAX)<<8) | (scene_id & SCENE_ID_MAX) )
#define CHANNEL_ID(union_id) ( union_id >> 21 )
#define DUNGEON_ID(union_id) ( (union_id >> 8) & DUNGEON_ID_MAX )
#define SCENE_ID(union_id) ( union_id & SCENE_ID_MAX )

// 副本类型
#define DUNGEON_TYPE_NONE           0       // 错误的副本类型
#define DUNGEON_TYPE_CITY           1       // 主城(永久副本，死亡可以回到主城)
#define DUNGEON_TYPE_DUNGEON        2       // 临时副本
#define DUNGEON_TYPE_WORLD          3       // 世界地图(永久副本)
#define DUNGEON_TYPE_RECONNECTION   4       // 可重连临时副本

// 战斗中的组
#define FRIEND_GROUP    1       // 玩家
#define ENEMY_GROUP     2       // 怪
#define OTHER_GROUP     3       // 其他
#define ALL_GROUP       -1      // 所有组

#endif // dungeon_type_h__
