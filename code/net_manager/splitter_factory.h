/*
 *	拆包器工厂，因为拆包器会在线程间传递，因此用工厂模式最安全
 */
#ifndef splitter_factory_h__
#define splitter_factory_h__

#include "afpacket_splitter.h"
#include "binary_splitter.h"

// 抽象工厂接口
class SplitterFactory
{
public:
    virtual Packet_Splitter* CreateSplitter() = 0;
    void DestorySplitter(Packet_Splitter* splitter)
    {
        CnDelete splitter;
    }
};
// 游戏协议拆包器
class AfPacketSplitterFactory : public SplitterFactory
{
public:
    virtual Packet_Splitter* CreateSplitter() {return CnNew AfPacket_Splitter;}
};
// 二进制拆包器
class BinarySplitterFactory : public SplitterFactory
{
public:
    virtual Packet_Splitter* CreateSplitter() {return CnNew Binary_Splitter;}
};

#endif // splitter_factory_h__
