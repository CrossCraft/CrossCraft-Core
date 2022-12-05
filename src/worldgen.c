#define FNL_IMPL
#include <CrossCraft/worldgen.h>
#include <CrossCraft/log.h>
#include <FastNoiseLite.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

static fnl_state state;
static int32_t worldgen_seed;

void CrossCraft_WorldGenerator_Init(int32_t seed) {
    state = fnlCreateState();
    state.seed = seed;
    state.noise_type = FNL_NOISE_PERLIN;
    state.frequency = 0.8f;
    worldgen_seed = seed;
}


// AMPLIFIED: amp = 2
float octave_noise(uint8_t octaves, float x, float y, uint16_t seed_modifier) {
    float sum = 0;
    float amp = 1.15f;
    float freq = 1;

    for (uint8_t i = 0; i < octaves; i++) {
        state.seed += seed_modifier;
        sum += fnlGetNoise2D(&state, x * freq, y * freq) * amp;

        amp *= 2;
        freq /= 2;
    }

    state.seed = worldgen_seed;

    return sum;
}

float combined_noise(uint8_t octaves, float x, float y, uint16_t seed_modifier) {
    float n2 = octave_noise(octaves, x, y, seed_modifier);
    return octave_noise(octaves, x + n2, y, seed_modifier);
}

float noise1(float x, float y) {
    return combined_noise(8, x, y, 1);
}

float noise2(float x, float y) {
    return combined_noise(8, x, y, 2);
}

float noise3(float x, float y) {
    return octave_noise(6, x, y, 0);
}

// Height of water
const int waterLevel = 32;

/**
 * Generates a heightmap
 * @param heightmap Map to generate
 * @param length Length of map (X)
 * @param width Width of map (Z)
 */
void create_heightmap(int16_t* heightmap, uint16_t length, uint16_t width) {
    for(int x = 0; x < length; x++) {
        for(int z = 0; z < width; z++) {
            float xf = (float)x;
            float zf = (float)z;

            float heightLow = noise1(xf * 1.3f, zf * 1.3f) / 6 - 4;
            float heightHigh = noise2(xf * 1.3f, zf * 1.3f) / 5 + 6;

            float hResult = 0.0f;

            if(noise3(xf, zf) / 8 > 0) {
                hResult = heightLow;
            } else {
                if(heightHigh > heightLow)
                    hResult = heightHigh;
                else
                    hResult = heightLow;
            }

            hResult /= 2.0f;

            if(hResult < 0)
                hResult *= 0.8f;

            heightmap[x + z * length] = (int16_t)(hResult + (float)waterLevel);
        }
    }
}

/**
 * Smooths out the heightmap
 * @param heightmap Map to smooth
 * @param length Length of map (X)
 * @param width Length of map (Z)
 */
void smooth_heightmap(int16_t* heightmap, uint16_t length, uint16_t width) {
    for(int x = 0; x < length; x++){
        for(int z = 0; z < width; z++) {
            float a = noise1((float)x * 2, (float)z * 2) / 8.0f;
            float b = (noise2((float)x * 2, (float)z * 2) > 0) ? 1 : 0;

            if(a > 2) {
                float c = ((float)heightmap[x + z * length] - b) / 2;
                float d = (float)((int)c * 2) + b;

                heightmap[x + z * length] = (int16_t)d;
            }
        }
    }
}

void create_strata(LevelMap* map, const int16_t* heightmap) {
    for(uint16_t x = 0; x < map->length; x++){
        for(uint16_t z = 0; z < map->width; z++){
            float dirt_thickness = octave_noise(8, x, z, 0) / 24.0f - 4.0f;
            int dirt_transition = heightmap[x + z * map->length];
            int stone_transition = dirt_transition + dirt_thickness;

            for (int y = 0; y < 64; y++) {
                int block_type = 0;

                if(y == 0 ){
                    block_type = 7;
                } else if (y == 1) {
                    block_type = 11;
                } else if (y <= stone_transition) {
                    block_type = 1;
                } else if (y <= dirt_transition) {
                    block_type = 3;
                }

                SetBlockInMap(map, x, y, z, block_type);
            }
        }
    }
}

