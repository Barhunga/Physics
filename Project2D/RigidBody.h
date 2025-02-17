#pragma once
#include "PhysicsObject.h"

class RigidBody : public PhysicsObject
{
public:
    //RigidBody() {}
    RigidBody(ShapeType shapeID, glm::vec2 position,
        glm::vec2 velocity, float orientation, float mass, float moment, float angularVelocity) :
        m_position(position), m_velocity(velocity), m_orientation(orientation), m_mass(mass), 
        m_moment(moment), m_angularVelocity(angularVelocity), PhysicsObject(shapeID) {} 
    ~RigidBody() {}

    virtual void fixedUpdate(glm::vec2 gravity, float timeStep) override;
    void applyForce(glm::vec2 force, glm::vec2 pos);
    void applyForceToActor(RigidBody* actor2, glm::vec2 force, glm::vec2 pos);
    void resolveCollision(RigidBody* actor2, glm::vec2 contact, glm::vec2* collisionNormal = nullptr);

    glm::vec2 getPosition() const { return m_position; }
    float getOrientatation() { return m_orientation; }
    glm::vec2 getVelocity() { return m_velocity; }
    float getMass() { return m_mass; }
    float getMoment() { return m_moment; }
    float getAngularVelocity() { return m_angularVelocity; }

    float getKineticEnergy();
    float getPotentialEnergy();
    virtual float getEnergy() override;

    void setVelocity(glm::vec2 velocity) { m_velocity = velocity; }

protected:
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    float m_mass;
    float m_orientation;    // 2D so we only need a single float (radians) to represent our orientation

    float m_angularVelocity;
    float m_moment; // moment of inertia, the rotational equivalent of mass.
};

