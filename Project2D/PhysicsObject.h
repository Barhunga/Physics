#pragma once
#include <glm/glm.hpp>
#include <Gizmos.h>

enum ShapeType {
    PLANE = 0,
    SPHERE,
    BOX,
    LAST
};

class PhysicsObject
{
protected:
    //PhysicsObject() {}
    PhysicsObject(ShapeType a_shapeID) : m_shapeID(a_shapeID) {}

    ShapeType m_shapeID;

public:
    virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
    virtual void draw() = 0;
    virtual void resetPosition() {};
    virtual float getEnergy() { return 0; }
    ShapeType getShapeID() { return m_shapeID; }
};

