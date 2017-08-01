#ifndef SCOPEDPOINTERVECTOR_H
#define SCOPEDPOINTERVECTOR_H

#include <memory>

#include "sdcontainers.h"
#include "unordered_set"

namespace Crown
{

template <typename T>
class ScopedPointerVector
{
	CnVector<std::unique_ptr<T> > m_Container;

public:
	void Add(T* ptr)
	{
		m_Container.push_back(std::unique_ptr<T>(ptr));
	}
};

template <typename T>
class ScopedPointerMap
{
	CnSet<T*> m_Container;

public:

    typename CnSet<T*>::iterator begin()
    {
        return m_Container.begin();
    }
    
    typename CnSet<T*>::iterator end()
    {
        return m_Container.end();
    }


	virtual ~ScopedPointerMap()
	{
		for (auto itor = m_Container.begin(); itor != m_Container.end(); ++itor)
		{
			CnDelete *itor;
		}
	}

	void Add(T* ptr)
	{
		m_Container.insert(ptr);
	}

	void RemoveFromMap(T* ptr)
	{
		CnAssert(m_Container.erase(ptr) > 0);
	}
};

}

// 使用std::map是为了暴露给swig使用.
template <typename K, typename V>
class CnAutoDeleteHashMap : public std::map<K, V>
{
public:
    virtual ~CnAutoDeleteHashMap()
    {
        for (auto itor = this->begin(); itor != this->end(); ++itor)
        {
            CnDelete itor->second;
        }
    }
};

#endif // SCOPEDPOINTERVECTOR_H
