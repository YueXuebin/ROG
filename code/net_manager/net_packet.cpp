#include "net_manager_pch.h"
#include "net_packet.h"
#include "hex_string.h"

// class Net_Packet

#define BS_MAKE64(lo, hi) ((uint64_t)((uint64_t)(lo) | (((uint64_t)(hi)) << 32)))
#define BS_LO32(x)  ((uint32_t)((x) & 0x00000000ffffffffULL))
#define BS_HI32(x)  ((uint32_t)(((x) & 0xffffffff00000000ULL) >> 32))

void Net_Packet::allocate_buff(uint32_t size)
{
    uint32_t nleftsize = 0;
    Net_Buff1* pNetBuf = NULL;
    do 
    {
        pNetBuf = new Net_Buff1;
        list_add_tail(&pNetBuf->item, &netBufhead);
        nleftsize += MAX_PACKET_LENGTH;
    } while (nleftsize < size);
}

void Net_Packet::release_buff()
{
    list_head* pos, *next;
    list_for_each_safe(pos, next, &netBufhead)
    {
        list_del(pos);
        Net_Buff1* pnetBuf = list_entry(pos, Net_Buff1, item);
        delete pnetBuf;
    }
    m_DataLength = 0;
    m_CurBuf = NULL;
    m_CurPos = 0;
    m_CurAbsPos = 0;
}


Net_Packet::Net_Packet()
: m_CurBuf(NULL), m_CurPos(0), m_CurAbsPos(0), m_DataLength(0), m_datagood(true)
{
    initdata(MAX_PACKET_LENGTH);
}

Net_Packet::Net_Packet(uint32_t size)
: m_CurBuf(NULL), m_CurPos(0), m_CurAbsPos(0), m_DataLength(0), m_datagood(true)
{
    initdata(size);
}

Net_Packet::Net_Packet(Net_Packet& packet)
: m_CurBuf(NULL), m_CurPos(0), m_CurAbsPos(0), m_DataLength(0), m_datagood(true),m_WriteIndex(packet.m_WriteIndex)
{
    //copy构造函数目的是把数据块的链表直接链接到新net packet中去
    INIT_LIST_HEAD(&netBufhead);
    if(!list_empty(&packet.netBufhead))
    {
        list_add_tail(&netBufhead, &packet.netBufhead);
        list_del_init(&packet.netBufhead);
        //当前buf和位置都指向第一块
        list_head* pnext = netBufhead.next;
        m_CurBuf = list_entry(pnext, Net_Buff1, item);
        m_CurPos = 0;
        m_CurAbsPos = 0;
        m_DataLength = packet.m_DataLength;
    }
}

Net_Packet::~Net_Packet()
{
    release_buff();	
}

void Net_Packet::initdata(uint32_t size)
{
    INIT_LIST_HEAD(&netBufhead);
    allocate_buff(size);

    //当前buf和位置都指向第一块
    list_head* pnext = netBufhead.next;
    m_CurBuf = list_entry(pnext, Net_Buff1, item);
    m_CurPos = 0;
    m_CurAbsPos = 0;
}


Bookmark Net_Packet::getcurpos()
{
    Bookmark bk;
    bk.absPos = m_CurAbsPos;
    bk.netBuf = m_CurBuf;
    bk.pos = m_CurPos;
    return bk;	
}

Bookmark Net_Packet::getBkByPos(uint32_t nabspos)
{
    Bookmark bk;
    bk.absPos = 0;
    bk.pos = 0;
    bk.netBuf = NULL;
    if (nabspos > m_DataLength)
    {
        return bk;
    }

    list_head* pos;
    list_head* head = const_cast<list_head*>(&netBufhead);
    Net_Buff1* pNetBuf = NULL;

    list_for_each(pos, head)
    {
        pNetBuf = list_entry(pos, Net_Buff1, item);
        if (nabspos < MAX_PACKET_LENGTH)
        {
            bk.pos = nabspos;
        }
        else
        {
            bk.pos = 0;
        }
        bk.absPos += nabspos > MAX_PACKET_LENGTH ? MAX_PACKET_LENGTH : nabspos;
        nabspos -= nabspos > MAX_PACKET_LENGTH ? MAX_PACKET_LENGTH : nabspos;
        if (nabspos == 0)
        {
            bk.netBuf = pNetBuf;
            break;
        }
    }
    return bk;
}
int Net_Packet::getdatalen()
{
    return m_DataLength;
}

