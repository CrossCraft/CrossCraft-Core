#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    float x, y, z;
} MCVector3;

typedef struct{
    uint32_t x, y, z;
} MCIVector3;

typedef struct{
    float x, y;
} MCVector2;

typedef struct {
    uint16_t type;
    uint16_t damage;
    int8_t count;
} SlotData;

typedef struct {
    SlotData slots[44];
} PlayerInventory;

typedef struct {
    SlotData slots[27];
} TileInventory;

typedef struct {
    uint32_t type;

    MCVector3 pos;
    MCVector2 rotation;
    MCVector3 motion;

    float fallDistance;

    uint16_t attackTime; // Immune ticks
    uint16_t hurtTime; // Time the entity is red
    uint16_t deathTime; // Number of ticks of death

    uint16_t air; // Number of ticks before player drowns
    int16_t fire; // Negative = number of ticks before fire; Positive = number of ticks before extinguished

    //Last two here are not used for entities other than player
    uint32_t score; // Player's score
    PlayerInventory* inventory;
} EntityData;

typedef struct {
    uint32_t type;
    uint32_t pos;

    TileInventory* items;
} TileEntityData;

#ifdef __cplusplus
};
#endif