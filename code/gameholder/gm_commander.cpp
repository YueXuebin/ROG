#include "gameholder_pch.h"
#include "gm_commander.h"
#include "player.h"
#include "scene.h"
#include "regular_dungeon.h"
#include "message_encoder.h"
#include "monster.h"
#include "boss.h"
#include "event_define.h"
#include "player_manager.h"
#include "ai_controller.h"
#include "equip_manager.h"
#include "item_manager.h"
#include "task_manager.h"
#include "babel_manager.h"
#include "sign_manager.h"
#include "center_handler.h"
#include "activity_manager.h"
#include "ride_manager.h"
#include "shop_manager.h"
#include "title_manager.h"
#include "ranking_manager.h"
#include "dungeon_manager.h"
#include "investment_manager.h"
#include "passive_skill_manager.h"
#include "trade_manager.h"
#include "guild_manager.h"
#include "wing_manager.h"
#include "horoscope_manager.h"
#include "activity_manager.h"
#include "soul_dungeon_manager.h"
#include "ridetrain.h"
#include "fashion_manager.h"
#include "zombie_manager.h"
#include "drop_object.h"
#include "multiserver_battle_info_manager.h"
#include "shorcut_manager.h"
#include "top_up_rank_manager.h"
#include "egg_manager.h"
#include "property_set_calculater.h"
#include "lua_engine.h"
#include "formula_manager.h"
#include "game_util.h"
#include "player_dungeon_manager.h"
#include "dungeon.h"
#include "bag_manager.h"
#include "monster_config_manager.h"
#include "pet_manager.h"
#include "soldier.h"
#include "system_config_manager.h"


IMPLEMENT_SINGLETON(GmCommander)

GmCommander::GmCommander()
{
    // 命令调用
    m_gmCmdMap["adddropobject"]=&GmCommander::AddDropOjbect;
    m_gmCmdMap["addmonster"]=&GmCommander::AddMonster;
    m_gmCmdMap["addboss"]=&GmCommander::AddBoss;
    m_gmCmdMap["addofflineplayer"]=&GmCommander::AddOfflinePlayer;
    m_gmCmdMap["killmonster"]=&GmCommander::KillMonster;
    m_gmCmdMap["killboss"]=&GmCommander::KillBoss;
    m_gmCmdMap["killall"]=&GmCommander::KillAll;
    m_gmCmdMap["askbattleunitproperty"]=&GmCommander::AskBattleUnitProperty;
    m_gmCmdMap["GetBableRewrd"]=&GmCommander::GetBableRewrd;
    m_gmCmdMap["aistop"]=&GmCommander::AiStop;
    m_gmCmdMap["aistoppet"]=&GmCommander::AiStopPet;
    m_gmCmdMap["damageme"]=&GmCommander::DamageMe;
    m_gmCmdMap["damage"]=&GmCommander::Damage;
    m_gmCmdMap["nodamage"]=&GmCommander::NoDamage;
    m_gmCmdMap["fullhpmp"]=&GmCommander::FullHpMp;
    m_gmCmdMap["updateactivity"]=&GmCommander::UpdateActivity;
    m_gmCmdMap["addactivity"]=&GmCommander::AddActivity;
    m_gmCmdMap["deletectivity"]=&GmCommander::DeleteActivity;
    m_gmCmdMap["setcareer"]=&GmCommander::SetCareer;
    m_gmCmdMap["setgroup"]=&GmCommander::SetGroup;
    m_gmCmdMap["addexp"]=&GmCommander::AddExp;
    m_gmCmdMap["levelup"]=&GmCommander::LevelUp;
    m_gmCmdMap["setlevel"]=&GmCommander::SetLevel;
    m_gmCmdMap["addpassiveskillpoint"]=&GmCommander::AddPassiveSkillPoint;
    m_gmCmdMap["resetpassiveskillpoint"]=&GmCommander::ResetPassiveSkillPoint;
    m_gmCmdMap["setgamepoint"]=&GmCommander::SetGamePoint;
    m_gmCmdMap["addgamepoint"]=&GmCommander::AddGamePoint;
    m_gmCmdMap["setmoneypoint"]=&GmCommander::SetMoneyPoint;
    m_gmCmdMap["addmoneypoint"]=&GmCommander::AddMoneyPoint;
    m_gmCmdMap["addbindmoneypoint"]=&GmCommander::AddBindMoneyPoint;
    m_gmCmdMap["setsoulpoint"]=&GmCommander::SetSoulPoint;
    m_gmCmdMap["addsoulpoint"]=&GmCommander::AddSoulPoint;
    m_gmCmdMap["sethonorpoint"]=&GmCommander::SetHonorPoint;
    m_gmCmdMap["addhonorpoint"]=&GmCommander::AddHonorPoint;
    m_gmCmdMap["setguildpoint"]=&GmCommander::SetGuildPoint;
    m_gmCmdMap["addguildpoint"]=&GmCommander::AddGuildPoint;
    m_gmCmdMap["addguildexp"]=&GmCommander::AddGuildExp;
    m_gmCmdMap["setvigor"]=&GmCommander::SetVigor;
    m_gmCmdMap["addvigor"]=&GmCommander::AddVigor;
    m_gmCmdMap["additem"]=&GmCommander::AddItem;

    m_gmCmdMap["addrecharge"] = &GmCommander::AddRecharge;          //添加充值测试，这个值要走gmt的

    m_gmCmdMap["clearbag"]=&GmCommander::ClearBag;
    m_gmCmdMap["delitembyid"]=&GmCommander::DelItemByID;
    m_gmCmdMap["delitembypos"]=&GmCommander::DelItemByPos;
    m_gmCmdMap["moveitem"]=&GmCommander:: MoveItem;
    m_gmCmdMap["addfollowpet"]=&GmCommander::AddFollowPet;
    m_gmCmdMap["followleveupexp"]=&GmCommander::RanomLeveupExp;
    m_gmCmdMap["followpetswallow"]=&GmCommander::OnFollowPetSwallow;
    m_gmCmdMap["addprotectsoul"]=&GmCommander::AddProtectSoul;
    m_gmCmdMap["addfashion"]=&GmCommander::AddFashion;
    m_gmCmdMap["fashioncompose"]=&GmCommander::FashiionCompose;
    m_gmCmdMap["equipopenslot"]=&GmCommander::EquipOpenSlot;
    m_gmCmdMap["upequiplevel"]=&GmCommander::UpEquipLevel;
    m_gmCmdMap["upequipstar"]=&GmCommander::UpEquipStar;
    m_gmCmdMap["upequipquality"]=&GmCommander::UpEqupQuality;
    m_gmCmdMap["finishequipstar"]=&GmCommander::FinishEquipStar;
    m_gmCmdMap["synthetizeJewel"]=&GmCommander::OnSynthetizeJewelReq;
    m_gmCmdMap["upgradejewel"]=&GmCommander::OnUpgradeJewelReq;
    m_gmCmdMap["equipInlaidjewel"]=&GmCommander::EquipInlaidJewel;
    m_gmCmdMap["tasktalk"]=&GmCommander::TaskTalk;
    m_gmCmdMap["taskkillmonster"]=&GmCommander::TaskKillMonster;
    m_gmCmdMap["addtask"]=&GmCommander::AddTask;
    m_gmCmdMap["taskcomplete"]=&GmCommander::TaskComplete;
    m_gmCmdMap["taketask"]=&GmCommander::TakeTask;
    m_gmCmdMap["taskchangedungeon"]=&GmCommander::TaskChangeDungeon;
    m_gmCmdMap["taskreward"]=&GmCommander::TaskRewards;
    m_gmCmdMap["taskquality"]=&GmCommander::TaskQuality;
    m_gmCmdMap["taskonecomplete"]=&GmCommander::TaskOneComplete;
    m_gmCmdMap["taskrefresh"]=&GmCommander::TaskRefresh;
    m_gmCmdMap["clearDungeon"]=&GmCommander::OnDungeonClear;
    m_gmCmdMap["taskreceivebox"]=&GmCommander::TaskReceiveBox;
    m_gmCmdMap["taskrefreshtime"]=&GmCommander::TaskreFreshTime;
    m_gmCmdMap["taskguardrefresh"]=&GmCommander::TaskGuardRefresh;
    m_gmCmdMap["chatsystem"]=&GmCommander::ChatSystem;
    m_gmCmdMap["chatmarquee"]=&GmCommander::ChatMarquee;
    m_gmCmdMap["sign"]=&GmCommander::Sign;
    m_gmCmdMap["isgamestart"]=&GmCommander::IsGameStart;
    m_gmCmdMap["racingrefresh"]=&GmCommander::RacingRefresh;
    m_gmCmdMap["racingenter"]=&GmCommander::RacingEnter;
    m_gmCmdMap["racingreward"]=&GmCommander::RacingReward;
    m_gmCmdMap["addridepet"]=&GmCommander::OnAddRide;
    m_gmCmdMap["starttrain"]=&GmCommander::StartTrain;
    m_gmCmdMap["endtrain"]=&GmCommander::EndTrain;
    m_gmCmdMap["opentrainpos"]=&GmCommander::OpenTrainPos;
    m_gmCmdMap["settrainmodels"]=&GmCommander::SetTrainModels;
    m_gmCmdMap["rightnowtrain"]=&GmCommander::RightNowTrain;
    m_gmCmdMap["minmarkride"]=&GmCommander::MinMarkRide;
    m_gmCmdMap["traceplayerlist"]=&GmCommander::TracePlayerList;
    m_gmCmdMap["traceplayernum"]=&GmCommander::TracePlayerNum;
    m_gmCmdMap["reloadmall"]=&GmCommander::ReloadMall;
    m_gmCmdMap["mallbuy"]=&GmCommander::MallBuyReq;
    m_gmCmdMap["bindmoneypointbuy"]=&GmCommander::BindMoneyPointBuyReq;
    m_gmCmdMap["mallinfo"]=&GmCommander::MallInfoReq;
    m_gmCmdMap["setgrowthpoint"]=&GmCommander::SetGrowthPoint;
    m_gmCmdMap["addgrowthpoint"]=&GmCommander::AddGrowthPoint;
    m_gmCmdMap["setvipcard"]=&GmCommander::SetVipCard;
    m_gmCmdMap["activeexperiencevip"]=&GmCommander::ActiveExperienceVip;
    m_gmCmdMap["setfcmflag"]=&GmCommander::SetFcmFlag;
    m_gmCmdMap["setfcmingame"]=&GmCommander::SetFcmInGameTime;
    m_gmCmdMap["setfcmoutgame"]=&GmCommander::SetFcmOutGameTime;
    m_gmCmdMap["setgmright"]=&GmCommander::SetGMRight;
    m_gmCmdMap["addtitle"]=&GmCommander::AddTitle;
    m_gmCmdMap["equiptitle"]=&GmCommander::EquipTitle;
    m_gmCmdMap["refreshranking"]=&GmCommander::RefreshRanking;
    m_gmCmdMap["useskill"]=&GmCommander::useskill;
    m_gmCmdMap["followevoluti"]=&GmCommander::OnFollowEvoluti;
    m_gmCmdMap["dayinvestment"]=&GmCommander::DayInvestment;
    m_gmCmdMap["levelinvestment"]=&GmCommander::LevelInvestment;
    m_gmCmdMap["investmentmanager"]=&GmCommander::InvestmentManager;
    m_gmCmdMap["acceptlivenessreward"]=&GmCommander::AcceptLivenessReward;
    m_gmCmdMap["levelactivity"]=&GmCommander::LevelActivity;
    m_gmCmdMap["specialactivity"]=&GmCommander::SpecialActivity;
    m_gmCmdMap["addbattlesoul"]=&GmCommander::AddBattleSoul;
    m_gmCmdMap["raidsmonsterdoor"]=&GmCommander::RaidsMonsterDoor;
    m_gmCmdMap["setguildbosstime"]=&GmCommander::SetGuildBossTime;
    m_gmCmdMap["rideabsorb"]=&GmCommander::RideAbsorb;
    m_gmCmdMap["levelupwing"]=&GmCommander::onAdvanceReq;
    m_gmCmdMap["activewing"]=&GmCommander::onActivateReq;
    m_gmCmdMap["horoscopelevelup"]=&GmCommander::OnHoroscopeLevelUpReq;
    m_gmCmdMap["lockspendcondition"]=&GmCommander::LockSpendCondition;
    m_gmCmdMap["addstarpoint"]=&GmCommander::AddStarPoint;
    m_gmCmdMap["setstarpoint"]=&GmCommander::SetStarPoint;
    m_gmCmdMap["entercityfight"]=&GmCommander::EnterCityFight;
    m_gmCmdMap["cityfightcityinfo"]=&GmCommander::CityFightCityInfo;
    m_gmCmdMap["citysurvival"]=&GmCommander::CitySurvival;
    m_gmCmdMap["calcplayerprop"]=&GmCommander::CalcPlayerProp;
    m_gmCmdMap["startsoulrains"]=&GmCommander::StartSoulRains;
    m_gmCmdMap["finishsoulrains"]=&GmCommander::FinishSoulRains;
    m_gmCmdMap["stopsoulrains"]=&GmCommander::StopSoulRains;
    m_gmCmdMap["opendungeon"]=&GmCommander::OpenDungeon;
    m_gmCmdMap["cleardungeon"]=&GmCommander::ClearDungeon;

    m_gmCmdMap["setmaxhp"]          = &GmCommander::SetMaxHp;
    m_gmCmdMap["setmaxmp"]          = &GmCommander::SetMaxMp;
    m_gmCmdMap["setreshp"]          = &GmCommander::SetResHp;
    m_gmCmdMap["setresmp"]          = &GmCommander::SetResMp;
    m_gmCmdMap["setattack"]         = &GmCommander::SetAttack;
    m_gmCmdMap["setdefence"]        = &GmCommander::SetDefence;
    m_gmCmdMap["setdodge"]          = &GmCommander::SetDodge;
    m_gmCmdMap["setenergyshield"] = &GmCommander::SetEnergyShield;
    m_gmCmdMap["sethitrate"]        = &GmCommander::SetHitRate;
    m_gmCmdMap["setcriticalattack"] = &GmCommander::SetCriticalAttack;
    m_gmCmdMap["setcriticaldamageattack"]   = &GmCommander::SetCriticalDamageAttack;
    m_gmCmdMap["setattackspeed"] = &GmCommander::SetAttackSpeed;
    m_gmCmdMap["setcastspeed"] = &GmCommander::SetCastSpeed;
    m_gmCmdMap["setmovespeed"] = &GmCommander::SetMoveSpeed;
    m_gmCmdMap["setfiredefence"]    = &GmCommander::SetFireDefence;
    m_gmCmdMap["seticedefence"]     = &GmCommander::SetIceDefence;
    m_gmCmdMap["setthunderdefence"] = &GmCommander::SetThunderDefence;
    m_gmCmdMap["setblock"] = &GmCommander::SetBlock;
    m_gmCmdMap["setmagicblock"] = &GmCommander::SetMagicBlock;

    m_gmCmdMap["setcriticalred"]    = &GmCommander::SetCriticalRed;
    m_gmCmdMap["setcriticaldamagered"]      = &GmCommander::SetCriticalDamageDef;


    m_gmCmdMap["singleanswerinti"]      = &GmCommander::CalSingleAnswerInt;
    m_gmCmdMap["startquestion"] = &GmCommander::CalSingleAnswerQuesitonReq;
    m_gmCmdMap["startresult"] = &GmCommander::CalSingleAnswerResultReq;
    m_gmCmdMap["answercd"] =&GmCommander::CalSingleAnswerTimeCDReq;
    m_gmCmdMap["getvipgit"] =&GmCommander::GetVipGiftReq;
    m_gmCmdMap["reqzombieinfo"] = &GmCommander::OnRequestZombieInfo;

    m_gmCmdMap["setmultiserverintegral"]=&GmCommander::SetMultiserverIntegralPoint;
    m_gmCmdMap["setmultiserverexploit"]=&GmCommander::SetMultiserverExploitPoint;
    m_gmCmdMap["addcrossserver"]=&GmCommander::AddCrossServer;
    m_gmCmdMap["addmultiserveralliance"]=&GmCommander::SetMultiserverAlliancePoint;
    m_gmCmdMap["addmultiserverseason"]=&GmCommander::SetMultiserverSeasonPoint;
    m_gmCmdMap["skillfashion"]=&GmCommander::SetSkillFashion;
    m_gmCmdMap["like"]=&GmCommander::SetLikeInfo;

    m_gmCmdMap["activityrank"]=&GmCommander::SetActivityRank;
    m_gmCmdMap["egg"]=&GmCommander::SetEggInfo;
    m_gmCmdMap["reloadlua"]=&GmCommander::ReloadLua;
    m_gmCmdMap["resetfestival"]=&GmCommander::SetResetFestival;
    m_gmCmdMap["formula"]=&GmCommander::SetFormula;

}