void Net_Packet::movetobegin()
{
    if (!list_empty(&netBufhead))
    {
        list_head* pnext = netBufhead.next;
        //当前buf和位置都指向第一块
        m_CurBuf = list_entry(pnext, Net_Buff1, item);
        m_CurPos = 0;
        m_CurAbsPos = 0;
    }
}

int Net_Packet::read_int8(int8& val)
{
    return readdata(reinterpret_cast<char*>(&val), sizeof(val));
}

int Net_Packet::read_uint8(uint8& val)
{
    return readdata(reinterpret_cast<char*>(&val), sizeof(val));
}

int Net_Packet::read_int16(int16& val)
{
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    val = ntohs(val);
    return ret;
}

int Net_Packet::read_uint16(uint16& val)
{
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    val = ntohs(val);
    return ret;
}

int Net_Packet::read_int32(int32& val)
{
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    val = ntohl(val);
    return ret;
}

int Net_Packet::read_uint32(uint32& val)
{
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    val = ntohl(val);
    return ret;
}

int Net_Packet::read_int64(int64& val)
{
    uint32_t hi, lo;
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    hi = ntohl(BS_HI32(val));
    lo = ntohl(BS_LO32(val));
    val = BS_MAKE64(lo, hi);
    return ret;
}

int Net_Packet::read_uint64(uint64& val)
{
    uint32_t hi, lo;
    int ret = 0;
    ret = readdata(reinterpret_cast<char*>(&val), sizeof(val));
    hi = ntohl(BS_HI32(val));
    lo = ntohl(BS_LO32(val));
    val = BS_MAKE64(lo, hi);
    return ret;
}

int Net_Packet::read_float(float& val)
{
    return readdata(reinterpret_cast<char*>(&val), sizeof(val));
}

int Net_Packet::read_double(double& val)
{
    return readdata(reinterpret_cast<char*>(&val), sizeof(val));
}

int Net_Packet::read_string(std::string& str)
{
    uint16 len = 0;
    int ret = 0;
    ret = read_uint16(len);
    if(ret == 0 || len > RECV_BUFFER_LENGTH)
        return 0;
    if(0 == read_string(str, len))
        return 0;
    return ret;
}

uint8_t Net_Packet::read_uint8()
{
    uint8_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret));
    return nret;
}

uint8_t Net_Packet::read_uint8(Bookmark pos)
{
    uint8_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret), pos);
    return nret;
}

uint16_t Net_Packet::read_uint16()
{
    uint16_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret));
    return ntohs(nret);
}

uint16_t Net_Packet::read_uint16(Bookmark pos)
{
    uint16_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret), pos);
    return ntohs(nret);
}

uint32_t Net_Packet::read_uint32()
{
    uint32_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret));
    return ntohl(nret);
}

uint32_t Net_Packet::read_uint32(Bookmark pos)
{
    uint32_t nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret), pos);
    return ntohl(nret);
}

uint64_t Net_Packet::read_uint64()
{
    uint32_t hi, lo;
    uint64_t data;
    readdata(reinterpret_cast<char*>(&data), sizeof(data));	
    hi = ntohl(BS_HI32(data));
    lo = ntohl(BS_LO32(data));
    return BS_MAKE64(lo, hi);
}

uint64_t Net_Packet::read_uint64(Bookmark pos)
{
    uint32_t hi, lo;
    uint64_t data;

    readdata(reinterpret_cast<char*>(&data), sizeof(data), pos);

    hi = ntohl(BS_HI32(data));
    lo = ntohl(BS_LO32(data));
    return BS_MAKE64(lo, hi);
}

