#include "navmesh_pch.h"
#include "newfunnel.h"
#include "portal.h"
#include "pool_manager.h"

NewFunnel::NewFunnel(void)
{
}

NewFunnel::~NewFunnel(void)
{
    for(std::vector<Portal*>::iterator iter = m_portalsList.begin(); iter != m_portalsList.end(); iter++)
    {
        //CnDelete *iter;
        g_PoolManager.DestoryPorTal(*iter);
    }
    m_portalsList.clear();
}

void NewFunnel::PushPortal(CoVec2* p1 , CoVec2* p2)
{
    if(p2 == NULL)
        p2 = p1;

    //Portal* curPortal = CnNew Portal(*p1, *p2);
    Portal* curPortal = g_PoolManager.CreatePorTal(*p1, *p2);
    m_portalsList.push_back(curPortal);
}

real NewFunnel::GetTriarea2(const CoVec2& a, const CoVec2& b, const CoVec2& c)
{
    real ax = b.x -a.x;
    real ay = b.y - a.y;
    real bx = c.x - a.x;
    real by = c.y - a.y;

    return (bx * ay - ax * by);
}

bool NewFunnel::GetVequal(const CoVec2& pointA , const CoVec2& pointB)
{
    return (Crown::Distance(pointA.x, pointA.y, pointB.x, pointB.y) < (0.001 * 0.001));
}

//拐角点法 找出最近的拐点
void NewFunnel::GetPathPoint(std::vector<CoVec2>& pathsVec)
{
     CoVec2 portalApex = m_portalsList[0]->GetLeft();
     CoVec2 portalLeft = m_portalsList[0]->GetLeft();
     CoVec2 protalRight = m_portalsList[0]->GetRight();

     int32 apexIndex = 0, leftIndex = 0, rightIndex = 0;
     //add start point
     pathsVec.push_back(portalApex);

     for (uint32 i= 1; i< m_portalsList.size(); i++)
     {
         CoVec2 left = m_portalsList[i]->GetLeft();
         CoVec2 right = m_portalsList[i]->GetRight();
         CoVec2 last = pathsVec[pathsVec.size() - 1];

         //update right 
         if(GetTriarea2(portalApex, protalRight, right) <= 0.0)
         {
             if(GetVequal(portalApex, protalRight) || GetTriarea2(portalApex, portalLeft, right) > 0.0)
             {
                 // Tighten the funnel.
                 protalRight = right;
                 rightIndex = i;
             }
             else
             {
                 // Right over left, insert left to path and restart scan from portal left point.
                 if(!IsEqual(last, portalLeft))
                 {
                     pathsVec.push_back(portalLeft);
                     // Make current left the new apex.
                     portalApex =  portalLeft;
                     apexIndex = leftIndex;

                     // Reset portal
                     portalLeft = portalApex;
                     protalRight = portalApex;
                     leftIndex = apexIndex;
                     rightIndex = apexIndex;
                     // resart scan
                     i = apexIndex;
                     continue;
                 }
             }
         }

         // update left vertex
         if(GetTriarea2(portalApex, portalLeft, left) >= 0.0)
         {
             if(GetVequal(portalApex, portalLeft) || GetTriarea2(portalApex, protalRight, left) < 0.0)
             {
                 // Tighten the funnel.
                 portalLeft = left;
                 leftIndex = i;
             }
             else
             {
                 // left over right, insert right to path and restart scan from portal right point.
                 if(!IsEqual(last, protalRight))
                 {
                     pathsVec.push_back(protalRight);
                     // Make current right the new apex.
                     portalApex =  protalRight;
                     apexIndex = rightIndex;

                     // Reset portal
                     portalLeft = portalApex;
                     protalRight = portalApex;
                     leftIndex = apexIndex;
                     rightIndex = apexIndex;
                     // resart scan
                     i = apexIndex;
                     continue;
                 }
             }
         }
     
   } // end for

     if(pathsVec.size() == 0 || (! GetVequal(pathsVec[pathsVec.size() - 1], m_portalsList[m_portalsList.size() - 1]->GetLeft() )))
     {
         pathsVec.push_back(m_portalsList[m_portalsList.size() - 1]->GetLeft());
     }
}
