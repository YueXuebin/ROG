#ifndef dance_config_h__
#define dance_config_h__

typedef struct DanceTemplate
{
    void Clear()
    {
        type    = 0;
        exp     = 0;
        time    = 0;
        everySecond = 0;
        gain    = 0;
        hp_rate = 0;
        mp_rate = 0;
    }

    uint32  type;
    uint32  exp;
    uint32  time;
    int32   everySecond;
    int32   gain;
    int32   hp_rate;
    int32   mp_rate;
}DanceTemplate;

class DanceConfig
{
public:
    DanceConfig();
    ~DanceConfig();

    DECLARE_SINGLETON(DanceConfig)

    bool LoadConfig(const char* path);

    const DanceTemplate*  GetDanceTemplate(uint32 danceType);

    const std::map<uint32, DanceTemplate>* GetDanceTemplate();
private:
    std::map<uint32, DanceTemplate> m_dance;
};

#endif // dance_config_h__
