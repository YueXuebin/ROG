#ifndef profile_canvas_h__
#define profile_canvas_h__

#include "object.h"

class ProfileTable;
class Button;
class TextObj;

class ProfileCanvas : public Object
{
public:
    ProfileCanvas();
    ~ProfileCanvas();

    void SetFPSText(const std::string& txt);

protected:
    void Init();
    void Uninit();

    virtual void OnLostDevice() {}
    virtual void OnResetDevice() {}

    virtual void Update();

    virtual void RecvMessage(const std::string& message, Object* pObject);

private:
    std::vector<ProfileTable*>  m_tables;

    PerformanceNode*    m_pCurrentNode;

    TextObj*    m_Text;

    Button*     m_BtnIn[5];
    Button*     m_BtnOut;

    Button*     m_BtnStop;

    Button*     m_BtnAdd;
    Button*     m_BtnSub;

};




#endif // profile_canvas_h__
