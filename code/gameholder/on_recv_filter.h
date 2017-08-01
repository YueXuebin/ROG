/*
 *	协议过滤器
 */
#ifndef on_recv_filter_h__
#define on_recv_filter_h__

class Player;

class MessageTimeRecorder
{
public:
    MessageTimeRecorder();
    ~MessageTimeRecorder();

    void Clear();

    uint32      ticks;
    uint32      count;
};

class OnRecvFilter
{
public:
    OnRecvFilter(Player* player);
    ~OnRecvFilter();

    void Update();

    bool OnRecv(uint32 msgID, void* data);

private:
    Player*     m_player;

    typedef std::map<uint32, MessageTimeRecorder>   MessageTimeMap;
    MessageTimeMap      m_MessageTimeMap;               // 协议接受时间表
};


#endif // on_recv_filter_h__
