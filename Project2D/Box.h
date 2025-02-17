#pragma once
#include "RigidBody.h"

class Box : public RigidBody
{
public:
    Box(glm::vec2 position, glm::vec2 velocity,
        float mass, float width, float height, glm::vec4 colour) :
        m_extents(width / 2, height / 2), m_colour(colour), RigidBody(BOX, position, velocity, 0, mass,
            1.0f / 12.0f * m_mass * (m_extents.x * 2) * (m_extents.y * 2), 0) {
    }
    ~Box() {}

    virtual void draw() override;
    virtual void fixedUpdate(glm::vec2 gravity, float timeStep) override;

    glm::vec2 getExtents() { return m_extents; }
    float getWidth() { return m_extents.x * 2; }
    float getHeight() { return m_extents.y * 2; }
    glm::vec2 getLocalX() { return m_localX; }
    glm::vec2 getLocalY() { return m_localY; }
    glm::vec4 getColour() { return m_colour; }

protected:
    glm::vec2 m_extents; // the halfedge lengths
    glm::vec4 m_colour;

    // store the local x,y axes of the box based on its angle of rotation
    glm::vec2 m_localX;
    glm::vec2 m_localY;
};

