#ifndef prop_notifier_h__
#define prop_notifier_h__

class BattleUnit;

/*
 *	属性通知器(只负责广播属性)
 */
class PropNotifier
{
    struct PropData
    {
        uint32  m_PropertyId;
        int32   m_Value;                // 当前的值
        bool    m_Dirty;                // 是否需要更新
        std::string m_StrValue;             // 通知其他玩家看到我的名字
    };

public:
    PropNotifier(BattleUnit* owner);
    ~PropNotifier();

    void Update();

    void NotifyProperty(uint32 property_id, int32 value, const std::string& strValue ="");

private:
    BattleUnit* m_Owner;

    typedef std::map<uint32, PropData> PropMap;    // <property id, value data>
    PropMap     m_PropMap;              // 属性值表

    bool        m_StartNotify;
    bool        m_UpdateDirty;          //
};

#endif // prop_notifier_h__
