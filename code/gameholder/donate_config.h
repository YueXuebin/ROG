/** 

*/
#ifndef donate_config_h__
#define donate_config_h__

typedef struct DonateTemplate
{
    uint32 money_point;
    uint32 reward_percent;
    uint32 title_id;
    uint32 autotime;

    void Clear()
    {
        money_point = 0;
        reward_percent = 0;
        title_id = 0;
        autotime = 0;
    }

}DonateTemplate;


class DonateConfig
{
public:
    DonateConfig();
    ~DonateConfig();

    DECLARE_SINGLETON(DonateConfig);

    bool LoadConfig(const char* path);

    const DonateTemplate& GetDonateTemplate();
private:
    DonateTemplate m_donate_template;

};






#endif