// The fillOblateSpheroid function takes in a pointer to a LevelMap structure,
// the center coordinates (center_x, center_y, center_z) of an oblate spheroid,
// the radius of the spheroid, and a block type represented as a uint8_t value.
// It then iterates over all points within the given radius of the center and
// checks if the point is within the spheroid and has a block type of 1.
// If so, it sets the block at that point to the given block type.
// This function is useful for filling an oblate spheroid with a specific block type in a level map.
void fillOblateSpheroid(LevelMap* map, int center_x, int center_y, int center_z, int radius, uint8_t blk) {
    for (int x = center_x - radius; x <= center_x + radius; x++) {
        for (int y = center_y - radius; y <= center_y + radius; y++) {
            for (int z = center_z - radius; z <= center_z + radius; z++) {
                // Check if point is within bounds of map and has block type of 1
                if(BoundCheckMap(map, x, y, z) && GetBlockFromMap(map, x, y, z) == 1) {
                    // Set block at point to given block type
                    SetBlockInMap(map, x, y, z, blk);
                }
            }
        }
    }
}


void create_caves(LevelMap* map) {
    int num_caves = (map->length * map->height * map->width) / 4096;
    for(int i = 0; i < num_caves; i++) {
        int cave_x = rand() % map->length;
        int cave_y = rand() % map->height;
        int cave_z = rand() % map->width;

        // Generate a random cave length
        int cave_length = (int)((rand() / (float)RAND_MAX + rand() / (float)RAND_MAX) * 200.0f);

        // Generate random initial angles and rate of change
        float theta = rand() / (float)RAND_MAX * M_PI * 2.0f;
        float delta_theta = 0.0f;
        float phi = rand() / (float)RAND_MAX * M_PI * 2.0f;
        float delta_phi = 0.0f;

        // Generate a random cave radius
        float cave_radius = rand() / (float)RAND_MAX * rand() / (float)RAND_MAX;

        for (int len = 0; len < cave_length; len++) {
            // Update cave position using the given angles
            cave_x += sinf(theta) * cosf(phi);
            cave_y += cosf(theta) * cosf(phi);
            cave_z += sinf(phi);

            // Update angles and rate of change
            theta += delta_theta * 0.2f;
            delta_theta = (delta_theta * 0.9f) + (rand() / (float) RAND_MAX - rand() / (float) RAND_MAX);
            phi += delta_phi / 4.0f;
            delta_phi = (delta_phi * 0.75f) + (rand() / (float) RAND_MAX - rand() / (float) RAND_MAX);

            if (rand() / (float) RAND_MAX >= 0.25f) {
                // Generate a random center position
                int center_x = cave_x + (rand() % 4 - 2) * 0.2f;
                int center_y = cave_y + (rand() % 4 - 2) * 0.2f;
                int center_z = cave_z + (rand() % 4 - 2) * 0.2f;

                // Compute the radius based on the height
                float radius = (map->height - center_y) / (float) map->height;
                radius = 1.2f + (radius * 3.5f + 1) * cave_radius;
                radius = radius * sinf(len * M_PI / cave_length);

                fillOblateSpheroid(map, center_x, center_y, center_z, radius, 0);
            }
        }
    }
}

