#include "robot_pch.h"
#include "script.h"
#include "tolua++.h"
#include "CLuaObj.h"


// 这里申明tolua++执行的函数
TOLUA_API int  tolua_interface_open(lua_State*);


CScriptEngine::CScriptEngine()
{
}

CScriptEngine::~CScriptEngine()
{
	if (m_pLuaS)
	{
		lua_close(m_pLuaS);
	    m_pLuaS = NULL;
	}
}

bool CScriptEngine::Init()
{
	m_pLuaS = lua_open();
	if (NULL == m_pLuaS)
		return false;
	
	luaL_openlibs(m_pLuaS);		// 添加所有的库文件

	tolua_interface_open(m_pLuaS);	// tolua++产生的函数，放在这里调用

	return true;
}

void CScriptEngine::Uninit()
{

}

bool CScriptEngine::LoadScript(const char* szLuaFile)
{
	if (NULL == szLuaFile)
		return false;

	// judge the result whether it is failed
	int nState = luaL_dofile(m_pLuaS, szLuaFile);
	if(nState != 0)
		return false;
	
	return true;
}

bool CScriptEngine::ExportCLuaObj(const char* szLuaClass, const char* szCClass,CLuaObj* pObj)
{
	if (NULL == szCClass || NULL == szLuaClass || NULL == pObj)
		return false;
    
	// every CClass Obj map to a LuaTable.
	lua_getglobal(m_pLuaS, szLuaClass);
	if (!lua_istable(m_pLuaS,-1) || lua_isnil(m_pLuaS,-1))
	{
		return false;
	}

	/// Copy a top element of the stack and push it to the top.
	// now the stack list is: luaTable, luaTable.
	lua_pushvalue(m_pLuaS, -1);

	/// (LuaTable[__index] = LuaTable), which means  set LMonster的__index column as itself: 
	/// LuaTable.__index = LuaTable
	lua_setfield(m_pLuaS, -2, "__index");

	/// every LuaTable mapped to CClass has function table, which 
	/// will hold all kind of functions exported.
	lua_getfield(m_pLuaS, -1, "Functions");
	if (!lua_istable(m_pLuaS, -1) || lua_isnil(m_pLuaS,-1))
	{
		lua_pop(m_pLuaS, 1);
		return true;
	}

	/// Get all export function from the Functions tables
	lua_pushnil(m_pLuaS);
	while(lua_next(m_pLuaS, -2))
	{
		if (!lua_isstring(m_pLuaS, -2))
		{
			lua_pop(m_pLuaS, 2);
			return false;
		}
		
		const char* pfuncName = (const char*)lua_tostring(m_pLuaS, -2);

		if (!lua_isfunction(m_pLuaS, -1))
		{
			lua_pop(m_pLuaS, 2);
			return false;
		}

		int nFunRef = luaL_ref(m_pLuaS, LUA_REGISTRYINDEX);
		pObj->AddIndexFunc(pfuncName,nFunRef);
	}
	lua_pop(m_pLuaS, 1);

    /// Register CClass Obj
	RegisterCLuaObj(szCClass, szLuaClass, pObj);

	return true;
}

bool CScriptEngine::RegisterCLuaObj(const char* szCClass, const char* szLuaClass, CLuaObj* pObj)
{
	if (NULL == szCClass || NULL == szLuaClass || NULL == pObj)
	{
		return false;
	}

	/// push user type to lua stack
	tolua_pushusertype(m_pLuaS, pObj, szCClass);

	/// create new table and push to the top of stack
	lua_newtable(m_pLuaS);

	// Get lua related table which is mapped to CObj.
	lua_getglobal(m_pLuaS, szLuaClass);

	// set the new tables metatable as LuaTable
	lua_setmetatable(m_pLuaS, -2);

	// set CObj's function environment as the newtable
	// so if CObj can't find a variable, it can find it from the newtable.
	// it's meta table is LuaTable
	lua_setfenv(m_pLuaS, -2);

	int nIndex = luaL_ref(m_pLuaS, LUA_REGISTRYINDEX);
	pObj->SetLuaObjIndex(nIndex);

	return true;
}

void CScriptEngine::ReleaseCObj(CLuaObj* pObj)
{
	if (NULL == pObj)
		return;

	/// 取消注册信息
	luaL_unref(m_pLuaS,LUA_REGISTRYINDEX,pObj->GetLuaObjIndex());
}

int CScriptEngine::CallLuaScriptFunc(CLuaObj* pObj, int nFuncIndex, int nValue)
{
	if (NULL == pObj)
		return -1;

	lua_rawgeti(m_pLuaS, LUA_REGISTRYINDEX, nFuncIndex);
	lua_rawgeti(m_pLuaS, LUA_REGISTRYINDEX, pObj->GetLuaObjIndex());
	lua_pushnumber(m_pLuaS,nValue);	

	// 调用Lua函数
	int nStatus = lua_pcall(m_pLuaS, 2, 0, 0);
	if (nStatus)
	{
		printf("调用lua函数出错\n");
		return -1;
	}

	return 0;
}

int CScriptEngine::CallLuaScriptFunc( CLuaObj* pObj, int nFuncIndex, const char* string1 )
{
    if (NULL == pObj)
        return -1;

    lua_rawgeti(m_pLuaS, LUA_REGISTRYINDEX, nFuncIndex);
    lua_rawgeti(m_pLuaS, LUA_REGISTRYINDEX, pObj->GetLuaObjIndex());
    lua_pushstring(m_pLuaS, string1);

    // 调用Lua函数
    int nStatus = lua_pcall(m_pLuaS, 2, 0, 0);
    if (nStatus)
    {
        printf("调用lua函数出错\n");
        return -1;
    }

    return 0;
}
