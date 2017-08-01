#pragma once

#include <fd/delegate.hpp>
#include <ScopedPointerContainers.h>

class SignalConnectionManager
{
    Crown::ScopedPointerVector<fd::multicast::scoped_token> m_PtrVector;
public:
	template <typename Signal, typename Functor>
	void Connect(Signal& s, Functor&& f)
	{
		// FIXME: The vector should be clean.
        m_PtrVector.Add(CnNew fd::multicast::scoped_token(s.add(f)));
	}
};