void create_vein(LevelMap* map, float abundance, uint8_t type) {
    int num_veins = (map->length * map->height * map->width * abundance) / 16384;
    for(int i = 0; i < num_veins; i++) {
        int vein_x = rand() % map->length;
        int vein_y = rand() % map->height;
        int vein_z = rand() % map->width;

        // Generate a random cave length
        int veinLength = (rand() / (float)RAND_MAX) * (rand() / (float)RAND_MAX) * 75 * abundance;

        // Generate random initial angles and rate of change
        float theta = (rand() / (float)RAND_MAX) * M_PI * 2;
        float delta_theta = 0;
        float phi = (rand() / (float)RAND_MAX) * M_PI * 2;
        float delta_phi = 0;

        for (int len = 0; len < veinLength; len++) {
            // Update cave position using the given angles
            vein_x += sinf(theta) * cosf(phi);
            vein_y += cosf(theta) * cosf(phi);
            vein_z += sinf(phi);

            // Update angles and rate of change
            theta += delta_theta * 0.2f;
            delta_theta = (delta_theta * 0.9f) + (rand() / (float) RAND_MAX - rand() / (float) RAND_MAX);
            phi += delta_phi / 4.0f;
            delta_phi = (delta_phi * 0.9f) + (rand() / (float) RAND_MAX - rand() / (float) RAND_MAX);

            float radius = abundance * sinf(len * M_PI / veinLength) + 1;

            fillOblateSpheroid(map, vein_x, vein_y, vein_z, radius, type);
        }
    }
}

void create_ores(LevelMap* map) {
    create_vein(map, 0.9f, 16);
    create_vein(map, 0.7f, 15);
    create_vein(map, 0.5f, 14);
}

void flood_fill_water(LevelMap* map) {
    // Flood-fill water into the map
    for (int x = 0; x < map->length; x++) {
        for (int z = 0; z < map->width; z++) {
            int y = waterLevel - 1;

            for(; y >= 0; y--) {
                if(GetBlockFromMap(map, x, y, z) == 0)
                    SetBlockInMap(map, x, y, z, 8);
                else
                    break;
            }
        }
    }

    // Add underground water sources
    int numWaterSources = map->length * map->width / 8000;
    for (int i = 0; i < numWaterSources; i++) {
        // Choose random x and z coordinates
        int x = rand() % map->length;
        int z = rand() % map->width;

        int y = waterLevel - (rand() % 24);

        if (GetBlockFromMap(map, x, y, z) == 0) {
            SetBlockInMap(map, x, y, z, 8);
        }
    }
}

void flood_fill_lava(LevelMap* map) {
    // Add underground lava sources
    int numLavaSources = map->length * map->width * map->height / 20000;
    for (int i = 0; i < numLavaSources; i++) {
        // Choose random x and z coordinates
        int x = rand() % map->length;
        int y = rand() % map->height - waterLevel;
        int z = rand() % map->width;

        if(y <= 0)
            continue ;

        if (GetBlockFromMap(map, x, y, z) == 0) {
            SetBlockInMap(map, x, y, z, 11);
        }
    }
}

void create_surface(LevelMap* map, int16_t* heightmap) {
    for (int x = 0; x < map->length; x++) {
        for (int z = 0; z < map->width; z++) {
            bool sandChance = (noise1(x, z) > 8);
            bool gravelChance = (noise2(x, z) > 12);

            int y = heightmap[x + z * map->length];
            uint8_t blockAbove = GetBlockFromMap(map, x, y + 1, z);

            if (blockAbove == 8 && gravelChance) {
                SetBlockInMap(map, x, y, z, 13);
            }

            if (blockAbove == 0) {
                if (y <= waterLevel && sandChance) {
                    SetBlockInMap(map, x, y, z, 12);
                } else {
                    while(GetBlockFromMap(map, x, y, z) == 0) {
                        y--;
                    }

                    uint8_t blkBelow = GetBlockFromMap(map, x, y, z);
                    if(blkBelow != 1 && blkBelow != 8) {
                        y += 1;
                        SetBlockInMap(map, x, y, z, 2);
                    }
                }
            }
        }
    }

}

void create_flowers(LevelMap* map, int16_t* heightmap) {
    int numPatches = map->width * map->length / 3000;

    for (int i = 0; i < numPatches; i++) {
        uint8_t flowerType = (rand() % 2 == 0) ? 37 : 38;
        uint16_t x = rand() % map->length;
        uint16_t z = rand() % map->width;

        for (int j = 0; j < 10; j++) {
            uint16_t fx = x;
            uint16_t fz = z;

            for (int k = 0; k < 5; k++) {
                fx += (rand() % 6) - (rand() % 6);
                fz += (rand() % 6) - (rand() % 6);

                if (BoundCheckMap(map, fx, 0, fz)) {
                    uint16_t fy = heightmap[fx + fz * map->length] + 1;

                    uint8_t blockBelow = GetBlockFromMap(map, fx, fy - 1, fz);

                    if (GetBlockFromMap(map, fx, fy, fz) == 0 && blockBelow == 2) {
                        SetBlockInMap(map, fx, fy, fz, flowerType);
                    }
                }
            }
        }
    }
}

