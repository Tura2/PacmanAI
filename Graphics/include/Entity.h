#pragma once

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void draw() const = 0;
};
