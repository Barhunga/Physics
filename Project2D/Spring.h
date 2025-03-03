#pragma once
#include "PhysicsObject.h"
#include "RigidBody.h"

class Spring : public PhysicsObject
{
public:
	Spring(RigidBody* body1, RigidBody* body2,
		float springCoefficient, float damping, float restLength,
		glm::vec2 contact1 = glm::vec2(0, 0), glm::vec2 contact2 = glm::vec2(0, 0)) :
		m_body1(body1), m_body2(body2), m_springCoefficient(springCoefficient), m_damping(damping), m_restLength(restLength),
		m_contact1(contact1), m_contact2(contact2), PhysicsObject(JOINT, 1) {
	}
	~Spring();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) override;
	virtual void draw() override;

	glm::vec2 getContact1() { return m_body1 ? m_body1->toWorld(m_contact1) : m_contact1; }
	glm::vec2 getContact2() { return m_body2 ? m_body1->toWorld(m_contact2) : m_contact2; }

protected:
	RigidBody* m_body1;
	RigidBody* m_body2;
	glm::vec2 m_contact1;
	glm::vec2 m_contact2;
	float m_damping;
	float m_restLength;
	float m_springCoefficient; // the restoring force;
};
