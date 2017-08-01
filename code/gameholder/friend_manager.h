//------------------------------------------------------------------------------
//
//    Copyright (C) DoinGame. All rights reserved.
//
//    File Name:   friend_manager.h
//    Author:      Tomren
//    Create Time: 2013-06-04
//    Description: 
//
//    Version history:
//
//
//------------------------------------------------------------------------------

#ifndef FRIEND_MANAGER_H
#define FRIEND_MANAGER_H

class Player;

// ==================================================
/// 好友管理系统
// ==================================================
class FriendManager
{
public:
    FriendManager(Player* player);
    ~FriendManager();

    //LIGHT_CITY_INFO&    GetLightCityInfo() { return m_city_info; }

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    //void SetPlayer(Player* player) { m_player = player; }

    void LoadInfo(FRIEND_LIST& info);

    void FillInfo(FRIEND_LIST& info);

private:

    // 好友信息请求
    void OnFriendInfoReq();

    // 好友信息通知
    void OnFriendInfoNtf(void* data);

    // 添加好友请求
    void OnFriendAddReq(void* data);

    // 添加好友应答
    void OnFriendAddAns(void* data);

    // 删除好友请求
    void OnFriendDelReq(void* data);

    // 快速加入好友请求
    void OnFriendQuickAddReq(void* data);

    // 发送添加好友应答
    void SendFriendAddAck(uint16 errcode, std::string src_name, std::string dst_name);

    // 发送删除好友应答
    void SendFriendDelAck(uint16 errcode);

    Player*             m_player;
    FRIEND_LIST         m_friend_info;
};

#endif
