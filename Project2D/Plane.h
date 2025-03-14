#pragma once
#include "RigidBody.h"

class Plane : public PhysicsObject
{
public:
    Plane(glm::vec2 normal, float distance, float elasticity, glm::vec4 colour) :
        m_normal(normal), m_distanceToOrigin(distance), m_colour(colour), PhysicsObject(PLANE, elasticity) {};
    ~Plane() {}

    virtual void fixedUpdate(glm::vec2 gravity, float timeStep) override {};
    virtual void draw() override;
    virtual void resetPosition() override;

    void resolveCollision(RigidBody* actor2, glm::vec2 contact);

    glm::vec2 getNormal() { return m_normal; }
    float getDistance() { return m_distanceToOrigin; }

    void setDistance(float distance) { m_distanceToOrigin = distance; }

protected:
    glm::vec4 m_colour;
    glm::vec2 m_normal;
    float m_distanceToOrigin;
};

