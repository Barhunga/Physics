#include "RigidBody.h"
#include "PhysicsScene.h"

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (m_isKinematic)
	{
		// allow for player control
		m_position += m_velocity * timeStep;
		// zero out movement before physics checks
		m_velocity = glm::vec2(0);
		m_angularVelocity = 0;
		return;
	}

	// apply drag
	m_velocity -= m_velocity * m_linearDrag * timeStep; 
	m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep; 

	// stop movement if value is super low
	if (length(m_velocity) < MIN_LINEAR_THRESHOLD) {
		m_velocity = glm::vec2(0, 0); 
	}
	if (abs(m_angularVelocity) < MIN_ANGULAR_THRESHOLD) {
		m_angularVelocity = 0;
	}

	// apply movement, gravity and rotation
	m_position += m_velocity * timeStep;
	applyForce(gravity * getMass() * timeStep, glm::vec2(0, 0));

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

void RigidBody::resolveCollision(RigidBody* actor2, glm::vec2 contact, glm::vec2* collisionNormal, float pen)
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

	if (v1 > v2) { // if they're moving closer
		// calculate the effective mass at contact point for each object
		// ie how much the contact point will move due to the force applied.
		float mass1 = 1.0f / (1.0f / getMass() + (r1 * r1) / getMoment());
		float mass2 = 1.0f / (1.0f / actor2->getMass() + (r2 * r2) / actor2->getMoment());

		float elasticity = (getElasticity() + actor2->getElasticity()) / 2.f;

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
	if (pen > 0) PhysicsScene::ApplyContactForces(this, actor2, normal, pen); 
}

float RigidBody::getKineticEnergy()
{
	return 0.5f * (getMass() * glm::dot(m_velocity, m_velocity) + getMoment() * m_angularVelocity * m_angularVelocity);
}

float RigidBody::getPotentialEnergy()
{
	return -getMass() * glm::dot(PhysicsScene::getGravity(), getPosition());
}

float RigidBody::getEnergy()
{
	return getKineticEnergy() + getPotentialEnergy();
}
