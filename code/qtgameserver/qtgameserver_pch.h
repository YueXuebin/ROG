#include <QtGui>

#include "sdu.h"

#include "commonlib.h"

// collision
#include "Collision.h"

// transfer
#include "transfer.h"

// gameholder
#include "gameholder.h"
#include "dungeon_manager.h"
#include "scene.h"
#include "player.h"
#include "monster.h"
#include "boss.h"
#include "npc.h"
#include "trigger.h"
#include "ai_controller.h"

extern float g_Scale;

const QColor DefaultColor(0, 0, 0);
const QColor PlayerColor(0, 220, 0);
const QColor MonsterColor(220, 0, 0);
const QColor BossColor(255, 120, 0);
const QColor NpcColor(0, 0, 220);
const QColor PetColor(0, 100, 0);
const QColor TriggerColor(0, 255, 255);
const QColor TriggerActiveColor(50, 200, 200);