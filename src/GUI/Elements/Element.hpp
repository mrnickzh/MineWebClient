#pragma once
#include <functional>
#include <map>

class Element {
public:
    std::string id;
    std::function<bool(int, int, int, bool)> callback;
    int x;
    int y;
    int wPer;
    int hPer;
    bool active = true;
    int fontsize;

    Element(std::string id, std::function<bool(int, int, int, bool)> callback, int wPer, int hPer, int fontsize) : id(id), callback(callback), wPer(wPer), hPer(hPer), fontsize(fontsize) {};
    virtual ~Element() = default;

    virtual void render() {}
    virtual void setPosition(int x, int y) {}
    virtual bool checkBounds(int x, int y) { return false; }
    virtual void reposition() {}
};
