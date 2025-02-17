#include "RigidBody.h"
#include "PhysicsScene.h"

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	m_position += m_velocity * timeStep;
	applyForce(gravity * m_mass * timeStep, glm::vec2(0, 0));

	m_orientation += m_angularVelocity * timeStep;
}

void RigidBody::applyForce(glm::vec2 force, glm::vec2 pos)
{
	// use getMass() and getMoment() here in case we ever get it to do something
	// more than just return mass
	m_velocity += force / getMass();

	m_angularVelocity += (force.y * pos.x - force.x * pos.y) / getMoment();
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec2 force, glm::vec2 pos)
{
	actor2->applyForce(force, pos - actor2->m_position);
	applyForce(-force, pos - m_position);
}

void RigidBody::resolveCollision(RigidBody* actor2, glm::vec2 contact, glm::vec2* collisionNormal)
{
	// if the normal is provided, use that. otherwise find the vector between their centers
	glm::vec2 normal = glm::normalize(collisionNormal ? *collisionNormal : actor2->getPosition() - m_position); 
	// get the vector perpendicular to the collision normal
	glm::vec2 perp(normal.y, -normal.x);

	// determine the total velocity of the contact points for the two objects,
	// for both linear and rotational
	
	// 'r' is the radius from axis to application of force
	float r1 = glm::dot(contact - m_position, -perp);
	float r2 = glm::dot(contact - actor2->m_position, perp);
	// velocity of the contact point on the objects
	float v1 = glm::dot(m_velocity, normal) - r1 * m_angularVelocity;
	float v2 = glm::dot(actor2->m_velocity, normal) + r2 * actor2->m_angularVelocity;

	//	return;
	if (v1 > v2) { // if they're moving closer
		// calculate the effective mass at contact point for each object
		// ie how much the contact point will move due to the force applied.
		float mass1 = 1.0f / (1.0f / m_mass + (r1 * r1) / m_moment);
		float mass2 = 1.0f / (1.0f / actor2->m_mass + (r2 * r2) / actor2->m_moment);

		float elasticity = 1;

		glm::vec2 force = (1.0f + elasticity) * mass1 * mass2 / (mass1 + mass2) * (v1 - v2) * normal;

		//// check kinetic energy before collision
		//float kePre = getKineticEnergy() + actor2->getKineticEnergy();

		applyForceToActor(actor2, force, contact);

		//// check velocities after collision
		//printf("%f %f, %f %f\n", m_velocity.x, m_velocity.y, actor2->m_velocity.x, actor2->m_velocity.y);

		//// check kinetic energy again after collision to ensure it is preserved - remove after introducing variable elasticity
		//float kePost = getKineticEnergy() + actor2->getKineticEnergy();
		//float deltaKE = kePost - kePre;
		////printf("Before: %f\nAfter: %f\n", kePre, kePost); // print total energy before and after collision
		//if (deltaKE > kePost * 0.01f)
		//	printf("Kinetic Energy discrepancy greater than 1%% detected!!\n");
	}
}

float RigidBody::getKineticEnergy()
{
	return 0.5f * (m_mass * glm::dot(m_velocity, m_velocity) + m_moment * m_angularVelocity * m_angularVelocity);
}

float RigidBody::getPotentialEnergy()
{
	return -getMass() * glm::dot(PhysicsScene::getGravity(), getPosition());
}

float RigidBody::getEnergy()
{
	return getKineticEnergy() + getPotentialEnergy();
}