GmCommander::~GmCommander()
{

}

void GmCommander::ParseGMCommand( Player* player, const std::string& cmd )
{
    Player*             who = 0;
    const char* cmdString   = 0;
    int32 cmdStringLen      = 0;
    GMFuncMap::iterator funcIt;
    char token[MAX_CHAT_TOKEN_LEN];
    std::list<std::string> cmdSplitStrings;

    if((player->GetRightFlag() & RIGHT_FLAG_GM) == 0)
    {
        return;
    }

    cmdString = cmd.c_str();
    cmdStringLen = cmd.length();

    for(uint32 idx = 0; idx < cmd.length();)
    {
        // 去前空格
        for(; (idx < cmd.length()) && (cmd.at(idx) == ' '); ++idx);

        memset(token, 0, MAX_CHAT_TOKEN_LEN);
        for(uint32 i = 0; (idx < cmd.length()) && (cmd.at(idx) != ' ') && (i < (MAX_CHAT_TOKEN_LEN-1)); ++i, ++idx)
        {
            token[i] = cmd.at(idx);
        }

        cmdSplitStrings.push_back(token);
    }

    // 命令解析
    std::string arg0;
    std::string arg1;

    if(cmdSplitStrings.size() < 2)
    {
        goto error;
    }

    arg0 = cmdSplitStrings.front();
    cmdSplitStrings.pop_front();

    arg1 = cmdSplitStrings.front();
    cmdSplitStrings.pop_front();

    // 判断权利
    // player

    if(arg0 == "#")
    {
        who = player;
    }
    else
    {
        who = PlayerManager::Instance()->GetPlayerByName(arg1);
        if(!who)
        {
            goto error;
        }
    }

    funcIt = m_gmCmdMap.find(arg1);
    if(funcIt != m_gmCmdMap.end())
    {
        (this->*(funcIt->second))(who, cmdSplitStrings);
    }
    else if (arg1 == "crash")
    {
        CnDbg("gm call crash\n");
        //#ifdef DEBUG        // crash server
        int32* p = 0;
        *p = 10;
        //#endif
    }
    else if(arg1 == "throw")
    {
        //#ifdef DEBUG        // crash server
        throw "throw";
        //#endif
    }
    else
    {
        ParseGMCommand2(who, arg1, cmdSplitStrings);
    }

    return;

error:
    return;
}

void GmCommander::ParseGMCommand2(Player* who, const std::string& cmd, std::list<std::string>& cmdSplitStrings)
{
    if(cmd == "calcplayerprop")
    {
        CalcPlayerProp(who, cmdSplitStrings);
    }
    else
    {
        CnDbg("gm command %s no exist\n", cmd.c_str());
    }
}

