/*
*	配置文件的读写
*/
#ifndef config_reader_h__
#define config_reader_h__


struct Data3
{
    int32 value1;
    int32 value2;
    int32 value3;
};


class ConfigReader
{
public:
    static bool Read(const TiXmlElement* node, const char* key, int8& val, int32* errCount=NULL);
    static bool Read(const TiXmlElement* node, const char* key, uint8& val, int32* errCount=NULL);
    static bool Read(const TiXmlElement* node, const char* key, int32& val, int32* errCount=NULL);
    static bool Read(const TiXmlElement* node, const char* key, uint32& val, int32* errCount=NULL);
    static bool Read(const TiXmlElement* node, const char* key, std::string& val, int32* errCount=NULL);

    // 解析数值段,例"2-10000" "30-3-1090"
    static Data3 parseData3(const std::string& str);
    // 解析多数值段,例"2-10000|3-100|3-2340" "30-3-1090|30-2-1000"
    static std::vector<Data3> parseData3List(const std::string& str, char delim = '|');
    // 解析数值列表,例"1-3-0-1010-21123-4-23"
    static std::vector<int32> parseIntList(const std::string& str, char delim = '-');

    // 解析奖励物品
    static REWARD_ITEM parseRewardItem(const Data3& data);
    static REWARD_ITEM parseRewardItem(const std::string& str);
    static std::vector<REWARD_ITEM> parseRewardItemList(const std::string& str, char delim = '|');
    static bool LoadRewardItem(TiXmlElement* itEle, REWARD_ITEM& reward_item);             // 加载奖励信息(TiXmlElement, 奖励列表)
};

#endif // config_reader_h__
