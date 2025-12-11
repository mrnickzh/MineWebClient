#pragma once
#include <functional>
#include <map>

class Element {
public:
    std::string id;
    std::function<void(int, int, int)> callback;
    int x;
    int y;
    bool active = true;

    Element(std::string id, std::function<void(int, int, int)> callback, int x, int y) : id(id), callback(callback), x(x), y(y) {};
    virtual ~Element() = default;

    virtual void render() {}
    virtual void setPosition(int x, int y) {}
};
