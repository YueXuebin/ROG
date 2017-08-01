/*
 *	物理引擎头文件
 */
#ifndef Collision_h__
#define Collision_h__

#include "CoWorld.h"
#include "CoObject.h"
#include "CoDynamicObject.h"
#include "CoBullet.h"
#include "CoPointBullet.h"

// 地图信息, 客户端和服务端相同
CoWorld* MakeCollisionWorld(const char* mapFileName);

#endif // Collision_h__
