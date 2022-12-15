#include <CrossCraft/event.h>
#include <unordered_map>
#include <vector>

static std::unordered_map<uint32_t, std::vector<EventListener>> listener_map;

extern "C" {
    void CrossCraft_Event_Subscribe(uint32_t type, EventListener listener) {
        if(listener_map.find(type) == listener_map.end()) {
            std::vector<EventListener> e;
            e.push_back(listener);
            listener_map.emplace(type, std::move(e));
        } else {
            listener_map[type].push_back(listener);
        }
    }
    void CrossCraft_Event_Push(uint32_t type, Event* event) {
        if(listener_map.find(type) == listener_map.end())
            return;

        for(auto& f : listener_map[type]) {
            if(f != nullptr)
                f(event);
        }
    }
};
