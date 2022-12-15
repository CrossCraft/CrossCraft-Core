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
    uint16_t eID;
    EntityBase base;
    void* next;
} Entity;

// Arrows
typedef struct {
    uint16_t lifeTime;
    bool playerFired;
    bool hit;

    UpdateHandler update;
} Arrow;

typedef struct {
    uint16_t lifeTime;
    SlotData item;

    UpdateHandler update;
} Drop;

typedef struct {
    uint16_t lifeTime;

    UpdateHandler update;
} TNT;

void CrossCraft_EntityMan_Init();
void CrossCraft_EntityMan_Deinit();
void CrossCraft_EntityMan_AddEntity(void* e);

void EntityUpdate(Entity* e);
void ArrowUpdate(Entity* e);
void DropUpdate(Entity* e);
void TNTUpdate(Entity* e);
void CrossCraft_EntityMan_Tick();

Entity* CrossCraft_Entity_CreateArrow(MCVector3 position, MCVector3 velocity, bool playerFired);
Entity* CrossCraft_Entity_CreateDrop(MCVector3 position, MCVector3 velocity, SlotData* data);
Entity* CrossCraft_Entity_CreateTNT(MCVector3 position, MCVector3 velocity, uint16_t fuse);


#ifdef __cplusplus
};
#endif