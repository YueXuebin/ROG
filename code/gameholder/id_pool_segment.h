/*
 *	一个32位ID，拆分为4个8位的段。每个段记录该位段的注册情况
 *
 *  一个32位ID的注册方式为
 *
 *       根          31-24位段     23-16位段       15-8位段      7-0位段
 *  IdPoolSegment->IdPoolSegment->IdPoolSegment->IdPoolSegment->IdPoolOctet
 *                     256个         256个           256个        256/32个
 *
 *  以一个树状链接来记录32位ID的使用情况
 */
#ifndef id_pool_segment_h__
#define id_pool_segment_h__

#define     MOD_31_24(x)    (x>>24&0xff)
#define     MOD_23_16(x)    (x>>16&0xff)
#define     MOD_15_8(x)     (x>>8&0xff)
#define     MOD_7_0(x)      (x&0xff)

enum{
    SEGMENT_32 = 1,
    SEGMENT_24,
    SEGMENT_16,
    SEGMENT_8,
};


class IdPoolOctet;
class IdPoolSegment
{
public:
    IdPoolSegment(uint32 type);
    ~IdPoolSegment();

    bool    RegisterID(uint32 id);
    bool    UnregisterID(uint32 id);

    void    CleanZero();

private:
    bool    IsAllNull();

private:
    IdPoolSegment*  m_child[256];
    IdPoolOctet*    m_Octets;
    uint32          m_type;

};


class IdPoolOctet
{
public:
    IdPoolOctet();
    ~IdPoolOctet();

    bool    RegisterID(uint32 id);
    bool    UnregisterID(uint32 id);

    bool    IsZero();

private:
    uint32    m_bits[8];
};

IdPoolSegment*  CreateIdPoolSegment(uint32 type);
void DestoryIdPoolSegment(IdPoolSegment* pPoolSegment);
IdPoolOctet*  CreateIdPoolOctet();
void DestoryIdPoolOctet(IdPoolOctet* pPoolOctet);


#endif // id_pool_segment_h__
