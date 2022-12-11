#include <stdio.h>
#include <CrossCraft/world.h>
#include <queue>

struct LightNode {
    uint16_t x, y, z;
    LightNode(uint16_t lx, uint16_t ly, uint16_t lz, uint16_t l) : x(lx), y(ly), z(lz), val(l) {}
    uint16_t val;
};

std::queue<LightNode> lightBfsQueue;
std::queue <LightNode> lightRemovalBfsQueue;

std::queue <LightNode> sunlightBfsQueue;
std::queue <LightNode> sunlightRemovalBfsQueue;


auto encodeID(uint16_t x, uint16_t z) -> uint32_t {
    uint16_t nx = x / 16;
    uint16_t ny = z / 16;
    uint32_t id = nx << 16 | (ny & 0xFFFF);
    return id;
}

auto checkAddID(uint32_t* updateIDs, uint32_t id) -> void {
    // Check that the ID is not already existing
    for(int i = 0; i < 10; i++) {
        if(id == updateIDs[i])
            return;
    }

    // Find a slot to insert.
    for(int i = 0; i < 10; i++) {
        if(updateIDs[i] == 0xFFFFFFFF) {
            updateIDs[i] = id;
            return;
        }
    }
}

auto updateID(uint16_t x, uint16_t z, uint32_t* updateIDs) -> void {
    checkAddID(updateIDs, encodeID(x, z));
}

auto propagate(uint16_t x, uint16_t y, uint16_t z, uint16_t lightLevel, uint32_t* updateIDs) -> void {
    auto map = CrossCraft_World_GetMapPtr();
    if(!BoundCheckMap(map, x, y, z))
        return;

    updateID(x, z, updateIDs);

    auto blk = GetBlockFromMap(map, x, y, z);

    if((blk == 0 || blk == 20 || blk == 18 || (blk >= 8 && blk <= 11) || (blk >= 37 && blk <= 40)) && GetLightFromMap(map, x, y, z) + 2 <= lightLevel) {
        if(blk == 18 || (blk >= 8 && blk <= 11) || (blk >= 37 && blk <= 40)) {
            lightLevel -= 2;
        }
        SetLightInMap(map, x, y, z, lightLevel - 1);
        lightBfsQueue.emplace(x, y, z, 0);
    }
}

auto propagate(uint16_t x, uint16_t y, uint16_t z, uint16_t lightLevel) -> void {
    auto map = CrossCraft_World_GetMapPtr();
    if(!BoundCheckMap(map, x, y, z))
        return;

    if(GetBlockFromMap(map, x, y, z) == 0 && GetLightFromMap(map, x, y, z) + 2 <= lightLevel) {
        SetLightInMap(map, x, y, z, lightLevel - 1);
        sunlightBfsQueue.emplace(x, y, z, 0);
    }
}

auto propagateRemove(uint16_t x, uint16_t y, uint16_t z, uint16_t lightLevel, uint32_t* updateIDs) -> void {
    auto map = CrossCraft_World_GetMapPtr();
    if(!BoundCheckMap(map, x, y, z))
        return;

    auto neighborLevel = GetLightFromMap(map, x, y, z);

    updateID(x, z, updateIDs);

    if(neighborLevel != 0 && neighborLevel < lightLevel) {
        SetLightInMap(map, x, y, z, 0);
        lightRemovalBfsQueue.emplace(x, y, z, neighborLevel);
    } else if (neighborLevel >= lightLevel) {
        lightBfsQueue.emplace(x, y, z, 0);
    }
}

auto propagateRemove(uint16_t x, uint16_t y, uint16_t z, uint16_t lightLevel) -> void {
    auto map = CrossCraft_World_GetMapPtr();
    if(!BoundCheckMap(map, x, y, z))
        return;

    auto neighborLevel = GetLightFromMap(map, x, y, z);

    if(neighborLevel != 0 && neighborLevel < lightLevel) {
        SetLightInMap(map, x, y, z, 0);
        sunlightRemovalBfsQueue.emplace(x, y, z, neighborLevel);
    } else if (neighborLevel >= lightLevel) {
        sunlightBfsQueue.emplace(x, y, z, 0);
    }
}


auto updateRemove(uint32_t* updateIDs) -> void {
    while (!lightRemovalBfsQueue.empty()) {
        auto node = lightRemovalBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        uint8_t lightLevel = node.val;
        lightRemovalBfsQueue.pop();

        propagateRemove(nx + 1, ny, nz, lightLevel, updateIDs);
        propagateRemove(nx - 1, ny, nz, lightLevel, updateIDs);
        propagateRemove(nx, ny + 1, nz, lightLevel, updateIDs);
        propagateRemove(nx, ny - 1, nz, lightLevel, updateIDs);
        propagateRemove(nx, ny, nz + 1, lightLevel, updateIDs);
        propagateRemove(nx, ny, nz - 1, lightLevel, updateIDs);
    }
}

auto updateRemove() -> void {
    while (!lightRemovalBfsQueue.empty()) {
        auto node = lightRemovalBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        uint8_t lightLevel = node.val;
        lightRemovalBfsQueue.pop();

        propagateRemove(nx + 1, ny, nz, lightLevel);
        propagateRemove(nx - 1, ny, nz, lightLevel);
        propagateRemove(nx, ny + 1, nz, lightLevel);
        propagateRemove(nx, ny - 1, nz, lightLevel);
        propagateRemove(nx, ny, nz + 1, lightLevel);
        propagateRemove(nx, ny, nz - 1, lightLevel);
    }
}


