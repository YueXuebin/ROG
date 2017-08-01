#ifndef package_config_h__
#define package_config_h__


class PackageConfig
{
public:
    PackageConfig(void);
    ~PackageConfig(void);

    DECLARE_SINGLETON(PackageConfig)

    bool LoadConfig(const char* path);

    const TREASURE_PACKAGE_INFO* GetTreasurePackage(int32 id);           // ��ȡ���ذ���

private:
    bool LoadTreasurePackage(const char* path);

    uint32 GetTimeUint(const std::string& base_value );                                   // ����ʱ��

    std::map<uint32, TREASURE_PACKAGE_INFO> m_treasurePackage;                          // �����list
};

#endif 