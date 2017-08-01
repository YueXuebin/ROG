#ifndef touch_controller_h__
#define touch_controller_h__

// 采集控制器

class BattleUnit;

class TouchController
{
public:
    TouchController(BattleUnit* owner);
    ~TouchController();

    void TouchStart(uint32 patchID);    // 开始
    void TouchEnd();                    // 结束

    void OnTouchBreak();                // 打断
    void OnStateChange(uint32 oldState, uint32 newState);
private:
    BattleUnit* m_Owner;

    uint32  m_touchID;          // 采集点ID
};

#endif // patch_controller_h__
