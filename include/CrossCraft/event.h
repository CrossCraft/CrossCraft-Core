#pragma once
#include <stdint.h>
#include <CrossCraft/entity.h>

#ifdef __cplusplus
extern "C" {
#endif

enum EventType {
    CROSSCRAFT_EVENT_TYPE_ADD_ENTITY,
    CROSSCRAFT_EVENT_TYPE_REMOVE_ENTITY,
    CROSSCRAFT_EVENT_TYPE_UPDATE_ENTITY
};

struct Event{
    uint32_t type;
};

struct EventEntity {
    uint32_t type;
    Entity* e;
};

struct EventEntityRemove {
    uint32_t type;
    uint32_t eid;
};

typedef void (*EventListener)(struct Event*);

void CrossCraft_Event_Subscribe(uint32_t type, EventListener listener);
void CrossCraft_Event_Push(uint32_t type, struct Event* event);

#ifdef __cplusplus
};
#endif