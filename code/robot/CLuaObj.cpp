#include "robot_pch.h"
#include "CLuaObj.h"


CLuaObj::CLuaObj(const char* name)
{
    Crown::SDStrSafeCpy(m_szName, LUAOBJ_NAME_MAX, name);	
	m_nLuaObjIndex = -1;
	m_oLuaObjEventFuncMap.clear();
	Reset();
}

CLuaObj::~CLuaObj()
{
	m_nLuaObjIndex = -1;
	m_oLuaObjEventFuncMap.clear();
	Reset();
}


void	CLuaObj::AddIndexFunc(const char* funcName,int nFuncIndex)
{
	if (m_oLuaObjEventFuncMap.find(funcName) == m_oLuaObjEventFuncMap.end())
	{
		m_oLuaObjEventFuncMap[funcName] = nFuncIndex;
	}
}

int		CLuaObj::FindFuncIndex(const char* funcName)
{
	for(std::map<const char*,int>::iterator iter = m_oLuaObjEventFuncMap.begin();
		iter != m_oLuaObjEventFuncMap.end(); iter++)
	{
		if(strcmp(iter->first, funcName) == 0)
			return iter->second;
	}

	return -1;
}

void	CLuaObj::Reset()
{
	m_nLuaId = 0;
}