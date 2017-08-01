#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

// scrip engine module
#include "lua.hpp"
#include "sdsingleton.h"

class CLuaObj;

class CScriptEngine : public Crown::CSingleton<CScriptEngine>
{	
public:
    CScriptEngine();
    ~CScriptEngine();

    /// Init lua, include init lua_State, open all libs and execute tolua++ generating function
    bool	Init();
    void	Uninit();

    // load lua file, here can load many lua files
    bool    LoadScript(const char* szLuaFile);

    // Export C++ Obj
    bool    ExportCLuaObj(const char* szLuaClass, const char* szCClass, CLuaObj* pObj);

    // Register CObj
    bool    RegisterCLuaObj(const char* szClass, const char* szLuaClass, CLuaObj* pObj); 

    /// Release C++ export Obj
    void    ReleaseCObj(CLuaObj* pObj);

    /// call lua scrip function 
    int     CallLuaScriptFunc(CLuaObj* pObj, int nFuncIndex, int nValue);
    int     CallLuaScriptFunc(CLuaObj* pObj, int nFuncIndex, const char* string1);

    lua_State*	GetLuaContext() {return m_pLuaS;}
private:
    lua_State*    m_pLuaS;
};



#endif