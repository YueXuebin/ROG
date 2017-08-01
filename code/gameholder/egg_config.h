//砸金蛋配置文件
#ifndef __EGG_CONFIG_CONFIG_H__
#define __EGG_CONFIG_CONFIG_H__

typedef std::vector<EGG_INFO_CONFI> EGG_ITEM_LIST;

class EggConfig
{
public:
    EggConfig(void);
    ~EggConfig(void);

    

    DECLARE_SINGLETON(EggConfig)

    bool LoadConfig(const char* path);

    void Clear();

    const EGG_INFO_CONFI* GetEggItemByType(uint32 type);

    EGG_ITEM_LIST GetEggConfigList() { return m_eggList; };

private:
    EGG_ITEM_LIST m_eggList;
};
#endif