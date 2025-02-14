#pragma once
#include "RigidBody.h"

class Box : public RigidBody
{
public:
    Box(glm::vec2 position, glm::vec2 velocity,
        float mass, float width, float height, glm::vec4 colour) :
        m_width(width), m_height(height), m_colour(colour), RigidBody(BOX, position, velocity, 0, mass,
            1.0f / 12.0f * m_mass * m_width * m_height, 0) {
    }
    ~Box() {}

    virtual void draw() override;

    glm::vec4 getColour() { return m_colour; }

protected:
    float m_width;
    float m_height;
    glm::vec4 m_colour;
};

