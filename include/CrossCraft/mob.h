#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "entity.h"

typedef enum {
    MOB_TYPE_PIG,
    MOB_TYPE_SHEEP,
    MOB_TYPE_ZOMBIE,
    MOB_TYPE_CREEPER,
    MOB_TYPE_SKELETON,
    MOB_TYPE_SPIDER
} MobType;

typedef struct {
    MobType type;

    uint16_t health;
    uint16_t air; // Number of ticks before drowns
    int16_t fire; // Negative = number of ticks before fire; Positive = number of ticks before extinguished

    uint16_t immuneTime;
    uint16_t hurtTime;
    uint16_t deathTime;

    UpdateHandler update;

    void* next;
} Mob;

typedef struct {
    bool hasWool;
} Sheep;

typedef struct {
    uint8_t shotTimer;
} Skeleton;

void MobUpdate(Entity* e);

//TODO: Creeper Fuse?

#ifdef __cplusplus
};
#endif