float Net_Packet::read_float()
{
    float nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret));
    return nret;
}

float Net_Packet::read_float(Bookmark pos)
{
    float nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret), pos);
    return nret;
}

double Net_Packet::read_double()
{
    double nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret));
    return nret;
}

double Net_Packet::read_double(Bookmark pos)
{
    double nret;
    readdata(reinterpret_cast<char*>(&nret), sizeof(nret), pos);
    return nret;
}

int Net_Packet::read_string(std::string& str, uint16_t nDataLen)
{
    m_datagood = true;

    //要拷贝的数据
    int ntocopysize = nDataLen;
    //当前块剩下的数据
    int nleftdatasize;
    //每次能够copy的数据
    int ncopylen = 0;
    //发生copy的数据总长度
    while(ntocopysize > 0)
    {
        nleftdatasize = islastnode() ? m_DataLength - m_CurAbsPos : MAX_PACKET_LENGTH - m_CurPos;
        if (nleftdatasize == 0)
        {
            m_datagood = false;
            CnAssert(false);      // 要读的数据超了
            return 0;
        }
        str.append(m_CurBuf->m_packet + m_CurPos, nleftdatasize > ntocopysize ? ntocopysize : nleftdatasize);
        ncopylen = nleftdatasize > ntocopysize ? ntocopysize : nleftdatasize;
        ntocopysize -= ncopylen;
        m_CurPos += ncopylen;
        m_CurAbsPos += ncopylen;

        if (ntocopysize > 0 || m_CurPos == MAX_PACKET_LENGTH)
        {
            if(islastnode())
            {
                if(ntocopysize == 0)
                {
                    // 必须正好读完这段buf
                    if(m_CurPos != MAX_PACKET_LENGTH)
                    {
                        CnAssert(false);
                        return 0;
                    }
                }
                else
                {
                    m_datagood = false; 
                    CnAssert(false);  // 要读取的数据超了
                    return 0;
                }
            }
            else
            {   // 取下一段
                list_head* pnext = m_CurBuf->item.next;
                m_CurBuf = list_entry(pnext, Net_Buff1, item);
                m_CurPos = 0;
            }
        }
    }
    return 1;
}

int Net_Packet::read_string(std::string& str, uint16_t nDataLen, Bookmark pos)
{
    int noldpos = m_CurPos;
    Net_Buff1* oldbuf = m_CurBuf;
    int noldabspos = m_CurAbsPos;

    m_CurBuf = pos.netBuf;
    m_CurPos = pos.pos;
    int nret = read_string(str, nDataLen);

    m_CurPos = noldpos;
    m_CurBuf = oldbuf;
    m_CurAbsPos = noldabspos;
    return nret;
}

int Net_Packet::readdata(char* pBuf, uint32_t nDataLen)
{
    m_datagood = true;

    //要拷贝的数据
    uint32_t ntocopysize = nDataLen;
    //当前块剩下的数据
    uint32_t nleftdatasize;
    //每次能够copy的数据
    uint32_t ncopylen = 0;
    //发生copy的数据总长度
    uint32_t nret = 0;
    while(ntocopysize > 0)
    {
        nleftdatasize = islastnode() ? m_DataLength - m_CurAbsPos : MAX_PACKET_LENGTH - m_CurPos;
        CnAssert(nleftdatasize <= MAX_PACKET_LENGTH); // 一段buff中读的数据不可能>MAX_PACKET_LENGTH
        if (nleftdatasize == 0)
        {
            m_datagood = false;
            CnAssert(false);      // 要读的数据超了
            nret = 0;
            break;
        }
        ncopylen = nleftdatasize > ntocopysize ? ntocopysize : nleftdatasize;
        memcpy(pBuf + nret, m_CurBuf->m_packet + m_CurPos, ncopylen);
        ntocopysize -= ncopylen;
        m_CurPos += ncopylen;
        m_CurAbsPos += ncopylen;
        nret += ncopylen;
        if (ntocopysize > 0 || m_CurPos == MAX_PACKET_LENGTH)
        {
            if(islastnode())
            {
                if(ntocopysize == 0)
                {
                    // 必须正好读完这段buf
                    if(m_CurPos != MAX_PACKET_LENGTH)
                    {
                        CnAssert(false);
                        return 0;
                    }
                }
                else
                {
                    m_datagood = false; 
                    CnAssert(false);  // 要读取的数据超了
                    return 0;
                }
            }
            else
            {   // 取下一段buf
                list_head* pnext = m_CurBuf->item.next;
                m_CurBuf = list_entry(pnext, Net_Buff1, item);
                m_CurPos = 0;
            }
        }
    }
    return nret;
}

