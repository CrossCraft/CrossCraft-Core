#include <CrossCraft/entity.h>
#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/event.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static Entity* entityList[1024];
static uint16_t eCounter = 0;

void CrossCraft_EntityMan_Init() {
    for(uint16_t i = 0; i < 1024; i++){
        entityList[i] = NULL;
    }
}

void CrossCraft_EntityMan_Deinit() {
    for(uint16_t i = 0; i < 1024; i++){
        if(entityList[i] != NULL) {
            free(entityList[i]);
        }
    }
}

bool test(MCIVector3 pos) {
    LevelMap* map = CrossCraft_World_GetMapPtr();
    if(BoundCheckMap(map, pos.x >> 5, pos.y >> 5, pos.z >> 5)) {
        uint8_t blk = GetBlockFromMap(map , pos.x >> 5, pos.y >> 5, pos.z >> 5);
        return blk != 0 && blk != 8 && blk != 9 && blk != 11 && blk != 6 && blk != 37 && blk != 38 &&
               blk != 39 && blk != 40 && blk != 10;
    } else {
        return false;
    }
}

void test_collide(EntityBase* e) {
    bool testX = false;
    bool testY = false;
    bool testZ = false;

    int xMin = (int) (e->pos.x - e->size.x / 2.0f);
    int xMax = (int) (e->pos.x + e->size.x / 2.0f);
    int yMin = (int) (e->pos.y - e->size.y);
    int yMax = (int) (e->pos.y);
    int zMin = (int) (e->pos.z - e->size.z / 2.0f);
    int zMax = (int) (e->pos.z + e->size.z / 2.0f);

    {
        int x;
        if (e->vel.x < 0.0) {
            x = (int) (e->pos.x - 0.3f + e->vel.x);
            testX = true;
        } else if (e->vel.x > 0.0) {
            x = (int) (e->pos.x + 0.3f + e->vel.x);
            testX = true;
        }
        if (testX) {
            for (int y = yMin; y <= yMax; y++) {
                for (int z = zMin; z <= zMax; z++) {
                    MCIVector3 pos = {x, y, z};
                    if (test(pos)) {
                        e->vel.x = 0;
                    }
                }
            }
        }
    }

    {
        int y;
        if (e->vel.y < 0.0) {
            y = (int) (e->pos.y - 1.8f + e->vel.y);
            testY = true;
        } else if (e->vel.y > 0.0) {
            y = (int) (e->pos.y + e->vel.y);
            testY = true;
        }

        if (testY) {
            for (int x = xMin; x <= xMax; x++) {
                for (int z = zMin; z <= zMax; z++) {
                    MCIVector3 pos = {x, y, z};
                    if (test(pos)) {
                        e->vel.y = 0;
                        e->is_falling = false;
                    }
                }
            }
        }
    }

    {
        int z;
        if (e->vel.z < 0.0) {
            z = (int) (e->pos.z - 0.3f + e->vel.z);
            testZ = true;
        } else if (e->vel.z > 0.0) {
            z = (int) (e->pos.z + 0.3f + e->vel.z);
            testZ = true;
        }

        if (testZ) {
            for (int x = xMin; x <= xMax; x++) {
                for (int y = yMin; y <= yMax; y++) {
                    MCIVector3 pos = {x, y, z};
                    if (test(pos)) {
                        e->vel.z = 0;
                    }
                }
            }
        }
    }

    MCIVector3 pos = {e->pos.x, e->pos.y , e->pos.z};
    if(BoundCheckMap(CrossCraft_World_GetMapPtr(), pos.x >> 5, pos.y >> 5, pos.z >> 5))
    {
        uint8_t blk = GetBlockFromMap(CrossCraft_World_GetMapPtr(), pos.x >> 5, pos.y >> 5, pos.z >> 5);
        if(blk == 8 || blk == 9){
            e->is_water = true;
        }
    }
}

