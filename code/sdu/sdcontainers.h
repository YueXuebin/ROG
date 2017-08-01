#pragma once

#ifndef SDCONTAINERS_H
#define SDCONTAINERS_H

#include "sdhashmap.h"
#include <vector>

template <class T>
class CnVector : public std::vector<T>
{
public:
	typedef typename CnVector<T>::const_iterator const_iterator;
	typedef typename CnVector<T>::iterator iterator;
	typedef typename CnVector<T>::size_type size_type;

	//CnVector() : std::vector<T> {}				// msvc不支持
	//explicit CnVector(size_type n) : std::vector<T>(n) {}

	bool has(T item) const
	{
		size_type size = this->size();
		for (size_type i = 0; i < size; ++i)
		{
			if (this->operator [](i) == item)
				return true;
		}
		return false;
	}

    iterator find(T item)
    {
        for (CnVector<T>::iterator itor = this->begin(); itor != this->end(); ++itor)
        {
            if (*itor == item)
                return itor;
        }
        return this->end();
    }

	// If the size of vector are larger than half of max long long, there would be an potential error.
    llong index_of(T item) const
	{
		size_type size = this->size();
		for (size_type i = 0; i < size; ++i)
		{
			if (this->operator [](i) == item)
				return i;
		}
		return -1;
	}
};


#endif // SDCONTAINERS_H