void GmCommander::StartSoulRains(Player* who, std::list<std::string>& arglist)
{
    //if(!who) return;
    //if(arglist.size()<1) return;

    //std::string arg1 = arglist.front();
    //arglist.pop_front();
    //uint32 id1 = Crown::SDAtou(arg1.c_str());

    //std::string arg2 = arglist.front();
    //arglist.pop_front();
    //int32 numstr = Crown::SDAtou(arg2.c_str());

    //SOUL_RADIS_DUNGEON_START starnum;
    //starnum.dungeon_id = id1;
    //starnum.num  = numstr;

    //SOUL_RAIDS_DUNGEON_START_REQ recvMsg;
    //recvMsg.soul_dungeon_list.push_back(starnum);
    //recvMsg.dungeon_num = 1 ;
    //who->GetSoulRadisMangaer()->StartSoulRains(&recvMsg);
}

void GmCommander::StopSoulRains(Player* who, std::list<std::string>& arglist)
{
    //if(!who) return;
    //NONE recvMsg;
    //who->GetSoulRadisMangaer()->StopSoulRains();
}
void GmCommander::FinishSoulRains(Player* who, std::list<std::string>& arglist)
{
    //if(!who) return;
    //NONE recvMsg;
    //who->GetSoulRadisMangaer()->FinishSoulRains(&recvMsg);
}
int32 StringToInt(const std::string& str)
{
    return SDAtoi(str.c_str());
}

void GmCommander::OpenTrainPos(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 fieldid = Crown::SDAtou(arg1.c_str());

    TRAIN_POST_REQ req;
    req.id = fieldid;
    // who->GetPetManager()->GetRideManager()->SetTrainField(&req);
}

void GmCommander::UpEquipLevel(Player* who, std::list<std::string>& arglist)
{
    //     if(!who) return;
    //     if(arglist.size()<1) return;
    // 
    //     std::string arg1 = arglist.front();
    //     //if(SDIsDigit(arg1.c_str())) return;
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     OPT_EQUIP_UP recvMsg;
    //     recvMsg.type = type;
    //     who->m_equip_mgr->OnEquipLevelUp(&recvMsg);
}
void GmCommander::RideAbsorb(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 petid = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 petid2 = 0;
    petid2 = Crown::SDAtou(arg2.c_str());

    std::string arg3 = arglist.front();
    arglist.pop_front();
    uint32 petid3 = 0;
    petid3 = Crown::SDAtou(arg3.c_str());


    std::string arg4 = arglist.front();
    arglist.pop_front();
    uint32 petid4 = 0;
    petid4 = Crown::SDAtou(arg4.c_str());

    std::string arg5 = arglist.front();
    arglist.pop_front();
    int32 type = 0;
    type = Crown::SDAtou(arg5.c_str());

    RIDE_PET_ABSORB_REQ absorb;
    absorb.pet_id = petid;
    absorb.choose_id1 = petid2;
    absorb.choose_id2 = petid3;
    absorb.choose_id3 = petid4;
    absorb.type = type;
    who->GetRideManager()->RideAbsorb(&absorb);

}

void GmCommander::UpdateRide(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 type = Crown::SDAtou(arg1.c_str());

    RIDE_PET_UPDATE_REQ rideupdate;
    rideupdate.type = type;
    who->GetRideManager()->GetUpdateRide(&rideupdate);
}
void GmCommander::CatchRide(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 petid = Crown::SDAtou(arg1.c_str());

    SET_RIDE_PET_REQ ridereq;
    ridereq.pet_id = petid;
    who->GetRideManager()->CaptureRide(&ridereq);
}
void GmCommander::RideSendGif(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 petid = Crown::SDAtou(arg1.c_str());

    SET_RIDE_PET_REQ ridereq;
    ridereq.pet_id = petid;
    who->GetRideManager()->SendGif(&ridereq);
}
void GmCommander::StartTrain(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()< 3) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 fieldid = Crown::SDAtou(arg1.c_str());


    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 pet_id = Crown::SDAtou(arg2.c_str());


    std::string arg3= arglist.front();
    arglist.pop_front();
    int32 itempos = Crown::SDAtou(arg3.c_str());

    STONE_TRAIN_REQ recvMsg ;
    recvMsg.id = fieldid;
    recvMsg.pet_id = pet_id;
    // recvMsg.item_pos = itempos;
    // who->GetPetManager()->GetRideManager()->StarTrain(&recvMsg);
}
void GmCommander::RightNowTrain(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()< 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 fieldid = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 petid = Crown::SDAtou(arg2.c_str());


    NOW_TRAIN_REQ recvMsg ;
    recvMsg.id = fieldid;
    recvMsg.pet_id = petid;
    // who->GetPetManager()->GetRideManager()->RightNowTrain(&recvMsg);
}

void GmCommander::SetTrainModels(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()< 3) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 fieldid = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 modelid = Crown::SDAtou(arg2.c_str());



    std::string arg3 = arglist.front();
    arglist.pop_front();
    uint32 petid = Crown::SDAtou(arg3.c_str());

    RIDE_PET_SET_MODELS_REQ recvMsg ;
    recvMsg.fieldid = fieldid;
    recvMsg.modeid = modelid;
    recvMsg.petid = petid;
    who->GetRideManager()->GetRideTrain()->SetTrainModels(&recvMsg);
}
void GmCommander::EndTrain(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()< 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 fieldid = Crown::SDAtou(arg1.c_str());

    RIDE_TRAIN_REQ recvMsg ;
    recvMsg.field_id = fieldid;
    // who->GetPetManager()->GetRideManager()->EndTrain(&recvMsg);

}
void GmCommander::AddMonster(Player* who, std::list<std::string>& arglist)
{
    int32 pawnId = 3001;
    real ra = Crown::SDRandomFloat((real)M_PI*2);
    int32 x = int32( who->GetX() + Crown::SDRandom(300) * cos(ra) );
    int32 y = int32( who->GetY() + Crown::SDRandom(300) * sin(ra) );
    int32 angle = Crown::SDRandom(360);
    int32 level = Crown::SDMax(who->GetScene()->GetRegular()->GetMapLevel(), 1);
    int32 quality = QUALITY_WHITE;
    bool bDungeonAssociate = false;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            pawnId = StringToInt(arg);
        else if(argCount == 2)
            quality = StringToInt(arg);
        else if(argCount == 3)
            level = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }

    level = Crown::SDMax(1, level);
    quality = Crown::SDMax(QUALITY_WHITE, Crown::SDMin(QUALITY_GOLD, quality));

    CoVec2 desPos = who->GetScene()->GetCoWorld()->CircleFind(CoVec2(x,y), 500.f, (real)who->GetRadius());

    MonsterData data;
    data.creatureType = 1;
    data.creatureId = pawnId;
    data.x = (int32)desPos.x;
    data.y = (int32)desPos.y;
    data.angle = angle;
    data.level = level;
    data.quality = quality;

    const MONSTER_ATTRIBUTE_REVISE_ROOT_MONSTER_STRUCT* monsterStruct = MonsterConfigManager::Instance()->GetMonsterAttributeReviseById(pawnId);
    if(!monsterStruct)
        return;

    if(monsterStruct->movie_name.length()>0 && monsterStruct->movie_time>0)
    {
        data.state = STATE_MOVIE;
        data.state_arg = monsterStruct->movie_time;
        data.movie_name = monsterStruct->movie_name;
    }

    Monster* pMonster = who->GetScene()->GetRegular()->AddMonster(data);
}

void GmCommander::AddOfflinePlayer(Player* who, std::list<std::string>& arglist)
{
    if(arglist.size() < 1)
        return;

    real ra = Crown::SDRandomFloat((real)M_PI*2);
    int32 x = int32( who->GetX() + Crown::SDRandom(300) * cos(ra) );
    int32 y = int32( who->GetY() + Crown::SDRandom(300) * sin(ra) );
    int32 angle = Crown::SDRandom(360);

    std::string name = arglist.front();
    arglist.pop_front();

    who->GetScene()->GetRegular()->CreateOfflinePlayerByName(name, x, y, angle);
}

void GmCommander::OnFollowEvoluti(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 petid = SDAtoi(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    int32 pos = SDAtoi(arg2.c_str());

    FOLLOW_PET_EVOLUTI_REQ recvMsg ;
    recvMsg.pet_id = petid;
    // recvMsg.itempos = pos;

    //who->GetPetManager()->GetFollowPetManager()->OnFollowEvoluti(&recvMsg);

}
void GmCommander::AddBoss(Player* who, std::list<std::string>& arglist)
{
    int32 pawnId = 30007;
    real ra = Crown::SDRandomFloat((real)M_PI*2);
    int32 x = int32( who->GetX() + Crown::SDRandom(300) * cos(ra) );
    int32 y = int32( who->GetY() + Crown::SDRandom(300) * sin(ra) );
    int32 angle = Crown::SDRandom(360);
    bool bDungeonAssociate = false;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            pawnId = StringToInt(arg);
        else if(argCount == 2)
            x = StringToInt(arg);
        else if(argCount == 3)
            y = StringToInt(arg);
        else if(argCount == 4)
            angle = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }

    CoVec2 desPos = who->GetScene()->GetCoWorld()->CircleFind(CoVec2(x,y), 500.f, (real)who->GetRadius());

    Boss* pBoss = who->GetScene()->GetRegular()->AddBoss(pawnId, (int32)desPos.x, (int32)desPos.y, angle, false);
    if(!pBoss)
        return;
    // 给怪加AI
    pBoss->GetAIController()->LoadAI("ai/active_atk.aix");
}

void GmCommander::KillMonster(Player* who, std::list<std::string>& arglist)
{
    int32 killNumber = 1;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            killNumber = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }

    who->GetScene()->GetRegular()->KillMonster(killNumber, who);
}

