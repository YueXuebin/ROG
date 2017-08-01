//VIP专属副本配置文件
#ifndef __VIP_ONLY_DUNGEON_CONFIG_H__
#define __VIP_ONLY_DUNGEON_CONFIG_H__

class VIPOnlyDungeonConfig
{
public:
    DECLARE_SINGLETON(VIPOnlyDungeonConfig)

    uint32 GetDungeonID() const { return m_dungeonID; }

    int32 GetSoulValue(bool isWin);

    bool HasVIPConfig(int level) const;

    const VIP_ONLY_DUNGEON_CFG* GetVIPConfig(int level) const;

    bool LoadConfig(const char* path);

    void Clear();

    VIPOnlyDungeonConfig();

    ~VIPOnlyDungeonConfig();

private:
    typedef std::map<int, VIP_ONLY_DUNGEON_CFG*> VIPOnlyDungeonCfgMap;

    uint32 m_dungeonID;
    int32 m_winSoulValue;
    int32 m_loseSoulValue;
    VIPOnlyDungeonCfgMap m_vipOnlyDungeonCfgMap;

};

#endif