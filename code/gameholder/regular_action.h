/*
 *	关卡行为基础类
 */
#ifndef regular_action_h__
#define regular_action_h__

class Regular;
class RegularAction;

// 解析关卡行为
RegularAction* ParseRegularAction(TiXmlElement* ActionElement, Regular* pRegular);

class RegularAction
{
public:
    RegularAction(Regular* pRegular);
    virtual ~RegularAction();

    virtual void ParseActionXml(TiXmlElement* ActionElement) {}

    virtual void Do() = 0;

    uint32 GetTargetID() {return m_targetID;}
    void SetTargetID(uint32 playerID) {m_targetID = playerID;}

protected:
    Regular*    m_pRegular;
    uint32      m_targetID;
};


#endif // regular_action_h__
