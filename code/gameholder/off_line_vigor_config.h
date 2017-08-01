
#ifndef off_line_vigor_config_h__
#define off_line_vigor_config_h__
typedef struct OffLineVigor
{
    void clear()
    {
        vip_level = 0;
        money_point = 0;
        recover_percent = 0;
    }
    int32 vip_level;
    int32 money_point;
    uint32 recover_percent;
}OffLineVigor;
class OffLineVigorConfig
{
public:
    OffLineVigorConfig(void);
    ~OffLineVigorConfig(void);

    DECLARE_SINGLETON(OffLineVigorConfig);

    bool LoadConfig(const char* path);

    const OffLineVigor* FindOffLineVigorById(uint32 id);

private: 
    std::map<uint32, OffLineVigor>      m_offlineVigormap;
};
#endif