void GmCommander::AddDropOjbect(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 id = SDAtoi(arg1.c_str());

    CoVec2 fromPos = who->GetPosition();

    RegularBuilder* regular = CnDynamicCast(RegularBuilder, who->GetScene()->GetRegular());
    if(!regular) return;

    RandomDropData data;
    std::vector<RandomDropData> data_list;

    int32 num = Crown::SDRandom(3, 8);          // 数量
    for (int32 i=0; i<num; ++i)
    {
        data.Clear();

        int32 typeID = id;
        if(typeID == 0)
            typeID = Crown::SDRandom(4)+1;

        if (typeID == 1)
        {
            data.firstType = ITEM_TYPE_EQUIP;
            data.secondType = Crown::SDRandom(EQUIP_SUB_TYPE_1, EQUIP_SUB_TYPE_14+1);
            data.quality = Crown::SDRandom(MAX_EQUIP_QUALITY);
        }
        else if(typeID == 2)
        {
            data.firstType = ITEM_TYPE_STONE;
            data.secondType = 0;//Crown::SDRandom(STONE_SUB_TYPE_SKILL, STONE_SUB_TYPE_GEM + 1);
        }
        else if (typeID == 3)
        {
            data.firstType = ITEM_TYPE_MATERIAL;
            data.secondType = 0;//Crown::SDRandom(MATERIAL_SUB_TYPE_MINERAL, MATERIAL_SUB_TYPE_ADD_HOLE + 1);
        }
        else if (typeID == 4)
        {
            data.firstType = ITEM_TYPE_CONSUME;
            data.secondType = 0;
        }
        else
            continue;
        data_list.push_back(data);
    }

    regular->DropItem(data_list, fromPos);
}

void GmCommander::AddTitle(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 titleId = SDAtoi(arg1.c_str());

    who->GetTitleManager()->AddTitle(titleId);
}

void GmCommander::EquipTitle(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 _titleid = SDAtoi(arg1.c_str());


    std::string arg4 = arglist.front();
    arglist.pop_front();
    int32 _have = SDAtoi(arg4.c_str());

    EQUIP_TITLE req;
    req.title_id = _titleid;
    req.b_have = _have; 
    who->GetTitleManager()->EquipTitle(&req);

}

void GmCommander::KillBoss(Player* who, std::list<std::string>& arglist)
{
    int32 killNumber = 1;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            killNumber = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }
    who->GetScene()->GetRegular()->KillBoss(killNumber, who);

}

void GmCommander::AiStop(Player* who, std::list<std::string>& arglist)
{
    bool stop = true;
    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            stop = (StringToInt(arg) != 0);

        argCount++;
        arglist.pop_front();
    }

    g_AIStop = stop;
}

void GmCommander::AiStopPet(Player* who, std::list<std::string>& arglist)
{
    bool stop = true;
    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            stop = (StringToInt(arg) != 0);

        argCount++;
        arglist.pop_front();
    }

    Soldier* pet = who->GetPetMgr()->GetSoldier();
    if(pet)
        pet->SetAIRun(!stop);
}

void GmCommander::NoDamage(Player* who, std::list<std::string>& arglist)
{
    bool noDamage = true;
    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            noDamage = (StringToInt(arg) != 0);

        argCount++;
        arglist.pop_front();
    }

    who->SetNoDamage(noDamage);
}

void GmCommander::DamageMe(Player* who, std::list<std::string>& arglist)
{
    int32 damage = 10;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            damage = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }

    DamageResult damageInfo;
    damageInfo.sum_damage = damage;
    damageInfo.damage_type = 0;
    who->Damage(damageInfo, who);
}

void GmCommander::Damage(Player* who, std::list<std::string>& arglist)
{
    int32 damage = 10;
    int32 range = 500;

    int32 argCount = 1;
    while(arglist.size() > 0)
    {
        std::string& arg = arglist.front();
        if(argCount == 1)
            damage = StringToInt(arg);
        else if(argCount == 2)
            range = StringToInt(arg);

        argCount++;
        arglist.pop_front();
    }

    BattleUnit* targets[50];
    int32 count = who->GetScene()->FindBattleUnitInCircle(who->GetPosition(), range, targets, 50);
    for(int32 ki=0; ki<count; ki++)
    {   
        DamageResult damageInfo;
        damageInfo.sum_damage = damage;
        damageInfo.damage_type = 0;

        if(targets[ki]->IsKindOf(&Monster::ms_RTTI))
        {
            Monster* pMonster = ((Monster*)targets[ki]);
            pMonster->Damage(damageInfo, who, NULL);
        }
        else if(targets[ki]->IsKindOf(&Boss::ms_RTTI))
        {
            Boss* pBoss = ((Boss*)targets[ki]);
            pBoss->Damage(damageInfo, who);
        }
    }
}

void GmCommander::FullHpMp(Player* who, std::list<std::string>& arglist)
{
    who->AddHP(who->GetMaxHp());
    who->AddMP(who->GetMaxMp());
    who->AddEnergyShield(who->GetMaxEnergyShield());
}

void GmCommander::UpdateActivity(Player* who, std::list<std::string>& arglist)
{
    if (!who)
        return;

    std::string arg;
    ACTIVITY_INFO info;

    if (arglist.size() > 0)
    {
        arg = arglist.front();
        if(!Crown::SDIsDigit(arg.c_str()))
            return;
        arglist.pop_front();
        info.activity_id = SDAtoi(arg.c_str());
    }

    if (arglist.size() > 0)
    {
        arg = arglist.front();
        if(!Crown::SDIsDigit(arg.c_str()))
            return;
        arglist.pop_front();
        info.activity_state = SDAtoi(arg.c_str());
    }

    ActivityManager::Instance()->UpdateActivity(info);
}

void GmCommander::AddActivity( Player* who, std::list<std::string>& arglist )
{
    if (!who)
        return;

    if (arglist.size() < 5)
        return;

    std::string arg;
    ACTIVITY_INFO info;

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.activity_id = SDAtoi(arg.c_str());

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.activity_type = SDAtoi(arg.c_str());

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.activity_state = SDAtoi(arg.c_str());

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.start_time = SDAtoi(arg.c_str());

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.end_time = SDAtoi(arg.c_str());

    ActivityManager::Instance()->AddActivity(info);
}

void GmCommander::DeleteActivity( Player* who, std::list<std::string>& arglist )
{
    if (!who)
        return;

    if (arglist.size() < 1)
        return;

    std::string arg;
    ACTIVITY_INFO info;

    arg = arglist.front();
    if(!Crown::SDIsDigit(arg.c_str()))
        return;
    arglist.pop_front();
    info.activity_id = SDAtoi(arg.c_str());

    ActivityManager::Instance()->DelActivity(info);
}

void GmCommander::SetCareer(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 career = SDAtoi(arg1.c_str());
    if((career < CAREER_WARRIOR) || (CAREER_ARCHER < career)) return;

    who->SetCareer(career);
}

void GmCommander::SetGroup(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 group = SDAtoi(arg1.c_str());

    who->SetGroup(group);
}

void GmCommander::AddExp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 exp = SDAtoi(arg1.c_str());

    who->AddExp(exp);
}

void GmCommander::LevelUp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    if(who->GetLevel() >= SystemConfigManager::Instance()->GetMaxLevel())
        return;

    who->AddLevel(1);
    who->GetPetMgr()->GetSoldier()->SetLevel(who->GetLevel());
}

void GmCommander::SetLevel(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 levelup = SDAtoi(arg1.c_str());

    who->SetLevel(levelup);
}

void GmCommander::AddPassiveSkillPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    who->AddPassiveSkillPoint(1);
}

void GmCommander::ResetPassiveSkillPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    who->ResetPassiveSkillPoint();
}

// ==================================================
// 设置游戏币
// ==================================================
void GmCommander::SetGamePoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 gamepoint = SDAtoi(arg1.c_str());
    if(gamepoint < 0) return;

    who->SetGamePoint(gamepoint);
}

// ==================================================
// 添加游戏币
// ==================================================
void GmCommander::AddGamePoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 gamepoint = SDAtoi(arg1.c_str());
    if(gamepoint < 0) return;
    who->AddGamePoint(gamepoint, REWARD_REASON_GM);
}

// ==================================================
// 设置金币
// ==================================================
void GmCommander::SetMoneyPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 moneypoint = SDAtoi(arg1.c_str());
    if(moneypoint < 0) return;

    who->SetMoneyPoint(moneypoint);
}

// ==================================================
// 添加金币
// ==================================================
void GmCommander::AddMoneyPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 moneypoint = SDAtoi(arg1.c_str());
    if(moneypoint < 0) return;
    who->AddMoneyPoint(moneypoint, REWARD_REASON_GM);
}

void GmCommander::AddBindMoneyPoint( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 bindMoneyPoint = SDAtoi(arg1.c_str());
    if(bindMoneyPoint < 0) return;

    who->AddBindMoneyPoint(bindMoneyPoint, REWARD_REASON_GM);
}

// ==================================================
// 设置精魂
// ==================================================
void GmCommander::SetSoulPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 soul_point = SDAtoi(arg1.c_str());
    if(soul_point < 0) return;

    who->SetSoulPoint(soul_point);
}


void  GmCommander::SetStarPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 star_point = SDAtoi(arg1.c_str());
    if(star_point < 0) return;

    who->SetStarPoint(star_point);
}
void  GmCommander::AddStarPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 starpoint = SDAtoi(arg1.c_str());
    if(starpoint < 0) return;
    who->AddStarPoint(starpoint);
}
// ==================================================
// 添加精魂
// ==================================================
void GmCommander::AddSoulPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 soul = SDAtoi(arg1.c_str());
    if(soul < 0) return;

    who->AddSoulPoint(soul, REWARD_REASON_GM);
}

// ==================================================
// 设置荣誉
// ==================================================
void GmCommander::SetHonorPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 honor_point = SDAtoi(arg1.c_str());
    if(honor_point < 0) return;

    who->SetHonorPoint(honor_point);
}

// ==================================================
// 添加荣誉
// ==================================================
void GmCommander::AddHonorPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    who->AddHonorPoint(honor);
}

// ==================================================
// 设置公会贡献点
// ==================================================
void GmCommander::SetGuildPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 guild_point = SDAtoi(arg1.c_str());
    if(guild_point < 0) return;

    who->SetGuildPoint(guild_point);
}

