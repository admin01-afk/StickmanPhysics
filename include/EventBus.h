#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <any>

class EventBus {
public:
    template <typename EventType>
    using Callback = std::function<void(const EventType&)>;

    // Subscribe to an event type
    template <typename EventType>
    static void Subscribe(Callback<EventType> cb) {
        auto& list = listeners()[typeid(EventType)];
        list.push_back([cb](const std::any& e) {
            cb(std::any_cast<const EventType&>(e));
        });
    }

    // Emit an event
    template <typename EventType>
    static void Emit(const EventType& event) {
        auto it = listeners().find(typeid(EventType));
        if (it != listeners().end()) {
            for (auto& cb : it->second)
                cb(event);
        }
    }

private:
    using AnyCallback = std::function<void(const std::any&)>;
    static std::unordered_map<std::type_index, std::vector<AnyCallback>>& listeners() {
        static std::unordered_map<std::type_index, std::vector<AnyCallback>> _listeners;
        return _listeners;
    }
};