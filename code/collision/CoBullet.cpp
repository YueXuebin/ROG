#include "CollisionPCH.h"
#include "CoBullet.h"


CoBullet::CoBullet(CoWorld* pCoWorld, CoBulletOwner* pOwner) :
m_pCoWorld(pCoWorld),
m_pOwner(pOwner)
{

}

CoBullet::~CoBullet()
{

}
