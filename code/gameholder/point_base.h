#ifndef point_base_h__
#define point_base_h__

#include "regular_builder.h"

enum{
    POINT_TYPE_BARREL = 1,      // 木桶
    POINT_TYPE_MONSTER,         // 出怪点
    POINT_TYPE_TOUCH,           // 触摸点(要读条)
    POINT_TYPE_TREASURE,        // 宝箱
    POINT_TYPE_TRIGGER          // 触摸点(非宝箱触摸)
};

/*
 *	事件点基础类
 *  关卡中会设置各种事件点（小怪点，宝箱点，npc点，boss点）
 *  通过事件点类,可以方便的完成关卡设计
 */
class PointBase
{
    CnDeclareRootRTTI(PointBase)
public:
    PointBase(RegularBuilder* pRogRegular, uint32 CheckPointID, uint32 type);
    virtual ~PointBase();

    uint32 GetType() {return m_type;}

    virtual bool Build() = 0;

    virtual void Update(uint32 curr_tick) = 0;

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    void SetPostionByCheckPoint(uint32 CheckPointID);            // 设置事件点位置

    virtual void OnEventTrigger() {}                            // 满足事件触发规则时，被触发的函数

    void LinkTrigger(PointBase* pPointBase);                    // 链式的事件触发
    uint32 GetEventId() {return m_EventId;}
    uint32 GetEventPointType() {return m_EventPointType;}
private:
    void SetEventTrigger(PointBase* triggerPoint, uint32 eventId, uint32 arg1, uint32 arg2);       // 设置事件触发规则

protected:
    RegularBuilder* m_pRogRegular;          // rog关卡对象

    uint32  m_type;                     // 事件点类型

    CoVec2  m_position;                 // 事件点位置
    int32   m_angle;                    // 事件点方向
    int32   m_CheckPointID;             // 指定所在CheckPointID

    uint32  m_EventPointType;           // 触发的事件点类型
    uint32  m_EventId;                  // 在关卡内设置的触发事件
    uint32  m_arg1;                     // 
    uint32  m_arg2;                     //

};


#endif // point_base_h__
