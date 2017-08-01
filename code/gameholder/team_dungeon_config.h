/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief 加载游戏配置
******************************************************************************/

#ifndef team_dungeon_config_h__
#define team_dungeon_config_h__

#include <map>

class Player;

typedef struct TeamDungeonTmp
{
    void Clear()
    {
        Id = 0;
        dungeonId = 0;
        level = 0;
        type = 0;
        physicalStrength = 0;
    }
    uint32 Id;
    uint32 dungeonId;
    int32 level;
    int32 type;
    int32 physicalStrength;
}TeamDungeonTmp;

class TeamDungeonConfig
{
public:
    TeamDungeonConfig();
    virtual ~TeamDungeonConfig();

    DECLARE_SINGLETON(TeamDungeonConfig)

    void Clear();                                   ///< 清空

    bool LoadConfig(const char* path);              ///< 加载配置文件

    const TeamDungeonTmp* GetTeamDungeonInfoByDungeonID(uint32 dungeonId);
private:
    std::map<int32, TeamDungeonTmp> m_team_dungeon_map;     ///< 组队副本
};

#endif