int Net_Packet::readdata(char* pBuf, uint32_t nDataLen, Bookmark pos)
{
    uint32_t noldpos = m_CurPos;
    Net_Buff1* oldbuf = m_CurBuf;
    int noldabspos = m_CurAbsPos;

    m_CurBuf = pos.netBuf;
    m_CurPos = pos.pos;
    m_CurAbsPos = pos.absPos;

    int nret = readdata(pBuf, nDataLen);

    m_CurPos = noldpos;
    m_CurBuf = oldbuf;
    m_CurAbsPos = noldabspos;
    return nret;
}

void Net_Packet::write_int8(int8 data)
{
    m_WriteIndex.push_back((uint8)INT8_TYPE);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint8(uint8_t data)
{
    m_WriteIndex.push_back((uint8_t)UINT8_TYPE);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint8(uint8_t data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)UINT8_TYPE);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);
}

void Net_Packet::write_int16(int16_t data)
{
    m_WriteIndex.push_back((uint8_t)INT16_TYPE);
    data = htons(data);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint16(uint16_t data)
{
    m_WriteIndex.push_back((uint8_t)UINT16_TYPE);
    data = htons(data);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint16(uint16_t data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)UINT16_TYPE);
    data = htons(data);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);	
}

void Net_Packet::write_int32(int32_t data)
{
    m_WriteIndex.push_back((uint8_t)INT32_TYPE);
    data = htonl(data);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint32(uint32_t data)
{
    m_WriteIndex.push_back((uint8_t)UINT32_TYPE);
    data = htonl(data);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint32(uint32_t data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)UINT32_TYPE);
    data = htonl(data);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);
}

void Net_Packet::write_int64(int64_t data)
{
    m_WriteIndex.push_back((uint8_t)INT64_TYPE);
    //(*this)<< BS_HI32(data) << BS_LO32(data);
    uint32_t hi = htonl(BS_HI32(data));
    uint32_t lo = htonl(BS_LO32(data));
    data = BS_MAKE64(lo, hi);
    write_data(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint64(uint64_t data)
{
    m_WriteIndex.push_back((uint8_t)UINT64_TYPE);
    //(*this)<< BS_HI32(data) << BS_LO32(data);
    uint32_t hi = htonl(BS_HI32(data));
    uint32_t lo = htonl(BS_LO32(data));
    data = BS_MAKE64(lo, hi);
    write_data(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_uint64(uint64_t data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)UINT64_TYPE);
    uint32_t hi = htonl(BS_HI32(data));
    uint32_t lo = htonl(BS_LO32(data));
    data = BS_MAKE64(lo, hi);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);
}

void Net_Packet::write_float(float data)
{
    m_WriteIndex.push_back((uint8_t)FLOAT_TYPE);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_float(float data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)FLOAT_TYPE);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);
}

void Net_Packet::write_double(double data)
{
    m_WriteIndex.push_back((uint8_t)DOUBLE_TYPE);
    appenddata(reinterpret_cast<char*>(&data), sizeof(data));
}

void Net_Packet::write_double(double data, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)DOUBLE_TYPE);
    write_data(reinterpret_cast<char*>(&data), sizeof(data), pos);	
}

void Net_Packet::write_data(const char* pbuf, uint32_t nsize)
{
    m_WriteIndex.push_back((uint8_t)STRING_TYPE);
    appenddata(pbuf, nsize);
}

