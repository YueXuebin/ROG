/*
 *	vip��
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

    int32   level;  // �ȼ�
    int32   value;  // vip��ֵ
}VipTemplate;

class VipConfig
{
public:
    VipConfig();
    ~VipConfig();

    DECLARE_SINGLETON(VipConfig)

    bool LoadConfig(const char* path);

    const VipTemplate*  GetVipTemplate(uint32 vipLevel);

    int32 GetVipValue(int32 id, int32 viplevel);    //���ݶ����VIP��id��VIP�ȼ��õ�VIP��Ȩֵ

    int32 GetVipLevel(int32 growthPoint);           //���ݳɳ�ֵ�õ�vip�ļ���

    int32 GetVipValueSum (int32 id);                //�õ�ĳ��vip��Ȩֵ���ܺ�

private:
    typedef std::vector<VipTemplate> VipTmplVert;
    typedef std::map<uint32,VipTmplVert> VipMap;

    // ����vipģ��
    void LoadVipTmpl(int32 num,TiXmlElement* vipEle , VipTmplVert& listviptemp);

    VipMap  m_vipMap;
};

#endif // vip_config_h__
