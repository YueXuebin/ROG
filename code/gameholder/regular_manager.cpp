#include "gameholder_pch.h"
#include "dungeon_manager.h"
#include "regular_manager.h"
#include "regular.h"
#include "regular_define.h"
#include "regular_data.h"
//
#include "regular_city.h"
#include "regular_home.h"
#include "regular_field_001.h"
#include "regular_field_002.h"
#include "regular_field_003.h"
#include "regular_field_004.h"
#include "regular_field_005.h"
#include "regular_field_006.h"
#include "regular_pvp.h"
#include "regular_maoxian_001.h"
#include "regular_maoxian_101.h"
#include "regular_maoxian_201.h"
#include "regular_maoxian_002.h"
#include "regular_maoxian_102.h"
#include "regular_maoxian_202.h"
#include "regular_maoxian_003.h"
#include "regular_maoxian_103.h"
#include "regular_maoxian_203.h"
#include "regular_maoxian_004.h"
#include "regular_maoxian_104.h"
#include "regular_maoxian_204.h"
#include "regular_maoxian_005.h"
#include "regular_maoxian_105.h"
#include "regular_maoxian_205.h"
#include "regular_maoxian_006.h"
#include "regular_maoxian_106.h"
#include "regular_maoxian_206.h"
#include "regular_maoxian_007.h"
#include "regular_maoxian_107.h"
#include "regular_maoxian_207.h"
#include "regular_maoxian_008.h"
#include "regular_maoxian_108.h"
#include "regular_maoxian_208.h"
#include "regular_maoxian_009.h"
#include "regular_maoxian_109.h"
#include "regular_maoxian_209.h"
#include "regular_maoxian_010.h"
#include "regular_maoxian_110.h"
#include "regular_maoxian_210.h"
#include "regular_maoxian_011.h"
#include "regular_maoxian_111.h"
#include "regular_maoxian_211.h"
#include "regular_maoxian_012.h"
#include "regular_maoxian_112.h"
#include "regular_maoxian_212.h"
#include "regular_maoxian_013.h"
#include "regular_maoxian_113.h"
#include "regular_maoxian_213.h"
#include "regular_maoxian_014.h"
#include "regular_maoxian_114.h"
#include "regular_maoxian_214.h"
#include "regular_maoxian_015.h"
#include "regular_maoxian_115.h"
#include "regular_maoxian_215.h"
#include "regular_maoxian_016.h"
#include "regular_maoxian_116.h"
#include "regular_maoxian_216.h"
#include "regular_maoxian_017.h"
#include "regular_maoxian_117.h"
#include "regular_maoxian_217.h"
#include "regular_maoxian_018.h"
#include "regular_maoxian_118.h"
#include "regular_maoxian_218.h"
#include "regular_maoxian_019.h"
#include "regular_maoxian_119.h"
#include "regular_maoxian_219.h"
#include "regular_maoxian_020.h"
#include "regular_maoxian_120.h"
#include "regular_maoxian_220.h"
#include "regular_maoxian_021.h"
#include "regular_maoxian_121.h"
#include "regular_maoxian_221.h"
#include "regular_maoxian_022.h"
#include "regular_maoxian_122.h"
#include "regular_maoxian_222.h"
#include "regular_maoxian_023.h"
#include "regular_maoxian_123.h"
#include "regular_maoxian_223.h"
#include "regular_maoxian_024.h"
#include "regular_maoxian_124.h"
#include "regular_maoxian_224.h"
#include "regular_maoxian_025.h"
#include "regular_maoxian_125.h"
#include "regular_maoxian_225.h"
#include "regular_yijie_001.h"
#include "regular_yijie_002.h"
#include "regular_yijie_003.h"
#include "regular_yijie_004.h"
#include "regular_yijie_005.h"
#include "regular_yijie_006.h"
#include "regular_yijie_007.h"
#include "regular_yijie_008.h"
#include "regular_yijie_009.h"
#include "regular_yijie_010.h"
#include "regular_yijie_011.h"
#include "regular_yijie_012.h"
#include "regular_yijie_013.h"
#include "regular_yijie_014.h"
#include "regular_yijie_015.h"
#include "regular_yijie_016.h"
#include "regular_yijie_017.h"
#include "regular_yijie_018.h"
#include "regular_yijie_019.h"
#include "regular_yijie_020.h"
#include "regular_yijie_021.h"
#include "regular_yijie_022.h"
#include "regular_yijie_023.h"
#include "regular_yijie_024.h"
#include "regular_yijie_025.h"
#include "regular_yijie_026.h"
#include "regular_yijie_027.h"
#include "regular_yijie_028.h"
#include "regular_yijie_029.h"
#include "regular_yijie_030.h"
#include "regular_yijie_031.h"
#include "regular_yijie_032.h"
#include "regular_yijie_033.h"
#include "regular_yijie_034.h"
#include "regular_yijie_035.h"
#include "regular_yijie_036.h"
#include "regular_yijie_037.h"
#include "regular_yijie_038.h"
#include "regular_yijie_039.h"
#include "regular_yijie_040.h"
#include "regular_daily_001.h"
#include "regular_daily_002.h"
#include "regular_daily_003.h"
#include "regular_daily_004.h"
#include "regular_daily_005.h"
#include "regular_daily_006.h"
#include "regular_daily_007.h"
#include "regular_daily_008.h"
#include "regular_first.h"

