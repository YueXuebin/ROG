#include "gameholder_pch.h"
#include "sub_regular.h"
#include "regular.h"

SubRegular::SubRegular(Regular* pRegular) :
m_Regular(pRegular)
{

}

SubRegular::~SubRegular()
{

}

Scene* SubRegular::GetScene()
{
    return m_Regular->GetScene();
}
