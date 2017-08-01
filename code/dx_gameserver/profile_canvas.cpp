#include "stdafx.h"
#include "profile_canvas.h"
#include "profile_table.h"
#include "button.h"
#include "text_obj.h"

const int32 MAX_TABLE_NUM = 5;

ProfileCanvas::ProfileCanvas()
{
    Init();
}

ProfileCanvas::~ProfileCanvas()
{
    Uninit();
}

void ProfileCanvas::Init()
{
    m_pCurrentNode = PerformanceManager::Instance()->GetRoot();

    for(int32 i=0; i<5; i++)
    {
        m_BtnIn[i] = new Button;
        m_BtnIn[i]->SetPosition(100*(i+1), 0);
        m_BtnIn[i]->SetWidth(50);
        m_BtnIn[i]->SetHeight(20);
        m_BtnIn[i]->SetText("in");
        AddChild(m_BtnIn[i]);
    }

    m_BtnOut = new Button;
    m_BtnOut->SetPosition(700, 0);
    m_BtnOut->SetWidth(50);
    m_BtnOut->SetHeight(20);
    m_BtnOut->SetText("out");
    AddChild(m_BtnOut);

    m_BtnAdd = new Button;
    m_BtnAdd->SetPosition(800, 0);
    m_BtnAdd->SetWidth(50);
    m_BtnAdd->SetHeight(20);
    m_BtnAdd->SetText("+");
    AddChild(m_BtnAdd);

    m_BtnSub = new Button;
    m_BtnSub->SetPosition(860, 0);
    m_BtnSub->SetWidth(50);
    m_BtnSub->SetHeight(20);
    m_BtnSub->SetText("-");
    AddChild(m_BtnSub);

    m_BtnStop = new Button;
    m_BtnStop->SetPosition(950, 0);
    m_BtnStop->SetWidth(50);
    m_BtnStop->SetHeight(20);
    m_BtnStop->SetText("stop");
    AddChild(m_BtnStop);

    m_Text = new TextObj;
    m_Text->SetPosition(0, 20);
    m_Text->SetText("-----------------");
    AddChild(m_Text);


    for(int32 i=0; i<MAX_TABLE_NUM; i++)
    {
        ProfileTable* pTable = new ProfileTable;
        AddChild(pTable);

        pTable->SetX(50);
        pTable->SetY(i*120+50);

        m_tables.push_back(pTable);
    }

}

void ProfileCanvas::Uninit()
{
    delete m_Text;

    for(int32 i=0; i<MAX_TABLE_NUM; i++)
    {
        ProfileTable* pTable = m_tables[i];
        RemoveChild(pTable);
        delete pTable;
    }
    m_tables.clear();

    for(int32 i=0; i<5; i++)
    {
        delete m_BtnIn[i];
    }

    delete m_BtnOut;

    delete m_BtnAdd;
    delete m_BtnSub;

    delete m_BtnStop;

}

void ProfileCanvas::Update()
{
    Object::Update();
    
    PerformanceNode* pRoot = PerformanceManager::Instance()->GetRoot();
    PerformanceNode* pFrame = PerformanceManager::Instance()->GetFrame();

    if(m_pCurrentNode == pRoot)
    {
        int32 childNum = pRoot->GetChildNum();
        if(childNum > MAX_TABLE_NUM)
            childNum = MAX_TABLE_NUM;

        for(int32 i=0; i<childNum; i++)
        {
            m_tables[i]->SetPerformanceNode( pRoot->GetChildByIndex(i) );
        }
        for(int32 i=childNum; i<MAX_TABLE_NUM; i++)
        {
            if(i == childNum)
                m_tables[i]->SetPerformanceNode( pFrame );
            else
                m_tables[i]->SetPerformanceNode( NULL );
        }
    }
    else
    {
        int32 childNum = m_pCurrentNode->GetChildNum();
        if(childNum > MAX_TABLE_NUM)
            childNum = MAX_TABLE_NUM;

        for(int32 i=0; i<childNum; i++)
        {
            m_tables[i]->SetPerformanceNode( m_pCurrentNode->GetChildByIndex(i) );
        }
        for(int32 i=childNum; i<MAX_TABLE_NUM; i++)
        {
            m_tables[i]->SetPerformanceNode( NULL );
        }

    }
}

void ProfileCanvas::RecvMessage(const std::string& message, Object* pObject)
{
    Object::RecvMessage(message, pObject);
    
    for(int32 i=0; i<5; i++)
    {
        if(pObject == m_BtnIn[i])
        {
            PerformanceNode* pChild = m_pCurrentNode->GetChildByIndex(i);
            if(pChild && pChild->GetChildNum() > 0)
                m_pCurrentNode = pChild;
        }
    }

    if(pObject == m_BtnOut)
    {
        PerformanceNode* pParent = m_pCurrentNode->GetParent();
        if(pParent)
            m_pCurrentNode = pParent;
    }
    else if(pObject == m_BtnAdd)
    {
        for(int32 i=0; i<MAX_TABLE_NUM; i++)
        {
            ProfileTable* pTable = m_tables[i];
            pTable->SetScale(pTable->GetScale()*2);
        }
    }
    else if(pObject == m_BtnSub)
    {
        for(int32 i=0; i<MAX_TABLE_NUM; i++)
        {
            ProfileTable* pTable = m_tables[i];
            pTable->SetScale(pTable->GetScale()/2.0f);
        }
    }
    else if(pObject == m_BtnStop)
    {
        PerformanceManager::Instance()->SetStop( !PerformanceManager::Instance()->IsStop() );
    }
}

void ProfileCanvas::SetFPSText(const std::string& txt)
{
    m_Text->SetText(txt);
}