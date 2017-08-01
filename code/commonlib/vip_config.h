/*
 *	vip表
 */
#ifndef vip_config_h__
#define vip_config_h__

typedef struct VipTemplate
{
    void Clear()
    {
        level = 0;
        value = 0;
    }

    int32   level;  // 等级
    int32   value;  // vip的值
}VipTemplate;

class VipConfig
{
public:
    VipConfig();
    ~VipConfig();

    DECLARE_SINGLETON(VipConfig)

    bool LoadConfig(const char* path);

    const VipTemplate*  GetVipTemplate(uint32 vipLevel);

    int32 GetVipValue(int32 id, int32 viplevel);    //根据定义的VIP宏id和VIP等级得到VIP特权值

    int32 GetVipLevel(int32 growthPoint);           //根据成长值得到vip的级别

    int32 GetVipValueSum (int32 id);                //得到某个vip特权值的总合

private:
    typedef std::vector<VipTemplate> VipTmplVert;
    typedef std::map<uint32,VipTmplVert> VipMap;

    // 加载vip模板
    void LoadVipTmpl(int32 num,TiXmlElement* vipEle , VipTmplVert& listviptemp);

    VipMap  m_vipMap;
};

#endif // vip_config_h__
