#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/worldgen.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static Level level;

void CrossCraft_World_Init() {
    CC_Internal_Log_Message(CC_LOG_DEBUG, "Generating base level template");
    srand(time(NULL));

    CrossCraft_WorldGenerator_Init(rand());

    LevelAbout abt = {
        .createdOn = time(NULL),
        .name = "A Nice World",
        .author = "Default"
    };

    LevelEnvironment env = {
            .timeOfDay = 1000,
            .skyBrightness = 11,
            .skyColor = 0,
            .fogColor = 0,
            .cloudColor = 0,
            .cloudHeight = 80,
            .surroundingGroundType = 0x07,
            .surroundingGroundHeight = 29,
            .surroundingWaterType = 0x08,
            .surroundingWaterHeight = 31
    };

    LevelMap map = {
            .length = 256,
            .height = 64,
            .width = 256,

            .spawnX = 128,
            .spawnZ = 128,
            .spawnY = 48,

            .blocks = NULL,
            .data = NULL
    };


    LevelEntityList e_list = {
            .count = 0,
            .entities = NULL
    };

    LevelTileEntityList te_list = {
            .count = 0,
            .entities = NULL
    };

    level.about = abt;
    level.environment =  env;
    level.map = map;
    level.entities = e_list;
    level.tileEntities = te_list;

    CC_Internal_Log_Message(CC_LOG_DEBUG, "Generated base level template");
}

void CrossCraft_World_Deinit() {
    CC_Internal_Log_Message(CC_LOG_INFO, "Destroying the world");

    if(level.map.blocks)
        free(level.map.blocks);

    if(level.map.data)
        free(level.map.data);

    if(level.entities.entities)
        free(level.entities.entities);

    if(level.tileEntities.entities)
        free(level.tileEntities.entities);

    CC_Internal_Log_Message(CC_LOG_DEBUG, "World freed");
}

void CrossCraft_World_Create_Map(uint8_t size) {
    switch(size) {
        case WORLD_SIZE_SMALL: {
            level.map.length = 128;
            level.map.width = 128;
            level.map.height = 64;
            break;
        }
        case WORLD_SIZE_HUGE: {
            level.map.length = 512;
            level.map.width = 512;
            level.map.height = 64;
            break;
        }
        case WORLD_SIZE_NORMAL:
        default: {
            level.map.length = 256;
            level.map.width = 256;
            level.map.height = 64;
            break;
        }
    }

    uint32_t blockCount = level.map.length * level.map.height * level.map.width;
    level.map.blocks = calloc(sizeof(uint8_t), blockCount);
    level.map.data = calloc(sizeof(uint8_t), blockCount);
}

#include <nbt.h>

/**
 * @brief Tries to load a world
 * @returns If the world was loaded
 */
bool CrossCraft_World_TryLoad(uint8_t slot, const char* prefix) {
    char buf[256] = {0};

    strcpy(buf, prefix);
    strcat(buf, "save.ccc");

    FILE* fptr = fopen(buf, "r");
    if(fptr) {
        slot = 5;
        // TODO: Load and convert

        fclose(fptr);
        return true;
    }

    if(slot >= 5) {
        return false;
    }
    sprintf(buf, "%slevel%d.mclevel", prefix, slot);

    CC_Internal_Log_Message(CC_LOG_INFO, "Attempting load.");
    CC_Internal_Log_Message(CC_LOG_DEBUG, buf);

    nbt_node* root = nbt_parse_path(buf);

    if(root == NULL) {
        CC_Internal_Log_Message(CC_LOG_WARN, "Could not load!");
        return false;
    }

    nbt_node* abt = nbt_find_by_name(root, "About");

    if(abt != NULL) {
        level.about.name = nbt_find_by_name(abt, "Name")->payload.tag_string;
        level.about.author = nbt_find_by_name(abt, "Author")->payload.tag_string;
        level.about.createdOn = nbt_find_by_name(abt, "CreatedOn")->payload.tag_long;

        CC_Internal_Log_Message(CC_LOG_TRACE, level.about.name);
        CC_Internal_Log_Message(CC_LOG_TRACE, level.about.author);
    }

    nbt_node *map = nbt_find_by_name(root, "Map");
    if(map != NULL) {
        level.map.width = nbt_find_by_name(map, "Width")->payload.tag_short;
        level.map.length = nbt_find_by_name(map, "Length")->payload.tag_short;
        level.map.height = nbt_find_by_name(map, "Height")->payload.tag_short;

        nbt_node* spawn = nbt_find_by_name(map, "Spawn");

        level.map.spawnX = nbt_list_item(spawn, 0)->payload.tag_short;
        level.map.spawnY = nbt_list_item(spawn, 1)->payload.tag_short;
        level.map.spawnZ = nbt_list_item(spawn, 2)->payload.tag_short;


        uint32_t blockCount = level.map.length * level.map.height * level.map.width;
        level.map.blocks = malloc(blockCount);
        level.map.data = malloc(blockCount);

        struct nbt_byte_array byteArray = nbt_find_by_name(map, "Blocks")->payload.tag_byte_array;
        memcpy(level.map.blocks, byteArray.data, blockCount);
        byteArray = nbt_find_by_name(map, "Data")->payload.tag_byte_array;
        memcpy(level.map.data, byteArray.data, blockCount);
    }

    nbt_free(root);

    return true;
}

/**
 * @brief Generates the world
 * @TODO Offer a callback for world percentage
 */
void CrossCraft_World_GenerateMap() {
    CrossCraft_WorldGenerator_Generate(&level.map);
}

/**
 * @brief Spawn the player into the world
 */
void CrossCraft_World_Spawn() {

}

LevelMap* CrossCraft_World_GetMapPtr() {
    return &level.map;
}