void Net_Packet::write_string(const std::string& str)
{
    m_WriteIndex.push_back((uint8_t)STRING_TYPE);
    write_uint16(str.length());
    appenddata(str.c_str(), str.length());
}

void Net_Packet::write_string(const std::string& str, Bookmark pos)
{
    if(getcurpos()== pos)//如果顺序写
        m_WriteIndex.push_back((uint8_t)STRING_TYPE);
    write_data(str.c_str(), str.length(), pos);
}

void Net_Packet::write_string_ex(const std::string& str, uint32_t nLen)
{
    m_WriteIndex.push_back((uint8_t)STRING_TYPE);
    write_uint16(nLen);
    appenddata(str.c_str(), str.length());
}

void Net_Packet::write_data(const char* pbuf, uint32_t nsize, Bookmark pos)
{
    //应该禁止此函数扩充块数
    if (pos.absPos + nsize > m_DataLength)
    {
        return ;
    }

    uint32_t noldpos = m_CurPos;
    Net_Buff1* oldbuf = m_CurBuf;
    uint32_t noldabspos = m_CurAbsPos;

    m_CurBuf = pos.netBuf;
    m_CurPos = pos.pos;

    appenddata(pbuf, nsize);

    m_DataLength -= nsize;

    m_CurPos = noldpos;
    m_CurBuf = oldbuf;
    m_CurAbsPos = noldabspos;
}

int Net_Packet::appenddata(const char* pBuf, uint32_t nLen)
{
    m_datagood = true;
    //计算当前块剩下的长度
    uint32_t ncurfreesize = MAX_PACKET_LENGTH - m_CurPos;
    if (nLen > ncurfreesize)
    {
        uint32_t nleftsize = nLen;
        //先填满当前块
        if (ncurfreesize > 0)
        {
            memcpy(m_CurBuf->m_packet + m_CurPos, pBuf, ncurfreesize);
            m_CurPos += ncurfreesize;
            m_CurAbsPos += ncurfreesize;
            nleftsize -= ncurfreesize;
            m_DataLength += ncurfreesize;
            assert(m_CurPos == MAX_PACKET_LENGTH);
        }
        //生成新块进行填充
        int ncopylen = 0;
        while(nleftsize > 0)
        {
            if (islastnode())
            {
                allocate_buff(MAX_PACKET_LENGTH);
            }
            m_CurPos = 0;
            list_head* pnext = m_CurBuf->item.next;
            m_CurBuf = list_entry(pnext, Net_Buff1, item);
            ncopylen = nleftsize > MAX_PACKET_LENGTH ? MAX_PACKET_LENGTH : nleftsize;
            memcpy(m_CurBuf->m_packet + m_CurPos, pBuf + nLen - nleftsize, ncopylen);
            m_DataLength += ncopylen;
            nleftsize = nleftsize - ncopylen;
            m_CurPos += ncopylen;
            m_CurAbsPos += ncopylen;
        }
    }
    else
    {
        memcpy(m_CurBuf->m_packet + m_CurPos, pBuf, nLen);
        m_CurPos += nLen;
        m_CurAbsPos += nLen;
        m_DataLength += nLen;
    }
    return 0;
}

char* Net_Packet::get_rptr(int& ndataLen)
{
    ndataLen = islastnode() ? m_DataLength - m_CurAbsPos : MAX_PACKET_LENGTH - m_CurPos;
    assert(ndataLen <= MAX_PACKET_LENGTH);
    return ndataLen == 0 ? NULL : m_CurBuf->m_packet + m_CurPos;
}

void Net_Packet::move_rptr(int ndataLen)
{
    assert(ndataLen + m_CurPos <= MAX_PACKET_LENGTH);       // 外面对move的使用逻辑，不会有跳buf段的情况
    if (ndataLen + m_CurPos < MAX_PACKET_LENGTH)
    {
        m_CurPos += ndataLen;
        m_CurAbsPos += ndataLen;
    }
    else
    {   // 这段代码不应该会被使用到
        m_CurAbsPos += ndataLen;
        if (!islastnode())
        {
            list_head* pnext = m_CurBuf->item.next;
            m_CurBuf = list_entry(pnext, Net_Buff1, item);
            m_CurPos = 0;
        }

    }
}