IMPLEMENT_SINGLETON(RegularManager)

RegularManager::RegularManager()
{
    // 主城001
    m_regularCreatorMap["city_001.dgn"] = CnNew TmplRegularCreator<RegularCity>();
    // 领地001
    m_regularCreatorMap["lingdi_001.dgn"] = CnNew TmplRegularCreator<RegularHome>();
    // 新手场景
    m_regularCreatorMap["first_001.dgn"] = CnNew TmplRegularCreator<RegularFirst>();
    // 竞技场
    m_regularCreatorMap["test_3v3.dgn"] = CnNew TmplRegularCreator<RegularPVP>();
    
    // 野外地图001 - 006
    m_regularCreatorMap["yewai_001.dgn"] = CnNew TmplRegularCreator<RegularField_001>();
    m_regularCreatorMap["yewai_002.dgn"] = CnNew TmplRegularCreator<RegularField_002>();
    m_regularCreatorMap["yewai_003.dgn"] = CnNew TmplRegularCreator<RegularField_003>();
    m_regularCreatorMap["yewai_004.dgn"] = CnNew TmplRegularCreator<RegularField_004>();
    m_regularCreatorMap["yewai_005.dgn"] = CnNew TmplRegularCreator<RegularField_005>();
    m_regularCreatorMap["yewai_006.dgn"] = CnNew TmplRegularCreator<RegularField_006>();

    // 冒险地图001
    m_regularCreatorMap["maoxian_001.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_001>();
    m_regularCreatorMap["maoxian_101.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_101>();
    m_regularCreatorMap["maoxian_201.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_201>();
    // 冒险地图002
    m_regularCreatorMap["maoxian_002.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_002>();
    m_regularCreatorMap["maoxian_102.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_102>();
    m_regularCreatorMap["maoxian_202.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_202>();
    // 冒险地图003
    m_regularCreatorMap["maoxian_003.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_003>();
    m_regularCreatorMap["maoxian_103.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_103>();
    m_regularCreatorMap["maoxian_203.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_203>();
    // 冒险地图004
    m_regularCreatorMap["maoxian_004.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_004>();
    m_regularCreatorMap["maoxian_104.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_104>();
    m_regularCreatorMap["maoxian_204.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_204>();
    // 冒险地图005
    m_regularCreatorMap["maoxian_005.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_005>();
    m_regularCreatorMap["maoxian_105.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_105>();
    m_regularCreatorMap["maoxian_205.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_205>();
    // 冒险地图006
    m_regularCreatorMap["maoxian_006.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_006>();
    m_regularCreatorMap["maoxian_106.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_106>();
    m_regularCreatorMap["maoxian_206.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_206>();
    // 冒险地图007
    m_regularCreatorMap["maoxian_007.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_007>();
    m_regularCreatorMap["maoxian_107.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_107>();
    m_regularCreatorMap["maoxian_207.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_207>();
    // 冒险地图008
    m_regularCreatorMap["maoxian_008.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_008>();
    m_regularCreatorMap["maoxian_108.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_108>();
    m_regularCreatorMap["maoxian_208.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_208>();
    // 冒险地图009
    m_regularCreatorMap["maoxian_009.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_009>();
    m_regularCreatorMap["maoxian_109.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_109>();
    m_regularCreatorMap["maoxian_209.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_209>();
    // 冒险地图010
    m_regularCreatorMap["maoxian_010.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_010>();
    m_regularCreatorMap["maoxian_110.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_110>();
    m_regularCreatorMap["maoxian_210.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_210>();
    // 冒险地图011
    m_regularCreatorMap["maoxian_011.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_011>();
    m_regularCreatorMap["maoxian_111.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_111>();
    m_regularCreatorMap["maoxian_211.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_211>();
    // 冒险地图012
    m_regularCreatorMap["maoxian_012.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_012>();
    m_regularCreatorMap["maoxian_112.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_112>();
    m_regularCreatorMap["maoxian_212.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_212>();
    // 冒险地图013
    m_regularCreatorMap["maoxian_013.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_013>();
    m_regularCreatorMap["maoxian_113.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_113>();
    m_regularCreatorMap["maoxian_213.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_213>();
    // 冒险地图014
    m_regularCreatorMap["maoxian_014.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_014>();
    m_regularCreatorMap["maoxian_114.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_114>();
    m_regularCreatorMap["maoxian_214.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_214>();
    // 冒险地图015
    m_regularCreatorMap["maoxian_015.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_015>();
    m_regularCreatorMap["maoxian_115.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_115>();
    m_regularCreatorMap["maoxian_215.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_215>();
    // 冒险地图016
    m_regularCreatorMap["maoxian_016.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_016>();
    m_regularCreatorMap["maoxian_116.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_116>();
    m_regularCreatorMap["maoxian_216.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_216>();
    // 冒险地图017
    m_regularCreatorMap["maoxian_017.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_017>();
    m_regularCreatorMap["maoxian_117.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_117>();
    m_regularCreatorMap["maoxian_217.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_217>();
    // 冒险地图018
    m_regularCreatorMap["maoxian_018.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_018>();
    m_regularCreatorMap["maoxian_118.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_118>();
    m_regularCreatorMap["maoxian_218.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_218>();
    // 冒险地图019
    m_regularCreatorMap["maoxian_019.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_019>();
    m_regularCreatorMap["maoxian_119.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_119>();
    m_regularCreatorMap["maoxian_219.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_219>();
    // 冒险地图020
    m_regularCreatorMap["maoxian_020.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_020>();
    m_regularCreatorMap["maoxian_120.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_120>();
    m_regularCreatorMap["maoxian_220.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_220>();
    // 冒险地图021
    m_regularCreatorMap["maoxian_021.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_021>();
    m_regularCreatorMap["maoxian_121.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_121>();
    m_regularCreatorMap["maoxian_221.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_221>();
    // 冒险地图022
    m_regularCreatorMap["maoxian_022.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_022>();
    m_regularCreatorMap["maoxian_122.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_122>();
    m_regularCreatorMap["maoxian_222.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_222>();
    // 冒险地图023
    m_regularCreatorMap["maoxian_023.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_023>();
    m_regularCreatorMap["maoxian_123.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_123>();
    m_regularCreatorMap["maoxian_223.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_223>();
    // 冒险地图024
    m_regularCreatorMap["maoxian_024.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_024>();
    m_regularCreatorMap["maoxian_124.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_124>();
    m_regularCreatorMap["maoxian_224.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_224>();
    // 冒险地图025
    m_regularCreatorMap["maoxian_025.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_025>();
    m_regularCreatorMap["maoxian_125.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_125>();
    m_regularCreatorMap["maoxian_225.dgn"] = CnNew TmplRegularCreator<RegularMaoXian_225>();

    // 异界地图
    m_regularCreatorMap["yijie_001.dgn"] = CnNew TmplRegularCreator<RegularYiJie_001>();        // 异界地图001
    m_regularCreatorMap["yijie_002.dgn"] = CnNew TmplRegularCreator<RegularYiJie_002>();        // 异界地图002
    m_regularCreatorMap["yijie_003.dgn"] = CnNew TmplRegularCreator<RegularYiJie_003>();        // 异界地图003
    m_regularCreatorMap["yijie_004.dgn"] = CnNew TmplRegularCreator<RegularYiJie_004>();        // 异界地图004
    m_regularCreatorMap["yijie_005.dgn"] = CnNew TmplRegularCreator<RegularYiJie_005>();        // 异界地图005
    m_regularCreatorMap["yijie_006.dgn"] = CnNew TmplRegularCreator<RegularYiJie_006>();        // 异界地图006
    m_regularCreatorMap["yijie_007.dgn"] = CnNew TmplRegularCreator<RegularYiJie_007>();        // 异界地图007
    m_regularCreatorMap["yijie_008.dgn"] = CnNew TmplRegularCreator<RegularYiJie_008>();        // 异界地图008
    m_regularCreatorMap["yijie_009.dgn"] = CnNew TmplRegularCreator<RegularYiJie_009>();        // 异界地图009
    m_regularCreatorMap["yijie_010.dgn"] = CnNew TmplRegularCreator<RegularYiJie_010>();        // 异界地图010
    m_regularCreatorMap["yijie_011.dgn"] = CnNew TmplRegularCreator<RegularYiJie_011>();        // 异界地图011
    m_regularCreatorMap["yijie_012.dgn"] = CnNew TmplRegularCreator<RegularYiJie_012>();        // 异界地图012
    m_regularCreatorMap["yijie_013.dgn"] = CnNew TmplRegularCreator<RegularYiJie_013>();        // 异界地图013
    m_regularCreatorMap["yijie_014.dgn"] = CnNew TmplRegularCreator<RegularYiJie_014>();        // 异界地图014
    m_regularCreatorMap["yijie_015.dgn"] = CnNew TmplRegularCreator<RegularYiJie_015>();        // 异界地图015
    m_regularCreatorMap["yijie_016.dgn"] = CnNew TmplRegularCreator<RegularYiJie_016>();        // 异界地图016
    m_regularCreatorMap["yijie_017.dgn"] = CnNew TmplRegularCreator<RegularYiJie_017>();        // 异界地图017
    m_regularCreatorMap["yijie_018.dgn"] = CnNew TmplRegularCreator<RegularYiJie_018>();        // 异界地图018
    m_regularCreatorMap["yijie_019.dgn"] = CnNew TmplRegularCreator<RegularYiJie_019>();        // 异界地图019
    m_regularCreatorMap["yijie_020.dgn"] = CnNew TmplRegularCreator<RegularYiJie_020>();        // 异界地图020
    m_regularCreatorMap["yijie_021.dgn"] = CnNew TmplRegularCreator<RegularYiJie_021>();        // 异界地图021
    m_regularCreatorMap["yijie_022.dgn"] = CnNew TmplRegularCreator<RegularYiJie_022>();        // 异界地图022
    m_regularCreatorMap["yijie_023.dgn"] = CnNew TmplRegularCreator<RegularYiJie_023>();        // 异界地图023
    m_regularCreatorMap["yijie_024.dgn"] = CnNew TmplRegularCreator<RegularYiJie_024>();        // 异界地图024
    m_regularCreatorMap["yijie_025.dgn"] = CnNew TmplRegularCreator<RegularYiJie_025>();        // 异界地图025
    m_regularCreatorMap["yijie_026.dgn"] = CnNew TmplRegularCreator<RegularYiJie_026>();        // 异界地图026
    m_regularCreatorMap["yijie_027.dgn"] = CnNew TmplRegularCreator<RegularYiJie_027>();        // 异界地图027
    m_regularCreatorMap["yijie_028.dgn"] = CnNew TmplRegularCreator<RegularYiJie_028>();        // 异界地图028
    m_regularCreatorMap["yijie_029.dgn"] = CnNew TmplRegularCreator<RegularYiJie_029>();        // 异界地图029
    m_regularCreatorMap["yijie_030.dgn"] = CnNew TmplRegularCreator<RegularYiJie_030>();        // 异界地图030
    m_regularCreatorMap["yijie_031.dgn"] = CnNew TmplRegularCreator<RegularYiJie_031>();        // 异界地图031
    m_regularCreatorMap["yijie_032.dgn"] = CnNew TmplRegularCreator<RegularYiJie_032>();        // 异界地图032
    m_regularCreatorMap["yijie_033.dgn"] = CnNew TmplRegularCreator<RegularYiJie_033>();        // 异界地图033
    m_regularCreatorMap["yijie_034.dgn"] = CnNew TmplRegularCreator<RegularYiJie_034>();        // 异界地图034
    m_regularCreatorMap["yijie_035.dgn"] = CnNew TmplRegularCreator<RegularYiJie_035>();        // 异界地图035
    m_regularCreatorMap["yijie_036.dgn"] = CnNew TmplRegularCreator<RegularYiJie_036>();        // 异界地图036
    m_regularCreatorMap["yijie_037.dgn"] = CnNew TmplRegularCreator<RegularYiJie_037>();        // 异界地图037
    m_regularCreatorMap["yijie_038.dgn"] = CnNew TmplRegularCreator<RegularYiJie_038>();        // 异界地图038
    m_regularCreatorMap["yijie_039.dgn"] = CnNew TmplRegularCreator<RegularYiJie_039>();        // 异界地图039
    m_regularCreatorMap["yijie_040.dgn"] = CnNew TmplRegularCreator<RegularYiJie_040>();        // 异界地图040

    // 日常地图
    m_regularCreatorMap["daily_001.dgn"] = CnNew TmplRegularCreator<RegularDaily_001>();        // 日常地图001
    m_regularCreatorMap["daily_002.dgn"] = CnNew TmplRegularCreator<RegularDaily_002>();        // 日常地图002
    m_regularCreatorMap["daily_003.dgn"] = CnNew TmplRegularCreator<RegularDaily_003>();        // 日常地图003
    m_regularCreatorMap["daily_004.dgn"] = CnNew TmplRegularCreator<RegularDaily_004>();        // 日常地图004
    m_regularCreatorMap["daily_005.dgn"] = CnNew TmplRegularCreator<RegularDaily_005>();        // 日常地图005
    m_regularCreatorMap["daily_006.dgn"] = CnNew TmplRegularCreator<RegularDaily_006>();        // 日常地图006
    m_regularCreatorMap["daily_007.dgn"] = CnNew TmplRegularCreator<RegularDaily_007>();        // 日常地图007
    m_regularCreatorMap["daily_008.dgn"] = CnNew TmplRegularCreator<RegularDaily_008>();        // 日常地图008
}

RegularManager::~RegularManager()
{
    RegularDocMap::iterator docIte, docIteEnd = m_regularDocMap.end();
    for(docIte = m_regularDocMap.begin(); docIte != docIteEnd; ++docIte)
    {
        CnDelete docIte->second;
    }
    m_regularDocMap.clear();

    RegularCreatorMap::iterator creatorIte, creatorIteEnd = m_regularCreatorMap.end();
    for (creatorIte = m_regularCreatorMap.begin(); creatorIte != creatorIteEnd; ++creatorIte)
    {
        CnDelete creatorIte->second;
    }
    m_regularCreatorMap.clear();
}

bool RegularManager::Init()
{
    return true;
}

Regular* RegularManager::CreateRegular(const char* xmlFileName)
{
    // 读入关卡文件
    RegularDoc* pRegularDoc = GetRegularDoc(xmlFileName);
    if(!pRegularDoc)
    {
        CnFatal("dgn %s not exist\n", xmlFileName);
        return NULL;
    }

    std::string designPath = RES_DESIGN_PATH;
    std::string fileName = pRegularDoc->m_fileName.substr(designPath.length());     // 关卡文件名

    // 创建关卡
    Regular* pNewRegular = CreateRegular(fileName, pRegularDoc);
    pNewRegular->SetFileName(xmlFileName);

    return pNewRegular;
}

Regular* RegularManager::CreateRegular(const std::string &fileName, RegularDoc *pDoc)
{
    RegularCreatorMap::iterator ite = m_regularCreatorMap.find(fileName);
    if (ite != m_regularCreatorMap.end())
    {
        return ite->second->Create(pDoc);
    }
    else
    {
        CnAssert(false);
        CnError("regular %s need creator\n");
        return CnNew RegularSingle(pDoc);
    }
}

RegularDoc* RegularManager::GetRegularDoc(uint32 dungeonId, uint32 sceneId)
{
    // 通过unionID获取关卡文件名
    std::string designFileName = DungeonManager::Instance()->GetDesignFileName(dungeonId, sceneId);
    if(designFileName.length() == 0)
    {
        CnAssert(false);        
        return NULL;
    }
    
    return GetRegularDoc(designFileName);
}

RegularDoc* RegularManager::GetRegularDoc(const std::string& xmlFileName)
{
    RegularDocMap::iterator regularIt;

    regularIt = m_regularDocMap.find(xmlFileName);
    if(regularIt != m_regularDocMap.end())
    {
        return regularIt->second;
    }

    TiXmlDocument* pXmlDoc = CnNew TiXmlDocument(xmlFileName.c_str());

    if(!pXmlDoc->LoadFile())
    {
        CnDelete pXmlDoc;
        CnFatal("load %s failed\n", xmlFileName.c_str());
        return NULL;
    }

    RegularDoc* pDoc = CnNew RegularDoc;
    pDoc->m_fileName = xmlFileName;
    pDoc->m_pDoc = pXmlDoc;

    // 获取规则类型
    TiXmlElement* rootElement = pXmlDoc->RootElement();

    LoadRegularTemplate(rootElement, *pDoc);

    m_regularDocMap[pDoc->m_fileName] = pDoc;

    return pDoc;
}

void RegularManager::LoadRegularTemplate(TiXmlElement* xmlEle, RegularDoc& regularTmpl)
{
    // 读入type失败
    if(xmlEle->QueryIntAttribute("type", &regularTmpl.m_type) == TIXML_NO_ATTRIBUTE)
    {
        CnError("Can't read regular:%s type", regularTmpl.m_fileName.c_str());
    }

    // 读取关卡时间
    xmlEle->Attribute("ready_time", &regularTmpl.m_ReadyTime);
    xmlEle->Attribute("start_time", &regularTmpl.m_RunTime);
    xmlEle->Attribute("end_time", &regularTmpl.m_EndTime);

    // 读取关卡中的所有单位
    for(TiXmlElement* childElement = xmlEle->FirstChildElement(); childElement; childElement = childElement->NextSiblingElement())
    {
        if(strcmp(childElement->Value(), "regular") == 0)
        {
        }
        // 小怪和NPC
        else if(strcmp(childElement->Value(), "pawn") == 0)
        {
            LoadMonsterAndNpc(childElement, regularTmpl);
        }
        // BOSS
        else if(strcmp(childElement->Value(), "boss") == 0)
        {
            LoadBoss(childElement, regularTmpl);
        }
        // 触发器
        else if(strcmp(childElement->Value(), "trigger") == 0)
        {
            LoadTrigger(childElement, regularTmpl);
        }
        // 路点
        else if(strcmp(childElement->Value(), "pin") == 0)
        {
            LoadPin(childElement, regularTmpl);
        }
        // 出怪点
        else if(strcmp(childElement->Value(), "spawn") == 0)
        {
            LoadSpawn(childElement, regularTmpl);
        }
        // 采集点
        else if(strcmp(childElement->Value(), "touch") == 0)
        {
            LoadPatch(childElement, regularTmpl);
        }
    }
}

void RegularManager::LoadMonsterAndNpc(TiXmlElement* pawnElement, RegularDoc& regularTmpl)
{
    MonsterData monsterData = ParseMonsterAndNpcData(pawnElement);

    if(monsterData.creatureType == PAWN_TYPE_MONSTER)
    {
        regularTmpl.m_monsterList.push_back(monsterData);
    }
    else if(monsterData.creatureType == PAWN_TYPE_NPC)
    {
        regularTmpl.m_npcList.push_back(monsterData);
    }
    else
    {
        CnAssert(false);
    }
}

void RegularManager::LoadBoss(TiXmlElement* bossElement, RegularDoc& regularTmpl)
{
    BossData bossData = ParseBossData(bossElement);
    regularTmpl.m_bossList.push_back(bossData);
}

void RegularManager::LoadTrigger(TiXmlElement* triggerElement, RegularDoc& regularTmpl)
{
    TriggerData triggerData = ParseTriggerData(triggerElement);

    // 确保ID不重复
    for(RegularDoc::TriggerDataMap::iterator iter = regularTmpl.m_triggerMap.begin(); iter != regularTmpl.m_triggerMap.end(); iter++)
    {
        if(iter->second.triggerId == triggerData.triggerId)
        {
            CnError("%s trigger id %d is duplicate\n", regularTmpl.m_fileName.c_str(), triggerData.triggerId);
        }
    }

    regularTmpl.m_triggerMap[triggerData.name] = triggerData;
}

void RegularManager::LoadPin(TiXmlElement* pinElement, RegularDoc& regularTmpl)
{
    CheckPointData pinData = ParseCheckPointData(pinElement);

    // 确保ID不重复
    for(RegularDoc::CheckPointDataMap::iterator iter = regularTmpl.m_pinMap.begin(); iter != regularTmpl.m_pinMap.end(); iter++)
    {
        if(iter->second.pinID == pinData.pinID)
        {
            CnError("%s checkpoint id %d is duplicate\n", regularTmpl.m_fileName.c_str(), pinData.pinID);
        }
    }

    regularTmpl.m_pinMap[pinData.name] = pinData;
}

void RegularManager::LoadSpawn(TiXmlElement* spawnElement, RegularDoc& regularTmpl)
{
    SpawnData spawnData = ParseSpawnData(spawnElement);
    regularTmpl.m_spawnMap[spawnData.name] = spawnData;
}

void RegularManager::LoadPatch(TiXmlElement* touchElement, RegularDoc& regularTmpl)
{
    TouchData touchData = ParseTouchData(touchElement);
    regularTmpl.m_touchMap[touchData.name] = touchData;
}
