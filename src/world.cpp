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
            printf("%d %d\n", id >> 16, id & 0xFFFF);
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

    if(GetBlockFromMap(map, x, y, z) == 0 && GetLightFromMap(map, x, y, z) + 2 <= lightLevel) {
        SetLightInMap(map, x, y, z, lightLevel - 1);
        lightBfsQueue.emplace(x, y, z, 0);
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

extern "C" {

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

}
