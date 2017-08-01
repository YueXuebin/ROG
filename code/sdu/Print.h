#pragma once

#ifndef PRINT_H
#define PRINT_H

#ifndef WIN // Fuck VS 2010
/*
#include <boost/format.hpp>

namespace Crown
{

template <typename TStream>
void Printf(TStream& out, boost::format& fmt)
{
	out << fmt;
}

template <typename TStream, typename THead, typename ... Tail>
void Printf(TStream& out, boost::format& fmt, const THead& head, const Tail&... args)
{
	Printf(out, fmt % head, args...);
}

template <typename TStream, typename ... Args>
void Printf(TStream& out, const char* s, const Args&... args)
{
	if (sizeof...(Args))
	{
		boost::format fmt(s);
		Printf(out, fmt, args...);
	}
	else
	{
		out << s;
	}
}

}

*/
#endif

#endif // PRINT_H
