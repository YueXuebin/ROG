#ifndef text_h__
#define text_h__

#include "object.h"

class TextObj : public Object
{
public:
    TextObj();
    virtual ~TextObj();

    virtual void OnLostDevice();
    virtual void OnResetDevice();

    virtual void Update();
    virtual void Render();

    void SetText(const std::string& text);
    
private:
    ID3DXFont*  m_pFont;
    RECT        m_Rect;
    DWORD       m_Color;
    std::string m_Text;
};


#endif // text_h__
