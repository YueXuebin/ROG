#ifndef button_h__
#define button_h__

#include "rect_object.h"

enum{
    BUTTON_NORMAL = 1,
    BUTTON_HOVER,
    BUTTON_DOWN
};

class TextObj;

class Button : public RectObject
{
public:
    Button();
    virtual ~Button();

    virtual void OnLButtonDown(int32 px, int32 py);
    virtual void OnLButtonUp(int32 px, int32 py);
    virtual void OnMouseMove(int32 px, int32 py);

    void SetText(const std::string& text);

private:
    void SetState(uint32 state);

private:
    uint32  m_ButtonState;
    TextObj*    m_Text;

};





#endif // button_h__


