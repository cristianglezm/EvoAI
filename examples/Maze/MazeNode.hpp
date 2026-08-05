#pragma once
#include <cstddef>
#include <string>
#include <JsonBox.h>

struct MazeNode{
    MazeNode() = default;
    MazeNode(JsonBox::Object o)
    : index(static_cast<std::size_t>(std::stoull(o["index"].getString())))
    , blocked(o["blocked"].getBoolean())
    , x(o["x"].getInteger())
    , y(o["y"].getInteger()){}
    JsonBox::Value toJson() const{
        JsonBox::Object o;
        o["index"] = JsonBox::Value(std::to_string(index));
        o["blocked"] = JsonBox::Value(blocked);
        o["x"] = JsonBox::Value(x);
        o["y"] = JsonBox::Value(y);
        return JsonBox::Value(o);
    }
    // data
    std::size_t index = 0;
    bool blocked = false;
    int x = 0;
    int y = 0;
};