auto updateSpread(uint32_t* updateIDs) -> void {
    while (!lightBfsQueue.empty()) {
        auto node = lightBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        uint8_t lightLevel = GetLightFromMap(CrossCraft_World_GetMapPtr(), nx, ny, nz);
        lightBfsQueue.pop();

        propagate(nx + 1, ny, nz, lightLevel, updateIDs);
        propagate(nx - 1, ny, nz, lightLevel, updateIDs);
        propagate(nx, ny + 1, nz, lightLevel, updateIDs);
        propagate(nx, ny - 1, nz, lightLevel, updateIDs);
        propagate(nx, ny, nz + 1, lightLevel, updateIDs);
        propagate(nx, ny, nz - 1, lightLevel, updateIDs);
    }
}


auto updateSpread() -> void {
    while (!lightBfsQueue.empty()) {
        auto node = lightBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        uint8_t lightLevel = GetLightFromMap(CrossCraft_World_GetMapPtr(), nx, ny, nz);
        lightBfsQueue.pop();

        propagate(nx + 1, ny, nz, lightLevel);
        propagate(nx - 1, ny, nz, lightLevel);
        propagate(nx, ny + 1, nz, lightLevel);
        propagate(nx, ny - 1, nz, lightLevel);
        propagate(nx, ny, nz + 1, lightLevel);
        propagate(nx, ny, nz - 1, lightLevel);
    }
}

auto updateSunlight() -> void {
    while (!sunlightBfsQueue.empty()) {
        auto node = sunlightBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        int8_t lightLevel = node.val - 1;
        sunlightBfsQueue.pop();
        if(lightLevel <= 0)
            continue;

        propagate(nx + 1, ny, nz, lightLevel);
        propagate(nx - 1, ny, nz, lightLevel);
        propagate(nx, ny + 1, nz, lightLevel);
        propagate(nx, ny - 1, nz, lightLevel);
        propagate(nx, ny, nz + 1, lightLevel);
        propagate(nx, ny, nz - 1, lightLevel);
    }
}

auto updateSunlightRemove() -> void {
    while (!sunlightRemovalBfsQueue.empty()) {
        auto node = sunlightRemovalBfsQueue.front();

        uint16_t nx = node.x;
        uint16_t ny = node.y;
        uint16_t nz = node.z;
        int8_t lightLevel = node.val;
        sunlightRemovalBfsQueue.pop();
        if(lightLevel <= 0)
            continue;


        propagateRemove(nx + 1, ny, nz, lightLevel);
        propagateRemove(nx - 1, ny, nz, lightLevel);
        propagateRemove(nx, ny + 1, nz, lightLevel);
        propagateRemove(nx, ny - 1, nz, lightLevel);
        propagateRemove(nx, ny, nz + 1, lightLevel);
        propagateRemove(nx, ny, nz - 1, lightLevel);
    }
}

extern "C" {
void CrossCraft_World_AddLight(uint16_t x, uint16_t y, uint16_t z, uint16_t light, uint32_t* updateIDs) {
    SetLightInMap(CrossCraft_World_GetMapPtr(), x, y, z, light);
    updateID(x, z, updateIDs);
    lightBfsQueue.emplace(x, y, z, light);

    updateSpread(updateIDs);
}


void CrossCraft_World_RemoveLight(uint16_t x, uint16_t y, uint16_t z, uint16_t light, uint32_t* updateIDs) {
    auto map = CrossCraft_World_GetMapPtr();

    auto val = GetLightFromMap(map, x, y, z);
    lightRemovalBfsQueue.emplace(x, y, z, val);

    SetLightInMap(map, x, y, z, light);
    updateID(x, z, updateIDs);

    updateRemove(updateIDs);
    updateSpread(updateIDs);
}

void singleCheck(uint16_t x, uint16_t y, uint16_t z) {
    auto map = CrossCraft_World_GetMapPtr();

    if(y == 0)
        return;

    auto lv = 15;
    for(int y2 = map->height - 1; y2 >= 0; y2--) {
        auto blk = GetBlockFromMap(map, x, y2, z);

        if(blk == 18 || (blk >= 37 && blk <= 40) || (blk >= 8 && blk <= 11)) {
            lv -= 2;
        } else if(blk != 0 && blk != 20) {
            lv = 0;
        }

        auto lv2 = GetLightFromMap(map, x, y2, z);

        if(lv2 < lv) {
            SetLightInMap(map, x, y2, z, lv);
            sunlightRemovalBfsQueue.emplace(x, y2, z, 0);
        } else {
            SetLightInMap(map, x, y2, z, lv);
            sunlightBfsQueue.emplace(x, y2, z, lv);
        }
    }
}

bool CrossCraft_World_CheckSunLight(uint16_t x, uint16_t y, uint16_t z) {
    singleCheck(x, y, z);
    singleCheck(x + 1, y, z);
    singleCheck(x - 1, y, z);
    singleCheck(x, y, z + 1);
    singleCheck(x, y, z - 1);

    updateSunlightRemove();
    updateSunlight();

    return true;
}

void CrossCraft_World_PropagateSunLight(uint32_t tick) {
    auto map = CrossCraft_World_GetMapPtr();

    for(int x = 0; x < map->length; x++) {
        for(int z = 0; z < map->width; z++) {
            auto lv = 4;
            if(tick >= 0 && tick <= 12000) {
                lv = 15;
            }

            for(int y = map->height - 1; y >= 0; y--) {
                auto blk = GetBlockFromMap(map, x, y, z);

                if(blk == 18 || (blk >= 37 && blk <= 40) || (blk >= 8 && blk <= 11)) {
                    lv -= 2;
                } else if(blk != 0 && blk != 20) {
                    break;
                }

                if(lv < 0)
                    break;

                SetLightInMap(map, x, y, z, lv);
                sunlightBfsQueue.emplace(x, y, z, lv);
            }
        }
    }

    updateSunlight();

}

}
