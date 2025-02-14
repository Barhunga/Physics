#include "Plane.h"

void Plane::draw()
{
    float lineSegmentLength = 300;
    glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
    // easy to rotate normal through 90 degrees around z
    glm::vec2 parallel(m_normal.y, -m_normal.x);
    glm::vec4 colourFade = m_colour;
    colourFade.a = 0;
    glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
    glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
    //aie::Gizmos::add2DLine(start, end, colour);
    aie::Gizmos::add2DTri(start, end, start - m_normal * 10.0f, m_colour, m_colour, colourFade);
    aie::Gizmos::add2DTri(end, end - m_normal * 10.0f, start - m_normal * 10.0f, m_colour, colourFade, colourFade);
}

void Plane::resetPosition()
{

}

void Plane::resolveCollision(RigidBody* actor2, glm::vec2 contact)
{
    // if the objects are already moving apart, we don't need to do anything
    if (glm::dot(m_normal, actor2->getVelocity()) >= 0)
        return;

    float elasticity = 1;
    float j = glm::dot(-(1 + elasticity) * (actor2->getVelocity()), m_normal) /
        (1 / actor2->getMass());
    glm::vec2 force = m_normal * j;

    actor2->applyForce(force, contact - actor2->getPosition());
}
