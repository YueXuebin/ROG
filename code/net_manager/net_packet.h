//! @file net_packet.h


#ifndef _NET_PACKET_H_
#define _NET_PACKET_H_


#include "config.h"
#include "os.h"
#include "pool_allocator.h"
#include "List.h"
#include "inet_addr.h"


enum DATA_TYPE
{
    INT8_TYPE       = 0,
    UINT8_TYPE      = 1,
    INT16_TYPE      = 2,
    UINT16_TYPE     = 3,
    INT32_TYPE      = 4,
    UINT32_TYPE     = 5,
    INT64_TYPE      = 6,
    UINT64_TYPE     = 7,
    FLOAT_TYPE      = 8,
    DOUBLE_TYPE     = 9,
    CHAR_TYPE       = 10,
    STRING_TYPE     = 11,
};

class Net_Buff1
{
public:
    list_head item;
    //! 包缓存
    char m_packet[MAX_PACKET_LENGTH];
    DECLARE_ALLOCATOR(Net_Buff1, NET_PACKET_POOL_COUNT)
};

struct Bookmark 
{
    uint32_t absPos;
    int pos;
    Net_Buff1* netBuf;
    bool operator==(Bookmark &bk)//判断是否是同一个包的同一个位置的书签 @lsp
    {
        bool ret = false;
        if(absPos==bk.absPos&& pos==bk.pos&& netBuf== bk.netBuf)
            ret= true;
        return ret;
    }

};

//! @class Net_Packet
//! @brief 网络包封装类
class Net_Packet
{
public:
    Net_Packet();
    explicit Net_Packet(uint32_t size);
    //注意：此copy构造函数会修改packet的值
    Net_Packet(Net_Packet& packet);
    virtual ~Net_Packet();

    DECLARE_ALLOCATOR(Net_Packet, NET_PACKET_POOL_COUNT)
    //!调用者读取数据接口
public:
    void cloneData(Net_Packet* packet); //克隆源packet的数据到自己缓冲中去
public:
    inline Net_Packet& operator >>(uint8_t &data)
    {
        data = read_uint8();
        return *this;
    }
    inline Net_Packet& operator >>(uint16_t &data)
    {
        data = read_uint16();
        return *this;
    }

    inline Net_Packet& operator >>(uint32_t &data)
    {
        data = read_uint32();
        return *this;
    }

    inline Net_Packet& operator >>(uint64_t &data)
    {
        data = read_uint64();
        return *this;
    }

    inline Net_Packet& operator >>(float &data)
    {
        data = read_float();
        return *this;
    }

    inline Net_Packet& operator >>(double &data)
    {
        data = read_double();
        return *this;
    }

    //!调用者插入数据接口
public:

    inline Net_Packet& operator <<(uint8_t data)
    {
        write_uint8(data);
        return *this;
    }

    inline Net_Packet& operator <<(uint16_t data)
    {
        write_uint16(data);
        return *this;
    }

    inline Net_Packet& operator <<(uint32_t data)
    {
        write_uint32(data);
        return *this;
    }

    inline Net_Packet& operator <<(uint64_t data)
    {
        write_uint64(data);
        return *this;
    }

    inline Net_Packet& operator <<(float data)
    {
        write_float(data);
        return *this;
    }

    inline Net_Packet& operator <<(double data)
    {
        write_double(data);
        return *this;
    }

    inline Net_Packet& operator <<(const char *data)
    {
        int nlen = strlen(data);
        write_data(data, nlen);
        return *this;
    }

    inline Net_Packet& operator <<(const std::string& str)
    {
        write_data(str.c_str(), str.length());
        return *this;
    }

public:
    //!获取当前绝对位置
    Bookmark getcurpos();

    Bookmark getBkByPos(uint32_t nabspos);
    //!获取数据长度
    int getdatalen();
    inline bool good() const
    {
        return m_datagood;
    }
    //!输出内存内容
    std::string dump();
    //输出包中有效信息
    std::string showPacket();
    //重置内容
    void clear();
    //!调用者读取数据接口
    void crunch();

public:
    int read_int8(int8& val);
    int read_uint8(uint8& val);