// ==================================================
// 添加公会贡献点
// ==================================================
void GmCommander::AddGuildPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 guild_point = SDAtoi(arg1.c_str());
    if(guild_point < 0) return;

    who->AddGuildPoint(guild_point, REWARD_REASON_GM);
}

// ==================================================
// 添加工会经验
// ==================================================
void GmCommander::AddGuildExp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 add_exp = SDAtoi(arg1.c_str());
    if(add_exp < 0) return;

    GUILD_DONATE_REQ sendMsg;
    sendMsg.Clear();

    for(int32 i=0; i<add_exp; ++i)
    {
        GuildManager::Instance()->OnGuildDonateReq(who, &sendMsg);
    }
}

// ==================================================
// 设置体力
// ==================================================
void GmCommander::SetVigor(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 vigor = SDAtoi(arg1.c_str());
    // if(vigor < 0) return;

    who->SetVigorValue(vigor);
}

// ==================================================
// 添加体力
// ================================================== 
void GmCommander::AddVigor(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 vigor = SDAtoi(arg1.c_str());

    who->AddVigorValue(vigor,1);
}

// ==================================================
// 设置成长点
// ==================================================
void GmCommander::SetGrowthPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 growthpoint = SDAtoi(arg1.c_str());
    if(growthpoint < 0) return;
    who->SetGrowthPoint(growthpoint);
}

// ==================================================
// 添加成长点
// ==================================================
void GmCommander::AddGrowthPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 growthpoint = SDAtoi(arg1.c_str());
    if(growthpoint < 0) return;
    who->AddGrowthPoint(growthpoint);

}

// ==================================================
// 设置VIP时效
// ==================================================
void GmCommander::SetVipCard(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    int32 vipCardId = SDAtoi(arg1.c_str());
    if(vipCardId < 0) return;

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 num = Crown::SDAtou(arg2.c_str());
    if (num < 0) return;

    who->CBVipOffer(vipCardId, num);
}

// 激活体验VIP
void GmCommander::ActiveExperienceVip(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    who->ActiveExperienceVIP();
}

// ==================================================
// 设置防沉迷标记
// ==================================================
void GmCommander::SetFcmFlag(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 fcmFlag = SDAtoi(arg1.c_str());
    if(fcmFlag < 0) return;

    FCM_INFO& fcmInfo = who->GetFcmInfo();

    fcmInfo.fcm_flag = fcmFlag;
    fcmInfo.ingame_time = now;
    fcmInfo.outgame_time = 0;
    fcmInfo.total_ingame_time = 0;
    fcmInfo.total_outgame_time = 0;

    who->SendToGate(MSG_FCM_INFO_ACK, &fcmInfo);
}

// ==================================================
// 设置防沉迷上线时间
// ==================================================
void GmCommander::SetFcmInGameTime(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 ingame_time = SDAtoi(arg1.c_str());

    FCM_INFO& fcmInfo = who->GetFcmInfo();

    fcmInfo.total_ingame_time = ingame_time;

    who->SendToGate(MSG_FCM_INFO_ACK, &fcmInfo);
}

// ==================================================
// 设置防沉迷下线时间
// ==================================================
void GmCommander::SetFcmOutGameTime(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    uint32 now = (uint32)Crown::SDNow().GetTimeValue();

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 outgame_time = SDAtoi(arg1.c_str());

    FCM_INFO& fcmInfo = who->GetFcmInfo();

    fcmInfo.total_outgame_time = outgame_time;

    who->SendToGate(MSG_FCM_INFO_ACK, &fcmInfo);
}

// ==================================================
// 设置GM权限
// ==================================================
void GmCommander::SetGMRight(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    std::string arg2 = arglist.front();
    arglist.pop_front();

    int32 right_flag = SDAtoi(arg1.c_str());
    int32 right_open = SDAtoi(arg2.c_str());

    if(right_open)
        who->SetRightFlag( who->GetRightFlag() | right_flag );
    else
        who->SetRightFlag( who->GetRightFlag() & ~right_flag );
}

