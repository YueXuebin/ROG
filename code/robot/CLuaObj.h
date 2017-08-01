#ifndef CLuaObj_h__
#define CLuaObj_h__

#define LUAOBJ_NAME_MAX		16

class CLuaObj
{
public:
	CLuaObj(const char* name);
	virtual ~CLuaObj();

	void	SetObjId(int nId)      { m_nLuaId = nId;   }
	int		GetObjId() const       { return m_nLuaId;  }

	//void	SetName(char* szName)  { strcpy_s(m_szName, LUAOBJ_NAME_MAX, szName); }
	const char* GetName()const     { return m_szName; }

	void	SetLuaObjIndex(int nLuaIndex)  { m_nLuaObjIndex = nLuaIndex;  }
	int		GetLuaObjIndex()const          { return m_nLuaObjIndex;       }

	void	AddIndexFunc(const char* funcName,int nFuncIndex);		// ���뺯��
	int		FindFuncIndex(const char* funcName);						// ��ѯ����

	void	Reset();

protected:
	char	m_szName[LUAOBJ_NAME_MAX];

private:
	int		m_nLuaId;
	
	int		m_nLuaObjIndex;
	std::map<const char*, int>   m_oLuaObjEventFuncMap;					// ������std::map�Ƿ���pkg����
};


#endif // CLuaObj_h__