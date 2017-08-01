/*
 *	buff管理器
 */
#ifndef buff_manager_h__
#define buff_manager_h__

class Buff;
class BuffData;


class BuffManager
{
public:
    BuffManager();
    ~BuffManager();

    DECLARE_SINGLETON(BuffManager)

    bool Init();

    const BuffData* GetBuffData(uint32 buffId);

    Buff*           CreateBuff(const BuffData* pBuffData, int32 buff_val1, int32 buff_val2, uint32 caster_id);

private:
    bool ParseBuffList();

private:
    typedef std::map<uint32, TiXmlDocument*>    BuffXMLMap;
    BuffXMLMap m_buffXmlMap;      // <buff id, buff描述xml>

    typedef std::map<uint32, BuffData*>     BuffDataMap;
    BuffDataMap m_buffDataMap;      // <buff id, buff data>
};


#endif // buff_manager_h__
