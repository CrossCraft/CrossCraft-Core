#include <CrossCraft/mob.h>

void MobUpdate(Entity* e) {
    //TODO: Do AI logic

    e->base.acc.y = -1;
    e->base.update(e);
}