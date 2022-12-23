#include <CrossCraft/player.h>
#include <CrossCraft/world.h>

PlayerInventory  CCplayerInv;
MCVector3 CCplayerPos;

void CrossCraft_Player_Init() {
    LevelMap* map = CrossCraft_World_GetMapPtr();

    CCplayerPos.x = map->spawnX + 0.5f;
    CCplayerPos.y = map->spawnY + 0.5f;
    CCplayerPos.z = map->spawnZ + 0.5f;

    for(int i = 0; i < 44; i++) {
        CCplayerInv.slots[i].type = -1;
        CCplayerInv.slots[i].damage = 0;
        CCplayerInv.slots[i].count = 0;
    }

    CCplayerInv.slots[8].type = 50;
    CCplayerInv.slots[8].count = 10;
    CCplayerInv.slots[8].damage = 0;

    CCplayerInv.slots[7].type = 46;
    CCplayerInv.slots[7].count = 10;
    CCplayerInv.slots[7].damage = 0;
}

void CrossCraft_Player_Deinit() {

}

MCVector3 CrossCraft_Player_GetPosition() {
    return CCplayerPos;
}

void CrossCraft_Player_SetPosition(MCVector3 pos) {
    CCplayerPos = pos;
}


PlayerInventory CrossCraft_Player_GetInventory() {
    return CCplayerInv;
}

void CrossCraft_Player_SetInventory(PlayerInventory* playerInventory) {
    CCplayerInv = *playerInventory;
}