std::string Net_Packet::dump()
{
    std::string strRet("");
    char* pOrgBuf = CnNew char[m_DataLength];


    uint32_t noldpos = m_CurPos;
    Net_Buff1* oldbuf = m_CurBuf;
    int noldabspos = m_CurAbsPos;

    movetobegin();
    readdata(pOrgBuf, m_DataLength);

    m_CurAbsPos = noldabspos;
    m_CurBuf = oldbuf;
    m_CurPos = noldpos;

    int nDestBufLen = m_DataLength * 2;
    char* pDestBuf = CnNew char[nDestBufLen];
    utils::HexString::bin2asc(pOrgBuf, m_DataLength, pDestBuf, nDestBufLen, nDestBufLen);
    strRet.append(pDestBuf, nDestBufLen);
    CnDelete [] pOrgBuf;
    CnDelete [] pDestBuf;
    return strRet;
}

std::string Net_Packet::showPacket()//LIANGSP
{
    std::ostringstream information;
    uint32_t noldpos = m_CurPos;
    Net_Buff1* oldbuf = m_CurBuf;
    int noldabspos = m_CurAbsPos;

    movetobegin();
    std::list<uint8_t>::iterator listiter;
    std::list<uint8_t>::iterator midlistiter;
    for(listiter= m_WriteIndex.begin();listiter!= m_WriteIndex.end();)
    {
        DATA_TYPE type= (DATA_TYPE)(*listiter);
        switch(type)
        {
        case UINT8_TYPE:
            {
                //首先读出 uint8
                uint8_t number=0;
                number=this->read_uint8();
                midlistiter = ++listiter; //后面那个迭代器数据
                if(midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== STRING_TYPE||midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== CHAR_TYPE)
                {
                    //后面那个数据是个字符串  要求一起和长度读出
                    if((DATA_TYPE)(*(midlistiter))== STRING_TYPE)
                    {
                        //读出string
                        std::string stringText;
                        read_string(stringText,(uint16_t)number);
                        information<<"string:"<<stringText<<" ";


                    }
                    else
                    {
                        //读出char
                        char* charText= CnNew char[number];
                        readdata(charText,(uint16_t)number);
                        information<<"char:"<<charText<<" ";

                    }
                    listiter = midlistiter;
                    ++listiter;
                }
                else
                {
                    //只读出一个uint8
                    information<<"uint8:"<<(uint32_t)number<<" ";
                    //++listiter;					
                }
                break;
            }
        case UINT16_TYPE:
            {
                uint16_t number=0;
                number=this->read_uint16();
                midlistiter = ++listiter; //后面那个迭代器数据
                if(midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== STRING_TYPE||midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== CHAR_TYPE)
                {
                    //后面那个数据是个字符串  要求一起和长度读出
                    if((DATA_TYPE)(*(midlistiter))== STRING_TYPE)
                    {
                        //读出string
                        std::string stringText;
                        read_string(stringText,(uint16_t)number);
                        information<<"string:"<<stringText<<" ";


                    }
                    else
                    {
                        //读出char
                        char* charText= CnNew char[number];
                        readdata(charText,(uint16_t)number);
                        information<<"char:"<<charText<<" ";

                    }
                    listiter = midlistiter;
                    ++listiter;
                }
                else
                {
                    //只读出一个uint16
                    information<<"uint16:"<<number<<" ";
                    //++listiter;					
                }
                break;
            }
        case UINT32_TYPE:
            {
                uint32_t number=0;
                number=this->read_uint32();
                midlistiter = ++listiter; //后面那个迭代器数据
                if(midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== STRING_TYPE||midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== CHAR_TYPE)
                {
                    //后面那个数据是个字符串  要求一起和长度读出
                    if((DATA_TYPE)(*(midlistiter))== STRING_TYPE)
                    {
                        //读出string
                        std::string stringText;
                        read_string(stringText,(uint16_t)number);
                        information<<"string:"<<stringText<<" ";


                    }
                    else
                    {
                        //读出char
                        char* charText= CnNew char[number];
                        readdata(charText,(uint16_t)number);
                        information<<"char:"<<charText<<" ";

                    }
                    listiter = midlistiter;
                    ++listiter;
                }
                else
                {
                    //只读出一个uint8
                    information<<"uint32:"<<number<<" ";
                    //++listiter;					
                }

                break;
            }
        case UINT64_TYPE:
            {
                uint64_t number=0;
                number=this->read_uint64();
                midlistiter = ++listiter; //后面那个迭代器数据
                if(midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== STRING_TYPE||midlistiter!=m_WriteIndex.end() &&(DATA_TYPE)(*(midlistiter))== CHAR_TYPE)
                {
                    //后面那个数据是个字符串  要求一起和长度读出
                    if((DATA_TYPE)(*(midlistiter))== STRING_TYPE)
                    {
                        //读出string
                        std::string stringText;
                        read_string(stringText,(uint16_t)number);
                        information<<"string:"<<stringText<<" ";


                    }
                    else
                    {
                        //读出char
                        char charText[25] = {0};
                        readdata(charText,(uint16_t)number);
                        information<<"char:"<<charText<<" ";

                    }
                    listiter = midlistiter;
                    ++listiter;
                }
                else
                {
                    //只读出一个uint8
                    information<<"uint64:"<<number<<" ";
                    //++listiter;					
                }
                break;
            }
        case STRING_TYPE:
            {
                //error
                ++listiter;	
                break;
            }
        case DOUBLE_TYPE:
            {
                double number=0;
                number=this->read_double();
                information<<"double:"<<number<<" ";
                ++listiter;		
                break;
            }
        case FLOAT_TYPE:
            {
                float number=0;
                number=this->read_float();
                information<<"float:"<<number<<" ";
                ++listiter;		
                break;
            }
        case CHAR_TYPE:
            {
                //error
                ++listiter;
                break;
            }
        default:
            {
                //LOG(ERROR)("unknown cmd %d", cmd);
                break;
            }
        }
    }


    m_CurAbsPos = noldabspos;
    m_CurBuf = oldbuf;
    m_CurPos = noldpos;
    return information.str();

}
void Net_Packet::clear()
{
    movetobegin();
    m_DataLength = 0;
    m_datagood = true;
}

