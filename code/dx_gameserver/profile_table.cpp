#include "stdafx.h"
#include "profile_table.h"
#include "line.h"
#include "text_obj.h"

ProfileTable::ProfileTable() :
m_pPerformanceNode(NULL)
{
    m_height = 100;
    m_scale = 1.0f;

    m_LineTable = new Line;
    POINT a[3];
    a[0].x = 0;
    a[0].y = 0;
    a[1].x = 0;
    a[1].y = m_height;
    a[2].x = 1000.f;
    a[2].y = m_height;
    m_LineTable->SetColor(0xffbbbbbb);
    m_LineTable->SetPoints(a, 3);

    AddChild(m_LineTable);


    m_LineProfile = new Line;
    m_LineProfile->SetColor(0xffff1111);
    POINT p;
    for(int32 i=0; i<POINT_NUM; i++)
    {
        p.x = i*5;
        p.y = 50 + (p.x %13);

        m_LineProfile->SetPoint(p, i);
    }

    AddChild(m_LineProfile);

    m_FunctionName = new TextObj;
    m_FunctionName->SetX(10);
    AddChild(m_FunctionName);
}   

ProfileTable::~ProfileTable()
{
    if(m_LineTable)
    {
        delete m_LineTable;
    }
    if(m_LineProfile)
    {
        delete m_LineProfile;
    }
    if(m_FunctionName)
    {
        delete m_FunctionName;
    }
}

void ProfileTable::SetPerformanceNode(PerformanceNode* pNode)
{
    m_pPerformanceNode = pNode;
    if(pNode)
    {
        char txt[200];
        SDSprintf(txt, "%d %s", pNode->GetEntryNum(), pNode->GetFullName().c_str());
        m_FunctionName->SetText(txt);
        m_LineProfile->SetVisible(true);
    }
    else
    {
        m_LineProfile->SetVisible(false);
        m_FunctionName->SetText("");
    }
}

void ProfileTable::Update()
{
    Object::Update();

    if(m_pPerformanceNode)
    {
        int32 timeIndex = PerformanceManager::Instance()->GetTimeIndex();

        POINT p;
        for(int32 i=0; i<POINT_NUM; i++)
        {
            uint32* deltaTimes = m_pPerformanceNode->GetDeltaTimes();
            p.x = i*5;
            int32 j = (timeIndex + i)%POINT_NUM;

            float h = m_scale * deltaTimes[j];
            h = min(m_height, h);
            p.y = m_height - h;

            m_LineProfile->SetPoint(p, i);
        }
    }
}
