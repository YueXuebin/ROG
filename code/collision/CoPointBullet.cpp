#include "CollisionPCH.h"
#include "CoMath.h"
#include "CoPointBullet.h"
#include "CoWorld.h"
#include "CoDynamicObject.h"

CoPointBullet::CoPointBullet(CoWorld* pCoWorld, CoBulletOwner* pOwner) :
CoBullet(pCoWorld, pOwner)
{
	m_LastPosX = m_LastPosY = m_PosX = m_PosY = 0.0f;
}

CoPointBullet::~CoPointBullet()
{

}

void CoPointBullet::SetPos(real x, real y)
{
	m_LastPosX = m_PosX;
	m_LastPosY = m_PosY;
	m_PosX = x;
	m_PosY = y;

	DBG("Bullet (%f, %f)", m_PosX, m_PosY);
}

bool CoPointBullet::CollisionTestWithCell()
{
	// 计算直线的BoundingBox
	real left, right;
	if(m_PosX > m_LastPosX)
	{
		left = m_LastPosX; right = m_PosX;
	}
	else
	{
		left = m_PosX; right = m_LastPosX;
	}
	real bottom, top;
	if(m_PosY > m_LastPosY)
	{
		bottom = m_PosY; top = m_LastPosY;
	}
	else
	{
		bottom = m_LastPosY; top = m_PosY;
	}

	// 是否超出地图范围
	if( left < m_pCoWorld->GetMinX() || right > m_pCoWorld->GetMaxX() || top < m_pCoWorld->GetMinY() || bottom > m_pCoWorld->GetMaxY() )
		return true;

	// 计算到直线方程 Ax+By+C=0
	real A = m_PosY - m_LastPosY;							// A = y2-y1
	real B = m_LastPosX - m_PosX;							// B = x1-x2
	real C = m_PosX * m_LastPosY - m_LastPosX * m_PosY;		// C = x2y1-x1y2

	// 计算与直线相交的Cell，是否阻挡
	int CMinX = (int)(left / m_pCoWorld->GetCellLength());
	int CMinY = (int)(top / m_pCoWorld->GetCellLength());
	int CMaxX = (int)(right / m_pCoWorld->GetCellLength());
	int CMaxY = (int)(bottom / m_pCoWorld->GetCellLength());

	for(int y = CMinY; y <= CMaxY; y ++)
	{
		for(int x = CMinX; x <= CMaxX; x++)
		{
			// 该Cell是否与直线相交
			CoVec2 p1( (real)x * m_pCoWorld->GetCellLength(), (real)y * m_pCoWorld->GetCellLength());
			CoVec2 p2( (real)(x+1) * m_pCoWorld->GetCellLength(), (real)y * m_pCoWorld->GetCellLength());
			CoVec2 p3( (real)(x+1) * m_pCoWorld->GetCellLength(), (real)(y+1) * m_pCoWorld->GetCellLength());
			CoVec2 p4( (real)x * m_pCoWorld->GetCellLength(), (real)(y+1) * m_pCoWorld->GetCellLength());

			real r1 = A*p1.x + B * p1.y + C;
			real r2 = A*p2.x + B * p2.y + C;
			real r3 = A*p3.x + B * p3.y + C;
			real r4 = A*p4.x + B * p4.y + C;

			if( ( r1 > 0.0f && r2 > 0.0f && r3 > 0.0f && r4 > 0.0f) ||
				(r1 < 0.0f && r2 < 0.0f && r3 < 0.0f && r4 < 0.0f) )
				continue;			// 该Cell不与直线相交

			if( m_pCoWorld->GetCellGrid(x,y)->GetState() == CELL_BLOCK )
				return true;

		}
	}

	return false;
}

void CoPointBullet::CollisionTestWithObject()
{
	if(!m_pOwner)
	{
		CnVerify(m_pOwner);		// 不要对没有主人的子弹做碰撞检测
		return ;
	}

	// 计算直线的BoundingBox
	real left, right;
	if(m_PosX > m_LastPosX)
	{
		left = m_LastPosX; right = m_PosX;
	}
	else
	{
		left = m_PosX; right = m_LastPosX;
	}
	real bottom, top;
	if(m_PosY > m_LastPosY)
	{
		bottom = m_PosY; top = m_LastPosY;
	}
	else
	{
		bottom = m_LastPosY; top = m_PosY;
	}

	// 计算到直线方程 Ax+By+C=0
	real A = m_PosY - m_LastPosY;							// A = y2-y1
	real B = m_LastPosX - m_PosX;							// B = x1-x2
	real C = m_PosX * m_LastPosY - m_LastPosX * m_PosY;	// C = x2y1-x1y2

	// 计算直线的BoundingBox所占据的Cell
	int CMinX = (int)(left / m_pCoWorld->GetCellLength());
	int CMinY = (int)(top / m_pCoWorld->GetCellLength());
	int CMaxX = (int)(right / m_pCoWorld->GetCellLength());
	int CMaxY = (int)(bottom / m_pCoWorld->GetCellLength());

	if(CMinX >= m_pCoWorld->GetCellMinX()) CMinX = m_pCoWorld->GetCellMinX();
	if(CMinY >= m_pCoWorld->GetCellMinY()) CMinY = m_pCoWorld->GetCellMinY();
	if(CMaxX >= m_pCoWorld->GetCellMaxX()) CMaxX = m_pCoWorld->GetCellMaxX();
	if(CMaxY >= m_pCoWorld->GetCellMaxY()) CMaxY = m_pCoWorld->GetCellMaxY();
	
	for(int y = CMinY; y <= CMaxY; y ++)
	{
		for(int x = CMinX; x <= CMaxX; x++)
		{	// 与Cell中的物理对象做碰撞检测
			CoWorld::Cell* pCellGrid = m_pCoWorld->GetCellGrid(x, y);
			CnVerify(pCellGrid);

			const std::vector<CoDynamicObject*>& Objects = pCellGrid->GetCollObjects();
			for(std::vector<CoDynamicObject*>::const_iterator iter = Objects.begin();
				iter != Objects.end(); ++iter)
			{	// 子弹的直线段 与 物理对象的碰撞检测
				CoDynamicObject* pObject = *iter;

				// 点A(m,n)到 直线ax+by+c=0 距离 d=am+bn+c/sqrt(a^2+b^2) 
				real dis = (A * pObject->GetPosX() + B * pObject->GetPosY() + C) /
					sqrt(A*A + B*B);

				if(dis > pObject->GetRadius())
					continue;		// 没有碰撞
				else
				{	// 子弹与物理对象发生了碰撞
					int32 ret = m_pOwner->OnHit(pObject->GetOwner());
					if(ret < 0)
					{
						// 不再继续碰撞检测了
						y = CMaxY+1;
						break;
					}
					
				}
				

			}	// for x
		}
	}	// for y

}
