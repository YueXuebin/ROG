#ifndef notifier_utils_h__
#define notifier_utils_h__

class SceneObject;
class Player;
class Scene;
class BattleUnit;
class Pet;
class OfflinePlayer;
class Monster;
class Boss;
class Npc;

class Trigger;
class Touch;
class DropObject;
class ItemBase;

// battle unit appear
void FILL_MSG_APPEAR_LIST(BATTLEUNIT_APPEAR_LIST& sendMsg, BattleUnit* pBattleUnit, Player* SeePlayer);

void FILL_PLAYER_APPEAR(PLAYER_APPEAR& PlayerAppear, Player* pPlayer);
void FILL_PET_APPEAR(PET_APPEAR& PetAppear, Pet* pPet);
void FILL_OFFLINE_PLAYER_APPEAR(OFFLINE_PLAYER_APPEAR& OfflinePlayerAppear, OfflinePlayer* pOfflinePlayer);
void FILL_MONSTER_APPEAR(MONSTER_APPEAR& MonsterAppear, Monster* pMonster);
void FILL_BOSS_APPEAR(BOSS_APPEAR& BossAppear, Boss* pBoss);
//void FILL_NPC_APPEAR(NPC_APPEAR& NpcAppear, Npc* pNpc);
void FILL_BATTLEUNIT_APPEAR(BATTLEUNIT_APPEAR& appear, BattleUnit* pBattleUnit);


// scene object appear
void FILL_MSG_SCENEOBJECT_APPEAR_LIST(SCENEOBJECT_APPEAR_LIST& sendMsg, SceneObject* pSceneObject, Player* SeePlayer);

void FILL_TRIGGER_APPEAR(TRIGGER_APPEAR& TriggerAppear, Trigger* pTrigger, Player* SeePlayer);
void FILL_TOUCH_APPEAR(TOUCH_APPEAR& TouchAppear, Touch* pTouch);
void FILL_DROP_OBJECT_APPEAR(DROP_OBJECT_APPEAR& DropObjectAppear, DropObject* pDropObject);
void FILL_SCENEOBJECT_APPEAR(SCENEOBJECT_APPEAR& appear, SceneObject* pSceneObject);

void FILL_ITEM_APPEAR(ITEM_INFO& item_info, const ItemBase* pItemBase);

#endif // notifier_utils_h__
