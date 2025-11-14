#pragma once

#include <vector>
#include <functional>

class EventListener {
protected:
    friend class EventBus;
    std::vector<std::function<void()>> unsubscribeFunctions;

    void registerUnsubscribe(std::function<void()> func) {
        unsubscribeFunctions.push_back(func);
    }

public:
    virtual ~EventListener() {
        for (auto& unsubscribe : unsubscribeFunctions) {
            unsubscribe();
        }
    }
};