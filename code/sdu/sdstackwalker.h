/*
 *	堆栈dump
 */
#ifndef sdstackwalker_h__
#define sdstackwalker_h__

#ifdef WIN
namespace Crown
{
	class SdStackWalker
	{
	public:
		SdStackWalker();
		void Dump();
	};
}
#else
namespace Crown
{
	class SdStackWalker
	{
	public:
		void Dump() {}
	};
}
#endif

#endif // sdstackwalker_h__
