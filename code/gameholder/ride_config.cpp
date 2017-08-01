#include "gameholder_pch.h"
#include "ride_config.h"

IMPLEMENT_SINGLETON(Ride_Config)
Ride_Config::Ride_Config(void)
{
    m_numCrit = 0;
    m_rideexpress = 0;
    m_rideMapLevelUp;
    m_rideStoneNum = 0;
    m_ItemID = 0;
    m_stonenum = 0;
    m_absorb_ord = 0;
    m_doulbeabsorb = 0;
    m_maxField = 0;
    m_intField = 0;

    m_ord_randnum = 0;
    m_ord_blue = 0;
    m_ord_pureple = 0;
    m_ord_gold = 0;
    m_ser_randnum = 0;
    m_ser_gold = 0;
    m_ser_pureple = 0;
    m_ser_blue = 0;

    m_ord_stonenum = 0;
    m_ord_ride_blue = 0;
    m_ord_ride_pureple = 0;
    m_ord_ride_gold = 0;
    m_ord_ride_while = 0;
    m_ord_ride_gree = 0;
    m_ser_stonenum = 0;
    m_ser_ride_blue = 0;
    m_ser_ride_pureple = 0;
    m_ser_ride_gold = 0;
    m_ser_ride_gree = 0;
    m_ser_ride_while = 0;

    m_maxgrass = 0;
    m_maxbug = 0;
    m_maxhoney = 0;

    m_grass_range = 0;
    m_bug_range = 0;
    m_honey_range = 0;

    m_bulestone = 0;
    m_pureplestone = 0;
    m_goldstone= 0;

    m_moneytoscend = 0;
    m_serupdate = 0;
    m_ordupdate = 0;

    m_rideSeniorSwa_Per = 0;
    m_rideOrdiSwa_Per = 0;

    m_senionrItemnum = 0;
    m_ordItemnum = 0;

    m_supermeStoneNum = 0;

    m_superblueNum = 0;
}

Ride_Config::~Ride_Config(void)
{

}
bool Ride_Config::LoadRideLevelConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load ridelevel file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    LoadRidePetLevel(xmlRoot);
    return true;
}

//bool Ride_Config::LoadRideMarkConfig(const char* path)
//{
//    TiXmlDocument xmlDoc;
//
//    if (!xmlDoc.LoadFile(path))
//    {
//        CnError("Load LoadRideGain file: %s failed\n", path);
//        return false;
//    }
//
//    TiXmlElement* xmlRoot = xmlDoc.RootElement();
//    if(!xmlRoot) 
//        return false;
//
//    for(TiXmlElement* subEle = xmlRoot->FirstChildElement(); subEle; subEle = subEle->NextSiblingElement())
//    {
//        if(Crown::SDStrcmp(subEle->Value(), "marks") == 0)
//        {
//            LoadMarks(subEle);
//        }
//        else if (Crown::SDStrcmp(subEle->Value(), "icons") == 0)
//        {
//            LoadRideAltas(subEle);
//        }
//        else if (Crown::SDStrcmp(subEle->Value(), "modes") == 0)
//        {
//            LoadTrainModels(subEle);
//        }
//        else if(Crown::SDStrcmp(subEle->Value(), "fields") == 0)
//        {
//            LoadTrainFields(subEle);
//        }
//    }
//
//    return true;
//}

bool Ride_Config::LoadRidePetConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load npc file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;

    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {
        LoadRidePet(petEle);
    }
    return true;
}


TrainField* Ride_Config::FindTrainField(uint32 fieldid)
{
    if(!fieldid)
        return NULL;
    Map_TraiFiles::iterator iterTrainModes;
    iterTrainModes = m_trainFields.find(fieldid);
    if(iterTrainModes != m_trainFields.end())
    {
        return &(iterTrainModes->second);
    }
    return NULL;
}

