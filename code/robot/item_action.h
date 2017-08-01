#ifndef item_action_h__
#define item_action_h__

//  µÀ¾ß²Ù×÷

#include "action_base.h"

class Item_Action : public ActionBase
{
public:
    Item_Action(Robot* pRobot, uint8 opt, uint8 src_pos, uint8 dst_pos);
    virtual ~Item_Action();

    virtual void OnRecv(uint32 msgID, void* data);

    virtual void OnStart();

private:
    void RecvOptBagItemNtf(void* data);

protected:
    int8 m_opt;
    int8 m_srcPos;
    int8 m_dstPos;
};

#endif // item_action_h__