    int read_int16(int16& val);
    int read_uint16(uint16& val);

    int read_int32(int32& val);
    int read_uint32(uint32& val);

    int read_int64(int64& val);
    int read_uint64(uint64& val);

    int read_float(float& val);
    int read_double(double& val);

    int read_string(std::string& str);

    // ============================================================

    uint8_t read_uint8();
    uint8_t read_uint8(Bookmark pos);

    uint16_t read_uint16();
    uint16_t read_uint16(Bookmark pos);

    uint32_t read_uint32();
    uint32_t read_uint32(Bookmark pos);

    uint64_t read_uint64();
    uint64_t read_uint64(Bookmark pos);

    float read_float();
    float read_float(Bookmark pos);

    double read_double();
    double read_double(Bookmark pos);

    int read_string(std::string& str, uint16_t nDataLen);           // 返回 1 成功 ，0 失败
    int read_string(std::string& str, uint16_t nDataLen, Bookmark pos);

    //!把内部数据copy一定长度的到目的缓冲区中
    int readdata(char* pBuf, uint32_t nDataLen);
    int readdata(char* pBuf, uint32_t nDataLen, Bookmark pos);
    //!调用者发送数据接口

public:
    void write_int8(int8 data);
    void write_uint8(uint8_t data);
    void write_uint8(uint8_t data, Bookmark pos);

    void write_int16(int16_t data);
    void write_uint16(uint16_t data);
    void write_uint16(uint16_t data, Bookmark pos);

    void write_int32(int32_t data);
    void write_uint32(uint32_t data);
    void write_uint32(uint32_t data, Bookmark pos);

    void write_int64(int64_t data);
    void write_uint64(uint64_t data);
    void write_uint64(uint64_t data, Bookmark pos);

    void write_float(float data);
    void write_float(float data, Bookmark pos);

    void write_double(double data);
    void write_double(double data, Bookmark pos);

    void write_string(const std::string& str);
    void write_string(const std::string& str, Bookmark pos);
    void write_string_ex(const std::string& str, uint32_t nLen);

    void write_data(const char* pbuf, uint32_t nsize);
    void write_data(const char* pbuf, uint32_t nsize, Bookmark pos);

private:
    //!初始化数据
    void initdata(uint32_t size);
    //!分配给定大小的内存块
    void allocate_buff(uint32_t size);
    //!删除掉所以内存块
    void release_buff();

    int appenddata(const char* pBuf, uint32_t nLen);

    bool islastnode() const
    {
        return m_CurBuf->item.next == &netBufhead;
    }

public:
    list_head* get_bufhead() { return const_cast<list_head*>(&netBufhead); }
    uint32_t get_datalength() { return m_DataLength; }

private:
    //!内存块链表头
    list_head netBufhead;
    //!当前块指针
    Net_Buff1* m_CurBuf;
    //!当前块相对位置
    int m_CurPos;
    //!当前绝对位置
    uint32_t m_CurAbsPos;
    //!有效数据长度
    uint32_t m_DataLength;
    //!长度不足标志
    bool m_datagood;

public:
    //!封包序列
    std::list<uint8_t>m_WriteIndex;
    //! 通道id
    uint32_t id;

    //! 对端地址
    INET_Addr remote_addr;
public:
    //!
    void movetobegin();

    //!返回当前块的指针和对应块的剩余数据长度 
    char* get_rptr(int& ndataLen);
    //!移动数据指针
    void move_rptr(int ndataLen);
public:

    enum NET_COMMAND
    {
        NET_TCP_ACCEPTOR_REGISTER,
        NET_TCP_CONNECTOR_REGISTER,
        NET_UDP_REGISTER,
        NET_DELETE
    };

    struct INTERNAL_CMD
    {
        uint32_t id;
        NET_COMMAND cmd;
        void* data;
        int mask;
        int timeout;
    } m_cmd;
};

#endif // _NET_PACKET_H_