TrainModes* Ride_Config::GetTrainMode(uint32 modeid)
{
    if(!modeid)
        return 0;
    Map_TrainModes::iterator itertrain;
    itertrain = m_trainModes.find(modeid);
    if(itertrain != m_trainModes.end())
    {
        return &(itertrain->second);
    }
    return 0;
}
void Ride_Config::LoadMarks(TiXmlElement* xmlRoot)
{
    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {
        MarkCondit markCondit;
        markCondit.Clear();

        int32 _id = 0;
        petEle->QueryIntAttribute("id", &_id);


        int32 type = 0;
        petEle->QueryIntAttribute("type", &type);
        markCondit.type = type;

        for(TiXmlElement* conEle = petEle->FirstChildElement(); conEle; conEle = conEle->NextSiblingElement())
        {
            int32 level = 0;
            conEle->QueryIntAttribute("level", &level);
            markCondit.level = level;


           int32 vip_level = 0;
            conEle->QueryIntAttribute("vip_level", &vip_level);
            markCondit.vip_level = vip_level;


            int32  _default = 0;
            conEle->QueryIntAttribute("default", &_default);
            markCondit.deafult = _default;
        }

        if (m_MarkCondit.find(_id) == m_MarkCondit.end())
        {
            m_MarkCondit[_id] = markCondit;
        }
        else
        {
            CnWarn("Duplicate mark ride condition: %u", _id);
        }

    }
}

const Map_MarkCondit& Ride_Config::GetMarkCondition()
{
    return m_MarkCondit;
}

bool Ride_Config::LoadConstConfig(const char* path)
{
    TiXmlDocument xmlDoc;

    if (!xmlDoc.LoadFile(path))
    {
        CnError("Load rideconst file: %s failed\n", path);
        return false;
    }

    TiXmlElement* xmlRoot = xmlDoc.RootElement();
    if(!xmlRoot) 
        return false;
    LoadConst(xmlRoot);
    return true;
}