void create_shrooms(LevelMap* map, int16_t* heightmap) {
    int numPatches = map->width * map->length * map->height / 2000;

    for (int i = 0; i < numPatches; i++) {
        uint8_t mushType = (rand() % 2 == 0) ? 39 : 40;
        uint16_t x = rand() % map->length;
        uint16_t y = rand() % map->height;
        uint16_t z = rand() % map->width;

        for (int j = 0; j < 20; j++) {
            uint16_t fx = x;
            uint16_t fy = y;
            uint16_t fz = z;

            for (int k = 0; k < 5; k++) {
                fx += (rand() % 6) - (rand() % 6);
                fy += (rand() % 2) - (rand() % 2);
                fz += (rand() % 6) - (rand() % 6);

                if (BoundCheckMap(map, fx, fy, fz) && BoundCheckMap(map, fx, fy - 1, fz) && fy < heightmap[fx + fz * map->length] - 1) {
                    uint8_t blockBelow = GetBlockFromMap(map, fx, fy - 1, fz);

                    if (GetBlockFromMap(map, fx, fy, fz) == 0 && blockBelow == 1) {
                        SetBlockInMap(map, fx, fy, fz, mushType);
                    }
                }
            }
        }
    }
}

bool isSpaceForTree(LevelMap* map, int x, int y, int z, int treeHeight) {
    // Check if the block below is grass
    if (BoundCheckMap(map, x, y -1 , z) && GetBlockFromMap(map, x, y - 1, z) != 2) {
        return false;
    }

    // Check if the trunk region is empty
    for (int i = x - 2; i <= x + 2; i++) {
        for (int j = y; j < y + treeHeight; j++) {
            for (int k = z - 2; k <= z + 2; k++) {
                if (BoundCheckMap(map, i, j, k) && GetBlockFromMap(map, i, j, k) != 0) {
                    return false;
                }
            }
        }
    }

    // Check if the canopy region is empty
    for (int i = x - 2; i <= x + 2; i++) {
        for (int j = y + treeHeight; j < y + treeHeight + 3; j++) {
            for (int k = z - 2; k <= z + 2; k++) {
                if (BoundCheckMap(map, i, j, k) && GetBlockFromMap(map, i, j, k) != 0) {
                    return false;
                }
            }
        }
    }

    // If all checks pass, return true
    return true;
}