void Net_Packet::crunch()
{
    uint32 len = get_datalength();
    list_head* pos, *next;
    list_for_each_safe(pos, next, &netBufhead)
    {
        if (len > 0)
        {
            len -= (Crown::SDMin<uint32>(MAX_PACKET_LENGTH, len));
        }
        else
        {
            list_del(pos);
            Net_Buff1* pnetBuf = list_entry(pos, Net_Buff1, item);
            delete pnetBuf;
        }
    }
}

void Net_Packet ::cloneData(Net_Packet* packet)
{
    if(packet->m_DataLength == 0)
        return;

    list_head* pos;
    list_head* head = const_cast<list_head*>(&packet->netBufhead);
    Net_Buff1* pNetBuf = NULL;

    uint32_t nDataLen = packet->m_DataLength;
    uint32_t nabspos = 0;
    list_for_each(pos, head)
    {
        pNetBuf = list_entry(pos, Net_Buff1, item);
        if (MAX_PACKET_LENGTH <= nDataLen)
        {
            write_data(pNetBuf->m_packet, MAX_PACKET_LENGTH);
            nDataLen -= MAX_PACKET_LENGTH;
        }
        else
        {
            write_data(pNetBuf->m_packet, nDataLen);
            nDataLen = 0;
        }
        if(nDataLen == 0)
            break;
    }
    m_WriteIndex = packet->m_WriteIndex;//LSP
}



IMPLEMENT_ALLOCATOR(Net_Packet)
IMPLEMENT_ALLOCATOR(Net_Buff1)
//IMPLEMENT_ALLOCATOR(Net_Buff2)



