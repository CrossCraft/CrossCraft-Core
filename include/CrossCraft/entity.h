#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef void (*UpdateHandler)();

typedef enum {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ARROW = 1,
    ENTITY_TYPE_DROP = 2,
    ENTITY_TYPE_TNT = 3,
    ENTITY_TYPE_MOB = 4
} EntityType;

// Base Entity
typedef struct {
    MCVector3 pos;
    MCVector3 vel;
    MCVector3 acc;
    MCVector3 size;
    MCVector2 rot;

    bool is_falling;
    bool is_water;

    UpdateHandler update;
} EntityBase;

typedef struct {
    EntityType eType;
    EntityBase base;
} Entity;

// Arrows
typedef struct {
    EntityType eType;
    EntityBase base;
    bool playerFired;
    uint16_t lifeTime;
    bool hit;

    UpdateHandler update;
} Arrow;

typedef struct {
    EntityType eType;
    EntityBase base;
    SlotData item;
    uint16_t lifeTime;

    UpdateHandler update;
} Drop;

typedef struct {
    EntityType eType;
    EntityBase base;
    uint16_t lifeTime;

    UpdateHandler update;
} TNT;

void CrossCraft_EntityMan_Init();
void CrossCraft_EntityMan_Deinit();
int CrossCraft_EntityMan_AddEntity(void* e);
Entity** CrossCraft_EntityMan_GetEntityList();

void EntityUpdate(Entity* e);
void ArrowUpdate(Arrow* e);
void DropUpdate(Drop* e);
void TNTUpdate(TNT* e);
void CrossCraft_EntityMan_Tick();

Arrow* CrossCraft_Entity_CreateArrow(MCVector3 position, MCVector3 velocity, bool playerFired);
Drop* CrossCraft_Entity_CreateDrop(MCVector3 position, MCVector3 velocity, SlotData* data);
TNT* CrossCraft_Entity_CreateTNT(MCVector3 position, MCVector3 velocity, uint16_t fuse);


#ifdef __cplusplus
};
#endif