void EntityUpdate(Entity* e) {
    e->base.vel.x += e->base.acc.x;
    e->base.vel.y += e->base.acc.y;
    e->base.vel.z += e->base.acc.z;

    e->base.is_falling = true;

    test_collide(&e->base);

    float mul = 1.0f;
    if(e->base.is_water)
        mul = 0.7f;

    e->base.pos.x += e->base.vel.x * mul;
    e->base.pos.y += e->base.vel.y * mul;
    e->base.pos.z += e->base.vel.z * mul;

    e->base.vel.x *= 0.99f;
    e->base.vel.y *= 0.99f;
    e->base.vel.z *= 0.99f;
}

void ArrowUpdate(Entity* e) {
    Arrow* a = (Arrow*)e->next;
    a->lifeTime -= 1;

    e->base.acc.y = -1;
    MCVector3 vel = e->base.vel;

    if(!a->hit) {
        e->base.update(e);
    }

    if(e->base.vel.x == 0 || e->base.vel.z == 0) {
        a->hit = true;
        e->base.vel = vel;
    }
}

void DropUpdate(Entity* e) {
    ((Drop*)e->next)->lifeTime -= 1;
    e->base.acc.y = -1;
    e->base.update(e);
}

void TNTUpdate(Entity* e) {
    ((TNT*)e->next)->lifeTime -= 1;
    e->base.acc.y = -1;
    e->base.update(e);

    if(((TNT*)e->next)->lifeTime == 0) {
        MCVector3 translated = e->base.pos;
        translated.x /= 32.0f;
        translated.y /= 32.0f;
        translated.z /= 32.0f;
        CrossCraft_World_Explode(translated);
    }
}

static uint8_t deleteCount = 0;
static Entity* deleteArray[32] = {NULL};

void CrossCraft_EntityMan_Tick() {
    for(uint16_t i = 0; i < 1024; i++) {
        if(entityList[i] == NULL)
            continue;

        Entity* e = entityList[i];

        struct EventEntity* ee = malloc(sizeof(struct EventEntity));
        ee->type = CROSSCRAFT_EVENT_TYPE_UPDATE_ENTITY;
        ee->e = e;

        if(e->eType == ENTITY_TYPE_ARROW) {
            Arrow* a = (Arrow*)(e->next);
            a->update(e);

            if(a->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount++] = e;
            }
        } else if (e->eType == ENTITY_TYPE_DROP) {
            Drop* d = (Drop*)(e->next);
            d->update(e);

            if(d->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount++] = e;
            }
        } else if (e->eType == ENTITY_TYPE_TNT) {
            TNT* t = (TNT*)(e->next);
            t->update(e);

            if(t->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount++] = e;
            }
        }

        CrossCraft_Event_Push(CROSSCRAFT_EVENT_TYPE_UPDATE_ENTITY, (struct Event*)ee);
    }

    for(uint8_t i = 0; i < deleteCount; i++) {
        struct EventEntityRemove* ee = malloc(sizeof(struct EventEntityRemove));
        ee->eid = deleteArray[i]->eID;
        ee->type = CROSSCRAFT_EVENT_TYPE_REMOVE_ENTITY;

        CrossCraft_Event_Push(CROSSCRAFT_EVENT_TYPE_REMOVE_ENTITY, (struct Event*)ee);

        int eid = deleteArray[i]->eID;

        if(entityList[eid] != NULL)
            free(entityList[eid]->next);

        if(entityList[eid] != NULL)
            free(entityList[eid]);

        entityList[eid] = NULL;
    }
}

EntityBase create_entity_base(MCVector3 position, MCVector3 velocity) {
    EntityBase ebase;

    ebase.pos = position;
    ebase.pos.x *= 32.0f;
    ebase.pos.y *= 32.0f;
    ebase.pos.z *= 32.0f;

    ebase.vel = velocity;
    ebase.vel.x *= 32.0f;
    ebase.vel.y *= 32.0f;
    ebase.vel.z *= 32.0f;

    ebase.acc.x = 0.0f;
    ebase.acc.y = 0.0f;
    ebase.acc.z = 0.0f;

    ebase.size.x = 1.0f;
    ebase.size.y = 1.0f;
    ebase.size.z = 1.0f;

    ebase.update = EntityUpdate;
    ebase.is_water = false;
    ebase.is_falling = false;

    return ebase;
}

