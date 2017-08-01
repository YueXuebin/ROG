/*
 *	Ö§³Ölua½Å±¾
 */
#ifndef case_lua_h__
#define case_lua_h__


#include "case_base.h"

class CLuaRobot;

class CaseLua : public CaseBase
{
public:
    CaseLua(Robot* pRobot);
    virtual ~CaseLua();

    virtual void OnRecv(uint32 msgID, void* data);
    virtual void Update(uint32 tickcount, uint32 deltacount);
    virtual void OnEnterGame();

    virtual void OnActionStart(ActionBase* action);
    virtual void OnActionEnd(ActionBase* action);

    virtual void OnAllActionEnd();

private:
    CLuaRobot*  m_pLuaRobot;

};

#endif // case_lua_h__
