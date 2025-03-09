#pragma once
#include "RigidBody.h"
class Sphere : public RigidBody
{
public:
    Sphere(glm::vec2 position, glm::vec2 velocity,
        float mass, float radius, float elasticity, glm::vec4 colour) :
        m_radius(radius), m_colour(colour), RigidBody(SPHERE, position, velocity, 0, mass, 
        0.5 * mass * radius * radius, 0, elasticity) {}
    ~Sphere() {}

    virtual void draw() override;

    float getRadius() { return m_radius; }
    glm::vec4 getColour() { return m_colour; }

    bool getIsHole() { return isHole; }
    void setIsHole(bool state) { isHole = state; }

    virtual bool IsInside(glm::vec2 point) override { return glm::distance(point, m_position) <= m_radius; }

protected:
    float m_radius;
    glm::vec4 m_colour;

    bool isHole = false;
};

