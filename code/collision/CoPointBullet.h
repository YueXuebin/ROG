/*
 *	射线性子弹
 */
#ifndef CoPointBullet_h__
#define CoPointBullet_h__

#include "CoBullet.h"

class CoPointBullet : public CoBullet
{
	friend class CoWorld;

	CoPointBullet(CoWorld* pCoWorld, CoBulletOwner* pOwner);
	virtual ~CoPointBullet();

public:
	virtual bool	CollisionTestWithCell();
	virtual void	CollisionTestWithObject();

	void	SetPos(real x, real y);

private:
	real	m_PosX, m_PosY;
	real	m_LastPosX, m_LastPosY;
};

#endif // CoPointBullet_h__