Entity* CrossCraft_Entity_CreateArrow(MCVector3 position, MCVector3 velocity, bool playerFired) {
    Entity* e = malloc(sizeof(Entity));
    e->eType = ENTITY_TYPE_ARROW;
    e->base = create_entity_base(position, velocity);
    e->base.size.x = 0.1f;
    e->base.size.y = 0.1f;
    e->base.size.z = 0.1f;
    e->next = malloc(sizeof(Arrow));

    Arrow* a = (Arrow*)e->next;
    a->lifeTime = 1200;
    a->playerFired = playerFired;
    a->update = ArrowUpdate;
    a->hit = false;

    return e;
}

Entity* CrossCraft_Entity_CreateDrop(MCVector3 position, MCVector3 velocity, SlotData* data) {
    Entity* e = malloc(sizeof(Entity));
    e->eType = ENTITY_TYPE_DROP;
    e->base = create_entity_base(position, velocity);
    e->base.size.x = 0.25f;
    e->base.size.y = 0.25f;
    e->base.size.z = 0.25f;
    e->next = malloc(sizeof(Drop));

    Drop* d = (Drop*)e->next;
    d->lifeTime = 6000;
    d->item = *data;
    d->update = DropUpdate;

    return e;
}

Entity* CrossCraft_Entity_CreateTNT(MCVector3 position, MCVector3 velocity, uint16_t fuse) {
    Entity* e = malloc(sizeof(Entity));
    e->eType = ENTITY_TYPE_TNT;
    e->base = create_entity_base(position, velocity);
    e->base.size.x = 0.1f;
    e->base.size.y = 0.1f;
    e->base.size.z = 0.1f;
    e->next = malloc(sizeof(TNT));

    TNT* a = (TNT*)e->next;
    a->lifeTime = fuse;
    a->update = TNTUpdate;

    return e;
}

#include <CrossCraft/mob.h>

uint16_t getHealth(MobType type) {
    switch(type) {
        case MOB_TYPE_PIG:
            return 10;
        case MOB_TYPE_SHEEP:
            return 8;
        case MOB_TYPE_ZOMBIE:
            return 20;
        case MOB_TYPE_CREEPER:
            return 20;
        case MOB_TYPE_SKELETON:
            return 20;
        case MOB_TYPE_SPIDER:
            return 16;
    }
}

Entity* CrossCraft_Entity_CreateMob(MCVector3 position, MCVector3 velocity, int type) {
    Entity* e = malloc(sizeof(Entity));
    e->eType = ENTITY_TYPE_TNT;
    e->base = create_entity_base(position, velocity);
    e->next = malloc(sizeof(Mob));

    Mob* m = (Mob*)e->next;
    m->type = type;
    m->air = 300;
    m->hurtTime = 0;
    m->immuneTime = 0;
    m->deathTime = 0;
    m->fire = -1;
    m->update = MobUpdate;

    m->health = getHealth(type);

    if(m->type == MOB_TYPE_SKELETON) {
        m->next = malloc(sizeof(Skeleton));
        Skeleton* s = m->next;
        s->shotTimer = 0;
    } else if(m->type == MOB_TYPE_SHEEP) {
        m->next = malloc(sizeof(Sheep));
        Sheep* s = m->next;
        s->hasWool = true;
    }

    return e;
}

void CrossCraft_EntityMan_AddEntity(void* e) {
    CC_Internal_Log_Message(CC_LOG_DEBUG, "Created Entity!");
    do {
        eCounter++;
    } while(entityList[eCounter] != NULL);

    entityList[eCounter] = e;
    entityList[eCounter]->eID = eCounter;

    struct EventEntity* ee = malloc(sizeof(struct EventEntity));
    ee->type = CROSSCRAFT_EVENT_TYPE_ADD_ENTITY;
    ee->e = e;
    CrossCraft_Event_Push(CROSSCRAFT_EVENT_TYPE_ADD_ENTITY, (struct Event*)ee);
}