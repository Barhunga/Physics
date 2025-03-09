#pragma once
#include <glm/glm.hpp>
#include <Gizmos.h>

enum ShapeType {
    JOINT = -1,
    PLANE = 0,
    SPHERE,
    BOX,
    LAST
};

class PhysicsObject
{
protected:
    //PhysicsObject() {}
    PhysicsObject(ShapeType a_shapeID, float elasticity) : m_shapeID(a_shapeID), m_elasticity(elasticity) {}

    ShapeType m_shapeID;
    float m_elasticity;

public:
    virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
    virtual void draw() = 0;
    virtual void resetPosition() {};
    virtual float getEnergy() { return 0; }
    ShapeType getShapeID() { return m_shapeID; }
    virtual glm::vec2 getVelocity() { return glm::vec2(0, 0); };
    float getElasticity() { return m_elasticity; }

    virtual void setVelocity(glm::vec2 velocity) {};

    virtual bool IsInside(glm::vec2 point) { return false; }
};

