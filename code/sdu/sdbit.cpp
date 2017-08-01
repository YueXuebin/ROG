#include "sdtype.h"
#include <stdio.h>
#include "SDuPCH.h"

namespace Crown
{
    uint32 SDBitSaveOne(uint32 bit, uint32 sdata)
    {
        CnAssert(bit < 32);
        uint32 k = 0x1 << (bit - 1);
        return k | sdata;
    }
    
    uint32 SDBitSaveZero(uint32 bit, uint32 sdata)
    {
        CnAssert(bit < 32);
        uint32 k = 0xFFFFFFFE << (bit - 1);
        return k & sdata;
    }

	/**
	* @
	* 读取一个整数中的bit位的值
	* @param bit : 第几位
	* @param sdata : 要读取的整数
	* @return : 返回第bit位的值
	*/
    uint32 SDBitRead(uint32 bit, uint32 sdata)
    {
        CnAssert(bit < 32);
        uint32 k = 0x1 << (bit -1);
        return (k & sdata) > 0;
    }

	/**
	* @
	* 按位保存数据到一个整数中
	* @param bit : 多少位
	* @param sdata : 要存储的数
	* @return : 返回存储过的数
	*/
    uint16 SDBitSaveOne(uint16 bit, uint16 sdata)
    {
        CnAssert(bit < 16);
        uint16 k = 0x1 << (bit - 1);
        return k | sdata;
    }

    uint16 SDBitSaveZero(uint16 bit, uint16 sdata)
    {
        CnAssert(bit < 16);
        uint16 k = 0xFFFFFFFE << (bit - 1);
        return k & sdata;
    }

	/**
	* @
	* 读取一个整数中的bit位的值
	* @param bit : 第几位
	* @param sdata : 要读取的整数
	* @return : 返回第bit位的值
	*/
    uint16 SDBitRead(uint16 bit, uint16 sdata)
    {
        CnAssert(bit < 16);
        uint16 k = 0x1 << (bit -1);
        return (k & sdata) > 0;
    }
}