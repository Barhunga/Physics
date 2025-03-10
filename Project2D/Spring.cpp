#include "Spring.h"

Spring::~Spring()
{
	delete m_body1;
	delete m_body2;
	m_body1 = nullptr;
	m_body2 = nullptr;
}

void Spring::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	// get the world coordinates of the ends of the springs
	glm::vec2 p1 = getContact1();
	glm::vec2 p2 = getContact2();
	float length = glm::distance(p1, p2);
	glm::vec2 direction = glm::normalize(p2 - p1);
	// apply damping
	glm::vec2 relativeVelocity = m_body2->getVelocity() - m_body1->getVelocity();
	// F = -kX - bv
	glm::vec2 force = direction * m_springCoefficient * (m_restLength - length) - m_damping * relativeVelocity;

	// cap the spring force to 1000 N to prevent numerical instability - base this limit on the spring coefficient and rest length 
	//const float threshold = 1000.0f;
	//float forceMag = glm::length(force);
	//if (forceMag > threshold)
	//	force *= threshold / forceMag;

	m_body1->applyForce(-force * timeStep, p1 - m_body1->getPosition()); 
	m_body2->applyForce(force * timeStep, p2 - m_body2->getPosition()); 
}

void Spring::draw()
{
	aie::Gizmos::add2DLine(getContact1(), getContact2(), glm::vec4(1, 1, 1, 1));
	//printf("drawing spring at\n%f %f\n%f %f\n", getContact1().x, getContact1().y, getContact2().x, getContact2().y);
}
