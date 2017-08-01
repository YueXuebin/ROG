/*
 *	Lua脚本引擎
 */
#ifndef lua_engine_h__
#define lua_engine_h__

#include "lua.hpp"
#include "lua_gen.h"

class CLuaObj;

class LuaEngine
{
public:
    LuaEngine(void);
    ~LuaEngine(void);

    DECLARE_SINGLETON(LuaEngine)

    /// Init lua, include init lua_State, open all libs and execute tolua++ generating function
    bool	Init();
    void	Uninit();

    bool    ReloadAllScript();

    // load lua file, here can load many lua files
    bool    LoadScript(const char* szLuaFile);

    // Export C++ Obj
    bool    ExportCLuaObj(const char* szLuaClass, const char* szCClass, CLuaObj* pObj);

    // Register CObj
    bool    RegisterCLuaObj(const char* szClass, const char* szLuaClass, CLuaObj* pObj); 

    // Release C++ export Obj
    void    ReleaseCObj(CLuaObj* pObj);

    // call lua scrip function 
    int     CallLuaScriptFunc(CLuaObj* pObj, int nFuncIndex, int nValue);
    int     CallLuaScriptFunc(CLuaObj* pObj, int nFuncIndex, const char* string1);

    lua_State*	GetLuaContext() {return m_pLuaS;}
    const std::string& GetLuaPath() {return m_LuaPath;}
    

private:
    lua_State*    m_pLuaS;
    std::string     m_LuaPath;

};


#endif // lua_engine_h__
