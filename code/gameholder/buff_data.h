/*
 *	buff数据
 */
#ifndef buff_data_h__
#define buff_data_h__


class BuffData
{
public:
    bool    m_addBuff;      // 加或去Buff
    bool    m_canSelf;      // 可以对自己
    bool    m_canFriend;    // 可以对盟友
    bool    m_canEnemy;     // 可以对敌人

    BUFF_INFO   m_BuffInfo;

    BuffData();
};

#endif // buff_data_h__