void Ride_Config::LoadConst(TiXmlElement* xmlRoot)
{
    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {

        if (Crown::SDStrcmp(petEle->Value(), "rideStoneNum")== 0)
        {
            int32 _rideStoneNum = 0;
            petEle->QueryIntAttribute("value", &_rideStoneNum);
            m_rideStoneNum = _rideStoneNum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "express")== 0)
        {
            int32 _express = 0;
            petEle->QueryIntAttribute("value", &_express);
            m_rideexpress = _express;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "crit")== 0)
        {
            int32 _crit = 0;
            petEle->QueryIntAttribute("value", &_crit);
            m_crit = _crit;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "critper")== 0)
        {
            int32 _critnum = 0;
            petEle->QueryIntAttribute("value", &_critnum);
            m_numCrit = _critnum ;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "itemid")== 0)
        {
            int32 _m_ItemID = 0;
            petEle->QueryIntAttribute("id", &_m_ItemID);
            m_ItemID = _m_ItemID;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "stonenum")== 0)
        {
            int32 _soneNum = 0;
            petEle->QueryIntAttribute("value", &_soneNum);
            m_stonenum = _soneNum ;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ordAbsorb")== 0)
        {
            int32 _absorb_ord = 0;
            petEle->QueryIntAttribute("value", &_absorb_ord);
            m_absorb_ord = _absorb_ord;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "douAbsorb")== 0)
        {
            int32 _douAbsorb = 0;
            petEle->QueryIntAttribute("value", &_douAbsorb);
            m_doulbeabsorb = _douAbsorb;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "maxField")== 0)
        {
            int32 _maxField = 0;
            petEle->QueryIntAttribute("value", &_maxField);
            m_maxField = _maxField;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "intField")== 0)
        {
            int32 _initField = 0;
            petEle->QueryIntAttribute("value", &_initField);
            m_intField = _initField;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "white")== 0)
        {
            RidePetQuality ridequality;
            int32 _min = 0;
            petEle->QueryIntAttribute("min", &_min);
            ridequality.m_min = _min;

            int32 _max = 0;
            petEle->QueryIntAttribute("max", &_max);
            ridequality.m_max = _max;
            m_vecridequility.insert(std::pair<std::string,RidePetQuality>("white",ridequality));
        }
        else if (Crown::SDStrcmp(petEle->Value(), "gree")== 0)
        {
            RidePetQuality ridequality;
            int32 _min = 0;
            petEle->QueryIntAttribute("min", &_min);
            ridequality.m_min = _min;

            int32 _max = 0;
            petEle->QueryIntAttribute("max", &_max);
            ridequality.m_max = _max;
            m_vecridequility.insert(std::pair<std::string, RidePetQuality>("gree", ridequality));
        }
        else if (Crown::SDStrcmp(petEle->Value(), "blue")== 0)
        {
            RidePetQuality ridequality;
            int32 _min = 0;
            petEle->QueryIntAttribute("min", &_min);
            ridequality.m_min = _min;

            int32 _max = 0;
            petEle->QueryIntAttribute("max", &_max);
            ridequality.m_max = _max;
            m_vecridequility.insert(std::pair<std::string,RidePetQuality>("blue",ridequality));
        }
        else if (Crown::SDStrcmp(petEle->Value(), "pureple")== 0)
        {
            RidePetQuality ridequality;
            int32 _min = 0;
            petEle->QueryIntAttribute("min", &_min);
            ridequality.m_min = _min;

            int32 _max = 0;
            petEle->QueryIntAttribute("max", &_max);
            ridequality.m_max = _max;
            m_vecridequility.insert(std::pair<std::string,RidePetQuality>("pureple",ridequality));
        }
        else if (Crown::SDStrcmp(petEle->Value(), "gold")== 0)
        {
            RidePetQuality ridequality;
            int32 _min = 0;
            petEle->QueryIntAttribute("min", &_min);
            ridequality.m_min = _min;

            int32 _max = 0;
            petEle->QueryIntAttribute("max", &_max);
            ridequality.m_max = _max;
            m_vecridequility.insert(std::pair<std::string,RidePetQuality>("gold",ridequality));
        }else if (Crown::SDStrcmp(petEle->Value(), "ord_blue")== 0)
        {
            int32 _ord_blue = 0;
            petEle->QueryIntAttribute("value", &_ord_blue);
            m_ord_blue = _ord_blue;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_pureple")== 0)
        {
            int32 _ord_pureple = 0;
            petEle->QueryIntAttribute("value", &_ord_pureple);
            m_ord_pureple = _ord_pureple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_gold")== 0)
        {
            int32 _ord_gold = 0;
            petEle->QueryIntAttribute("value", &_ord_gold);
            m_ord_gold = _ord_gold;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_randnum")== 0)
        {
            int32 _ord_randnum = 0;
            petEle->QueryIntAttribute("value", &_ord_randnum);
            m_ord_randnum = _ord_randnum;
        } 
        else if (Crown::SDStrcmp(petEle->Value(), "ser_blue")== 0)
        {
            int32 _ser_blue = 0;
            petEle->QueryIntAttribute("value", &_ser_blue);
            m_ser_blue = _ser_blue;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_pureple")== 0)
        {
            int32 _ser_pureple = 0;
            petEle->QueryIntAttribute("value", &_ser_pureple);
            m_ser_pureple = _ser_pureple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_gold")== 0)
        {
            int32 _ser_gold = 0;
            petEle->QueryIntAttribute("value", &_ser_gold);
            m_ser_gold = _ser_gold;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_randnum")== 0)
        {
            int32 _ser_randnum = 0;
            petEle->QueryIntAttribute("value", &_ser_randnum);
            m_ser_randnum = _ser_randnum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_stonenum")== 0)
        {
            int32 _ser_stonenum = 0;
            petEle->QueryIntAttribute("value", &_ser_stonenum);
            m_ser_stonenum  = _ser_stonenum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_ride_blue")== 0)
        {
            int32 _ser_ride_blue= 0;
            petEle->QueryIntAttribute("value", &_ser_ride_blue);
            m_ser_ride_blue  = _ser_ride_blue;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_ride_pureple")== 0)
        {
            int32 _ser_ride_pureple = 0;
            petEle->QueryIntAttribute("value", &_ser_ride_pureple);
            m_ser_ride_pureple = _ser_ride_pureple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_ride_gold")== 0)
        {
            int32 _ser_ride_gold = 0;
            petEle->QueryIntAttribute("value", &_ser_ride_gold);
            m_ser_ride_gold = _ser_ride_gold;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_stonenum")== 0)
        {
            int32 _ord_stonenum = 0;
            petEle->QueryIntAttribute("value", &_ord_stonenum);
            m_ord_stonenum = _ord_stonenum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_ride_blue")== 0)
        {
            int32 _ord_ride_blue= 0;
            petEle->QueryIntAttribute("value", &_ord_ride_blue);
            m_ord_ride_blue = _ord_ride_blue;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_ride_pureple")== 0)
        {
            int32 _ord_ride_pureple = 0;
            petEle->QueryIntAttribute("value", &_ord_ride_pureple);
            m_ord_ride_pureple = _ord_ride_pureple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_ride_gold")== 0)
        {
            int32 _ord_ride_gold = 0;
            petEle->QueryIntAttribute("value", &_ord_ride_gold);
            m_ord_ride_gold = _ord_ride_gold;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_ride_green")== 0)
        {
            int32 _ord_ride_green = 0;
            petEle->QueryIntAttribute("value", &_ord_ride_green);
            m_ord_ride_gree = _ord_ride_green;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_ride_white")== 0)
        {
            int32 _ord_ride_white = 0;
            petEle->QueryIntAttribute("value", &_ord_ride_white);
            m_ord_ride_while = _ord_ride_white;
        }

        else if (Crown::SDStrcmp(petEle->Value(), "ser_ride_white")== 0)
        {
            int32 _ser_ride_white = 0;
            petEle->QueryIntAttribute("value", &_ser_ride_white);
            m_ser_ride_while = _ser_ride_white;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_ride_green")== 0)
        {
            int32 _ser_ride_green = 0;
            petEle->QueryIntAttribute("value", &_ser_ride_green);
            m_ser_ride_gree = _ser_ride_green;
        }

        else if (Crown::SDStrcmp(petEle->Value(), "grass_range")== 0)
        {
            int32 _grass_range = 0;
            petEle->QueryIntAttribute("value", &_grass_range);
            m_grass_range = _grass_range;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "bug_range")== 0)
        {
            int32 _bug_range = 0;
            petEle->QueryIntAttribute("value", &_bug_range);
            m_bug_range = _bug_range;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "honey_range")== 0)
        {
            int32 _honey_range = 0;
            petEle->QueryIntAttribute("value", &_honey_range);
            m_honey_range = _honey_range;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "grass_max")== 0)
        {
            int32 _grass_max= 0;
            petEle->QueryIntAttribute("value", &_grass_max);
            m_maxgrass = _grass_max;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "bug_max")== 0)
        {
            int32 _bug_max= 0;
            petEle->QueryIntAttribute("value", &_bug_max);
            m_maxbug = _bug_max;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "honey_max")== 0)
        {
            int32 _honey_max= 0;
            petEle->QueryIntAttribute("value", &_honey_max);
            m_maxhoney = _honey_max;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "bulestone")== 0)
        {
            int32 bulestone= 0;
            petEle->QueryIntAttribute("value", &bulestone);
            m_bulestone = bulestone;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "pureplestone")== 0)
        {
            int32 pureplestone = 0;
            petEle->QueryIntAttribute("value", &pureplestone );
            m_pureplestone = pureplestone ;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "goldstone")== 0)
        {
            int32 goldstone= 0;
            petEle->QueryIntAttribute("value", &goldstone);
            m_goldstone = goldstone;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "moneyscend")== 0)
        {
            int32 moneyscend= 0;
            petEle->QueryIntAttribute("value", &moneyscend);
            m_moneytoscend = moneyscend;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ord_upadte")== 0)
        {
            int32 ord_upadte= 0;
            petEle->QueryIntAttribute("value", &ord_upadte);
            m_ordupdate = ord_upadte;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ser_update")== 0)
        {
            int32 ser_update= 0;
            petEle->QueryIntAttribute("value", &ser_update);
            m_serupdate = ser_update;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ord_pureptime")== 0)
        {
            int32 ordpurple = 0;
            petEle->QueryIntAttribute("value", &ordpurple);
            m_ordpurple = ordpurple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_purepnum")== 0)
        {
            int32 ord_return_purple = 0;
            petEle->QueryIntAttribute("value", &ord_return_purple);
            m_ord_return_purple = ord_return_purple;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_goldtime")== 0)
        {
            int32 goldtime = 0;
            petEle->QueryIntAttribute("value", &goldtime);
            m_ordgold = goldtime;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_goldnum")== 0)
        {
            int32 ord_goldnum = 0;
            petEle->QueryIntAttribute("value", &ord_goldnum);
            m_ord_return_gold =  ord_goldnum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_pureptime")== 0)
        {
            int32 ser_pureptime = 0;
            petEle->QueryIntAttribute("value", &ser_pureptime);
            m_serpurple = ser_pureptime;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_purepnum")== 0)
        {
            int32 ser_purepnum = 0;
            petEle->QueryIntAttribute("value", &ser_purepnum);
            m_ser_return_purple = ser_purepnum;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_goldtime")== 0)
        {
            int32 ser_goldtime = 0;
            petEle->QueryIntAttribute("value", &ser_goldtime);
            m_sergold = ser_goldtime;
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_goldnum")== 0)
        {
            int32 ser_goldnum = 0;
            petEle->QueryIntAttribute("value", &ser_goldnum);
            m_ser_return_gold = ser_goldnum;
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ser_swaper")== 0)
        {
             petEle->QueryIntAttribute("value", &m_rideSeniorSwa_Per);
        }
        else if(Crown::SDStrcmp(petEle->Value(), "ord_swaper")== 0)
        {
             petEle->QueryIntAttribute("value", &m_rideOrdiSwa_Per);
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ord_itemnum")== 0)
        {
             petEle->QueryIntAttribute("value", &m_ordItemnum);
        }
        else if (Crown::SDStrcmp(petEle->Value(), "ser_itemnum")== 0)
        {
            petEle->QueryIntAttribute("value", &m_senionrItemnum);
        }
        else if(Crown::SDStrcmp(petEle->Value(), "superme_itemnum")== 0)
        {
            petEle->QueryIntAttribute("value", &m_supermeStoneNum);
        }
        else if(Crown::SDStrcmp(petEle->Value(), "super_blueNum")== 0)
        {
            petEle->QueryIntAttribute("value", &m_superblueNum);
        }
    }
}
MarkCondit* Ride_Config::FindMarkCondit(uint32 id)
{
    if(!id)
        return 0;
    Map_MarkCondit::iterator iterMakr;
    iterMakr = m_MarkCondit.find(id);
    if(iterMakr != m_MarkCondit.end())
    {
        return &(iterMakr->second);
    }

    return 0;
}

void Ride_Config::LoadTrainFields(TiXmlElement* xmlRoot)
{
    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {
        int32 id = 0;
        petEle->QueryIntAttribute("id", &id);


        TrainField trainFiles;
        trainFiles.Clear();

        for (TiXmlElement* conditionEle = petEle->FirstChildElement(); conditionEle; conditionEle = conditionEle->NextSiblingElement())
        {
         /*   int32 vip_level = 0;
            conditionEle->QueryIntAttribute("vip_level", &vip_level);
            trainFiles.vip_level = vip_level;*/

            int32 monyepoint = 0;
            conditionEle->QueryIntAttribute("moneypoint", &monyepoint);
            trainFiles.monyepoint = monyepoint;

            int32 _default = 0;
            conditionEle->QueryIntAttribute("default", &_default);
            trainFiles.deafult = _default;
        }

        if (m_trainFields.find(id) == m_trainFields.end())
        {
            m_trainFields[id] = trainFiles;
        }
        else
        {
            CnWarn("Duplicate ride train fields: %u\n", id);
        }
    }
}
void Ride_Config::LoadTrainModels(TiXmlElement* xmlRoot)
{
    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {


        int32 id = 0;
        petEle->QueryIntAttribute("id", &id);

        TrainModes trainmodes;
        trainmodes.Clear();

    /*    int32 condition = 0;
        petEle->QueryIntAttribute("condition", &condition);
        trainmodes.condition = condition;*/
        if(id/1000 > 10)
            CnAssert(false);

        int32 time = 0;
        petEle->QueryIntAttribute("time", &time);
        trainmodes.time = time;

        int32 exp = 0;
        petEle->QueryIntAttribute("exp", &exp);
        trainmodes.exp = exp;

        std::string effect_value = petEle->Attribute("spend");
        std::vector<std::string> values = Crown::SDSplitStrings(effect_value, ' ');
        for(uint32 i=0; i<values.size(); ++i)
        {
            std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
            if(key_value.size() == 2)
            {
                trainmodes.spend.key = SDAtoi(key_value[0].c_str());
                trainmodes.spend.num= SDAtoi(key_value[1].c_str());
            }
        }

        if (m_trainModes.find(id) == m_trainModes.end())
        {
            m_trainModes[id] = trainmodes;
        }
        else
        {
            CnWarn("Duplicate ride trainmodes: %u\n", id);
        }
    }
}
void Ride_Config::LoadRideAltas(TiXmlElement* xmlRoot)
{
    for(TiXmlElement* petEle = xmlRoot->FirstChildElement(); petEle; petEle = petEle->NextSiblingElement())
    {
        int32 _id = 0;
        petEle->QueryIntAttribute("id", &_id);

        AtlasStruct atlastruct; 
        atlastruct.Clear();

        int32 quality = 0;
        petEle->QueryIntAttribute("quality", &quality);
        atlastruct.quality = quality;


        int32 num = 0;
        petEle->QueryIntAttribute("num", &num);
        atlastruct.num = num;


        std::string effect_value = petEle->Attribute("addpro");
        std::vector<std::string> values = Crown::SDSplitStrings(effect_value, ' ');
        for(uint32 i=0; i<values.size(); ++i)
        {
            std::vector<std::string> key_value = Crown::SDSplitStrings(values[i], '-');
            REWARD_ITEM reward_item;
            if(key_value.size() == 2)
            {
                reward_item.id = SDAtoi(key_value[0].c_str());
                reward_item.num= SDAtoi(key_value[1].c_str());
            }

            atlastruct.rewardlist.push_back(reward_item);
        }


        if (m_mapAltas.find(_id) == m_mapAltas.end())
        {
            m_mapAltas[_id] = atlastruct;
        }
        else
        {
            CnWarn("Duplicate mark ride condition: %u", _id);
        }
    }
}

const Map_Atlas& Ride_Config::GetMapAltas()
{
    return m_mapAltas;
}

AtlasStruct* Ride_Config::GetAltasStruct(uint32 atlasid)
{
    Map_Atlas::iterator mapaltas;
    mapaltas = m_mapAltas.find(atlasid);
    if (mapaltas != m_mapAltas.end())
    {
        return &(mapaltas->second);
    }

    return 0;
}

Map_TraiFiles& Ride_Config::GetTrainFields()
{
    return m_trainFields;
}

// 获得骑宠模板
RidePetTemplate* Ride_Config::GetRidePet(uint32 id)
{
    if(!id)
        return 0;
    RideTempMap::iterator petIt;
    petIt = m_ride_pet.find(id);
    if(petIt != m_ride_pet.end())
    {
        return &(petIt->second);
    }
    return 0;
}
int32 Ride_Config::GetRidePetLeveupTem(int32 id, int32 level)
{
    std::map<uint32,Vec_Ride_levelUp>::iterator petIt;
    petIt = m_rideMapLevelUp.find(id);
    if(petIt != m_rideMapLevelUp.end())
    {
        Vec_Ride_levelUp::iterator vec_leveup;
        for (vec_leveup = petIt->second.begin(); vec_leveup != petIt->second.end() ; vec_leveup++)
        {
            if(vec_leveup->level == level)
            {
                return vec_leveup->experess;
            }
        }
    }
    return 0;
}

int32 Ride_Config::CaluteRideExpress(uint32 id, int32 level)
{
    int32 curexpress = 0;

    std::map<uint32,Vec_Ride_levelUp>::iterator petIt = m_rideMapLevelUp.find(id);
    if(petIt != m_rideMapLevelUp.end())
    {
        Vec_Ride_levelUp::iterator vec_leveup;
        for (vec_leveup = petIt->second.begin(); vec_leveup != petIt->second.end() ; vec_leveup++)
        {
            if(vec_leveup->level < level)
            {
                curexpress = curexpress + vec_leveup->experess;
            }
        }
    }

    return curexpress;
}

RidePetLeveUpTempl* Ride_Config::GetRideLeveTempl(int32 id, int32 level)
{
    if(!id)
        return NULL;

    std::map<uint32,Vec_Ride_levelUp>::iterator petIt = m_rideMapLevelUp.find(id);
    if(petIt != m_rideMapLevelUp.end())
    {
        Vec_Ride_levelUp::iterator vec_leveup;
        for (vec_leveup = petIt->second.begin(); vec_leveup != petIt->second.end() ; vec_leveup++)
        {
            if(vec_leveup->level == level)
            {
                return &(*vec_leveup);
            }
        }
    }
    return NULL;
}
int32 Ride_Config::FindRideNextLevel(uint32 id, int32 curexprss)
{
    std::map<uint32,Vec_Ride_levelUp>::iterator petIt = m_rideMapLevelUp.find(id);
    if(petIt != m_rideMapLevelUp.end())
    {
        Vec_Ride_levelUp::iterator vec_leveup;
        for (vec_leveup = petIt->second.begin(); vec_leveup != petIt->second.end() ; vec_leveup++)
        {
            if(vec_leveup->experess >curexprss)
            {
                return vec_leveup->level;
            }
        }
    }
    return 0;
}

void Ride_Config::LoadRidePetLevel(TiXmlElement* petEle)
{
    for(TiXmlElement* idEle = petEle->FirstChildElement(); idEle; idEle = idEle->NextSiblingElement())
    {
        int32 _id = 0;
        idEle->QueryIntAttribute("id", &_id);

        Vec_Ride_levelUp vec_followup;
        for(TiXmlElement* levelEle = idEle->FirstChildElement(); levelEle; levelEle = levelEle->NextSiblingElement())
        {
            RidePetLeveUpTempl petLevel;
            petLevel.Clear();

            int32 _level = 0;
            levelEle->QueryIntAttribute("level", &_level);
            petLevel.level = _level;

            int32 _Exp = 0;
            levelEle->QueryIntAttribute("exp", &_Exp);
            petLevel.experess = _Exp;

            int32 _res_mp = 0;
            levelEle->QueryIntAttribute("res_mp", &_res_mp);
            petLevel.res_mp = _res_mp;

            int32 _move_speed= 0;
            levelEle->QueryIntAttribute("speed", &_move_speed);
            petLevel.speed = _move_speed;
            
            int32 _res_hp = 0;
            levelEle->QueryIntAttribute("res_hp", &_res_hp);
            petLevel.res_hp = _res_hp;

            vec_followup.push_back(petLevel);
        }

        if(m_rideMapLevelUp.find(_id) == m_rideMapLevelUp.end())
        {
            m_rideMapLevelUp[_id] = vec_followup;
        }
        else
        {
            CnWarn("Duplicate LoadRidePetLevel: %u\n", _id);
        }

    }

}

RidePetQuality* Ride_Config::GetRidePetQuality(const std::string& quality)
{
    Vec_RidePetQuality::iterator vec_ridequality;
    vec_ridequality = m_vecridequility.find(quality);
    if(vec_ridequality!= m_vecridequility.end())
    {
        return &(vec_ridequality->second);
    }
    return 0;
}

void Ride_Config::LoadRidePet(TiXmlElement* petEle)
{
    RidePetTemplate pet;

    if(!petEle) return;

    pet.Clear();

    int32 _id = 0;
    petEle->QueryIntAttribute("id", &_id);
    pet.id = _id;

    int32 _type = 0;
    petEle->QueryIntAttribute("ride_type", &_type);
    pet.type = _type;


    int32 _level = 0;
    petEle->QueryIntAttribute("level", &_level);
    pet.level = _level;

    std::string _name = petEle->Attribute("name");
    pet.name = _name;

    int32 _free_number = 0;
    petEle->QueryIntAttribute("free_number", &_free_number);
    pet.free_number = _free_number;


    int32 _express_price = 0;
    petEle->QueryIntAttribute("express_price", &_express_price);
    pet.express_price = _express_price;

    int32 fighting = 0;
    petEle->QueryIntAttribute("fighting", &fighting);
    pet.fighting = fighting;

    int32 isopen = 0;
    petEle->QueryIntAttribute("isopen", &isopen);
    pet.isopen = isopen;

    int32 skillID = 0;
    petEle->QueryIntAttribute("skill_id", &skillID);
    pet.skillID = skillID;

    if(m_ride_pet.find(pet.id) == m_ride_pet.end())
    {
        m_ride_pet[pet.id] = pet;
    }
    else
    {
        CnWarn("Duplicate RidePetID: %u\n", pet.id);
    }
}
