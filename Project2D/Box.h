#pragma once
#include "RigidBody.h"

class Box : public RigidBody
{
public:
    Box(glm::vec2 position, glm::vec2 velocity,
        float mass, float width, float height, float orientation, float elasticity, glm::vec4 colour) :
        m_extents(width, height), m_colour(colour), RigidBody(BOX, position, velocity, orientation, mass,
            1.0f / 12.0f * mass * (width * 2) * (height * 2), 0, elasticity) {}
    ~Box() {}

    virtual void draw() override;

    glm::vec2 getExtents() const { return m_extents; }
    float getWidth() { return m_extents.x * 2; }
    float getHeight() { return m_extents.y * 2; }

    glm::vec4 getColour() { return m_colour; }

    bool checkBoxCorners(const Box& box, glm::vec2& contact, int& numContacts, float& pen, glm::vec2& edgeNormal);

protected:
    glm::vec2 m_extents; // the halfedge lengths
    glm::vec4 m_colour;
};

