/*
 * 连续碰撞检测对象(如子弹)的基类
 */
#ifndef CoBullet_h__
#define CoBullet_h__

class CoWorld;
class CoObjectOwner;

class CoBulletOwner
{
public:
	CoBulletOwner() {}
	virtual ~CoBulletOwner() {}
	
	virtual int32	OnHit(CoObjectOwner* pObjectOwner) = 0;		// return 0 不继续碰撞检测 1 继续碰撞检测

};

class CoBullet
{
	friend class CoWorld;
protected:
	CoBullet(CoWorld* pCoWorld, CoBulletOwner* pOwner);
	virtual ~CoBullet();

public:

	virtual bool	CollisionTestWithCell() = 0;		// 和场景的碰撞检测
	virtual void	CollisionTestWithObject() = 0;		// 和物理对象的碰撞检测

protected:
	CoWorld* m_pCoWorld;
	CoBulletOwner*	m_pOwner;

private:
	real m_PosX, m_PosY;		// 位置
};



#endif // CoBullet_h__
