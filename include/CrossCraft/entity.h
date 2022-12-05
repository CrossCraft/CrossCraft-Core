#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef void (*HitHandler)(int damage, MCVector3 from, bool player);
typedef void (*CollisionTest)();
typedef void (*UpdateHandler)();

void EntityOnHit(int damage, MCVector3 from, bool player);
void EntityCollisionTest();

typedef enum {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ARROW = 1,
    ENTITY_TYPE_DROP = 2,
    ENTITY_TYPE_TNT = 3,
    ENTITY_TYPE_MOB = 4
} EntityType;

// Base Entity
typedef struct {
    EntityType eType;
    MCVector3 pos;
    MCVector3 vel;
    MCVector3 acc;
    MCVector3 size;
    MCVector2 rot;

    bool is_falling;

    HitHandler onHit = EntityOnHit;
    CollisionTest testCollide = EntityCollisionTest;
} Entity;


void ArrowUpdate();

// Arrows
typedef struct {
    Entity e;
    bool playerArrow;      
    uint16_t lifeTime;

    UpdateHandler update = ArrowUpdate;
} Arrow;

void DropUpdate();

typedef struct {
    Entity e;
    SlotData item;

    UpdateHandler update = DropUpdate;
} Drop;

void TNTUpdate();

typedef struct {
    Entity e;
    uint16_t fuse;
    bool dead;
    uint16_t immune;

    UpdateHandler update = TNTUpdate;
} TNT;



#ifdef __cplusplus
};
#endif