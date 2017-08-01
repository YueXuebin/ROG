/*
*	规则引擎管理器
*/
#ifndef regular_manager_h__
#define regular_manager_h__

class Regular;
class RegularDoc;

class RegularCreator
{
public:
    virtual Regular* Create(RegularDoc *pDoc) = 0;

    RegularCreator() {}

    virtual ~RegularCreator() {}
};

template<class T>
class TmplRegularCreator : public RegularCreator
{
public:
    Regular* Create(RegularDoc *pDoc)
    {
        return CnNew T(pDoc);
    }

    TmplRegularCreator() {}

    ~TmplRegularCreator() {}
};

class RegularManager
{
public:
    RegularManager();
    ~RegularManager();

    DECLARE_SINGLETON(RegularManager)

    bool Init();

    Regular* CreateRegular(const char* xmlFileName);
    Regular* CreateRegular(const std::string &fileName, RegularDoc *pDoc);

    RegularDoc* GetRegularDoc(const std::string& xmlFileName);      // 创建关卡文档对象
    RegularDoc* GetRegularDoc(uint32 dungeonId, uint32 sceneId);

private:
    void LoadRegularTemplate(TiXmlElement* xmlEle, RegularDoc& regularTmpl);        // 加载规则模板
    void LoadMonsterAndNpc(TiXmlElement* pawnElement, RegularDoc& regularTmpl);     // 加载怪物和NPC
    void LoadBoss(TiXmlElement* bossElement, RegularDoc& regularTmpl);              // 加载BOSS
    void LoadTrigger(TiXmlElement* triggerElement, RegularDoc& regularTmpl);        // 加载 Trigger
    void LoadPin(TiXmlElement* pinElement, RegularDoc& regularTmpl);                // 加载 Pin
    void LoadSpawn(TiXmlElement* spawnElement, RegularDoc& regularTmpl);            // 解析出怪点
    void LoadPatch(TiXmlElement* patchElement, RegularDoc& regularTmpl);            // 加载采集点

private:
    typedef std::map<std::string, RegularDoc*> RegularDocMap;
    typedef std::map<std::string, RegularCreator*> RegularCreatorMap;

    RegularDocMap m_regularDocMap;
    RegularCreatorMap m_regularCreatorMap;
};

#endif // regular_manager_h__
