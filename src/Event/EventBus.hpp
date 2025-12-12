#pragma once

#include <functional>
#include <typeindex>
#include <memory>
#include <mutex>

class EventBus {
private:
    static EventBus* instance;
    std::unordered_map<std::type_index, std::vector<std::function<void(void*)>>> handlers;
    std::recursive_mutex handlersMutex = std::recursive_mutex();

    EventBus() = default;

public:
    EventBus(const EventBus&) = delete;

    static EventBus& getInstance() {
        if (!instance) {
            instance = new EventBus();
        }
        return *instance;
    }

    template<typename EventType>
    void publish(EventType* event) {
        std::lock_guard<std::recursive_mutex> guard(handlersMutex);
        auto it = handlers.find(typeid(EventType));
        if (it != handlers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    template<typename EventType, typename ListenerType>
    void subscribe(ListenerType* listener, void (ListenerType::*memberFunc)(EventType*)) {
        std::type_index type = typeid(EventType);
        auto handler = [=](void* event) {
            (listener->*memberFunc)(static_cast<EventType*>(event));
        };
        handlers[type].push_back(handler);

        listener->registerUnsubscribe([this, type, handler]() {
            auto& vec = handlers[type];
            vec.erase(std::remove(vec.begin(), vec.end(), handler), vec.end());
        });
    }

    template<typename EventType>
    void subscribeLambda(std::function<void(EventType*)> func) {
        std::type_index type = typeid(EventType);
        auto handler = [func](void* event) {
            func(static_cast<EventType*>(event));
        };
        handlers[type].push_back(handler);
    }
};

#define SUBSCRIBE(listener, eventClass, function) \
    EventBus::getInstance().subscribe<eventClass>(listener, &std::remove_reference_t<decltype(*listener)>::function)

#define L_SUBSCRIBE(eventClass, lambda) \
    EventBus::getInstance().subscribeLambda<eventClass>(lambda)

inline EventBus* EventBus::instance = nullptr;