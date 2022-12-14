#include <CrossCraft/log.h>
#include <CrossCraft/world.h>
#include <CrossCraft/worldgen.h>
#include <CrossCraft/indev.h>
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

    level.map.spawnX = level.map.length / 2;
    level.map.spawnY = level.map.height / 2;
    level.map.spawnZ = level.map.width / 2;

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

    if(slot >= 5) {
        return false;
    }
    sprintf(buf, "%slevel%d.mclevel", prefix, slot);
    printf("%s\n", buf);

    CC_Internal_Log_Message(CC_LOG_INFO, "Attempting load.");
    CC_Internal_Log_Message(CC_LOG_DEBUG, buf);

    nbt_node* root = nbt_parse_path(buf);

    if(root == NULL) {
        CC_Internal_Log_Message(CC_LOG_WARN, "Could not load!");
        return false;
    }

    nbt_node* abt = nbt_find_by_name(root, "About");

    if(abt != NULL) {
        level.about.name = strdup(nbt_find_by_name(abt, "Name")->payload.tag_string);
        level.about.author = strdup(nbt_find_by_name(abt, "Author")->payload.tag_string);
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


    char* res = nbt_dump_ascii(root);
    FILE* dump = fopen("dump.txt", "w");
    for(size_t i = 0; res[i] != 0; i++) {
        fprintf(dump, "%c", res[i]);
    }
    fclose(dump);

    nbt_free(root);

    return true;
}

struct nbt_list* create_generic(const char* string, nbt_type type) {
    struct nbt_list* new_list = malloc(sizeof(struct nbt_list));
    new_list->data = malloc(sizeof(nbt_node));
    new_list->data->name = strdup(string);
    new_list->data->type = type;

    return new_list;
}

void create_about(struct nbt_list* list) {
    struct nbt_list* CreatedOn = create_generic("CreatedOn", TAG_LONG);
    CreatedOn->data->payload.tag_long = level.about.createdOn;

    struct nbt_list* Name = create_generic("Name", TAG_STRING);
    Name->data->payload.tag_string = strdup(level.about.name);

    struct nbt_list* Author = create_generic("Author", TAG_STRING);
    Author->data->payload.tag_string = strdup(level.about.author);

    list_add_tail(&CreatedOn->entry, &list->entry);
    list_add_tail(&Name->entry, &list->entry);
    list_add_tail(&Author->entry, &list->entry);
}


void create_map(struct nbt_list* list) {
    struct nbt_list* Width = create_generic("Width", TAG_LONG);
    Width->data->payload.tag_short = level.map.width;

    struct nbt_list* Length = create_generic("Length", TAG_LONG);
    Length->data->payload.tag_short = level.map.length;

    struct nbt_list* Height = create_generic("Height", TAG_LONG);
    Height->data->payload.tag_short = level.map.height;

    struct nbt_list* Spawn = malloc(sizeof(struct nbt_list));
    Spawn->data = malloc(sizeof(nbt_node));
    Spawn->data->name = strdup("Spawn");
    Spawn->data->type = TAG_LIST;
    Spawn->data->payload.tag_list = malloc(sizeof(struct nbt_list));
    Spawn->data->payload.tag_list->data = malloc(sizeof(struct nbt_node));
    Spawn->data->payload.tag_list->data->name = NULL;
    Spawn->data->payload.tag_list->data->type = TAG_SHORT;
    Spawn->data->payload.tag_list->data->payload.tag_short = 3;
    INIT_LIST_HEAD(&Spawn->data->payload.tag_list->entry);

    {
        struct nbt_list *X = create_generic(NULL, TAG_LONG);
        X->data->payload.tag_short = level.map.spawnX;

        struct nbt_list *Y = create_generic(NULL, TAG_LONG);
        Y->data->payload.tag_short = level.map.spawnY;

        struct nbt_list *Z = create_generic(NULL, TAG_LONG);
        Z->data->payload.tag_short = level.map.spawnZ;

        list_add_tail(&X->entry, &Spawn->data->payload.tag_list->entry);
        list_add_tail(&Y->entry, &Spawn->data->payload.tag_list->entry);
        list_add_tail(&Z->entry, &Spawn->data->payload.tag_list->entry);
    }

    struct nbt_list* Blocks = create_generic("Blocks", TAG_BYTE_ARRAY);
    Blocks->data->payload.tag_byte_array.length = level.map.length * level.map.width * level.map.height;
    Blocks->data->payload.tag_byte_array.data = malloc(Blocks->data->payload.tag_byte_array.length);

    memcpy(Blocks->data->payload.tag_byte_array.data, level.map.blocks, level.map.length * level.map.width * level.map.height);

    struct nbt_list* Data = create_generic("Data", TAG_BYTE_ARRAY);
    Data->data->payload.tag_byte_array.length = level.map.length * level.map.width * level.map.height;
    Data->data->payload.tag_byte_array.data = malloc(Data->data->payload.tag_byte_array.length);

    memcpy(Data->data->payload.tag_byte_array.data, level.map.data, level.map.length * level.map.width * level.map.height);

    list_add_tail(&Width->entry, &list->entry);
    list_add_tail(&Length->entry, &list->entry);
    list_add_tail(&Height->entry, &list->entry);
    list_add_tail(&Spawn->entry, &list->entry);
    list_add_tail(&Blocks->entry, &list->entry);
    list_add_tail(&Data->entry, &list->entry);
}

