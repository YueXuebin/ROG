/*
 *	buff功能工厂
 */
#ifndef buff_func_factory_h__
#define buff_func_factory_h__


class BuffFuncBase;

BuffFuncBase*   CreateBuffFunc(int32 buffFuncType);

#endif // buff_func_factory_h__
