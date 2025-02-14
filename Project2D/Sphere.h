#pragma once
#include "RigidBody.h"
class Sphere : public RigidBody
{
public:
    Sphere(glm::vec2 position, glm::vec2 velocity,
        float mass, float radius, glm::vec4 colour) : 
        m_radius(radius), m_colour(colour), RigidBody(SPHERE, position, velocity, 0, mass, 
        0.5 * m_mass * m_radius * m_radius, 0) {}
    ~Sphere() {}

    virtual void draw() override;

    float getRadius() { return m_radius; }
    glm::vec4 getColour() { return m_colour; }

protected:
    float m_radius;
    glm::vec4 m_colour;
};

