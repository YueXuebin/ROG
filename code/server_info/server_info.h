#ifndef SERVERINFO_H
#define SERVERINFO_H

enum
{
    kNumber             = 1,        // 玩家数
    kInformation        = 2,        // 玩家信息
    kLevelPlayNumber    = 3,        // 到达等级N的玩家数
    kPlayerDungeon      = 4,        // 玩家在哪个副本
    kDungeonInfo        = 5,        // 副本信息
};

class ServerInfo
{

public:
    ServerInfo() {};  
    ~ServerInfo() {};

    DECLARE_SINGLETON(ServerInfo)

    bool Init(const char* path);                                ///< 初始化
    void Uninit();                                              ///< 释放

    int32 GetUserCount(){return UserCount;}
    int32 GetPCU(){return PCU;}
    int32 GetACU(){return ACP;}

    void SetOption(int32 val){option = val;}
    int32 GetOption(){return option;}

private:
    int32 UserCount;                                            ///< 当前同时在线人数
    int32 PCU;                                                  ///< 最高同时在线人数
    int32 ACP;                                                  ///< 平均在线人数
    int32 option;                                               ///< 查看不同内容
};
































#endif