void growTree(LevelMap* map, int x, int y, int z, int treeHeight) {
    int max = y + treeHeight;
    int m = max;

    for (; m >= y; m--) {
        if (m == max) {
            SetBlockInMap(map, x-1, m, z, 18);
            SetBlockInMap(map, x+1, m, z, 18);
            SetBlockInMap(map, x, m, z - 1, 18);
            SetBlockInMap(map, x, m, z + 1, 18);
            SetBlockInMap(map, x, m, z, 18);
        } else if (m == max - 1) {
            SetBlockInMap(map, x-1, m, z, 18);
            SetBlockInMap(map, x+1, m, z, 18);
            SetBlockInMap(map, x, m, z - 1, 18);
            SetBlockInMap(map, x, m, z + 1, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x - 1, m, z - 1, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x - 1, m, z + 1, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x + 1, m, z - 1, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x + 1, m, z + 1, 18);

            SetBlockInMap(map, x, m, z, 17);
        } else if (m == max - 2 || m == max - 3) {
            SetBlockInMap(map, x-1, m, z, 18);
            SetBlockInMap(map, x+1, m, z, 18);
            SetBlockInMap(map, x, m, z-1, 18);
            SetBlockInMap(map, x, m, z+1, 18);

            SetBlockInMap(map, x-1, m, z-1, 18);
            SetBlockInMap(map, x-1, m, z+1, 18);
            SetBlockInMap(map, x+1, m, z-1, 18);
            SetBlockInMap(map, x+1, m, z+1, 18);

            SetBlockInMap(map, x-2, m, z-1, 18);
            SetBlockInMap(map, x-2, m, z, 18);
            SetBlockInMap(map, x-2, m, z+1, 18);

            SetBlockInMap(map, x+2, m, z-1, 18);
            SetBlockInMap(map, x+2, m, z, 18);
            SetBlockInMap(map, x+2, m, z+1, 18);

            SetBlockInMap(map, x-1, m, z-2, 18);
            SetBlockInMap(map, x, m, z-2, 18);
            SetBlockInMap(map, x+1, m, z-2, 18);

            SetBlockInMap(map, x-1, m, z+2, 18);
            SetBlockInMap(map, x, m, z+2, 18);
            SetBlockInMap(map, x+1, m, z+2, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x-2, m, z-2, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x+2, m, z-2, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x-2, m, z+2, 18);

            if (rand() % 2 == 0)
                SetBlockInMap(map, x+2, m, z+2, 18);

            SetBlockInMap(map, x, m, z, 17);
            } else {
                SetBlockInMap(map, x, m, z, 17);
            }
    }

}


void create_trees(LevelMap* map, int16_t* heightmap) {
    int numPatches = map->width * map->length / 1000;

    for (int i = 0; i < numPatches; i++) {
        uint16_t x = rand() % map->length;
        uint16_t z = rand() % map->width;

        for (int j = 0; j < 20; j++) {
            uint16_t fx = x;
            uint16_t fz = z;

            for (int k = 0; k < 20; k++) {
                fx += (rand() % 6) - (rand() % 6);
                fz += (rand() % 6) - (rand() % 6);

                if (BoundCheckMap(map, fx, 0, fz) && (rand() / (float)RAND_MAX) <= 0.25f) {
                    uint16_t fy = heightmap[fx + fz * map->length] + 1;
                    uint16_t th = rand() % 3 + 4;

                    if(isSpaceForTree(map, fx, fy, fz, th)) {
                        growTree(map, fx, fy, fz, th);
                    }

                }
            }
        }
    }
}

void create_plants(LevelMap* map, int16_t* heightmap) {
    create_flowers(map, heightmap);
    create_shrooms(map, heightmap);
    create_trees(map, heightmap);
}

/**
 * Generate a map
 * https://github.com/UnknownShadow200/ClassiCube/wiki/Minecraft-Classic-map-generation-algorithm
 * @param map
 */
void CrossCraft_WorldGenerator_Generate(LevelMap* map) {
    int16_t* heightMap = calloc(sizeof(int16_t), map->length * map->width);

    // Generate a heightmap
    CC_Internal_Log_Message(CC_LOG_INFO, "Raising...");
    create_heightmap(heightMap, map->length, map->width);

    // Smooth heightmap
    CC_Internal_Log_Message(CC_LOG_INFO, "Eroding...");
    smooth_heightmap(heightMap, map->length, map->width);

    // Create Strata
    CC_Internal_Log_Message(CC_LOG_INFO, "Soiling...");
    create_strata(map, heightMap);

    // Create Caves
    CC_Internal_Log_Message(CC_LOG_INFO, "Carving...");
    create_caves(map);
    create_ores(map);

    // Watering
    CC_Internal_Log_Message(CC_LOG_INFO, "Watering...");
    flood_fill_water(map);

    // Melting
    CC_Internal_Log_Message(CC_LOG_INFO, "Melting...");
    flood_fill_lava(map);

    // Growing Surface Layer
    CC_Internal_Log_Message(CC_LOG_INFO, "Growing...");
    create_surface(map, heightMap);

    // Planting Flora
    CC_Internal_Log_Message(CC_LOG_INFO, "Planting...");
    create_plants(map, heightMap);

    free(heightMap);
}