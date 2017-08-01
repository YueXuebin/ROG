#include "GridWorldPCH.h"
#include "GridCell.h"
#include "GridObject.h"



// 将对象挂到链表上
void GridCell::AddObject(GridObject* object)
{
    CnAssert(!object->m_Next);
    if(!m_ObjectLink)
    {
        m_ObjectLink = object;
    }
    else
    {
        GridObject* last = m_ObjectLink;
        while(last->m_Next)
        {
            last = last->m_Next;
        }

        last->m_Next = object;
    }
}
// 将对象从链表上取下
void GridCell::RemoveObject(GridObject* object)
{
    CnAssert(m_ObjectLink);
    GridObject* here = m_ObjectLink;
    GridObject* last = NULL;

    bool find = false;
    do 
    {
        if(here == object)
        {
            find = true;
            break;
        }

        last = here;
        here = here->m_Next;
    } 
    while (here);

    if(!find)
    {
        CnAssert(false);
        return;
    }

    if(!last)
    {
        CnAssert(here == m_ObjectLink);
        m_ObjectLink = m_ObjectLink->m_Next;
        object->m_Next = NULL;
    }
    else
    {
        CnAssert(here != m_ObjectLink);
        CnAssert(last->m_Next == object);
        CnAssert(here == object);
        last->m_Next = here->m_Next;
        object->m_Next = NULL;
    }
}