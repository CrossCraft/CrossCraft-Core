#include <CrossCraft/entity.h>
#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
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

Entity** CrossCraft_EntityMan_GetEntityList() {
    return entityList;
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

void ArrowUpdate(Arrow* e) {
    e->lifeTime -= 1;

    e->base.acc.y = -1.0f;

    MCVector3 vel = e->base.vel;

    if(!e->hit) {
        e->base.update(e);
    }

    if(e->base.vel.x == 0 || e->base.vel.z == 0) {
        e->hit = true;
        e->base.vel = vel;
    }
}

void DropUpdate(Drop* e) {
    e->lifeTime -= 1;
    e->base.acc.y = -32;
    e->base.update(e);
}

void TNTUpdate(TNT* e) {
    e->lifeTime -= 1;
    e->base.acc.y = -32;
    e->base.update(e);

    if(e->lifeTime == 0) {
        //TODO: EXPLODE!
    }
}

static uint8_t deleteCount = 0;
static Entity* deleteArray[32] = {NULL};

void CrossCraft_EntityMan_Tick() {
    for(uint16_t i = 0; i < 1024; i++) {
        if(entityList[i] == NULL)
            continue;

        Entity* e = entityList[i];
        if(e->eType == ENTITY_TYPE_ARROW) {
            Arrow* a = (Arrow*)e;
            a->update(a);

            if(a->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount] = e;
            }
        } else if (e->eType == ENTITY_TYPE_DROP) {
            Drop* d = (Drop*)e;
            d->update(d);

            if(d->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount] = e;
            }
        } else if (e->eType == ENTITY_TYPE_TNT) {
            TNT* t = (TNT*)e;
            t->update(t);

            if(t->lifeTime == 0 && deleteCount < 32) {
                deleteArray[deleteCount] = e;
            }
        }
    }

    for(uint8_t i = 0; i < deleteCount; i++) {
        free(deleteArray[i]);
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

Arrow* CrossCraft_Entity_CreateArrow(MCVector3 position, MCVector3 velocity, bool playerFired) {
    Arrow* a = malloc(sizeof(Arrow));
    a->eType = ENTITY_TYPE_ARROW;
    a->base = create_entity_base(position, velocity);
    a->base.size.x = 0.1f;
    a->base.size.y = 0.1f;
    a->base.size.z = 0.1f;
    a->lifeTime = 1200;
    a->playerFired = playerFired;
    a->update = ArrowUpdate;
    a->hit = false;

    return a;
}

Drop* CrossCraft_Entity_CreateDrop(MCVector3 position, MCVector3 velocity, SlotData* data) {
    Drop* a = malloc(sizeof(Drop));
    a->eType = ENTITY_TYPE_DROP;
    a->base = create_entity_base(position, velocity);
    a->lifeTime = 1200;
    a->item = *data;
    a->update = DropUpdate;

    return a;
}

TNT* CrossCraft_Entity_CreateTNT(MCVector3 position, MCVector3 velocity, uint16_t fuse) {
    TNT* a = malloc(sizeof(TNT));
    a->eType = ENTITY_TYPE_DROP;
    a->base = create_entity_base(position, velocity);
    a->lifeTime = fuse;
    a->update = TNTUpdate;

    return a;
}

int CrossCraft_EntityMan_AddEntity(void* e) {
    CC_Internal_Log_Message(CC_LOG_DEBUG, "Created Entity!");
    do {
        eCounter++;
    } while(entityList[eCounter] != NULL);

    entityList[eCounter] = e;

    return eCounter;
}