/**
 * @brief Save the world
 * @param slot World slot
 */
void CrossCraft_World_Save(uint8_t slot, const char* prefix) {
    char buf[256] = {0};

    sprintf(buf, "%slevel%d.mclevel", prefix, slot);
    printf("%s\n", buf);

    // Create MCLEVEL Root
    nbt_node* tree = malloc(sizeof(nbt_node));
    tree->name = strdup("MinecraftLevel");
    tree->type = TAG_COMPOUND;
    tree->payload.tag_compound = malloc(sizeof(struct nbt_list));
    tree->payload.tag_compound->data = NULL;
    INIT_LIST_HEAD(&tree->payload.tag_compound->entry);

    struct nbt_list* about_list = malloc(sizeof(struct nbt_list));
    about_list->data = malloc(sizeof(nbt_node));
    about_list->data->name = strdup("About");
    about_list->data->type = TAG_COMPOUND;
    about_list->data->payload.tag_compound = malloc(sizeof(struct nbt_list));
    about_list->data->payload.tag_compound->data = NULL;
    INIT_LIST_HEAD(&about_list->data->payload.tag_compound->entry);

    create_about(about_list->data->payload.tag_compound);
    list_add_tail(&about_list->entry, &tree->payload.tag_compound->entry);


    struct nbt_list* map_list = malloc(sizeof(struct nbt_list));
    map_list->data = malloc(sizeof(nbt_node));
    map_list->data->name = strdup("Map");
    map_list->data->type = TAG_COMPOUND;
    map_list->data->payload.tag_compound = malloc(sizeof(struct nbt_list));
    map_list->data->payload.tag_compound->data = NULL;
    INIT_LIST_HEAD(&map_list->data->payload.tag_compound->entry);

    create_map(map_list->data->payload.tag_compound);
    list_add_tail(&map_list->entry, &tree->payload.tag_compound->entry);

    FILE* fptr = fopen(buf, "wb");
    if(nbt_dump_file(tree, fptr, STRAT_GZIP) != NBT_OK) {
        CC_Internal_Log_Message(CC_LOG_ERROR, "COULD NOT SAVE!");
    } else {
        CC_Internal_Log_Message(CC_LOG_INFO, "SAVED WORLD!");
    }
    fclose(fptr);

    nbt_free(tree);
}

/**
 * @brief Generates the world
 * @TODO Offer a callback for world percentage
 */
void CrossCraft_World_GenerateMap(WorldType worldType) {
    switch(worldType) {
        case WORLD_TYPE_ORIGINAL:
            CrossCraft_WorldGenerator_Generate_Original(&level.map);
            break;
        case WORLD_TYPE_FLAT:
            CrossCraft_WorldGenerator_Generate_Flat(&level.map);
            break;
        case WORLD_TYPE_ISLAND:
            CrossCraft_WorldGenerator_Generate_Island(&level.map);
            break;
        case WORLD_TYPE_WOODS:
            CrossCraft_WorldGenerator_Generate_Woods(&level.map);
            break;
        case WORLD_TYPE_FLOATING:
            CrossCraft_WorldGenerator_Generate_Floating(&level.map);
            break;
    }
}

/**
 * @brief Spawn the player into the world
 */
void CrossCraft_World_Spawn() {
    CC_Internal_Log_Message(CC_LOG_INFO, "Attempting spawn...");
    for(int i = 0; i < 30; i++) {
        int x = ((rand() % (level.map.length / 2)) - (level.map.length / 4)) + level.map.spawnX;
        int z = ((rand() % (level.map.width / 2)) - (level.map.width / 4)) + level.map.spawnZ;

        for(int y = level.map.height - 1; y >= 31; y--) {
            uint8_t blk = GetBlockFromMap(CrossCraft_World_GetMapPtr(), x, y, z);

            if(blk != 0 && !(blk == 8 || blk == 9)) {
                level.map.spawnX = x - 1;
                level.map.spawnY = y + 3;
                level.map.spawnZ = z - 1;

                CC_Internal_Log_Message(CC_LOG_INFO, "Spawned!");

                //TODO: Set Player Pos Through Communications System

                CrossCraft_Indev_House(x, y, z);

                return;
            }
        }
    }
}

LevelMap* CrossCraft_World_GetMapPtr() {
    return &level.map;
}