void GmCommander::SetMaxHp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMaxHp();
    who->SetMaxHp(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetMaxMp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMaxMp();
    who->SetMaxMp(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetAttack(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    //who->ClearAttack();
    //who->SetAttack(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetDefence(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearArmor();
    who->SetArmor(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetDodge(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearDodge();
    who->SetDodge(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetHitRate(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearHit();
    who->SetHit(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetCriticalAttack(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearCrit();
    who->SetCrit(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetCriticalRed(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    //     who->ClearCriticalAttackReduction();
    //     who->SetCriticalAttackReduction(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetCriticalDamageAttack(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearCritDamage();
    who->SetCritDamage(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetCriticalDamageDef(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetFireDefence(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearFireResistance();
    who->SetFireResistance(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetIceDefence(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearIceResistance();
    who->SetIceResistance(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetThunderDefence(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearLightningResistance();
    who->SetLightningResistance(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetResHp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearHpRecovery();
    who->SetHpRecovery(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetResMp(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMpRecovery();
    who->SetMpRecovery(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetBlock(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearAttackBlock();
    who->SetAttackBlock(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetMagicBlock(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMagicBlock();
    who->SetMagicBlock(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetEnergyShield(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMaxEnergyShield();
    who->SetMaxEnergyShield(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetAttackSpeed(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearAttackSpeed();
    who->SetAttackSpeed(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetCastSpeed(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearCastSpeed();
    who->SetCastSpeed(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

void GmCommander::SetMoveSpeed(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    int32 value = SDAtoi(arg1.c_str());

    who->ClearMoveSpeed();
    who->SetMoveSpeed(value);

    //who->GetPropertySetCalculater()->SetCheat();
    who->SetPropDirty();
}

// ==================================================
// 添加跟宠
// ==================================================
void GmCommander::AddFollowPet(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1 ) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 petId = Crown::SDAtou(arg1.c_str());

    //std::string typestr = arglist.front();
    //arglist.pop_front();
    //int32 type = Crown::SDAtou(typestr.c_str());

    //std::string arg3 = arglist.front();
    //arglist.pop_front();
    //uint32 _timed = Crown::SDAtou(arg3.c_str());


    //who->GetFollowPetManager()->AddFollowPetInfo(petId);

    //who->GetEquipManager()->AddFollowPet(petId);
    // who->GetPetManager()->GetFollowPetManager()->FingerGuess(petId,type,_timed);
}

void GmCommander::AddProtectSoul(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1 ) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 petId = Crown::SDAtou(arg1.c_str());

    // who->GetPetManager()->GetFollowPetManager()->AddProtectSoul(petId);
}

void GmCommander::OnDungeonClear(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    RegularDungeon* pRegularDungeon = CnDynamicCast(RegularDungeon, who->GetScene()->GetRegular());
    if(pRegularDungeon)
        pRegularDungeon->DungeonClear(true);
}

void GmCommander::OnAddRide(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 petId = Crown::SDAtou(arg1.c_str());
    who->GetRideManager()->AddRidePet(petId);
}

void GmCommander::MinMarkRide(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<3) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 id = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 petId = Crown::SDAtou(arg2.c_str());


    std::string arg3 = arglist.front();
    arglist.pop_front();
    int32 type = Crown::SDAtou(arg3.c_str());

    RIDE_PET_MARK_REQ remsg;
    remsg.id = id;
    remsg.pet_id = petId;
    remsg.type = type;
    who->GetRideManager()->MinMarkRide(&remsg);
}
void GmCommander::AddFashion(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 fashionId = Crown::SDAtou(arg1.c_str());
    who->GetFashionManager()->AddFashion(fashionId);
}

void GmCommander::FashiionCompose(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 fashionId = Crown::SDAtou(arg1.c_str());
    FASHION_COMPOSE_REQ fashioncompose;
    fashioncompose.id = fashionId;
    who->GetFashionManager()->FashionCompose(&fashioncompose);
}

void GmCommander::EquipOpenSlot( Player* who, std::list<std::string>& arglist )
{
    //     if(!who) return;
    //     if(arglist.size()<2) return;
    // 
    //     std::string arg1 = arglist.front();
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     std::string arg2 = arglist.front();
    //     arglist.pop_front();
    //     int32 slot_num = SDAtoi(arg2.c_str());
    // 
    //     EQUIP_OPEN_SLOT_REQ recvMsg;
    //     recvMsg.equip_type = type;
    //     recvMsg.slot_num = slot_num;
    //     who->OnRecv(MSG_EQUIP_OPEN_SLOT_REQ, &recvMsg);
}

// 升级或一键升级翅膀
void GmCommander::onAdvanceReq(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();

    arglist.pop_front();

    int32 type = SDAtoi(arg1.c_str());
    if(type < 0) return;

    WING_ADVANCE_REQ recvMsg;
    recvMsg.type = type;
    who->m_wing_mgr->onAdvanceReq(&recvMsg);
}

//激活翅膀
void GmCommander::onActivateReq(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();

    arglist.pop_front();

    int32 type = SDAtoi(arg1.c_str());
    if(type < 0) return;


    int32 id = SDAtoi(arg1.c_str());
    if(id < 0) return;

    WING_ACTIVATE_REQ recvMsg;
    recvMsg.id = id;
    who->m_wing_mgr->onActivateReq(&recvMsg);
}

void GmCommander::LockSpendCondition(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 type = SDAtoi(arg1.c_str());
    if(type < 0) return;

    WING_LOCK_REQ recvMsg;
    recvMsg.protype = type;
    //who->m_wing_mgr->onLockSpendCondition(&recvMsg);
}
// 设置装备升星
void GmCommander::UpEquipStar(Player* who, std::list<std::string>& arglist)
{
    //     if(!who) return;
    //     if(arglist.size()<1) return;
    // 
    //     std::string arg1 = arglist.front();
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     OPT_EQUIP_UP recvMsg;
    //     recvMsg.type = type;
    //     who->m_equip_mgr->OnEquipStarUp(&recvMsg);
}

void GmCommander::OnHoroscopeLevelUpReq(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 type = SDAtoi(arg1.c_str());
    if(type < 0) return;

    NONE horose;
    //horose.type = type;
    who->m_horoscope_mgr->OnHoroscopeLevelUpReq(&horose);
}
void GmCommander::FinishEquipStar(Player* who, std::list<std::string>& arglist)
{
    //     if(!who) return;
    //     if(arglist.size()<1) return;
    // 
    //     std::string arg1 = arglist.front();
    //     //if(SDIsDigit(arg1.c_str())) return;
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     OPT_EQUIP_UP recvMsg;
    //     recvMsg.type = type;
    //     who->m_equip_mgr->OnFinishStartUp(&recvMsg);
}
// 设置装备升品质
void GmCommander::UpEqupQuality(Player* who, std::list<std::string>& arglist)
{
    //     if(!who) return;
    //     if(arglist.size()<1) return;
    // 
    //     std::string arg1 = arglist.front();
    //     //if(SDIsDigit(arg1.c_str())) return;
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     OPT_EQUIP_UP recvMsg;
    //     recvMsg.type = type;
    //     who->m_equip_mgr->OnEquipQualityUp(&recvMsg);
}

void GmCommander::OnSynthetizeJewelReq(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 itemId = SDAtoi(arg1.c_str());


    std::string arg2 = arglist.front();
    arglist.pop_front();

    int32 num = SDAtoi(arg2.c_str());


    //SYNTHETIZE_JEWEL_REQ recMsg;
    //recMsg.item_id = itemId;
    //recMsg.num = num;
    //who->GetBag()->OnSynthetizeJewelReq(&recMsg);
}

void GmCommander::OnUpgradeJewelReq( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<2) return;

    std::string arg1 = arglist.front();
    int32 equip_type = SDAtoi(arg1.c_str());
    arglist.pop_front();

    arg1 = arglist.front();
    int32 jewel_pos = SDAtoi(arg1.c_str());
    arglist.pop_front();

    UPGRADE_JEWEL_REQ recMsg;
    recMsg.equip_type = equip_type;
    recMsg.jewel_pos = jewel_pos;
    who->OnRecv(MSG_UPGRADE_JEWEL_REQ, &recMsg);
}

void GmCommander::EquipInlaidJewel(Player* who, std::list<std::string>& arglist)
{

    //     if(!who) return;
    //     if(arglist.size()<3) return;
    // 
    //     std::string arg1 = arglist.front();
    //     arglist.pop_front();
    // 
    //     int32 type = SDAtoi(arg1.c_str());
    //     if(type < 0) return;
    // 
    //     std::string arg2 = arglist.front();
    //     arglist.pop_front();
    //     int32 jewel_pos = SDAtoi(arg2.c_str());
    // 
    //     std::string arg3 = arglist.front();
    //     arglist.pop_front();
    // 
    //     int32 item_pos = SDAtoi(arg3.c_str());
    // 
    //     EQUIP_INLAID_JEWEL_REQ recvMsg;
    //     recvMsg.equip_type = type;
    //     recvMsg.jewel_pos = jewel_pos;
    //     recvMsg.item_pos = item_pos;
    //     who->m_equip_mgr->OnEquipInlaidJewelReq(&recvMsg);
}

void GmCommander::AddItem(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    std::string arg2 = arglist.front();
    arglist.pop_front();

    uint32 itemID = Crown::SDAtou(arg1.c_str());
    int32  itemNum = SDAtoi(arg2.c_str());

    ItemManager* itemMgr = who->GetItemManager();

    if(itemID == 0 || itemNum == 0)
        return;

    //itemMgr->AddItemByNum(itemID, itemNum, REWARD_REASON_GM);


    if(itemID >= 10000 && itemID < 13000)
    {
        // 装备
    }
    else if(itemID >= 13000 && itemID < 14000)
    {
        // 消耗品
    }
    else if(itemID >= 14000 && itemID < 15000)
    {
        // 材料
        ItemMaterial* pMaterial = ItemFactory::CreateMaterial(itemID);
        if(!pMaterial)
        {
            return;
        }

        pMaterial->SetNum(itemNum);
        who->GetItemManager()->AddToBag(REASON_ITEM_ADD_GM, pMaterial);

    }
    else if(itemID >= 17000 && itemID < 17999)
    {
        // 宝石

        ItemStone* pStone= ItemFactory::CreateStone(itemID);

        if(!pStone)
        {
            return;
        }
        who->GetItemManager()->AddToBag(REASON_ITEM_ADD_GM, pStone);
    }
    else
    {

    }

}

void GmCommander::DelItemByID(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    std::string arg2 = arglist.front();
    //if(SDIsDigit(arg2.c_str())) return;
    arglist.pop_front();

    uint32 itemID = Crown::SDAtou(arg1.c_str());
    int32  itemNum = SDAtoi(arg2.c_str());

    //ItemManager* itemMgr = who->GetItemManager();
    //itemMgr->DelItemByID(itemID, itemNum);
}

void GmCommander::DelItemByPos(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    std::string arg2 = arglist.front();
    //if(SDIsDigit(arg2.c_str())) return;
    arglist.pop_front();

    int32 pos = SDAtoi(arg1.c_str());
    int32 itemNum = SDAtoi(arg2.c_str());

    BagManager* itemMgr = who->GetBagMgr();
    itemMgr->SubItem(0, pos);
}

void GmCommander::MoveItem(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    std::string arg2 = arglist.front();
    //if(SDIsDigit(arg2.c_str())) return;
    arglist.pop_front();

    int32 src_pos = SDAtoi(arg1.c_str());
    int32 dst_pos = SDAtoi(arg2.c_str());

    BagManager* itemMgr = who->GetBagMgr();
    //itemMgr->SwapItem(src_pos, dst_pos);
}

void GmCommander::ClearBag(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    //ItemManager* itemMgr = who->GetItemManager();
    //itemMgr->ClearBag();
}

void GmCommander::AddTask(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmAddTask(task_id);
}

void GmCommander::TaskComplete(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    who->GetTaskManager()->GmCompleteTask();
}

void GmCommander::TaskTalk(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    //if(SDIsDigit(arg1.c_str())) return;
    arglist.pop_front();

    uint32 npc_id = Crown::SDAtou(arg1.c_str());

    who->SendTaskEvent(TASK_EVENT_TALK_NPC, npc_id, 0);
}

void GmCommander::TaskKillMonster(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 3) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    std::string arg2 = arglist.front();
    arglist.pop_front();

    std::string arg3 = arglist.front();
    arglist.pop_front();

    uint32 monster_id = Crown::SDAtou(arg1.c_str());
    uint32 num = Crown::SDAtou(arg2.c_str());
    uint32 map_id = Crown::SDAtou(arg3.c_str());

    who->SendTaskEvent(TASK_EVENT_KILL_MONSTER, monster_id, num, UNION_ID(0, map_id, 0));
}

void GmCommander::OpenDungeon(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 dungeon_id = Crown::SDAtou(arg1.c_str());
    if(dungeon_id == 0)
        return;

    who->GetPlayerDungeonManager()->OpenDungeon(dungeon_id);
}

void GmCommander::ClearDungeon(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    who->GetScene()->GetDungeon()->DungeonClear(true);
}

void GmCommander::TakeTask( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTakeTask(task_id);
}

void GmCommander::TaskChangeDungeon( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTaskChangeDugeon(task_id);
}

void GmCommander::TaskRewards( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTaskReward(task_id);
}

void GmCommander::TaskQuality( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTaskQualityReq(task_id);
}

void GmCommander::TaskOneComplete( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 task_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTaskComplete(task_id);
}

void GmCommander::TaskRefresh( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;

    who->GetTaskManager()->GmTaskRefresh();
}

void GmCommander::TaskreFreshTime( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;

    who->GetTaskManager()->GmTaskRefreshTimeReq();
}

void GmCommander::TaskGuardRefresh( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;

    who->GetTaskManager()->GmTaskGuardRefresh();
}

void GmCommander::TaskReceiveBox( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 box_id = Crown::SDAtou(arg1.c_str());

    who->GetTaskManager()->GmTaskReceiveBox(box_id);
}

void GmCommander::ClearTeamDungeon(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();


}

void GmCommander::IsGameStart(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;

    bool isStart = who->GetScene()->GetRegular()->IsGameStart();

    CHAT_MSG sendMsg;
    sendMsg.channel = CHAT_CHANNEL_DEBUG;
    sendMsg.chat = isStart ? "start" : "stop";
    who->SendToGate(MSG_CHAT_ACK, &sendMsg);
}

// ==================================================
// 发布系统消息
// ==================================================
void GmCommander::ChatSystem(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    CHAT_MSG chat_msg;
    chat_msg.src_name = who->GetName();
    chat_msg.channel = CHAT_CHANNEL_SYSTEM;
    chat_msg.chat = arg1;
    who->OnRecv(MSG_CHAT_REQ, &chat_msg);
}

// ==================================================
// 发布跑马灯
// ==================================================
void GmCommander::ChatMarquee(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    CHAT_MSG chat_msg;
    chat_msg.src_name = who->GetName();
    chat_msg.channel = CHAT_CHANNEL_MARQUEE;
    chat_msg.chat = arg1;
    who->OnRecv(MSG_CHAT_REQ, &chat_msg);
}

void GmCommander::RanomLeveupExp(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;
    if (arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    std::string arg2 = arglist.front();
    arglist.pop_front();

    int32 type = Crown::SDAtou(arg1.c_str());
    uint32 id = Crown::SDAtou(arg2.c_str());

    FOLLOW_PET_RANDOM_EXP currandomExp;
    currandomExp.Clear();
    currandomExp.type = type;
    currandomExp.pet_id = id;
    //who->GetFollowPetManager()->RandomLeveupExp(&currandomExp);
}

void GmCommander::OnFollowPetSwallow(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;
    if (arglist.size() <2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    std::string arg2 = arglist.front();
    arglist.pop_front();

    uint32 id1 = Crown::SDAtou(arg1.c_str());
    uint32 id2 = Crown::SDAtou(arg2.c_str());

    FOLLOW_PET_SWALLOW_REQ recvMsg;
    recvMsg.Clear();
    recvMsg.cur_pet_id = id1;
    recvMsg.choose_petid = id2;
    recvMsg.SwallowType = 1;
    //who->GetFollowPetManager()->OnFollowPetSwallow(&recvMsg);
}

// ==================================================
// 签到
// ==================================================
void GmCommander::Sign(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;

    if (arglist.size() == 1)
    {

    }
    else
    {
        who->GetSignManager()->OnSignReq();
    }
}

// ==================================================
// 添加邮件
// ==================================================
void GmCommander::AddMail(Player* who, std::list<std::string>& arglist)
{
    //if (!who) return;
    //if (arglist.size() < 2) return;

    //std::string arg1 = arglist.front();
    //arglist.pop_front();

    //std::string arg2 = arglist.front();
    //arglist.pop_front();

    //uint32 tier = Crown::SDAtou(arg1.c_str());
    //uint32 stage = Crown::SDAtou(arg2.c_str());
}

///< 刷新赛马信息
void GmCommander::RacingRefresh(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;

    //who->GetRacingManager()->DailyRefresh();
}

///< 参加赛马
void GmCommander::RacingEnter(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;

    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    uint32 money = Crown::SDAtou(arg1.c_str());

    //who->GetRacingManager()->EnterRacing(money);
}

///< 领取赛马奖励
void GmCommander::RacingReward(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    //who->GetRacingManager()->RacingReward();
}

void GmCommander::TracePlayerList(Player* who, std::list<std::string>& arglist)
{
    if (!who) return;

    CHAT_MSG sendMsg;
    sendMsg.channel = CHAT_CHANNEL_INTEGRATED;

    sendMsg.chat += "\n";

    PlayerManager::PlayerMap playerMap = PlayerManager::Instance()->GetPlayers();
    for(PlayerManager::PlayerMap::iterator iter = playerMap.begin(); iter != playerMap.end(); iter++)
    {
        Player* player = iter->second;
        Scene* scene = player->GetScene();

        sendMsg.chat += player->GetName();
        if(scene)
        {
            char numchar[100];

            sendMsg.chat += " ";
            Crown::SDItoa(numchar, 100, scene->GetDungeonID());
            sendMsg.chat += numchar;
            sendMsg.chat += " ";
            Crown::SDItoa(numchar, 100, scene->GetChannelID());
            sendMsg.chat += numchar;
        }

        sendMsg.chat += "\n";
    }

    who->SendToGate(MSG_CHAT_ACK, &sendMsg);
}

void GmCommander::TracePlayerNum( Player* who, std::list<std::string>& arglist )
{
    if (!who) return;

    CHAT_MSG sendMsg;
    sendMsg.channel = CHAT_CHANNEL_INTEGRATED;

    uint32 playerNum1 = PlayerManager::Instance()->GetPlayerNum();
    uint32 playerNum2 = DungeonManager::Instance()->GetPlayerNum();
    char str[100];
    SDSnprintf(str, 100, "%d/%d", playerNum1, playerNum2);

    sendMsg.chat = str;

    who->SendToGate(MSG_CHAT_ACK, &sendMsg);
}

///< 重新加载商城信息
void GmCommander::ReloadMall(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    who->GetShopManager()->GMReloadMallInfo();
}

///< 商城购买请求
void GmCommander::MallBuyReq(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    //if(arglist.size() < 2) return;

    //std::string arg1 = arglist.front();
    //arglist.pop_front();
    //uint32 product = Crown::SDAtou(arg1.c_str());

    //std::string arg2 = arglist.front();
    //arglist.pop_front();
    //uint32 num = Crown::SDAtou(arg2.c_str());

    //BUY_REQ sendMsg;
    //sendMsg.Clear();
    //sendMsg.product_id = product;
    //sendMsg.num = num;

    //who->GetMallManager()->GMBuyReq(MSG_BUY_REQ, &sendMsg);
}

void GmCommander::MallInfoReq(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    who->GetShopManager()->GMMallInfoReq();
}

// 重新生成排行榜
void GmCommander::RefreshRanking(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    RankingManager::Instance()->GMRefreshRanking();
}

void GmCommander::AskBattleUnitProperty(Player* who, std::list<std::string>& arglist)
{
    if (!who)
        return;

    if(arglist.size() < 1) 
        return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 objectId = SDAtoi(arg1.c_str());

    BattleUnit* pUnit = who->GetScene()->GetBattleUnit(objectId);
    if(!pUnit)
        return;

    std::string txt;
    txt = "attack:" +  Crown::ToString(pUnit->GetBaseDamage()) + "\n"
        + "defence:" +  Crown::ToString(pUnit->GetArmor()) + "\n"
        + "hitrate:" +  Crown::ToString(pUnit->GetHit()) + "\n"
        + "dodge:" +  Crown::ToString(pUnit->GetDodge()) + "\n"
        + "critical:" +  Crown::ToString(pUnit->GetCrit()) + "\n"
        + "critical_dmg:" +  Crown::ToString(pUnit->GetCritDamage()) + "\n"
        + "fire_def:" +  Crown::ToString(pUnit->GetFireResistance()) + "\n"
        + "ice_def:" +  Crown::ToString(pUnit->GetIceResistance()) + "\n"
        + "thunder_def:" +  Crown::ToString(pUnit->GetLightningResistance());

    CHAT_MSG sendMsg;
    sendMsg.channel = CHAT_CHANNEL_DEBUG;
    sendMsg.chat = txt;
    who->SendToGate(MSG_CHAT_ACK, &sendMsg);
}

void GmCommander::KillAll(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }
    who->GetScene()->GetRegular()->KillAll();
}

void GmCommander::GetBableRewrd(Player* who, std::list<std::string>& arglist)
{
    if (!who)
    {
        return;
    }
    who->OnRewardInfoCount();
}
void GmCommander::Changedungeon(Player* who, std::list<std::string>& arglist)
{
    if (!who)
    {
        return;
    }
    // who->OnChangeDungeon()
}
void GmCommander::useskill(Player* who, std::list<std::string>& arglist)
{
    if(!who)
    {
        return;
    }

    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 skillid = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    int32 skillObjectID = Crown::SDAtou(arg2.c_str());

    SHOOT_TARGET target;
    SkillArgumentData args;

    who->UseSkill(skillid, target, args, skillObjectID, 0);
}

void GmCommander::DayInvestment( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 type = Crown::SDAtou(arg1.c_str());
    if(0 == type)
    {
        NONE none;
        who->OnRecv(MSG_DAY_INVESTMENT_REQ, &none);
    }
    else if (1 == type)
    {
        NONE none;
        who->OnRecv(MSG_DAY_INVESTMENT_INFO_REQ, &none);
    }
    else if (2 == type)
    {
        std::string arg2 = arglist.front();
        arglist.pop_front();
        uint32 day = Crown::SDAtou(arg2.c_str());
        DAY_INVESTMENT_ACCEPT_REWARD_REQ req;
        req.Clear();
        req.day = day;
        who->OnRecv(MSG_DAY_INVESTMENT_ACCEPT_REWARD_REQ, &req);
    }
}

void GmCommander::LevelInvestment( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 type = Crown::SDAtou(arg1.c_str());
    if(0 == type)
    {
        std::string arg2 = arglist.front();
        arglist.pop_front();

        LEVEL_INVESTMENT_REQ req;
        req.Clear();
        req.invest_money = Crown::SDAtou(arg2.c_str());

        who->OnRecv(MSG_LEVEL_INVESTMENT_REQ, &req);
    }
    else if (1 == type)
    {
        NONE none;
        who->OnRecv(MSG_LEVEL_INVESTMENT_INFO_REQ, &none);
    }
    else if (2 == type)
    {
        std::string arg2 = arglist.front();
        arglist.pop_front();

        LEVEL_INVESTMENT_ACCEPT_REWARD_REQ req;
        req.Clear();
        req.level = Crown::SDAtou(arg2.c_str());

        who->OnRecv(MSG_LEVEL_INVESTMENT_ACCEPT_REWARD_REQ, &req);
    }
}

void GmCommander::AddRecharge(Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;
    if(arglist.size() < 1)
        return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 money = Crown::SDAtou(arg1.c_str());
    who->SetTotalRecharge(money);
    //who->GetTotalRecharge()
}

void GmCommander::InvestmentManager( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 type = Crown::SDAtou(arg1.c_str());
    if(0 == type)
    {
        INVESTMENT_OPERATION_NTF info;
        info.Clear();

        info.name = arglist.front();
        arglist.pop_front();

        info.type = Crown::SDAtou(arglist.front().c_str());
        arglist.pop_front();

        info.operation = Crown::SDAtou(arglist.front().c_str());
        arglist.pop_front();

        info.money = Crown::SDAtou(arglist.front().c_str());
        arglist.pop_front();

        InvestmentManager::Instance()->InvestmentOperationNTF(who, &info);
    }
    else if (1 == type)
    {
        INVESTMENT_OPERATION_REQ req;
        req.Clear();
        req.count = Crown::SDAtou(arglist.front().c_str());
        who->OnRecv(MSG_INVESTMENT_OPERATION_REQ, &req);
    }
}

void GmCommander::LevelActivity( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 type = Crown::SDAtou(arg1.c_str());
    if(0 == type)
    {;
    }
    else if (1 == type)
    {
    }
}

void GmCommander::SpecialActivity( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    std::string arg1 = arglist.front();
    uint32 type = Crown::SDAtou(arg1.c_str());

    if(4 == type)
    {
        SPECIAL_ACTIVITY_INFO_REQ req;
        req.Clear();

        arglist.pop_front();
        std::string arg2 = arglist.front();
        req.activity_id = Crown::SDAtou(arg2.c_str());

        who->OnRecv(MSG_SPECIAL_ACTIVITY_INFO_REQ, &req);
    }
    else if(5 == type)
    {
        SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ req;
        req.Clear();

        arglist.pop_front();
        std::string arg2 = arglist.front();
        req.activity_id = Crown::SDAtou(arg2.c_str());

        arglist.pop_front();
        std::string arg3 = arglist.front();
        req.param = Crown::SDAtou(arg3.c_str());

        who->OnRecv(MSG_SPECIAL_ACTIVITY_ACCEPT_REWARD_REQ, &req);
    }
}

void GmCommander::AddBattleSoul(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 battleSoul = SDAtoi(arg1.c_str());
    if(battleSoul < 0) return;
    who->AddBattleSoulPoint(battleSoul);
}

void GmCommander::RaidsMonsterDoor( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    //who->GetMonsterDoorManager()->OnRecv(MSG_MONSTER_DOOR_RAID_REQ, 0);
}

// 设置公会BOSS开启时间
void GmCommander::SetGuildBossTime(Player* who, std::list<std::string>& arglist)
{
    Crown::CSDDateTime now = Crown::SDNow();
    if(!who)
    {
        return;
    }

    GUILD_BOSS_SET_TIME_REQ sendMsg;
    sendMsg.start_time = (now.GetHour()*3600) + (now.GetMinute()*60) + now.GetSecond();
    who->SendToCenter(MSG_GUILD_BOSS_SET_TIME_REQ, &sendMsg);
}

void GmCommander::BindMoneyPointBuyReq( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 item_id = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 item_num = Crown::SDAtou(arg2.c_str());

    BIND_MONEY_POINT_BUY_REQ sendMsg;
    sendMsg.Clear();
    sendMsg.item_id = item_id;
    sendMsg.item_num = item_num;

    who->OnRecv(MSG_BIND_MONEY_POINT_BUY_REQ, &sendMsg);

}

void GmCommander::AcceptLivenessReward( Player* who, std::list<std::string>& arglist )
{
    if(!who)
    {
        return;
    }

    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 index = Crown::SDAtou(arg1.c_str());


    LIVENESS_ACCEPT_REWARD_REQ sendMsg;
    sendMsg.Clear();
    sendMsg.index = index;

    who->OnRecv(MSG_LIVENESS_ACCEPT_REWARD_REQ, &sendMsg);
}

void GmCommander::EnterCityFight( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 city_id = Crown::SDAtou(arg1.c_str());

    CITY_FIGHT_ENTER_REQ sendMsg;
    sendMsg.city_id = city_id;

    who->OnRecv(MSG_CITY_FIGHT_ENTER_REQ, &sendMsg);
}

void GmCommander::CityFightCityInfo( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    NONE sendMsg;
    who->OnRecv(MSG_CITY_FIGHT_INFO_REQ, &sendMsg);
}

void GmCommander::CitySurvival( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    NONE none;
    who->OnRecv(MSG_CITY_SURVIVAL_ENTER_REQ, &none);
}

void GmCommander::CalcPlayerProp( Player* who, std::list<std::string>& arglist )
{
    if(!who)
        return;

    who->CalcProperty();
}

void GmCommander::CalSingleAnswerInt(Player* who, std::list<std::string>& arglist)
{
    if(!who)
        return;

    NONE none;
    who->OnRecv(MSG_SINGLE_ANSWER_NTF, &none);
}

void GmCommander::CalSingleAnswerQuesitonReq(Player* who, std::list<std::string>& arglist)
{
    if(!who)
        return;

    NONE none;
    who->OnRecv(MSG_SINGLE_ANSWER_QUESTION_REQ, &none);
}

void GmCommander::CalSingleAnswerResultReq(Player* who, std::list<std::string>& arglist)
{
    if (!who)
        return;

    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 questionid = Crown::SDAtou(arg1.c_str());

    std::string arg2 = arglist.front();
    arglist.pop_front();
    uint32 reulst = Crown::SDAtou(arg2.c_str());

    SINGLE_RESLUT_REQ sendmsg;
    sendmsg.question_id = questionid;
    sendmsg.result = reulst;
    who->OnRecv(MSG_SINGLE_ANSWER_RESULT_REQ,&sendmsg);
}

void GmCommander::CalSingleAnswerTimeCDReq(Player* who, std::list<std::string>& arglist)
{
    if (!who)
        return;
    NONE sendmsg;
    who->OnRecv(MSG_SINGLE_ANSWER_CD_TIME_REQ,&sendmsg);
}

void GmCommander::GetVipGiftReq(Player* who, std::list<std::string>& arglist)
{
    if(!who)
        return;

    if(arglist.size() < 1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();
    uint32 viplevel = Crown::SDAtou(arg1.c_str());

    VIP_GIFT_REQ vipgift;
    vipgift.level = viplevel;
    who->OnRecv(MSG_VIP_GIFT_REQ, &vipgift);
}

void GmCommander::OnRequestZombieInfo(Player* who, std::list<std::string>& arglist)
{
    if (!who)
        return;

    who->GetZombieManager()->OnRecv(MSG_ZOMBIE_WAR_INFO_REQ, NULL);
}


// ==================================================
// 设置跨服积分
// ==================================================
void GmCommander::SetMultiserverIntegralPoint(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    who->m_multiserver_battle_info->SetIntegralPoint(honor);
}

// ==================================================
// 设置跨服功勋点
// ==================================================
void GmCommander::SetMultiserverExploitPoint( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    who->m_multiserver_battle_info->SetExploitPoint(honor);
}

void GmCommander::AddCrossServer(Player* who, std::list<std::string>& arglist)
{
    if(!who) return;
    if(arglist.size()<2) return;

    std::string ip = arglist.front();
    arglist.pop_front();

    std::string port_str = arglist.front();
    arglist.pop_front();

    uint32 port = SDAtoi(port_str.c_str());

    SERVER_INFO info;
    info.server_ip = ip;
    info.server_port = port;

    CenterHandler::Instance()->RecvCrossServerAdd(&info);
}

void GmCommander::SetMultiserverAlliancePoint( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    who->m_multiserver_battle_info->AddAlliancePoint(honor);
}

void GmCommander::SetMultiserverSeasonPoint( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    who->m_multiserver_battle_info->AddSeasonPoint(honor);
}

void GmCommander::SetSkillFashion( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    who->m_fashion_mgr->GmSkillFashion();
}

void GmCommander::SetLikeInfo( Player* who, std::list<std::string>& arglist )
{
    who->m_shorcut_mgr->OnGmSetLike();
}

void GmCommander::SetActivityRank( Player* who, std::list<std::string>& arglist )
{
    TopUpRankManager::Instance()->GmActivityRankReq(9999);
}

void GmCommander::SetEggInfo( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size() < 2) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    std::string arg2 = arglist.front();
    arglist.pop_front();


    uint32 type = SDAtoi(arg1.c_str());
    uint32 num = SDAtoi(arg2.c_str());


    if(type < 0) return;
    if(num < 0) return;

    who->GetEggManager()->GmAddEgg(type, num);
}

void GmCommander::ReloadLua(Player* who, std::list<std::string>& arglist)
{
    LuaEngine::Instance()->ReloadAllScript();
}

void GmCommander::SetResetFestival( Player* who, std::list<std::string>& arglist )
{
    TopUpRankManager::Instance()->GmActivityRankReq(8888);
}

void GmCommander::SetFormula( Player* who, std::list<std::string>& arglist )
{
    if(!who) return;
    if(arglist.size()<1) return;

    std::string arg1 = arglist.front();
    arglist.pop_front();

    int32 honor = SDAtoi(arg1.c_str());
    if(honor < 0) return;

    for (int32 i = 0; i < honor; i++)  
    {
        real dodgePro = FormulaManager::Instance()->CalcHitRatePro(6648, 0);
        // 附加伤害的计算
        real random = Crown::SDRandomFloat(1.0f);

        int32 physics_value = (int32)FormulaManager::Instance()->CalcNormalAttack(2000, random, 0, 66, 0, 0);

        real random2 = Crown::SDRandomFloat(1.0f);
        int32 buff_damage = (int32)FormulaManager::Instance()->CalcSkillAttack(0, random2, 0, 1, 23, 0, 0, 0, 0, 0);

        real criAttPro = FormulaManager::Instance()->CalcCriAttPro(2369, 113285);
        real CriPro = FormulaManager::Instance()->CalcCriAttDamage(1, 0, 100500);
        //m_all_battle_power = (int32)FormulaManager::Instance()->CalcBattlePower(allBattlePowerProp);

        // 翅膀战斗力
        PropertySet     baseProp;

        baseProp.Add(12, 74400);

        baseProp.Add(99, 53000);

        baseProp.Add(67,124800);

        baseProp.Add(158,1876);

        baseProp.Add(100,96600);

        //baseProp.Add(69,114000);
        baseProp.Add(69,112000);

        //baseProp.Add(161,3425);
        baseProp.Add(161,3400);

        baseProp.Add(68,44000);

        //baseProp.Add(13,32500);
        baseProp.Add(13,32000);

        //baseProp.Add(11,93500);
        baseProp.Add(11,92000);

        baseProp.Add(74,45000);

        baseProp.Add(159,1400);

        baseProp.Add(75,5000);

        baseProp.Add(160,2000);

        //BattlePowerProp oneBattlePowerProp;
        //oneBattlePowerProp.AddPropList(baseProp.m_list);
        //
        //uint32 m_wing_base_battle_power = (int32)FormulaManager::Instance()->CalcBattlePower(oneBattlePowerProp);
    }

    SERVER_MARQUEE marquee;
    marquee.id = MARQUEE_EQUIP_GEM_CRIT;
    marquee.parame_list.push_back(MarQueePlayerName(who->GetVipLevel(), who->GetName()));
    who->OnMarQuee(marquee);
}
