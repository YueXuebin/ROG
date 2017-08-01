/*
 *	公共定义
 */
#ifndef common_define_h__
#define common_define_h__

const uint32 g_ServerUpdateTime = 50;                               // Server端update周期(ms)
const uint32 g_ServerUpdateTimeBusy = 300;                          // Server端Update周期(ms)繁忙
const uint32 g_ServerUpdateTimeThreshold = 1000;                    // Server端update周期(ms)最大阀值
const uint32 g_ServerUpdateFrequency = 1000 / g_ServerUpdateTime;   // Server端update频率(次/s)

#define VERSION_FULL ((VERSION_PROTOCOL<<24)+(VERSION_MAJOR<<16)+(VERSION_MINOR<<8)+VERSION_REVISION)
#define GET_VERSION_PROTOCOL(x) ((x>>24)&0xff)
#define GET_VERSION_MAJOR(x) ((x>>16)&0xff)
#define GET_VERSION_MINOR(x) ((x>>8)&0xff)
#define GET_VERSION_REVISION(x) (x&0xff)


#endif // common_define_h__
